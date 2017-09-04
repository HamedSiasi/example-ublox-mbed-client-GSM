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

#ifndef __SIMPLECLIENT_H__
#define __SIMPLECLIENT_H__

#include "mbed-client/m2minterfacefactory.h"
#include "mbed-client/m2mdevice.h"
#include "mbed-client/m2minterfaceobserver.h"
#include "mbed-client/m2minterface.h"
#include "mbed-client/m2mobject.h"
#include "mbed-client/m2mobjectinstance.h"
#include "mbed-client/m2mresource.h"
#include "mbed-client/m2mconfig.h"
#include "mbed-client/m2mblockmessage.h"
#include "security.h"
#include "mbed.h"
#include "battery_gauge_bq27441.h"
#include "mbed_stats.h"
#include "mbed-client/m2mfirmware.h"
#include "mbed-client/m2mserver.h"



#define STRINGIFY(s) #s

M2MInterface::NetworkStack NETWORK_STACK = M2MInterface::LwIP_IPv4;
M2MInterface::BindingMode SOCKET_MODE = M2MInterface::UDP_QUEUE; //default:UDP//


struct MbedClientDevice {
    const char* Manufacturer;     //0
    const char* Type;             //17
    const char* ModelNumber;      //1
    const char* SerialNumber;     //2
    const char* FirmwareVersion ; //3
    const char* SoftwareVersion ; //19
    const char* HardwareVersion ; //18
    int BatteryLevel;             //9
    int BatteryStatus;            //20
    int MemoryFree;               //10
    int MemoryTotal;              //21
    int AvailablePowerSources;    //6
};





/* This example program for the u-blox C030 board instantiates
 * the BQ27441 battery gauge and performs a few example calls to
 * the battery gauge API.  Progress may be monitored with a
 * serial terminal running at 9600 baud.  The LED on the C030
 * board will turn green when this program is operating correctly
 * and will turn red if there is a failure.
 */
int batteryPercentage()
{
	int retval = 0;
    I2C i2C(I2C_SDA_B, I2C_SCL_B);
    BatteryGaugeBq27441 gauge;
    int32_t reading;
    bool stop = false;
    if (gauge.init(&i2C)) {
#ifdef TARGET_UBLOX_C030
        if (gauge.disableBatteryDetect()) {
            //printf ("Battery detection input disabled (it is not connected on C030).\n");
        }
#endif
        while (!stop) {
            if (gauge.isBatteryDetected()) {
                if (!gauge.isGaugeEnabled()) {
                    gauge.enableGauge();
                }
                if (gauge.isGaugeEnabled()) {
                    if (gauge.getRemainingPercentage(&reading)) {
                    	retval = (int)reading;
                        printf("Remaining battery percentage: %d%%. \r\n", (int) reading);
                        stop = true;
                    } else {
                    	printf("Waiting ... \r\n");
                        wait_ms(500);
                    }
                } else {
                    printf("Battery gauge could not be enabled. \r\n");
                    stop = true;
                }
            } else {
                printf("Battery not detected !!! \r\n");
                stop = true;
            }
            wait_ms(500);
        }//while
    }
    return (int)retval;
}




/*
 * Runtime statistics
 * Heap stats
 * https://docs.mbed.com/docs/mbed-os-handbook/en/5.4/advanced/runtime_stats/
 */
int memoryFree(void)
{
    mbed_stats_heap_t heap_stats;
    //printf("Starting heap stats example\r\n");
    void *allocation = malloc(1000);
    //printf("Freeing 1000 bytes\r\n");
    mbed_stats_heap_get(&heap_stats);
    //printf("Current heap: %lu\r\n", heap_stats.current_size);
    //printf("Max heap size: %lu\r\n", heap_stats.max_size);
    free(allocation);
    mbed_stats_heap_get(&heap_stats);
    //printf("Current heap after: %lu\r\n", heap_stats.current_size);
    //printf("Max heap size after: %lu\r\n", heap_stats.max_size);
    printf("memoryFree: %d \r\n", 192000-heap_stats.current_size);
    return (int)(192000-heap_stats.current_size);
}


// void, const char*> value_updated_callback;
//void rebootFunction(const char* argument){
//	printf("rebootFunction Handling ! %s \r\n", argument );
//	NVIC_SystemReset();
//}












/*
* Wrapper for mbed client stack that handles all callbacks, error handling, and
* other shenanigans to make the mbed client stack easier to use.
*
* The end user should only have to care about configuring the parameters at the
* top of this file and making sure they add the security.h file correctly.
*/
class MbedClient: public M2MInterfaceObserver {
    
public:

    /*
     * (1) constructor
     * Constructor for MbedClient object, initialize private variables
     */
    MbedClient(struct MbedClientDevice device)
    {
        _interface = NULL;
        _bootstrapped = false;
        _error = false;
        _registered = false;
        _unregistered = false;
        _register_security = NULL;
        _value = 0;
        _object = NULL;
        _device = device;
    }



    /*
     * (2) de-constructor
     * Destructor for MbedClient object, you can ignore this
     */
    ~MbedClient()
    {
        if (_interface) {
            delete _interface;
        }
        if (_register_security) {
            delete _register_security;
        }
    }






    /*
     *  (3) Interface
     *  Creates M2MInterface using which endpoint can
     *  setup its name, resource type, life time, connection mode,
     *  Currently only LwIPv4 is supported.
     */
    void create_interface(const char *server_address, void *handler=NULL)
    {
        // Randomizing listening port for Certificate mode connectivity
        _server_address = server_address;
        uint16_t port = 0; // Network interface will randomize with port 0

        // Create mDS interface object, this is the base object everything else attaches to
        _interface = M2MInterfaceFactory::create_interface(*this,
                                                          MBED_ENDPOINT_NAME,       // endpoint_name The endpoint name of mbed Client string
                                                          "",                       // endpoint_type The endpoint type of mbed Client, default is empty. string
                                                          100,                      // life_time The lifetime of the endpoint in seconds
                                                          port,                     // listen_port The listening port for the endpoint, default is 5683.
                                                          MBED_DOMAIN,              // The domain of the endpoint, default is empty. string
                                                          SOCKET_MODE,              // The binding mode of the endpoint, default is NOT_SET
                                                          NETWORK_STACK,            // The underlying network stack to be used for the connection
                                                          "");                      // context address string


        const char *binding_mode = (SOCKET_MODE == M2MInterface:: UDP_QUEUE) ? " UDP_QUEUE" : "TCP";
        printf("SOCKET_MODE : %s \r\n", binding_mode);
        printf("Connecting to %s \r\n", server_address);
        if (_interface) {
            _interface->set_platform_network_handler(handler);
        }
    }



    /*
     *  Check private variable to see if the registration was sucessful or not
     */
    bool register_successful()
    {
        return _registered;
    }

    /*
     *  Check private variable to see if un-registration was sucessful or not
     */
    bool unregister_successful()
    {
        return _unregistered;
    }





    /*
     * object 0
     *
     *  Creates register server object with mbed device server address and other parameters
     *  required for client to connect to mbed device server.
     */
    M2MSecurity* create_register_object()
    {
        // Create security object using the interface factory.
        // this will generate a security ObjectID and ObjectInstance

    	printf("simpleClient - create_register_object \r\n");
        M2MSecurity *security = M2MInterfaceFactory::create_security(M2MSecurity::M2MServer);

        // Make sure security ObjectID/ObjectInstance was created successfully
        if (security) {
            // Add ResourceID's and values to the security ObjectID/ObjectInstance
            security->set_resource_value(M2MSecurity::M2MServerUri, _server_address);
            security->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::Certificate);
            security->set_resource_value(M2MSecurity::ServerPublicKey, SERVER_CERT, sizeof(SERVER_CERT) - 1);
            security->set_resource_value(M2MSecurity::PublicKey, CERT, sizeof(CERT) - 1);
            security->set_resource_value(M2MSecurity::Secretkey, KEY, sizeof(KEY) - 1);
        }
        return security;
    }





    /*
     * object 3
     *
     * Creates device object which contains mandatory resources linked with
     * device endpoint.
     */
    M2MDevice* create_device_object()
    {
        // Create device objectID/ObjectInstance
    	printf("simpleClient - create_device_object \r\n");
        M2MDevice *device = M2MInterfaceFactory::create_device();
        // Make sure device object was created successfully
        if (device) {
            // Add resourceID's to device objectID/ObjectInstance
            device->create_resource(M2MDevice::Manufacturer,     _device.Manufacturer);    //0
            device->create_resource(M2MDevice::DeviceType,       _device.Type);            //17
            device->create_resource(M2MDevice::ModelNumber,      _device.ModelNumber);     //1
            device->create_resource(M2MDevice::SerialNumber,     _device.SerialNumber);    //2
            device->create_resource(M2MDevice::HardwareVersion,  _device.HardwareVersion); //18
            device->create_resource(M2MDevice::FirmwareVersion,  _device.FirmwareVersion); //3
            device->create_resource(M2MDevice::SoftwareVersion,  _device.SoftwareVersion); //19
            device->create_resource(M2MDevice::BatteryLevel,     _device.BatteryLevel);    //9
            device->create_resource(M2MDevice::BatteryStatus,    _device.BatteryStatus);   //20
            device->create_resource(M2MDevice::MemoryFree,       _device.MemoryFree);      //10
            device->create_resource(M2MDevice::MemoryTotal,      _device.MemoryTotal);     //21
        }
        return device;
    }



    /*
     * object 5
     */
    M2MFirmware* create_firmware_object()
    {
    	printf("simpleClient - create_firmware_object \r\n");
    	M2MFirmware *firmware = M2MInterfaceFactory::create_firmware();
    	if (firmware) {
    		//firmware->create_resource(M2MFirmware::PackageUri, "PackageUri");       //1RW
    		firmware->create_resource(M2MFirmware::PackageName, "PackageName");       //6R Optional
    		firmware->create_resource(M2MFirmware::PackageVersion, "PackageVersion"); //7R Optional
			//firmware->create_resource(M2MFirmware::State, (int64_t)0);
    		//firmware->create_resource(M2MFirmware::UpdateSupportedObjects, (int64_t)0);
    		//firmware->create_resource(M2MFirmware::UpdateResult, (int64_t)0);
    	}
    	return firmware;
    }



    /*
     * object 1
     */
    M2MServer* create_server_object()
    {
    	printf("simpleClient - create_server_object \r\n");
    	M2MServer *server = M2MInterfaceFactory::create_server();
    	if (server) {
    		server->create_resource(M2MServer::ShortServerID,       (uint32_t) 12  ); //0R
    		server->create_resource(M2MServer::Lifetime,            (uint32_t) 100 ); //1RW
    		//server->create_resource(M2MServer::DefaultMinPeriod,    (uint32_t)12 ); //2RW
    		//server->create_resource(M2MServer::DefaultMaxPeriod,    (uint32_t)12 ); //3RW
    		//server->create_resource(M2MServer::DisableTimeout,      (uint32_t)12 ); //5RW
    		//server->create_resource(M2MServer::NotificationStorage, (uint32_t)12 ); //6RW
    	}
    	return server;
    }




    /*
     * Register an object
     */
    void test_register(M2MSecurity *register_object, M2MObjectList object_list)
    {
    	printf("simpleClient - test_register \r\n");
        if (_interface) {
            // Register function
            _interface->register_object(register_object, object_list);
        }
    }


    /*
     * Unregister all objects
     */
    void test_unregister()
    {
    	printf("simpleClient - test_unregister \r\n");
        if (_interface) {
            // Unregister function
            _interface->unregister_object(NULL); // NULL will unregister all objects
        }
    }

    /*
     * Callback from mbed client stack when the bootstrap
     * is successful, it returns the mbed Device Server object
     * which will be used for registering the resources to
     * mbed Device server.
     */
    void bootstrap_done(M2MSecurity *server_object)
    {
    	printf("simpleClient - bootstrap_done \r\n");
        if (server_object) {
            _bootstrapped = true;
            _error = false;
            printf("Bootstrapped\n");
        }
    }

    /*
     * Callback from mbed client stack when the registration
     * is successful, it returns the mbed Device Server object
     * to which the resources are registered and registered objects.
     */
    void object_registered(M2MSecurity */*security_object*/, const M2MServer & /*server_object*/)
    {
        _registered = true;
        _unregistered = false;
        printf("Registered object successfully ! \r\n\n\n\n");
    }

    /*
     * Callback from mbed client stack when the unregistration
     * is successful, it returns the mbed Device Server object
     * to which the resources were unregistered.
     */
    void object_unregistered(M2MSecurity */*server_object*/)
    {
    	printf("Unregistered Object Successfully ! \r\n\n\n\n");
        _unregistered = true;
        _registered = false;
    }




    /*
     * Callback from mbed client stack when registration is updated
     */
    void registration_updated(M2MSecurity */*security_object*/, const M2MServer & /*server_object*/)
    {
        /* The registration is updated automatically and frequently by the
        *  mbed client stack. This print statement is turned off because it
        *  tends to happen a lot.
        */
        printf("Registration updated! :) \r\n\n\n\n");
    }

    /*
     * Callback from mbed client stack if any error is encountered
     * during any of the LWM2M operations. Error type is passed in
     * the callback.
     */
    void error(M2MInterface::Error error)
    {
        _error = true;
        switch (error){
            case M2MInterface::AlreadyExists:
                printf("[ERROR:] M2MInterface::AlreadyExist \r\n");
                break;
            case M2MInterface::BootstrapFailed:
                printf("[ERROR:] M2MInterface::BootstrapFailed \r\n");
                break;
            case M2MInterface::InvalidParameters:
                printf("[ERROR:] M2MInterface::InvalidParameters \r\n");
                break;
            case M2MInterface::NotRegistered:
                printf("[ERROR:] M2MInterface::NotRegistered \r\n");
                break;
            case M2MInterface::Timeout:
                printf("[ERROR:] M2MInterface::Timeout \r\n");
                break;
            case M2MInterface::NetworkError:
                printf("[ERROR:] M2MInterface::NetworkError \r\n");
                break;
            case M2MInterface::ResponseParseFailed:
                printf("[ERROR:] M2MInterface::ResponseParseFailed \r\n");
                break;
            case M2MInterface::UnknownError:
                printf("[ERROR:] M2MInterface::UnknownError \r\n");
                break;
            case M2MInterface::MemoryFail:
                printf("[ERROR:] M2MInterface::MemoryFail \r\n");
                break;
            case M2MInterface::NotAllowed:
                printf("[ERROR:] M2MInterface::NotAllowed \r\n");
                break;
            case M2MInterface::SecureConnectionFailed:
                printf("[ERROR:] M2MInterface::SecureConnectionFailed \r\n");
                break;
            case M2MInterface::DnsResolvingFailed:
                printf("[ERROR:] M2MInterface::DnsResolvingFailed \r\n");
                break;
            default:
                break;
        }
    }







    /*
     * Callback from mbed client stack if any value has changed
     *  during PUT operation. Object and its type is passed in
     *  the callback.
     *  BaseType enum from m2mbase.h
     *       Object = 0x0, Resource = 0x1, ObjectInstance = 0x2, ResourceInstance = 0x3
     */
    void value_updated(M2MBase *base, M2MBase::BaseType type)
    {
        printf("PUT request received! \r\n");
        printf("\nValue updated of Object name %s and Type %d\n", base->name(), type);
        printf("Name :'%s', \nPath : '%s', \nType : '%d' (0 for Object, 1 for Resource), \nType : '%s'  \r\n\n\n",
               base->name(),
               base->uri_path(),
               type,
               base->resource_type());
    }






    /*
     * Update the registration period
     */
    void test_update_register()
    {
        if (_registered) {
            _interface->update_registration(_register_security, 100);
        }
    }

    /*
     * Manually configure the security object private variable
     */
   void set_register_object(M2MSecurity *register_object)
   {
        if (_register_security == NULL) {
            _register_security = register_object;
        }
    }

private:

    /*
     *  Private variables used in class
     */
    M2MInterface             *_interface;
    M2MSecurity              *_register_security;
    M2MObject                *_object;
    volatile bool            _bootstrapped;
    volatile bool            _error;
    volatile bool            _registered;
    volatile bool            _unregistered;
    int                      _value;
    struct MbedClientDevice  _device;
    String                   _server_address;
};

#endif // __SIMPLECLIENT_H__
