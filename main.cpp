/**
 ***************************************************** 
 *                      INCLUDES
 *****************************************************
 */
#include <stdio.h>
#include <string>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "webScraperClient.h"

char ssid[] = "<network_ssid>";
char pass[] = "<network_password>";

volatile bool new_entry_received = false; // Flag to indicate a new entry
std::string received_entry_data; // Store the received entry data
/**
 ***************************************************** 
 *       INTERRUPT HANDLER / CALLBACK PROTOTYPES
 *****************************************************
 */
void udp_recv_callback(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, uint16_t port);
void run_udp_beacon_receiver_callback();

/**
 ***************************************************** 
 *                  PRIVATE FUNCTIONS
 *****************************************************
 */
int main()
{
    stdio_init_all();

    if (cyw43_arch_init_with_country(CYW43_COUNTRY_UK))
    {
        printf("Failed to initialize");
        return -1;
    }
    else
    {
        printf("Initialized.\n");
    }

    cyw43_arch_enable_sta_mode();
    int ret_val = cyw43_arch_wifi_connect_timeout_ms(ssid, pass, CYW43_AUTH_WPA2_AES_PSK, 8000);

    if (ret_val)
    {
        printf("Failed to connect; ret_val = %d\n", ret_val);
        return -1;
    }
    else
    {
        printf("Connected.\n");
    }
    print_ip_address();
    run_udp_beacon_receiver_callback();
    return 0;
}

/**
 ***************************************************** 
 *          INTERRUPT HANDLERS / CALLBACKS
 *****************************************************
 */

void udp_recv_callback(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, uint16_t port)
{
    if (p != NULL)
    {
        // Print the received packet
        printf("Received packet from %s:%d\n", ipaddr_ntoa(addr), port);
        printf("Data: %.*s\n", p->len, (char *)p->payload);

        // Check if the message indicates a new entry
        std::string data((char *)p->payload, p->len);
        if (data.find("NEW_ENTRY:") == 0) {
            printf("New entry has been received!\n");

            // Store the received data and set the flag
            received_entry_data = data;
            new_entry_received = true; // Set the flag
        }

        // Free pbuf
        pbuf_free(p);
    }
}

void run_udp_beacon_receiver_callback()
{
    struct udp_pcb* pcb = udp_new();
    if (pcb == NULL)
    {
        printf("Failed to create UDP PCB\n");
        return;
    }
    else
    {
        printf("PCB UDP successfully created.\n");
    }
    err_t err = udp_bind(pcb, IP_ADDR_ANY, UDP_PORT);

    if (err != ERR_OK)
    {
        printf("Failed to bind UDP PCB: error=%d\n", err);
        udp_remove(pcb);
        return;
    }
    else
    {
        printf("Successful UDP PCB bind.\n");
    }
    udp_recv(pcb, udp_recv_callback, NULL);

    while (true)
    {
#if PICO_CYW43_ARCH_POLL
        cyw43_arch_poll();
        sleep_ms(BEACON_INTERVAL_MS);
#else
        sleep_ms(BEACON_INTERVAL_MS);
#endif
        // Check if a new entry has been received
        if (new_entry_received)
        {
            new_entry_received = false; // Reset the flag

            // Print the received entry data
            printf("Received entry data: %s\n", received_entry_data.c_str());

            // Blink the LED
            for (int i = 0; i < 20; ++i)
            {
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
                sleep_ms(250);
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
                sleep_ms(250);
            }
        }
    }
    // Cleanup code in case of exit (though it will likely not be reached in this loop)
    udp_remove(pcb);
}
