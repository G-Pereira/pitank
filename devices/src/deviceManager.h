#ifndef USB_DEVICEMANAGER_H
#define USB_DEVICEMANAGER_H

#endif //USB_DEVICEMANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include "libusb/libusb.h"

/*###############    CONFIGURATION   ###############*/
#define MAX_CONTROLLERS 4
/*##################################################*/

typedef struct {
    uint16_t vendor;
    uint16_t product;
    libusb_device *usbDevice;
    libusb_device_handle *handler = NULL;

} controller;

class DeviceManager {
public:
    void init();

    void addDevice(uint16_t vendorId, uint16_t productId);

    void startDevices();

    void unload();

private:
    controller controllers[MAX_CONTROLLERS];
    int numberOfDevices = 0;
    libusb_device **devices;
    libusb_context *context = NULL;

    ssize_t list;
    size_t i;
};