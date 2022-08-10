/*
usbtest

SPDXID: BSD-2-Clause

Copyright (c) 2022 Raspberry Pi Ltd

*/


#include <stdio.h>
#include <string.h>

#include "libusb.h"
#include <unistd.h>

#define USB_CTRL_GET_TIMEOUT     5000
#define USB_PORT_FEAT_TEST       21
#define NUM_PORTS_TO_CHECK       14

void dump_device(libusb_device *dev)
{
	struct libusb_device_descriptor desc;

	int ret = libusb_get_device_descriptor(dev, &desc);
	
	if (ret < 0)
	{
		printf("Failed to get device descriptor");
		return;
	}

	printf("Device on bus %u, device %u: %04x - %04x\n", libusb_get_bus_number(dev), libusb_get_device_address(dev), desc.idVendor, desc.idProduct);
}

int main(int argc, char *argv[])
{
    printf("USB Test code - places USB port in XHCI test mode\n");
    
    libusb_init(NULL);
    
    libusb_device **devs;

    int num_devs = libusb_get_device_list(NULL, &devs);
    
	if (num_devs < 0)
	{
	    printf("No devices found\n");
		libusb_exit(NULL);
		return 1;
	}

	for (int i = 0; devs[i]; i++)
	{
	    struct libusb_device_descriptor desc;
    	//dump_device(devs[i]);
		
	    int ret = libusb_get_device_descriptor(devs[i], &desc);
	
    	if (ret >=0)
    	{
	    	if (desc.bDeviceClass == LIBUSB_CLASS_HUB)
	    	{
	    	    struct libusb_device_handle *devh = NULL;
	    	    
                int rc = libusb_open(devs[i], &devh);
                
                if (rc == 0)
                {
                    if (libusb_kernel_driver_active (devh, 0))
                    {
                        printf("Detaching kernel driver\n");
                        libusb_detach_kernel_driver	(devh, 0);
	                }

                    for (int port = 0; port < NUM_PORTS_TO_CHECK; port++)
                    {
    	    	        rc = libusb_control_transfer(devh,
                                LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_OTHER,
                                LIBUSB_REQUEST_SET_FEATURE, USB_PORT_FEAT_TEST,
                                port, NULL, 0, USB_CTRL_GET_TIMEOUT );

                        if (rc == 0)                                
                            printf("Successfully set TEST on hub %d:%d port %d\n", libusb_get_bus_number(devs[i]),
                                     libusb_get_device_address(devs[i]), port);        
                        
                    }
                    
                    libusb_close(devh);
                }
                else
                    printf("Failed to open device err %s. If this is an access error, please run using 'sudo %s'\n", libusb_error_name(rc), argv[0]);
	    	}
        }
    }     
	
	
	libusb_free_device_list(devs, 1);
	
	libusb_exit(NULL);
	
	
    return 0;
}