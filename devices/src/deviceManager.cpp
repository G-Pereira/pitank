#include "deviceManager.h"

void DeviceManager::init() {
    if (libusb_init(&context) < 0) {
        fprintf(stderr, "Error Initializing USB");
        exit(1);
    }
}

void DeviceManager::startDevices() {
    list = libusb_get_device_list(context, &devices);

    if (list < 0) {
        fprintf(stderr, "Error getting list of USB devices");
        libusb_free_device_list(devices, 1);
        libusb_exit(context);
        exit(1);
    }

    // Go through all usb devices
    for (i = 0; i < list; i++) {
        struct libusb_device_descriptor desc;
        if (libusb_get_device_descriptor(devices[i], &desc) >= 0) {
            uint8_t j;
            for (j = 0; j < numberOfDevices; j++) {
                if (desc.idVendor == controllers[j].vendor && desc.idProduct == controllers[j].product) {
                    // Controller Found, so add it to our controllers
                    controllers[j].usbDevice = devices[i];
                    int open = libusb_open(controllers[j].usbDevice, &controllers[j].handler);
                    if (open == 0) {
                        // Take control of the controller
                        if (libusb_kernel_driver_active(controllers[j].handler, 0) == 1) {
                            printf("Joystick!\n");
                            if (libusb_detach_kernel_driver(controllers[j].handler, 0) != 0) {
                                fprintf(stderr,
                                        "Failed to take control of the controller number %d\n"
                                                "Are you using it?\n", j);
                            }
                        } else{
                            printf("Joystick!\n");
                        }
                    } else {
                        if (open == -3) {
                            fprintf(stderr, "You don't have permissions to access the controller\n");
                        } else {
                            fprintf(stderr, "Failed to open the controller number %d\n"
                                    "Maybe it's faulty, please do maintenance\n"
                                    "ERROR CODE: %d", j, open);
                        }
                    }
                }
            }
        }
    }
    libusb_free_device_list(devices, 1);
}

void DeviceManager::unload() {
    libusb_free_device_list(devices, 1);
    libusb_exit(context);
}

void DeviceManager::addDevice(uint16_t vendorId, uint16_t productId) {
    if (numberOfDevices == MAX_CONTROLLERS) {
        fprintf(stderr,
                "Device Manager: You can only add up to %d controllers, you can change that value in the configurations file",
                MAX_CONTROLLERS);
        exit(1);
    } else {
        controllers[numberOfDevices].vendor = vendorId;
        controllers[numberOfDevices].product = productId;
        numberOfDevices++;
    }
}