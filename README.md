# usbtest

Quick and dirty application to try and put Pi4 USB hubs in to XHCI test mode

## Prerequisites

sudo apt install libusb-1.0-0-dev

## Build

gcc -lusb -I/usr/include/libusb-1.0 -o usbtest usbtest.c -lusb-1.0
