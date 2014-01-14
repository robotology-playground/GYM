/* 
 *  Grabber for YARP_PointCloud2 messages with generic pcl::PointCloud<T> output. 
 *
 *  email: v.varricchio@gmail.com 
 */

#include <shared/yarp_msgs/YARP_PointCloud2.h>
#include "YARP_grabber.hpp"

template <class PCL_PointCloud_t>
class YARP_PointCloud2_grabber : public yarp_msg_grabber<YARP_PointCloud2, PCL_PointCloud_t>  {
         
public: 
  void update_data(YARP_PointCloud2& msg){
    pcl::fromPCLPointCloud2(msg.getPointCloud2(), this->data);
  }
};