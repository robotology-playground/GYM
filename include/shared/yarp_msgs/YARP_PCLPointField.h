#ifndef YARPMSG_TYPE_PCLPointField
#define YARPMSG_TYPE_PCLPointField

#include <string>
#include <vector>
#include <yarp/os/Portable.h>

#include <pcl/io/pcd_io.h>

class YARP_PCLPointField : public pcl::PCLPointField, public yarp::os::Portable {
public:
  YARP_PCLPointField(){
  };
  
  YARP_PCLPointField(const pcl::PCLPointField& pclpf): pcl::PCLPointField(pclpf){
  }
  
  bool read(yarp::os::ConnectionReader& connection) {
    // *** name ***
    int len = connection.expectInt();
    name.resize(len);
    if (!connection.expectBlock((char*)name.c_str(),len)) return false;
    
    // *** offset ***
    offset = connection.expectInt();

    // *** datatype ***
    datatype = connection.expectInt();

    // *** count ***
    count = connection.expectInt();
    
    return !connection.isError();
  }

  bool write(yarp::os::ConnectionWriter& connection) {
    // *** name ***
    connection.appendInt(name.length());
    connection.appendExternalBlock((char*)name.c_str(),name.length());
    
    // *** offset ***
    connection.appendInt(offset);

    // *** datatype ***
    connection.appendInt(datatype);

    // *** count ***
    connection.appendInt(count);
    
    return !connection.isError();
  }
};

#endif