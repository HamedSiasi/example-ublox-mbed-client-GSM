/* mbed Microcontroller Library
 * Copyright (c) 2017 u-blox
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <string>
#include <sstream>
#include <vector>


#include "mbedtls/entropy_poll.h"
#include "UbloxATCellularInterface.h"
#include "UbloxPPPCellularInterface.h"
#include "UbloxATCellularInterfaceN2xx.h"
#include "simpleclient.h"
#include "security.h"
#include "mbed_trace.h"
#include "mbed.h"






//#define NBIOT
// If you wish to use LWIP and the PPP cellular interface, select
// the line UbloxPPPCellularInterface, otherwise select the line
// UbloxATCellularInterface.  Using the AT cellular interface does not
// require LWIP and hence uses less RAM.  It also allows other AT command
// operations (e.g. sending an SMS) to happen during a data transfer.
#ifdef NBIOT
#define INTERFACE_CLASS  UbloxATCellularInterfaceN2xx //NB//
#else
#define INTERFACE_CLASS  UbloxATCellularInterface  //GSM//
//#define INTERFACE_CLASS  UbloxPPPCellularInterface
#endif


//imei: 357518080022684
//imsi: 721001728180800

// The credentials of the SIM in the board.  If PIN checking is enabled
// for your SIM card you must set this to the required PIN.
#define PIN "0000"


// Network credentials.  You should set this according to your
// network/SIM card.  For C030 boards, leave the parameters as NULL
// otherwise, if you do not know the APN for your network, you may
// either try the fairly common "internet" for the APN (and leave the
// username and password NULL), or you may leave all three as NULL and then
// a lookup will be attempted for a small number of known networks
// (see APN_db.h in mbed-os/features/netsocket/cellular/utils).
#define APN         NULL
#define USERNAME    NULL
#define PASSWORD    NULL

// LEDs
DigitalOut ledRed(LED1, 1);
DigitalOut ledGreen(LED2, 1);
DigitalOut ledBlue(LED3, 1);

// The user button
volatile bool buttonPressed = false;


static void ledOn() {
    ledGreen = 1;
    ledBlue = 0;
    ledRed = 0;
}

static void ledOff() {
    ledBlue = 1;
    ledRed = 1;
    ledGreen = 0;
}




// Resource values for the Device Object
struct MbedClientDevice device = {
        "ublox",                    // (0)Manufacturer
        "nbiot",                    // (17)Type
        "C030-N211-0-00",           // (1)ModelNumber
        "0590",                     // (2)SerialNumber
		"1.1",                      // (3)FirmwareVersion
		"1.2",                      // (19)SoftwareVersion
		"1.3",                      // (18)HardwareVersion
		0,                          // (9)BatteryLevel
		0,                          // (20)BatteryStatus
		0,                          // (10)MemoryFree
		192000,                     // (21)MemoryTotal
		1                           // (6)AvailablePowerSources
        };






class LedResource {
public:
    LedResource() {
    	/*** Obj ***/
        ledObject = M2MInterfaceFactory::create_object("3311");
        /*** Instance ***/
        M2MObjectInstance *inst = ledObject->create_object_instance();

        /*** Resouce - an observable resource ***/
        M2MResource *onResource = inst->create_dynamic_resource("5850", "On/Off", M2MResourceInstance::BOOLEAN, true/*observation*/);
        onResource->set_operation(M2MBase::GET_PUT_POST_ALLOWED);
        onResource->set_value(false);

        /*** Resouce - an multi-valued resource ***/
        M2MResource *dimmerResource = inst->create_dynamic_resource("5851", "Dimmer", M2MResourceInstance::BOOLEAN, true/*observation*/);
        dimmerResource->set_operation(M2MBase::GET_PUT_POST_ALLOWED);
        dimmerResource->set_value(false);
    }

    ~LedResource() {
    }

    M2MObject *get_object() {
        return ledObject;
    }

private:
    M2MObject *ledObject;
};

static void cbButton()
{
    buttonPressed = true;
    ledOn();
}







class TempResource {
public:

    TempResource() {
        // (1) Obj
        temp_object = M2MInterfaceFactory::create_object("3303");

        // (2) Instance
        M2MObjectInstance* temp_inst = temp_object->create_object_instance();

        // (3) Resouce
        M2MResource* status_res = temp_inst->create_dynamic_resource(
        		"5700"                                /* resource_name     */,
        		"Temperature"                         /* resource_type     */,
				M2MResourceInstance::STRING           /* type              */,
				true                                  /* observable        */
				                                      /* multiple_instance */);

        // (4) Read(GET) and Write(PUT)
        status_res->set_operation(M2MBase::GET_PUT_POST_ALLOWED);

        // (5) Value of the resource
        status_res->set_value((const uint8_t*)"20.5" /*value*/, (const uint32_t) sizeof("20.5") /*value_length*/);

        // (6) Callback
        status_res->set_execute_function( execute_callback(this, &TempResource::Handling) );

        // (7) Delay
        status_res->set_delayed_response(true);
    }
    M2MObject* get_object() {
        return temp_object;
    }
    void Handling(void *argument) {
    	printf("TempResource I am executing !! \r\n");
    	printf("arg: %s \r\n\n\n\n\n\n\n", (char*)argument );
    }
private:
    M2MObject* temp_object;
};







/*
 *
 * This example program for the u-blox C030 board instantiates mbedClient
 * and runs it over a UbloxATCellularInterface or a UbloxPPPCellularInterface to
 * the mbed connector.
 * Progress may be monitored with a serial terminal running at 9600 baud.
 * The LED on the C030 board will turn green when this program is
 * operating correctly, pulse blue when an mbedClient operation is completed
 * and turn red if there is a failure.
 *
 *
 * Note: mbedClient malloc's around 34 kbytes of RAM, more than is available on
 * the C027 platform, hence this example will not run on the C027 platform.
 *
 * IMPORTANT to use this example you must first register with the mbed Connector:
 *
 * https://connector.mbed.com/
 *
 * ... using your mbed developer credentials and generate your own copy of the file
 * security.h, replacing the empty one in this directory with yours and
 * recompiling/downloading the code to your board.
 *
 */

int main()
{
	// no-debug the cell interface
	ledOff();
    INTERFACE_CLASS *interface = new INTERFACE_CLASS();
	// If you need to debug the cellular interface, comment out the
	// instantiation above and uncomment the one below.
    //INTERFACE_CLASS *interface = new INTERFACE_CLASS(MDMTXD, MDMRXD, MBED_CONF_UBLOX_CELL_BAUD_RATE, true /*DebugAT*/);

    MbedClient *mbedClient = new MbedClient(device);
    M2MObjectList objectList;
    M2MSecurity *registerObject;
    M2MDevice *deviceObject;
    M2MFirmware *firmwareObject;
    M2MServer  *serverObject;

    LedResource ledResource;
    TempResource tempResource;

    unsigned int seed;
    size_t len;
    InterruptIn userButton(SW0);

    // Attach a function to the user button
    userButton.rise(&cbButton);
    
#if MBED_CONF_MBED_TRACE_ENABLE
    mbed_trace_init();
#endif
    srand(seed);

    // Randomize source port
#ifdef MBEDTLS_TEST_NULL_ENTROPY
    mbedtls_null_entropy_poll(NULL, (unsigned char *) &seed, sizeof seed, &len);
#else
    mbedtls_hardware_poll(NULL, (unsigned char *) &seed, sizeof seed, &len);
#endif


    printf("\r\nStarting up ...\r\nPlease wait up to 180 seconds for network registration to complete... \r\n");
    if (interface->init(PIN)) {
        printf("Registered with network. \r\n");
        



        // Create endpoint interface to manage register and unregister
        //mbedClient->create_interface("coap://api.connector.mbed.com:5684", interface);
        mbedClient->create_interface("coap://169.45.82.18:5684", interface); // api.connector.mbed.com




        // Create objects of varying types, see simpleclient.h for more details on implementation.
        registerObject = mbedClient->create_register_object(); // Server object specifying connector info
        deviceObject = mbedClient->create_device_object();     // Device resources object
        firmwareObject = mbedClient->create_firmware_object();
        serverObject = mbedClient->create_server_object();




        // Add objects to list
        objectList.push_back(deviceObject);
        objectList.push_back(firmwareObject);
        objectList.push_back(serverObject);
        objectList.push_back(ledResource.get_object());
        objectList.push_back(tempResource.get_object());




        // Set endpoint registration object
        mbedClient->set_register_object(registerObject);

        printf("Updating object registration in a loop (with a 30 second refresh period) until the user button is presed... \r\n");
        interface->set_credentials(APN, USERNAME, PASSWORD);
        while (!buttonPressed) {
            // Make sure cellular is connected
        	ledOn();
            if (interface->connect() == 0) {
                printf("Still connected to packet network ... \r\n");
                if (mbedClient->register_successful()) {
                    printf("Updating registration ... \r\n");

                    /**********************************************************************************************************/
                    deviceObject->set_resource_value(M2MDevice::BatteryLevel, (int)batteryPercentage());
                    deviceObject->set_resource_value(M2MDevice::MemoryFree  , (int)memoryFree());

                    /**********************************************************************************************************/

                    mbedClient->test_update_register();
                } else {
                    printf("Registering... \r\n");
                    mbedClient->test_register(registerObject, objectList);
                }
            } else {
                ledOff();
                printf("Failed to connect, will retry (have you checked that an antenna is plugged in and your APN is correct?)... \r\n");
            }
            ledOff();
            Thread::wait(30000);
        }//while
        


        wait_ms(500);
        printf("User button was pressed, stopping ... \r\n");
        mbedClient->test_unregister();
        interface->disconnect();
        interface->deinit();
        ledOff();
        printf("Stopped.\n");
        M2MDevice::delete_instance();
    } else {
        ledOff();
        printf("Unable to initialise the interface. \r\n");
    }
}
// End Of File
