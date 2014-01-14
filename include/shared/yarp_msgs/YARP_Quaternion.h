/* 
 *  YARP Implementation of the ros message geometry_msgs/Quaternion
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
    return "YARP_Quaternion";
  }

  YARP_Quaternion(): x(0), y(0), z(0), w(1){
  };

  YARP_Quaternion(const double& x_, const double& y_, const double& z_, const double& w_): x(x_), y(y_), z(z_), w(w_){
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
