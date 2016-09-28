/////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010, Valhalla Wireless
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the Valhalla Wireless nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Valhalla Wireless BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// History
// creation t.elliott 2010
//

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Acts as a very simple DHCP server.   It disables MS Netbios on the client.
//  It sets the IP address, Netmask, Broadcast, MTU for the client interface.
//
//  NEED TO FIX :
//	It will respond to any requests and always hands out the same IP address for every request
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <string.h>

#include <network/dhcps.h>
#include <lwip/udp.h>

struct __attribute__((packed)) dhcp_msg {
        uint8_t op, htype, hlen, hops;
        uint8_t xid[4];
        uint16_t secs, flags;
        uint8_t ciaddr[4];
        uint8_t yiaddr[4];
        uint8_t siaddr[4];
        uint8_t giaddr[4];
        uint8_t chaddr[16];
        uint8_t sname[64];
        uint8_t file[128];
        uint8_t options[312];
};

dhcps_state s;

//#define USE_CLASS_B_NET 1

#define BOOTP_BROADCAST 0x8000

#define DHCP_REQUEST        1
#define DHCP_REPLY          2
#define DHCP_HTYPE_ETHERNET 1
#define DHCP_HLEN_ETHERNET  6
#define DHCP_MSG_LEN      236

#define DHCPS_SERVER_PORT  67
#define DHCPS_CLIENT_PORT  68

#define DHCPDISCOVER  1
#define DHCPOFFER     2
#define DHCPREQUEST   3
#define DHCPDECLINE   4
#define DHCPACK       5
#define DHCPNAK       6
#define DHCPRELEASE   7

#define DHCP_OPTION_SUBNET_MASK   1
#define DHCP_OPTION_ROUTER        3
#define DHCP_OPTION_DNS_SERVER    6
#define DHCP_OPTION_REQ_IPADDR   50
#define DHCP_OPTION_LEASE_TIME   51
#define DHCP_OPTION_MSG_TYPE     53
#define DHCP_OPTION_SERVER_ID    54
#define DHCP_OPTION_INTERFACE_MTU 26
#define DHCP_OPTION_PERFORM_ROUTER_DISCOVERY 31
#define DHCP_OPTION_BROADCAST_ADDRESS 28
#define DHCP_OPTION_REQ_LIST     55
#define DHCP_OPTION_END         255

static const uint8_t xid[4] = {0xad, 0xde, 0x12, 0x23};
static const uint8_t magic_cookie[4] = {99, 130, 83, 99};
static struct udp_pcb *pcb_dhcps;
static struct ip_addr broadcast_dhcps;
static struct ip_addr server_address;
static struct ip_addr client_address;
static struct dhcp_msg *m;
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
static uint8_t * add_msg_type(uint8_t *optptr, uint8_t type)
{

        *optptr++ = DHCP_OPTION_MSG_TYPE;
        *optptr++ = 1;
        *optptr++ = type;
        return optptr;
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
static uint8_t * add_offer_options(uint8_t *optptr)
{
        struct ip_addr ipadd;

        ipadd.addr = *( (uint32_t *) &server_address);

#ifdef USE_CLASS_B_NET
        *optptr++ = DHCP_OPTION_SUBNET_MASK;
        *optptr++ = 4;  //len
        *optptr++ = 255;
        *optptr++ = 240;	//note this is different from interface netmask and broadcast address
        *optptr++ = 0;
        *optptr++ = 0;
#else
        *optptr++ = DHCP_OPTION_SUBNET_MASK;
        *optptr++ = 4;  //len
        *optptr++ = 255;
        *optptr++ = 255;	//note this is different from interface netmask and broadcast address
        *optptr++ = 255;
        *optptr++ = 0;
#endif

        *optptr++ = DHCP_OPTION_LEASE_TIME;
        *optptr++ = 4;  //len
        *optptr++ = 0x00;
        *optptr++ = 0x01;
        *optptr++ = 0x51;
        *optptr++ = 0x80; 	//1 day

        *optptr++ = DHCP_OPTION_SERVER_ID;
        *optptr++ = 4;  //len
        *optptr++ = ip4_addr1( &ipadd);
        *optptr++ = ip4_addr2( &ipadd);
        *optptr++ = ip4_addr3( &ipadd);
        *optptr++ = ip4_addr4( &ipadd);

#ifdef CLASS_B_NET
        *optptr++ = DHCP_OPTION_BROADCAST_ADDRESS;
        *optptr++ = 4;  //len
        *optptr++ = ip4_addr1( &ipadd);
        *optptr++ = 255;
        *optptr++ = 255;
        *optptr++ = 255;
#else
        *optptr++ = DHCP_OPTION_BROADCAST_ADDRESS;
        *optptr++ = 4;  //len
        *optptr++ = ip4_addr1( &ipadd);
        *optptr++ = ip4_addr2( &ipadd);
        *optptr++ = ip4_addr3( &ipadd);
        *optptr++ = 255;
#endif

        *optptr++ = DHCP_OPTION_INTERFACE_MTU;
        *optptr++ = 2;  //len
#ifdef CLASS_B_NET
        *optptr++ = 0x05;	//mtu of 1500
        *optptr++ = 0xdc;
#else
        *optptr++ = 0x02;	//mtu of 576
        *optptr++ = 0x40;
#endif

        *optptr++ = DHCP_OPTION_PERFORM_ROUTER_DISCOVERY;
        *optptr++ = 1;  //len
        *optptr++ = 0x00; //dont do router discovery

        //disable microsoft netbios over tcp
        *optptr++ = 43;	//vendor specific
        *optptr++ = 6;	//length of embedded option

        *optptr++ = 0x01;	//vendor specific (microsoft disable netbios over tcp)
        *optptr++ = 4;  //len
        *optptr++ = 0x00;
        *optptr++ = 0x00;
        *optptr++ = 0x00;
        *optptr++ = 0x02; 	//disable=0x02,  enable = 0x00

        return optptr;
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
static uint8_t * add_end(uint8_t *optptr)
{

        *optptr++ = DHCP_OPTION_END;
        return optptr;
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

static void create_msg(struct dhcp_msg *m)
{
        struct ip_addr client;

        client.addr = *( (uint32_t *) &client_address);

        m->op = DHCP_REPLY;
        m->htype = DHCP_HTYPE_ETHERNET;
        m->hlen = 6;  //mac id length
        m->hops = 0;
        memcpy((char *) xid, (char *) m->xid, sizeof(m->xid));
        m->secs = 0;
        m->flags = htons(BOOTP_BROADCAST); /*  Broadcast bit. */

        memcpy((char *) m->yiaddr, (char *) &client.addr, sizeof(m->yiaddr));

        memset((char *) m->ciaddr, 0, sizeof(m->ciaddr));
        memset((char *) m->siaddr, 0, sizeof(m->siaddr));
        memset((char *) m->giaddr, 0, sizeof(m->giaddr));
        memset((char *) m->sname, 0, sizeof(m->sname));
        memset((char *) m->file, 0, sizeof(m->file));

        memset((char *) m->options, 0, sizeof(m->options));
        memcpy((char *) m->options, (char *) magic_cookie, sizeof(magic_cookie));
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
static void send_offer(struct pbuf *p)
{
        uint8_t *end;

        create_msg(m);

        end = add_msg_type(&m->options[4], DHCPOFFER);
        end = add_offer_options(end);
        end = add_end(end);

        udp_sendto( pcb_dhcps, p, &broadcast_dhcps, DHCPS_CLIENT_PORT );

}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
static void send_nak(struct pbuf *p)
{

        uint8_t *end;

        create_msg(m);

        end = add_msg_type(&m->options[4], DHCPNAK);
        end = add_end(end);

        udp_sendto( pcb_dhcps, p, &broadcast_dhcps, DHCPS_CLIENT_PORT );

}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
static void send_ack(struct pbuf *p)
{

        uint8_t *end;

        create_msg(m);

        end = add_msg_type(&m->options[4], DHCPACK);
        end = add_offer_options(end);
        end = add_end(end);

        udp_sendto( pcb_dhcps, p, &broadcast_dhcps, DHCPS_CLIENT_PORT );

}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
static uint8_t parse_options(uint8_t *optptr, int16_t len)
{
        struct ip_addr client;

        client.addr = *( (uint32_t *) &client_address);

        uint8_t *end = optptr + len;
        int16_t type = 0;

        s.state = DHCPS_STATE_IDLE;

        while (optptr < end) {
                switch ((int16_t) *optptr) {

                case DHCP_OPTION_MSG_TYPE:
                        type = *(optptr + 2);
                        break;

                case DHCP_OPTION_REQ_IPADDR	:
                        if ( memcmp( (char *) &client.addr, (char *) optptr+2,4)==0 ) {
                                s.state = DHCPS_STATE_ACK;
                        } else {
                                s.state = DHCPS_STATE_NAK;
                        }
                        break;
                case DHCP_OPTION_END:
                        break;
                }


                optptr += optptr[1] + 2;
        }

        switch ( type ) {
        case	DHCPDECLINE	:
                s.state = DHCPS_STATE_IDLE;
                break;

        case	DHCPDISCOVER	:
                s.state = DHCPS_STATE_OFFER;
                break;

        case	DHCPREQUEST	:
                if ( !(s.state == DHCPS_STATE_ACK || s.state == DHCPS_STATE_NAK) ) {
                        s.state = DHCPS_STATE_NAK;
                }
                break;

        case	DHCPRELEASE	:
                s.state = DHCPS_STATE_IDLE;
                break;
        }
        return s.state;
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
static int16_t parse_msg(struct pbuf *p)
{

        m = (struct dhcp_msg *) p->payload;

        if ( memcmp( (char *) m->options, (char *) magic_cookie, sizeof(magic_cookie)) ==0 ) {
                return parse_options(&m->options[4], p->len);
        }
        return 0;
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
static void handle_dhcp(void *arg, struct udp_pcb *pcb, struct pbuf *p, struct ip_addr *addr, uint16_t port)
{

struct pbuf *q;
int16_t tlen;


        if (p==NULL) return;

	tlen = p->tot_len;
	if(p->next != NULL) {
		q = pbuf_coalesce(p, PBUF_TRANSPORT);
		if(q->tot_len != tlen) {
			pbuf_free(p);	
			return;
		}
	}
	else {
		q = p;
	}

        switch ( parse_msg(p) ) {

        case	DHCPS_STATE_OFFER	:
                send_offer(q);
                break;
        case	DHCPS_STATE_ACK	:
                send_ack(q);
                break;
        case	DHCPS_STATE_NAK	:
                send_nak(q);
                break;

        }

        pbuf_free(q);
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void dhcps_init(struct ip_addr server_ip, struct ip_addr client_ip){
        pcb_dhcps = udp_new();

        server_address.addr=server_ip.addr;
        client_address.addr=client_ip.addr;


        IP4_ADDR(&broadcast_dhcps,255,255,255,255);
//#ifdef DISCOVERY
//        IP4_ADDR(&server_address,192,168,10,21);
//        IP4_ADDR(&client_address,192,168,10,20);
//#else
//        IP4_ADDR(&server_address,192,168,10,31);
//        IP4_ADDR(&client_address,192,168,10,30);
//#endif
//#ifdef IP2
//        IP4_ADDR(&server_address,192,168,10,21);
//        IP4_ADDR(&client_address,192,168,10,20);
//#endif
        udp_bind(pcb_dhcps, IP_ADDR_ANY, DHCPS_SERVER_PORT );

        udp_recv(pcb_dhcps, handle_dhcp, NULL);

}
