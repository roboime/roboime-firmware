#include <network/ip_stack.h>

extern "C"{
#include <lwip/init.h>
#include <lwip/ip_frag.h>
#include <lwip/netif.h>
#include <lwip/tcp_impl.h>
#include <netif/etharp.h>
#include <network/dhcps.h>
#include <httpd.h>
}
#include <radio/serialnumber.h>


IP_STACK::IP_STACK() {
	uint8_t ip=DeviceID_8[0]+DeviceID_8[1] + DeviceID_8[2];
	if(ip==0 || ip==10 || ip==255 || ip==254 || ip==0x27){
		ip+=DeviceID_8[3];
	}
	lwip_init();

	IP4_ADDR(&ip_radio,192,168,10,10);

	IP4_ADDR(&ip_client,192,168,10,ip);

	IP4_ADDR(&ip_msk,255,255,255,0);
	IP4_ADDR(&ip_gw,0,0,0,0);

	netif_add(&rndisif,&ip_radio,&ip_msk,&ip_gw,NULL,rndisif_init,ethernet_input );
	netif_set_up(&rndisif);
	netif_set_default(&rndisif);

//	IP4_ADDR(&ip_add,192,168,11,21);
//	netif_add(&netairif,&ip_add,&ip_msk,&ip_gw,NULL,netairif_init,ethernet_input );
//	netif_set_up(&netairif);

	httpd_init();
	dhcps_init(ip_radio, ip_client);

}

void IP_STACK::Timer(uint32_t LocalTime) {
	uint32_t timestamp=LocalTime;
	static uint32_t TimeTCP=0;
	static uint32_t TimeARP=0;
	static uint32_t TimeIP=0;

	if((timestamp-TimeARP)>ARP_TMR_INTERVAL){
		 etharp_tmr();
		 TimeARP=timestamp;
	}

	if((timestamp-TimeTCP)>TCP_TMR_INTERVAL ){
		 tcp_tmr();
		 TimeTCP=timestamp;
	}

	if((timestamp-TimeIP)>IP_TMR_INTERVAL){
		ip_reass_tmr();
		TimeIP=timestamp;
	}
}

uint16_t IP_STACK::InputBuffer(uint8_t* buffer, uint16_t len) {
	return rndisif_inputbuffer(&rndisif, buffer, len);
}

uint16_t IP_STACK::GetData(uint8_t *buffer, uint16_t len) {
	return rndisif_getdata(buffer, len);
}
