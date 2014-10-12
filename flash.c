#include <stdio.h>

#include "libusb.h"


static const int INTERFACE_NUMBER = 0;

static const unsigned char RED = 0x02;
static const unsigned char OFF = 0x00;

int main(int argc, char *argv[]) {
    if (argc != 2 || (strcmp(argv[1], "on") != 0 && strcmp(argv[1], "off"))) {
        fprintf(stderr, "Usage: warning-light [on | off]\n");
        return 1;
    }

    int r;

    r = libusb_init(NULL);
    if (r < 0) {
        return r;
    }


    libusb_device_handle *devh = libusb_open_device_with_vid_pid(NULL, 0x0fc5, 0xb080);

    if (devh != NULL) {
        libusb_detach_kernel_driver(devh, INTERFACE_NUMBER);
        r = libusb_claim_interface(devh, INTERFACE_NUMBER);


        if (r >= 0) {
            unsigned char command = strcmp("on", argv[1]) == 0 ? RED : OFF;

            unsigned char buffer[] = { 
                0x65, // 8 byte packet size
                0x0C, // Command type
                command, // DataLSB - Command parameter
                0xFF, // DataMSB - Command parameter
                0x00, // HID, first byte
                0x00, // HID, second byte
                0x00, // HID, third byte
                0x00  // HID, fourth byte
            };

            libusb_control_transfer (
                devh,
                0x21, // bmRequestType (host to device)
                0x09, // bRequest (set configuration)
                0x0635, // wValue (configuration value)
                0x000, // wIndex
                buffer, // Data / message
                sizeof(buffer), // wLength, Number of bytes to be sent
                0 // Timeout
            );

            libusb_release_interface(devh, 0);
        } else {
            fprintf(stderr, "libusb_claim_interface error %d\n", r);
        }

        libusb_close(devh);
    } else {
        fprintf(stderr, "No device\n");
    }

    libusb_exit(NULL);
    return 0;
}
