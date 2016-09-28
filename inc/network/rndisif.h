#pragma once

#include <netif/etharp.h>

err_t rndisif_init(struct netif *netif);
uint16_t rndisif_inputbuffer(struct netif *netif, uint8_t *buffer, uint16_t len);
uint16_t rndisif_getdata(uint8_t *buffer, uint16_t len);
