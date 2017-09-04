/*
 * mbed SDK
 * Copyright (c) 2017 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Automatically generated configuration file.
// DO NOT EDIT, content will be overwritten.

#ifndef __MBED_CONFIG_DATA__
#define __MBED_CONFIG_DATA__

// Configuration parameters
#define MBED_CONF_UBLOX_CELL_N2XX_AT_PARSER_BUFFER_SIZE          256                            // set by library:ublox-cell-n2xx
#define MBED_CONF_UBLOX_CELL_AT_PARSER_TIMEOUT                   8000                           // set by library:ublox-cell
#define MBED_CONF_UBLOX_CELL_AT_PARSER_BUFFER_SIZE               256                            // set by library:ublox-cell
#define MBED_CONF_LWIP_ENABLE_PPP_TRACE                          0                              // set by library:lwip
#define MBED_CONF_LWIP_SOCKET_MAX                                4                              // set by library:lwip
#define MBED_CONF_LWIP_UDP_SOCKET_MAX                            4                              // set by library:lwip
#define MBED_CONF_LWIP_ADDR_TIMEOUT                              5                              // set by library:lwip
#define MODEM_ON_BOARD_UART                                      1                              // set by target:UBLOX_C030
#define MBED_CONF_LWIP_IPV4_ENABLED                              1                              // set by library:lwip
#define MBED_CONF_LWIP_DEFAULT_THREAD_STACKSIZE                  512                            // set by library:lwip
#define MBED_CONF_PPP_CELL_IFACE_APN_LOOKUP                      0                              // set by library:ppp-cell-iface
#define MBED_CONF_EVENTS_PRESENT                                 1                              // set by library:events
#define MBED_CONF_LWIP_TCP_SERVER_MAX                            4                              // set by library:lwip
#define MBED_CONF_LWIP_TCPIP_THREAD_STACKSIZE                    1200                           // set by library:lwip
#define MBED_CONF_LWIP_DEBUG_ENABLED                             0                              // set by library:lwip
#define MBED_CONF_LWIP_PPP_THREAD_STACKSIZE                      768                            // set by library:lwip
#define MBED_CONF_DRIVERS_UART_SERIAL_RXBUF_SIZE                 256                            // set by library:drivers
#define MBED_CONF_UBLOX_CELL_N2XX_BAUD_RATE                      9600                           // set by library:ublox-cell-n2xx
#define MBED_CONF_NSAPI_PRESENT                                  1                              // set by library:nsapi
#define MBED_CONF_MBED_CLIENT_RECONNECTION_COUNT                 3                              // set by library:mbed-client
#define MBED_CONF_FILESYSTEM_PRESENT                             1                              // set by library:filesystem
#define MBED_CONF_PPP_CELL_IFACE_BAUD_RATE                       115200                         // set by library:ppp-cell-iface
#define MBED_CONF_LWIP_IP_VER_PREF                               4                              // set by library:lwip
#define MBED_CONF_PPP_CELL_IFACE_AT_PARSER_BUFFER_SIZE           256                            // set by library:ppp-cell-iface
#define MBED_CONF_PLATFORM_STDIO_FLUSH_AT_EXIT                   1                              // set by library:platform
#define MODEM_ON_BOARD                                           1                              // set by target:UBLOX_C030
#define MBED_CONF_PLATFORM_STDIO_BAUD_RATE                       9600                           // set by library:platform
#define MBED_CONF_MBED_CLIENT_TCP_KEEPALIVE_TIME                 300                            // set by library:mbed-client
#define MBED_CONF_LWIP_IPV6_ENABLED                              0                              // set by library:lwip
#define MBED_CONF_NANOSTACK_HAL_NVM_CFSTORE                      0                              // set by library:nanostack-hal
#define MBED_CONF_MBED_CLIENT_SN_COAP_MAX_BLOCKWISE_PAYLOAD_SIZE 0                              // set by library:mbed-client
#define MBED_CONF_MBED_TRACE_ENABLE                              0                              // set by application[*]
#define MBED_CONF_PLATFORM_DEFAULT_SERIAL_BAUD_RATE              9600                           // set by library:platform
#define MBED_CONF_MBED_CLIENT_RECONNECTION_INTERVAL              5                              // set by library:mbed-client
#define MBED_CONF_UBLOX_CELL_N2XX_AT_PARSER_TIMEOUT              8000                           // set by library:ublox-cell-n2xx
#define MBED_CONF_LWIP_TCP_SOCKET_MAX                            4                              // set by library:lwip
#define MBED_CONF_RTOS_PRESENT                                   1                              // set by library:rtos
#define MBED_CONF_LWIP_TCP_ENABLED                               1                              // set by library:lwip
#define MBED_CONF_PPP_CELL_IFACE_AT_PARSER_TIMEOUT               8000                           // set by library:ppp-cell-iface
#define MBED_CONF_UBLOX_CELL_BAUD_RATE                           115200                         // set by library:ublox-cell
#define MBED_CONF_MBED_CLIENT_EVENT_LOOP_SIZE                    1024                           // set by library:mbed-client
#define MBED_CONF_NANOSTACK_HAL_EVENT_LOOP_THREAD_STACK_SIZE     6144                           // set by library:nanostack-hal
#define NSAPI_PPP_AVAILABLE                                      1                              // set by application[UBLOX_C030]
#define MBED_CONF_DRIVERS_UART_SERIAL_TXBUF_SIZE                 256                            // set by library:drivers
#define MBED_CONF_MBED_CLIENT_RECONNECTION_LOOP                  1                              // set by library:mbed-client
#define MBED_CONF_LWIP_USE_MBED_TRACE                            0                              // set by library:lwip
#define MBED_CONF_PLATFORM_STDIO_CONVERT_NEWLINES                1                              // set by application[*]
#define MBED_CONF_LWIP_ETHERNET_ENABLED                          0                              // set by application[UBLOX_C030]
// Macros
#define MBED_CLIENT_C_NEW_API                                                                   // defined by library:mbed-client
#define MBEDTLS_USER_CONFIG_FILE                                 "mbedtls_mbed_client_config.h" // defined by application
#define UNITY_INCLUDE_CONFIG_H                                                                  // defined by library:utest
#define MBED_HEAP_STATS_ENABLED                                  1                              // defined by application

#endif
