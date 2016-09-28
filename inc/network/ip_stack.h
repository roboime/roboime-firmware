#pragma once
#include <inttypes.h>
#include <lwip/ip_addr.h>
#include <network/rndisif.h>

class IP_STACK{
public:
	IP_STACK();
	void Timer(uint32_t LocalTime);
	uint16_t InputBuffer(uint8_t *buffer, uint16_t len);
	uint16_t GetData(uint8_t *buffer, uint16_t len);
protected:
	struct netif rndisif;
	struct netif netairif;
	ip_addr_t ip_client;
	ip_addr_t ip_radio;
	ip_addr_t ip_msk;
	ip_addr_t ip_gw;
};
