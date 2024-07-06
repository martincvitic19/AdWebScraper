/**
 ***************************************************** 
 *                      INCLUDES
 *****************************************************
 */
#include <stdio.h>
#include <string>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include <string.h>

/**
 ***************************************************** 
 *                      MACROS
 *****************************************************
 */
#define UDP_PORT 4444
#define BEACON_MSG_LEN_MAX 127
#define BEACON_TARGET "255.255.255.255"
#define BEACON_INTERVAL_MS 1000

/**
 ***************************************************** 
 *           PUBLIC FUNCTION PROTOTYPES
 *****************************************************
 */

/**
 * @brief Prints the IP address of the default network interface.
 * 
 * This function retrieves the IP address of the default network interface and
 * prints it to the standard output. If the IP address cannot be retrieved, 
 * an error message is printed instead.
 * 
 * @note This function assumes that the network interface and IP stack are 
 *       properly initialized.
 */
void print_ip_address();

/**
 * @brief Continuously sends UDP beacon messages.
 * 
 * This function initializes a UDP protocol control block (PCB) and repeatedly
 * sends beacon messages to a predefined target address at regular intervals.
 * Each beacon message contains a counter value that increments with each 
 * message sent.
 * 
 * The function runs indefinitely in a loop, sending a beacon message every 
 * BEACON_INTERVAL_MS milliseconds. The function also handles error reporting
 * if a message fails to send.
 * 
 * @note If PICO_CYW43_ARCH_POLL is defined, the function will poll the 
 *       cyw43_arch periodically to handle Wi-Fi driver or lwIP work that 
 *       needs to be done.
 */
void run_udp_beacon();