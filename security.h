

/*

 * Copyright (c) 2015 ARM Limited. All rights reserved.

 * SPDX-License-Identifier: Apache-2.0

 * Licensed under the Apache License, Version 2.0 (the License); you may

 * not use this file except in compliance with the License.

 * You may obtain a copy of the License at

 *

 * http://www.apache.org/licenses/LICENSE-2.0

 *

 * Unless required by applicable law or agreed to in writing, software

 * distributed under the License is distributed on an AS IS BASIS, WITHOUT

 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

 * See the License for the specific language governing permissions and

 * limitations under the License.

 */

#ifndef __SECURITY_H__

#define __SECURITY_H__

 

#include <inttypes.h>

 

#define MBED_DOMAIN "e5728ced-5cfe-4812-833f-375eb9ba65d2"

#define MBED_ENDPOINT_NAME "7cfd8a28-65f4-49cb-8f16-d4ee889e7ab7"

 

const uint8_t SERVER_CERT[] = "-----BEGIN CERTIFICATE-----\r\n"

"MIIBmDCCAT6gAwIBAgIEVUCA0jAKBggqhkjOPQQDAjBLMQswCQYDVQQGEwJGSTEN\r\n"

"MAsGA1UEBwwET3VsdTEMMAoGA1UECgwDQVJNMQwwCgYDVQQLDANJb1QxETAPBgNV\r\n"

"BAMMCEFSTSBtYmVkMB4XDTE1MDQyOTA2NTc0OFoXDTE4MDQyOTA2NTc0OFowSzEL\r\n"

"MAkGA1UEBhMCRkkxDTALBgNVBAcMBE91bHUxDDAKBgNVBAoMA0FSTTEMMAoGA1UE\r\n"

"CwwDSW9UMREwDwYDVQQDDAhBUk0gbWJlZDBZMBMGByqGSM49AgEGCCqGSM49AwEH\r\n"

"A0IABLuAyLSk0mA3awgFR5mw2RHth47tRUO44q/RdzFZnLsAsd18Esxd5LCpcT9w\r\n"

"0tvNfBv4xJxGw0wcYrPDDb8/rjujEDAOMAwGA1UdEwQFMAMBAf8wCgYIKoZIzj0E\r\n"

"AwIDSAAwRQIhAPAonEAkwixlJiyYRQQWpXtkMZax+VlEiS201BG0PpAzAiBh2RsD\r\n"

"NxLKWwf4O7D6JasGBYf9+ZLwl0iaRjTjytO+Kw==\r\n"

"-----END CERTIFICATE-----\r\n";

 

const uint8_t CERT[] = "-----BEGIN CERTIFICATE-----\r\n"

"MIIBzzCCAXOgAwIBAgIEYUE4hjAMBggqhkjOPQQDAgUAMDkxCzAJBgNVBAYTAkZ\r\n"

"JMQwwCgYDVQQKDANBUk0xHDAaBgNVBAMME21iZWQtY29ubmVjdG9yLTIwMTgwHh\r\n"

"cNMTcwODE4MTAyNjE2WhcNMTgxMjMxMDYwMDAwWjCBoTFSMFAGA1UEAxNJZTU3M\r\n"

"jhjZWQtNWNmZS00ODEyLTgzM2YtMzc1ZWI5YmE2NWQyLzdjZmQ4YTI4LTY1ZjQt\r\n"

"NDljYi04ZjE2LWQ0ZWU4ODllN2FiNzEMMAoGA1UECxMDQVJNMRIwEAYDVQQKEwl\r\n"

"tYmVkIHVzZXIxDTALBgNVBAcTBE91bHUxDTALBgNVBAgTBE91bHUxCzAJBgNVBA\r\n"

"YTAkZJMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEHlUJFESnvfum9jYmPM2YO\r\n"

"D3xGSeN0KtXtpDiWZfLFjtY3udM1JyQ0WAsn4lvGnFEIbvlAv5A1VDxYWwVootj\r\n"

"cjAMBggqhkjOPQQDAgUAA0gAMEUCIQCSIwwN3AFxRpSQN8cjosLfwFOiVjrqOrz\r\n"

"YBGQ+XBwuPQIgR/yrj1F1Xho3n+cL5b3xPArkCoHj1OmmZUiO4Rb95Gs=\r\n"

"-----END CERTIFICATE-----\r\n";

 

const uint8_t KEY[] = "-----BEGIN PRIVATE KEY-----\r\n"

"MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgLHtEtXD4nDGI9kkn\r\n"

"8kE8IGtpCy7bwk3BWXm9X4jhCQmhRANCAAQeVQkURKe9+6b2NiY8zZg4PfEZJ43Q\r\n"

"q1e2kOJZl8sWO1je50zUnJDRYCyfiW8acUQhu+UC/kDVUPFhbBWii2Ny\r\n"

"-----END PRIVATE KEY-----\r\n";

 

#endif //__SECURITY_H__
