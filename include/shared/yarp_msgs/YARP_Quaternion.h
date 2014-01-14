/* 
 *  YARP Implementation of the ros message geometry_msgs/Point
 *
 *  email: v.varricchio@gmail.com 
 */

#ifndef YARPMSG_TYPE_geometry_msgs_Quaternion
#define YARPMSG_TYPE_geometry_msgs_Quaternion

#include <string>
#include <yarp/os/Portable.h>

class YARP_Quaternion : public yarp::os::Portable{
public:
  double x,y,z,w;
  
  yarp::os::ConstString getTypeName() const {
    return "YARP_Point";
  }

  YARP_Quaternion(): x(0), y(0), z(0), w(0){
  };
 
  bool read(yarp::os::ConnectionReader& connection) {
    x = connection.expectDouble();
    y = connection.expectDouble();
    z = connection.expectDouble();
    w = connection.expectDouble();   
    
    return !connection.isError();
  }

  bool write(yarp::os::ConnectionWriter& connection) {
    connection.appendDouble(x);
    connection.appendDouble(y);
    connection.appendDouble(z);
    connection.appendDouble(w);
    
    return !connection.isError();
  }
};

#endif
