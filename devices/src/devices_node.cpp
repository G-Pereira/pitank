#include "deviceManager.h"
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>

int main(int argc, char **argv){
    ros::init(argc, argv, "pitank_devices");
    ros::NodeHandle nh;
    geometry_msgs::Twist CmdVel[4];
    ros::Publisher CmdVel_pub[4];

    DeviceManager device;
    device.init();
    device.addDevice(0x79, 0x06);
    device.addDevice(0x2563, 0x523);
    device.getDevices();

    CmdVel_pub[0] = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 1);
    CmdVel_pub[1] = nh.advertise<geometry_msgs::Twist>("/cmd_vel2", 1);
    CmdVel_pub[2] = nh.advertise<geometry_msgs::Twist>("/cmd_vel3", 1);
    CmdVel_pub[3] = nh.advertise<geometry_msgs::Twist>("/cmd_vel4", 1);

    ros::Rate loop_rate(50);

    while(ros::ok()){
        device.prepareDevices();

        device.releaseDevices();
        ros::spinOnce();
        loop_rate.sleep();
    }

    device.unload();

    return 0;
}