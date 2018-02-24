#include "deviceManager.h"
#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <bitset>

int main(int argc, char **argv){
    ros::init(argc, argv, "joy_raw");
    ros::NodeHandle nh;
    sensor_msgs::Joy msgJoy[4];
    ros::Publisher pubJoy[4];
    unsigned char USBbuffer[9];

    // Helper class
    DeviceManager deviceManager;
    deviceManager.init();

    // Register vendorId and productId of the controllers 
    deviceManager.addControllerType(0x0079, 0x0006);
    deviceManager.addControllerType(0x2563, 0x0523);
    // Connect controllers
    deviceManager.getDevices();

    pubJoy[0] = nh.advertise<sensor_msgs::Joy>("joy_raw1", 1);
    pubJoy[1] = nh.advertise<sensor_msgs::Joy>("joy_raw2", 1);
    pubJoy[2] = nh.advertise<sensor_msgs::Joy>("joy_raw3", 1);
    pubJoy[3] = nh.advertise<sensor_msgs::Joy>("joy_raw4", 1);

    ros::Rate loop_rate(50);

    while(ros::ok()){
        // Go through every controller and read its data
        for(int i = 0; i < deviceManager.controllers.size();i++){
            // Check if there was any error opening the device
            // If so search again and attempt to reconnect
            if (deviceManager.controllers[i].handler==NULL){
                deviceManager.getDevices();
            } else{
                // Take control of the controller
                // If there was an error release it and search and connect again
                if(libusb_claim_interface(deviceManager.controllers[i].handler, 0) != 0){
                    deviceManager.controllers[i].handler=NULL;
                    deviceManager.getDevices();
                } else{
                    // Initialize values to return
                    msgJoy[i].axes.push_back(0.0); // left x
                    msgJoy[i].axes.push_back(0.0); // left y
                    msgJoy[i].axes.push_back(0.0); // right x
                    msgJoy[i].axes.push_back(0.0); // right y

                    int bitsReceived;
                    // Read the controllers' buffer
                    libusb_bulk_transfer(deviceManager.controllers[i].handler,0x81,USBbuffer,8,&bitsReceived,1000);
                    // If empty data was received send 0 too (initial values)
                    if(bitsReceived == 0){
                        pubJoy[i].publish(msgJoy[i]);
                        continue;
                    }

                    // See data that is being received
                    printf("controller %d = ", i+1);

                    int j,k;

                    for(j = 0; j < 4; k++){
                        msgJoy[i].axes.push_back((float)USBbuffer[j]/255.0);
                    }

                    // Read buttons
                    for(int j = 5; k < 8; k++){
                        msgJoy[i].buttons.push_back(USBbuffer[k]/128==1);
                        msgJoy[i].buttons.push_back(USBbuffer[k]/64==1);
                        msgJoy[i].buttons.push_back(USBbuffer[k]/32==1);
                        msgJoy[i].buttons.push_back(USBbuffer[k]/16==1);
                        msgJoy[i].buttons.push_back(USBbuffer[k]/8==1);
                        msgJoy[i].buttons.push_back(USBbuffer[k]/4==1);
                        msgJoy[i].buttons.push_back(USBbuffer[k]/2==1);
                        msgJoy[i].buttons.push_back(USBbuffer[k]/1==1);
                    }

                    // Send commands from joystick
                    pubJoy[i].publish(msgJoy[i]);

                    // Release the controller
                    libusb_release_interface(deviceManager.controllers[i].handler, 0);
                    
                    for (j = 0; j < 8; j++)
                        std::cout << std::bitset<8>(USBbuffer[j]) << " ";
                    std::cout << std::endl;
                    for (j = 5; j < 8; j++)
                        std::cout << (int)(USBbuffer[j]/255==1) << " ";
                    std::cout << std::endl;
                    std::cout << std::endl;
                }
            }
        }
        ros::spinOnce();
        loop_rate.sleep();
    }

    deviceManager.unload();

    return 0;
}