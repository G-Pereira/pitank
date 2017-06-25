#ifndef USB_DEVICEMANAGER_H
#define USB_DEVICEMANAGER_H

#endif //USB_DEVICEMANAGER_H

#include <iostream>
#include <vector>
#include "libusb/libusb.h"

/*###############    CONFIGURATION   ###############*/

/*##################################################*/

typedef struct {
    uint16_t vendor;
    uint16_t product;
    libusb_device *usbDevice = NULL;
    libusb_device_handle *handler = NULL;

} controller;

class DeviceManager {
public:
    std::vector<controller> controllers;

    void init();

    void addDevice(uint16_t vendorId, uint16_t productId);

    void getDevices();

    void prepareDevices();

    void releaseDevices();

    void unload();

private:
    libusb_device **devices;
    libusb_context *context = NULL;

    ssize_t list;
    size_t i;
};