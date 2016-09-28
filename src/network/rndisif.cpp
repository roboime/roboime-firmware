/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

#include <lwip/debug.h>
#include <lwip/def.h>
#include <lwip/err.h>
#include <lwip/mem.h>
#include <lwip/netif.h>
#include <lwip/opt.h>
#include <lwip/pbuf.h>
//#include <lwip/snmp.h>
#include <lwip/stats.h>
#include <netif/etharp.h>
#include <stddef.h>
//#include "netif/ppp_oe.h"

#include <radio/serialnumber.h>
//#include <usb/usbd_cdc_rndis.h>

#include <utils/circularbuffer.h>

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct rndisif {
	struct eth_addr *ethaddr;
	/* Add whatever per-interface state that is needed here. */
};


/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void low_level_init(struct netif *netif){

	/* set MAC hardware address length */
	netif->hwaddr_len = ETHARP_HWADDR_LEN;

	/* set MAC hardware address */
	netif->hwaddr[0] = 0x42;
	netif->hwaddr[1] = 0x78;
	netif->hwaddr[2] = 0x75;
	netif->hwaddr[3] = DeviceID_8[0];
	netif->hwaddr[4] = DeviceID_8[1];
	netif->hwaddr[5] = DeviceID_8[2];

	/* maximum transfer unit */
	netif->mtu = 1500;

	/* device capabilities */
	/* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

	/* Do whatever else is needed to initialize interface. */
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

#include <queue>
CircularBuffer<uint8_t, false> _outbuffer(0,4096);
std::queue<uint16_t>  _outbuffersizelist;

static err_t low_level_output(struct netif *netif, struct pbuf *p){
	struct pbuf *q;
	uint16_t size=0;

	for(q = p; q != NULL; q = q->next) {
		/* Send the data from the pbuf to the interface, one pbuf at a
       time. The size of the data in each pbuf is kept in the ->len
       variable. */
		size+=_outbuffer.In((uint8_t*)q->payload, q->len);
	}
	_outbuffersizelist.push(size);

	LINK_STATS_INC(link.xmit);

	return ERR_OK;
}

uint16_t rndisif_getdata(uint8_t *buffer, uint16_t len){
	if(_outbuffersizelist.size()){
		if(len>_outbuffersizelist.front()){
			len=_outbuffersizelist.front();
		}
		_outbuffer.Out(buffer, len);
		if(len<_outbuffersizelist.front()){
			_outbuffer.Out(0,_outbuffersizelist.front()-len);
		}
		_outbuffersizelist.pop();
	} else {
		return 0;
	}
	return len;
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t rndisif_init(struct netif *netif){
	struct rndisif *rndisif;

	LWIP_ASSERT("netif != NULL", (netif != NULL));

	rndisif = (struct rndisif*)mem_malloc(sizeof(struct rndisif));
	if (rndisif == NULL) {
		LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
		return ERR_MEM;
	}

#if LWIP_NETIF_HOSTNAME
	/* Initialize interface hostname */
	netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

	/*
	 * Initialize the snmp variables and counters inside the struct netif.
	 * The last argument should be replaced with your link speed, in units
	 * of bits per second.
	 */
	NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

	netif->state = rndisif;
	netif->name[0] = IFNAME0;
	netif->name[1] = IFNAME1;
	/* We directly use etharp_output() here to save a function call.
	 * You can instead declare your own function an call etharp_output()
	 * from it if you have to do some checks before sending (e.g. if link
	 * is available...) */
	netif->output = etharp_output;
	netif->linkoutput = low_level_output;

	rndisif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

	/* initialize the hardware */
	low_level_init(netif);

	return ERR_OK;
}



//void rndisif_poll(struct netif *netif){
//	struct eth_hdr *ethhdr;
//	static uint8_t buffer[2048];
//	int32_t len=0;
//	uint32_t pos=0;
//	struct pbuf *p, *q;
//
////TODO	len=RNDIS_RXRead(buffer,2048);
//	if(len>0){
//		p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
//		if (p != NULL) {
//			for(q = p; q != NULL; q = q->next) {
//				memcpy(q->payload, buffer+pos, q->len);
//				pos+=q->len;
//			}
//			/* points to packet payload, which starts with an Ethernet header */
//			ethhdr = (eth_hdr*)p->payload;
//
//			switch (htons(ethhdr->type)) {
//			/* IP or ARP packet? */
//			case ETHTYPE_IP:
//			case ETHTYPE_ARP:
//				/* full packet send to tcpip_thread to process */
//				if (netif->input(p, netif)!=ERR_OK){
//					LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
//					pbuf_free(p);
//					p = NULL;
//				}
//				break;
//
//			default:
//				pbuf_free(p);
//				p = NULL;
//				break;
//			}
//		}
//	}
//}

uint16_t rndisif_inputbuffer(struct netif *netif, uint8_t *buffer, uint16_t len){
	struct eth_hdr *ethhdr;
	uint32_t pos=0;
	struct pbuf *p, *q;

	if(len>0){
		p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
		if (p != NULL) {
			for(q = p; q != NULL; q = q->next) {
				memcpy(q->payload, buffer+pos, q->len);
				pos+=q->len;
			}
			ethhdr = (eth_hdr*)p->payload;

			switch (htons(ethhdr->type)) {
			case ETHTYPE_IP:
			case ETHTYPE_ARP:
				if (netif->input(p, netif)!=ERR_OK){
					LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
					pbuf_free(p);
					p = NULL;
				}
				break;
			default:
				pbuf_free(p);
				p = NULL;
				break;
			}
		}
	}
	return len;
}

