#include "deviceManager.h"
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <bitset>

/*###############    CONFIGURATION   ###############*/
#define NUMBER_OF_CONTROLLERS 4
/*##################################################*/

int main(int argc, char **argv){
    ros::init(argc, argv, "pitank_controllers");
    ros::NodeHandle nh;
    geometry_msgs::Twist CmdVel[NUMBER_OF_CONTROLLERS];
    ros::Publisher CmdVel_pub[NUMBER_OF_CONTROLLERS];
    unsigned char USBbuffer[9];

    // Helper class
    DeviceManager deviceManager;
    deviceManager.init();

    // Register vendorId and productId of the controllers 
    deviceManager.addControllerType(0x0079, 0x0006);
    deviceManager.addControllerType(0x2563, 0x0523);
    // Connect controllers
    deviceManager.getDevices();

    CmdVel_pub[0] = nh.advertise<geometry_msgs::Twist>("/cmd_vel1", 1);
    CmdVel_pub[1] = nh.advertise<geometry_msgs::Twist>("/cmd_vel2", 1);
    CmdVel_pub[2] = nh.advertise<geometry_msgs::Twist>("/cmd_vel3", 1);
    CmdVel_pub[3] = nh.advertise<geometry_msgs::Twist>("/cmd_vel4", 1);

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
                    CmdVel[i].linear.x 	= 0;
                    CmdVel[i].linear.z 	= 0;
                    CmdVel[i].angular.z	= 0;

                    int bitsReceived;
                    // Read the controllers' buffer
                    libusb_bulk_transfer(deviceManager.controllers[i].handler,0x81,USBbuffer,8,&bitsReceived,1000);
                    // If empty data was received send 0 too (initial values)
                    if(bitsReceived == 0){
                        CmdVel_pub[i].publish(CmdVel[i]);
                        continue;
                    }

                    // See data that is being received
                    printf("controller %d = ", i+1);

                    for (int j = 0; j < 8; j++)
                        std::cout << std::bitset<8>(USBbuffer[j]) << " ";
                    std::cout << std::endl;

                    // Release the controller
                    libusb_release_interface(deviceManager.controllers[i].handler, 0);
                }
            }
        }
        ros::spinOnce();
        loop_rate.sleep();
    }

    deviceManager.unload();

    return 0;
}