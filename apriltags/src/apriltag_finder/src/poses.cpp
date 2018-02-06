#include "apriltag_finder/poses.h"

void posesProcessing::init_variables() {
  arrow = visualization_msgs::Marker::ARROW;
  nFrames = 0;
  numberTags = 0;
  init = 0;
  timerFlag = 0;
  dAngle = 0;
  dPx = 0;
  dPy = 0;
  dPz = 0;
  avAngle = 0;
  avPx = 0;
  avPy = 0;
  avPz = 0;
  timer = 0;
}

int posesProcessing::verifyAddedTag() {

  if(numberTags > totalAngle.size()) {
    csvFile.open("../posesMeasurements.csv", ios_base::out | ios_base::app);

    csvFile << "\n\n\nTag aditioned - Reseting all the parameters\n\n\n";

    if(!csvFile.is_open())
      ROS_INFO("ERROR OPENING THE FILE\n");

    csvFile.close();
    return 1;
  }
  else
    return 0;
}

int posesProcessing::verifyRemovedTag() {

  if(numberTags < totalAngle.size()) {
    csvFile.open("../posesMeasurements.csv", ios_base::out | ios_base::app);

    csvFile << "\n\n\nTag removed - Reseting all the parameters\n\n\n";

    if(!csvFile.is_open())
      ROS_INFO("ERROR OPENING THE FILE\n");

    csvFile.close();
    return 1;
  }
  else
    return 0;
}

void posesProcessing::clearMaps() {
  totalAngle.clear();
  totalPx.clear();
  totalPy.clear();
  totalPz.clear();
}

void posesProcessing::resetParameters() {
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

void posesProcessing::markerType() {
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

void posesProcessing::writeFile(float x, float y, float z, int id) {
  float rate = nFrames/timer;

  if(!init) {
    remove("../posesMeasurements.csv");
    remove("../posesMeasurementsFoo.csv");
  }

  csvFile.open("../posesMeasurements.csv", ios_base::out | ios_base::app);
  csvFoo.open("../posesMeasurementsFoo.csv", ios_base::out | ios_base::app);

  csvFile << ",,--> Tag id: [" << id << "],--> Frame nr: [" << nFrames <<"]," << "Average frames/sec: [" << rate << "]\n\n";
  csvFile << ",,Position, Aveage, Error\n\n";
  csvFile << ",x," << x << "," << avPx << "," <<  dPx << "\n";
  csvFile << ",y," << y << "," << avPy << "," <<  dPy << "\n";
  csvFile << ",z," << z << "," << avPz << "," <<  dPz << "\n\n\n";
  csvFile << ",,Angle, Average, Error\n\n";
  csvFile << ",," << angle << "," << avAngle << "," << dAngle << "\n\n,------,----------,----------,----------\n\n\n";
  csvFoo  << id << "," << x << "," << y << "," << z << "," << dPx << "," << dPy << "," << dPz << "," << angle << "," << abs(dAngle) << "\n";

  if(!csvFile.is_open())
    ROS_INFO("ERROR OPENING CSV FILE\n");
  if(!csvFoo.is_open())
    ROS_INFO("ERROR OPENING Foo FILE\n");

  csvFile.close();
  csvFoo.close();
  init += 1;
}
