#include <shared/utils/Pose2D.h>
#include "YARP_grabber.hpp"

class YARP_Pose2D_grabber: public yarp_msg_grabber<yarp::os::Bottle, Pose2D>  {
public: 
  void update_data(yarp::os::Bottle& b){
    this->data.x = b.get(0).asDouble();
    this->data.y = b.get(1).asDouble();
    this->data.yaw = b.get(2).asDouble();
  }
};