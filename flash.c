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
    libusb_context *ctx;

    r = libusb_init(&ctx);
    if (r < 0) {
        return r;
    }

    libusb_set_debug(ctx, 1); // error messages are printed to stderr
    libusb_device_handle *devh = libusb_open_device_with_vid_pid(ctx, 0x0fc5, 0xb080);

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

            /* http://www.usb.org/developers/hidpage/HID1_11.pdf page 50 */
            libusb_control_transfer (
                devh,
                0x21, // bmRequestType (host to device, type: class, recipient: interface)
                0x09, // bRequest (SET_REPORT - allows the host to send a report to the device, possibly setting the state of input, output, or feature controls)
                0x0635, // wValue (Report Type / ID - Specific to the device)
                0x000, // wIndex (interface)
                buffer, // Data / message
                sizeof(buffer), // wLength (report length)
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

    libusb_exit(ctx);
    return 0;
}
