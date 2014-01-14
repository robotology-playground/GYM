#ifndef YARP_POSE2D
#define YARP_POSE2D

#include <math.h>
#include <vector>
#include <stdio.h>

class Pose2D{

public:

  double x;
  double y;
  double yaw;
  
  Pose2D():x(0), y(0), yaw(0){
  }
  
  Pose2D(double x_, double y_, double yaw_):x(x_), y(y_), yaw(yaw_){
  }
};

#endif