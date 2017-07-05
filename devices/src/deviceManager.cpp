#include "deviceManager.h"

void DeviceManager::init() {
    if (libusb_init(&context) < 0) {
        fprintf(stderr, "Error Initializing USB");
        exit(1);
    }
}

// Search, filter and get control of controllers
void DeviceManager::getDevices() {
    // List devices
    list = libusb_get_device_list(context, &devices);

    if (list < 0) {
        fprintf(stderr, "Error getting list of USB devices");
        // Release memory associated to the device list 
        libusb_free_device_list(devices, 1);
        libusb_exit(context);
        exit(1);
    }

    // Go through the list of usb devices
    for (i = 0; i < list; i++) {
        // Get information about the usb device
        struct libusb_device_descriptor desc;
        if (libusb_get_device_descriptor(devices[i], &desc) >= 0) {
            // Go through the list of know controllers to filter usb devices
            for (int j = 0; j < supportedControllers.size(); j++) {
                // Check if it maches the vendor and model of the controllers supported
                if (desc.idVendor == supportedControllers[j].vendor && desc.idProduct == supportedControllers[j].product) {
                    // Controller Found, so add it to our controllers
                    int newDeviceIndex = addController(desc.idVendor, desc.idProduct, devices[i], NULL);
                    if(newDeviceIndex == -1){
                        libusb_free_device_list(devices, 1);
                        fprintf(stderr, "You have at least one controller connected above the supportted limit");
                        return;
                    }
                    // Open the controller
                    int open = libusb_open(controllers[newDeviceIndex].usbDevice, &controllers[newDeviceIndex].handler);
                    if (open == 0) {
                        // Take control of the controller
                        if (libusb_kernel_driver_active(controllers[newDeviceIndex].handler, 0) == 1) {
                            if (libusb_detach_kernel_driver(controllers[newDeviceIndex].handler, 0) != 0) {
                                fprintf(stderr,"Failed to take control of the controller number %d\nAre you using it?\n", newDeviceIndex+1);
                            }
                        }
                    } else {
                        // Define the controller as lost
                        controllers[newDeviceIndex].handler=NULL;
                        if (open == LIBUSB_ERROR_ACCESS) {
                            fprintf(stderr, "You don't have permissions to access the controller\n");
                        } else if (open == LIBUSB_ERROR_NO_MEM){
                            fprintf(stderr, "No memory available to allocate controller %d\n", newDeviceIndex+1);
                        } else if (open == LIBUSB_ERROR_NO_DEVICE){
                            fprintf(stderr, "Controller %d has been disconnected\n", newDeviceIndex+1);
                        } else if (open == LIBUSB_ERROR_IO){
                            fprintf(stderr, "Failed to open the controller number %d due to an unknown error\n", newDeviceIndex+1, open);
                        }
                    }
                }
            }
        }
    }
    libusb_free_device_list(devices, 1);
    // List lost devices
    for(int i = 0; i< controllers.size(); i++){
        if(controllers[i].handler == NULL){
            printf("Joystick %d disconnected\n", i+1);
        }
    }
}

// Add new or reconected controllers
int DeviceManager::addController(uint16_t vendorId, uint16_t productId, libusb_device *device, libusb_device_handle *handler) {
    if (controllers.size() == 4){
        return -1;
    }
    Controller newController;
    newController.vendor = vendorId;
    newController.product = productId;
    newController.usbDevice = device;
    newController.handler = handler;
    // In case a device is lost, when reconnecting put it back in the same place
    // Might not always return to the same place, in case more than one controllers are reconnecting
    for(int i = 0; i< controllers.size(); i++){
        if(controllers[i].handler == NULL){
            controllers[i]= newController;
            return i;
        }
    }
    // If everything is in place add it to the end of the list
    controllers.push_back(newController);
    return controllers.size()-1;
}

// Add trusted controllers to filter usb devices
void DeviceManager::addControllerType(uint16_t vendorId, uint16_t productId) {
    SupportedController newControllerType;
    newControllerType.vendor = vendorId;
    newControllerType.product = productId;
    supportedControllers.push_back(newControllerType);
}

// Release memory
void DeviceManager::unload() {
    libusb_exit(context);
}