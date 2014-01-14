/* 
 *  YARP Implementation of the ros message geometry_msgs/Point
 *
 *  email: v.varricchio@gmail.com 
 */

#ifndef YARPMSG_TYPE_geometry_msgs_Point
#define YARPMSG_TYPE_geometry_msgs_Point

#include <string>
#include <yarp/os/Portable.h>

class YARP_Point : public yarp::os::Portable{
public:
  double x,y,z;
  
  yarp::os::ConstString getTypeName() const {
    return "YARP_Point";
  }

  YARP_Point(): x(0), y(0), z(0){
  };

  YARP_Point(const double& x_, const double& y_, const double& z_): x(x_), y(y_), z(z_){
  };
 
  bool read(yarp::os::ConnectionReader& connection) {
    x = connection.expectDouble();
    y = connection.expectDouble();
    z = connection.expectDouble();
    
    return !connection.isError();
  }

  bool write(yarp::os::ConnectionWriter& connection) {
    connection.appendDouble(x);
    connection.appendDouble(y);
    connection.appendDouble(z);
    
    return !connection.isError();
  }
};

#endif
