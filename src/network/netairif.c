#include "lwip/opt.h"

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include <netif/etharp.h>
//#include "usbd_cdc_rndis.h"

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct netairif {
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
	netif->hwaddr[0] = 0xFC;
	netif->hwaddr[1] = 0xFE;
	netif->hwaddr[2] = 0xFF;
	netif->hwaddr[3] = 0xAB;
	netif->hwaddr[4] = 0xBB;
	netif->hwaddr[5] = 0xCC;

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


static err_t low_level_output(struct netif *netif, struct pbuf *p){
	uint8_t buffer[2048];
	uint32_t pos=0;
//	uint32_t i=0;
	struct pbuf *q;

	pos=0;
	//TODO  initiate transfer();

#if ETH_PAD_SIZE
	pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif
	for(q = p; q != NULL; q = q->next) {
		/* Send the data from the pbuf to the interface, one pbuf at a
       time. The size of the data in each pbuf is kept in the ->len
       variable. */
		memcpy((buffer+pos), q->payload, q->len);
		pos+=q->len;
	}
//	if(STM_EVAL_PBGetState(BUTTON_USER)){
//		while(!RNDIS_TXEmpty());
//		RNDIS_DataTx(buffer, pos);
//	}

	//TODO  signal that packet should be sent();

#if ETH_PAD_SIZE
	pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

	LINK_STATS_INC(link.xmit);

	return ERR_OK;
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
err_t netairif_init(struct netif *netif){
	struct netairif *netairif;

	LWIP_ASSERT("netif != NULL", (netif != NULL));

	netairif = mem_malloc(sizeof(struct netairif));
	if (netairif == NULL) {
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

	netif->state = netairif;
	netif->name[0] = IFNAME0;
	netif->name[1] = IFNAME1;
	/* We directly use etharp_output() here to save a function call.
	 * You can instead declare your own function an call etharp_output()
	 * from it if you have to do some checks before sending (e.g. if link
	 * is available...) */
	netif->output = etharp_output;
	netif->linkoutput = low_level_output;

	netairif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

	/* initialize the hardware */
	low_level_init(netif);

	return ERR_OK;
}



/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */

void netairif_poll(struct netif *netif){
	struct eth_hdr *ethhdr;
	static uint8_t buffer[2048];
	int32_t len=0;
	uint32_t pos=0;
	struct pbuf *p, *q;

	return;
//TODO	len=RNDIS_RXRead(buffer,2048);
	if(len>0){
		p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
		if (p != NULL) {
			for(q = p; q != NULL; q = q->next) {
				memcpy(q->payload, buffer+pos, q->len);
				pos+=q->len;
			}
			/* points to packet payload, which starts with an Ethernet header */
			ethhdr = p->payload;

			switch (htons(ethhdr->type)) {
			/* IP or ARP packet? */
			case ETHTYPE_IP:
			case ETHTYPE_ARP:
				/* full packet send to tcpip_thread to process */
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
}
