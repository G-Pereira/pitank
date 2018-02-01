#include "ros/ros.h"
#include "geometry_msgs/PoseArray.h"
#include "geometry_msgs/Pose.h"
#include "ros/publisher.h"
#include "visualization_msgs/Marker.h"
#include "visualization_msgs/MarkerArray.h"
#include "apriltags_ros/AprilTagDetection.h"
#include "apriltags_ros/AprilTagDetectionArray.h"
#include "vector"
#include "fstream"
#include "map"
#include "stdio.h"

#define PI 3.14159265358979323846

using namespace std;

ros::Publisher markerPub;
uint32_t arrow = visualization_msgs::Marker::ARROW;
int nFrames = 0, numberTags = 0, init = 0, timerFlag = 0;
float angle, dAngle = 0, dPx = 0, dPy = 0, dPz = 0, avAngle = 0, avPx = 0, avPy = 0, avPz = 0, timer = 0;
visualization_msgs::MarkerArray array;
visualization_msgs::Marker marker;
map<int,float> totalAngle, totalPx, totalPy, totalPz;
map<int,float>::iterator it;

class posesProcessing {
public:
  apriltags_ros::AprilTagDetection tagInfo;
  ofstream csvFile, csvFoo;
  void posesDetection(const apriltags_ros::AprilTagDetectionArray& msg);
};

int verifyAddedTag() {
  posesProcessing poses;

  if(numberTags > totalAngle.size()) {
    poses.csvFile.open("../posesMeasurements.csv", ios_base::out | ios_base::app);

    poses.csvFile << "\n\n\nTag aditioned - Reseting all the parameters\n\n\n";

    if(!poses.csvFile.is_open())
      ROS_INFO("ERROR OPENING THE FILE\n");

    poses.csvFile.close();
    return 1;
  }
  else
    return 0;
}

int verifyRemovedTag() {
  posesProcessing poses;

  if(numberTags < totalAngle.size()) {
    poses.csvFile.open("../posesMeasurements.csv", ios_base::out | ios_base::app);

    poses.csvFile << "\n\n\nTag removed - Reseting all the parameters\n\n\n";

    if(!poses.csvFile.is_open())
      ROS_INFO("ERROR OPENING THE FILE\n");

    poses.csvFile.close();
    return 1;
  }
  else
    return 0;
}

void clearMaps() {
  totalAngle.clear();
  totalPx.clear();
  totalPy.clear();
  totalPz.clear();
}

void resetParameters() {
  for(it = totalAngle.begin(); it != totalAngle.end(); ++it)
    totalAngle[it->first] = 0;
  for(it = totalPx.begin(); it != totalPx.end(); ++it)
    totalPx[it->first] = 0;
  for(it = totalPy.begin(); it != totalPy.end(); ++it)
    totalPy[it->first] = 0;
  for(it = totalPz.begin(); it != totalPz.end(); ++it)
    totalPz[it->first] = 0;
  nFrames = 0;
}

void markerType() {
  marker.scale.x = 0.5f;
  marker.scale.y = 0.2f;
  marker.scale.z = 0.2f;

  marker.color.r = 1.0f;
  marker.color.g = 0.0f;
  marker.color.b = 0.0f;
  marker.color.a = 1.0;

  marker.lifetime = ros::Duration();
  array.markers.push_back(marker);
}

void writeFile(float x, float y, float z, int id) {
  posesProcessing poses;
  float rate = nFrames/timer;

  if(!init) {
    remove("../posesMeasurements.csv");
    remove("../posesMeasurementsFoo.csv");
  }

  poses.csvFile.open("../posesMeasurements.csv", ios_base::out | ios_base::app);
  poses.csvFoo.open("../posesMeasurementsFoo.csv", ios_base::out | ios_base::app);

  poses.csvFile << ",,--> Tag id: [" << id << "],--> Frame nr: [" << nFrames <<"]," << "Average frames/sec: [" << rate << "]\n\n";
  poses.csvFile << ",,Position, Aveage, Error\n\n";
  poses.csvFile << ",x," << x << "," << avPx << "," <<  dPx << "\n";
  poses.csvFile << ",y," << y << "," << avPy << "," <<  dPy << "\n";
  poses.csvFile << ",z," << z << "," << avPz << "," <<  dPz << "\n\n\n";
  poses.csvFile << ",,Angle, Average, Error\n\n";
  poses.csvFile << ",," << angle << "," << avAngle << "," << dAngle << "\n\n,------,----------,----------,----------\n\n\n";
  poses.csvFoo  << id << "," << x << "," << y << "," << z << "," << dPx << "," << dPy << "," << dPz << "," << angle << "," << abs(dAngle) << "\n";

  if(!poses.csvFile.is_open())
    ROS_INFO("ERROR OPENING CSV FILE\n");
  if(!poses.csvFoo.is_open())
    ROS_INFO("ERROR OPENING Foo FILE\n");

  poses.csvFile.close();
  poses.csvFoo.close();
  init += 1;
}

void posesProcessing::posesDetection(const apriltags_ros::AprilTagDetectionArray& msg) {
 // if(timer < 60 && !timerFlag) {

    numberTags = msg.detections.size();

    if(verifyAddedTag())
      resetParameters();

    if(verifyRemovedTag()) {
      resetParameters();
      clearMaps();
    }

    if(numberTags)
      nFrames += 1;

    ROS_INFO("\n-\n");
    ROS_INFO("Measurement number: [%d]\n", nFrames);

    for(int i = 0; i < msg.detections.size(); i++) {
      tagInfo = msg.detections[i];

      //angle, averages and erros calculation
      angle = atan2(2 * (tagInfo.pose.pose.orientation.x * tagInfo.pose.pose.orientation.y + tagInfo.pose.pose.orientation.z * tagInfo.pose.pose.orientation.w),1-2*(tagInfo.pose.pose.orientation.y * tagInfo.pose.pose.orientation.y + tagInfo.pose.pose.orientation.z * tagInfo.pose.pose.orientation.z)) * 180 / PI;

      if(totalAngle.find(tagInfo.id) == totalAngle.end()) {
        totalAngle[tagInfo.id] = angle;
        totalPx[tagInfo.id] = tagInfo.pose.pose.position.x;
        totalPy[tagInfo.id] = tagInfo.pose.pose.position.y;
        totalPz[tagInfo.id] = tagInfo.pose.pose.position.z;
      }
      else {
        totalAngle[tagInfo.id] += angle;
        totalPx[tagInfo.id] += tagInfo.pose.pose.position.x;
        totalPy[tagInfo.id] += tagInfo.pose.pose.position.y;
        totalPz[tagInfo.id] += tagInfo.pose.pose.position.z;
      }

      avAngle = totalAngle[tagInfo.id] / nFrames;
      avPx = totalPx[tagInfo.id] / nFrames;
      avPy = totalPy[tagInfo.id] / nFrames;
      avPz = totalPz[tagInfo.id] / nFrames;
      dAngle = avAngle - angle;
      dPx = avPx - tagInfo.pose.pose.position.x;
      dPy = avPy - tagInfo.pose.pose.position.y;
      dPz = avPz - tagInfo.pose.pose.position.z;

      //display results
      ROS_INFO("Tag id: [%d]\n", tagInfo.id);
      ROS_INFO("\nPosition:       Average:       Error:\nx: [%f]     [%f]     [%f]\ny: [%f]     [%f]     [%f]\nz: [%f]     [%f]     [%f]\n\n", tagInfo.pose.pose.position.x, avPx, dPx, tagInfo.pose.pose.position.y, avPy, dPy, tagInfo.pose.pose.position.z, avPz, dPz);
      ROS_INFO("\nangle:          Average:       Error:\n[%f]     [%f]     [%f]\n", angle, avAngle, dAngle);

      //write results to a file
      writeFile(tagInfo.pose.pose.position.x,tagInfo.pose.pose.position.y,tagInfo.pose.pose.position.z, tagInfo.id);

      //set markers positions/angle to display on rviz
      marker.header.frame_id = "camera";
      marker.header.stamp = tagInfo.pose.header.stamp;

      marker.ns = "poses";
      marker.id = i;
      marker.type = arrow;
      marker.action = visualization_msgs::Marker::ADD;

      marker.pose.position.x = tagInfo.pose.pose.position.x;
      marker.pose.position.y = tagInfo.pose.pose.position.y;
      marker.pose.position.z = tagInfo.pose.pose.position.z;
      marker.pose.orientation.x = tagInfo.pose.pose.orientation.x;
      marker.pose.orientation.y = tagInfo.pose.pose.orientation.y;
      marker.pose.orientation.z = tagInfo.pose.pose.orientation.z;
      marker.pose.orientation.w = tagInfo.pose.pose.orientation.w;

      markerType();
    }

    ROS_INFO("\n---\n");

    //publish markers on rviz
    if(markerPub.getNumSubscribers() > 0)
      markerPub.publish(array);
  //}
}

void timerCallback(const ros::TimerEvent&) {
  posesProcessing poses;

  timer+= 1;

  /*if(timer == 60) {
    ROS_INFO("Tags rotation in progress - 30 seconds! \n");

    poses.csvFile.open("../posesMeasurements.csv", ios_base::out | ios_base::app);
    poses.csvFile << "\n\nTags rotation in progress - 30 seconds! \n";
    if(!poses.csvFile.is_open())
      ROS_INFO("ERROR OPENING THE FILE\n");
    poses.csvFile.close();

    clearMaps();
    timerFlag = 1;
    timer = 0;
  }

  if(timer == 30 && timerFlag) {
    timer = 0;
    timerFlag = 0;
  }*/
}

int main(int argc, char **argv) {
  posesProcessing poses;
  ros::init(argc, argv, "poses");
  ros::NodeHandle nh;

  ros::Timer rosTimer = nh.createTimer(ros::Duration(1.0), timerCallback);

  ros::Subscriber sub = nh.subscribe("tag_detections", 1, &posesProcessing::posesDetection, &poses);
  markerPub = nh.advertise<visualization_msgs::MarkerArray>("visualization_marker", 1);

  ros::spin();

  return 0;
}
