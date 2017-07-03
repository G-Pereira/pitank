#ifndef USB_DEVICEMANAGER_H
#define USB_DEVICEMANAGER_H

#endif //USB_DEVICEMANAGER_H

#include <iostream>
#include <vector>
#include "libusb/libusb.h"

typedef struct{
    uint16_t vendor;
    uint16_t product;
} SupportedController;

typedef struct {
    uint16_t vendor;
    uint16_t product;
    libusb_device *usbDevice = NULL;
    libusb_device_handle *handler = NULL;

} Controller;

class DeviceManager {
public:
    std::vector<Controller> controllers;

    void init();

    // Search, filter and get control of controllers
    void getDevices();

    // Add trusted controllers to filter usb devices
    void addControllerType(uint16_t vendorId, uint16_t productId);

    // Release memory
    void unload();

private:
    std::vector<SupportedController> supportedControllers;
    libusb_device **devices;
    libusb_context *context = NULL;

    ssize_t list;
    size_t i;

    // Add new or reconected controllers
    int addController(uint16_t vendorId, uint16_t productId, libusb_device *device, libusb_device_handle *handler);
};