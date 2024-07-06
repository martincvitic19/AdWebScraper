/**
 ***************************************************** 
 *                      INCLUDES
 *****************************************************
 */
#include "webScraperClient.h"

/**
 ***************************************************** 
 *                  PUBLIC FUNCTIONS
 *****************************************************
 */
void print_ip_address()
{
    const ip4_addr_t* ip = netif_ip4_addr(netif_default);
    if (ip)
    {
        printf("IP address: %s\n", ip4addr_ntoa(ip));
    }
    else
    {
        printf("Failed to get IP address\n");
    }
}

void run_udp_beacon()
{
    struct udp_pcb* pcb = udp_new();

    ip_addr_t addr;
    ipaddr_aton(BEACON_TARGET, &addr);

    int counter = 0;
    while (true)
    {
        struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, BEACON_MSG_LEN_MAX + 1, PBUF_RAM);
        char *req = (char *)p->payload;
        memset(req, 0, BEACON_MSG_LEN_MAX + 1);
        snprintf(req, BEACON_MSG_LEN_MAX, "%d\n", counter);
        err_t er = udp_sendto(pcb, p, &addr, UDP_PORT);
        pbuf_free(p);
        if (er != ERR_OK)
        {
            printf("Failed to send UDP packet! error=%d", er);
        }
        else
        {
            printf("Sent packet %d\n", counter);
            counter++;
        }

#if PICO_CYW43_ARCH_POLL
        cyw43_arch_poll();
        sleep_ms(BEACON_INTERVAL_MS);
#else
        sleep_ms(BEACON_INTERVAL_MS);
#endif
    }
}