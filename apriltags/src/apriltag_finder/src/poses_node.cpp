#include <ros/ros.h>
#include "apriltag_finder/poses.h"

void posesProcessing::posesDetection(const apriltags_ros::AprilTagDetectionArray& msg) {
  if(timer < 60 && !timerFlag) {

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
  }
}

void posesProcessing::timerCallback(const ros::TimerEvent&) {

  timer+= 1;

  if(timer == 60) {
    ROS_INFO("Tags rotation in progress - 30 seconds! \n");

    csvFile.open("../posesMeasurements.csv", ios_base::out | ios_base::app);
    csvFile << "\n\nTags rotation in progress - 30 seconds! \n";
    if(!csvFile.is_open())
      ROS_INFO("ERROR OPENING THE FILE\n");
    csvFile.close();

    clearMaps();
    timerFlag = 1;
    timer = 0;
  }

  if(timer == 30 && timerFlag) {
    timer = 0;
    timerFlag = 0;
  }
}


int main(int argc, char **argv) {
  posesProcessing poses;
  ros::init(argc, argv, "poses");
  ros::NodeHandle nh;

  poses.init_variables();

  ros::Timer rosTimer = nh.createTimer(ros::Duration(1.0), &posesProcessing::timerCallback, &poses);

  ros::Subscriber sub = nh.subscribe("tag_detections", 1, &posesProcessing::posesDetection, &poses);
  poses.markerPub = nh.advertise<visualization_msgs::MarkerArray>("visualization_marker", 1);

  ros::spin();

  return 0;
}
