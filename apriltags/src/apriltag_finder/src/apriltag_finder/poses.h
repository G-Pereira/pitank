#ifndef POSES_H
#define POSES_H

#endif // POSES_H

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

class posesProcessing {
public:
  apriltags_ros::AprilTagDetection tagInfo;
  ros::Publisher markerPub;

  void posesDetection(const apriltags_ros::AprilTagDetectionArray& msg);
  void timerCallback(const ros::TimerEvent&);

  int verifyAddedTag();
  int verifyRemovedTag();
  void clearMaps();
  void markerType();
  void writeFile(float x, float y, float z, int id);
  void resetParameters();
  void init_variables();

private:
  uint32_t arrow;
  int nFrames, numberTags, init, timerFlag ;
  float angle, dAngle, dPx, dPy, dPz, avAngle, avPx, avPy, avPz, timer;
  visualization_msgs::MarkerArray array;
  visualization_msgs::Marker marker;
  map<int,float> totalAngle, totalPx, totalPy, totalPz;
  map<int,float>::iterator it;
  ofstream csvFile, csvFoo;

};
