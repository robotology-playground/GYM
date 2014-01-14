/* 
 *  YARP Implementation of the ros message sensor_msgs/PointCloud2, with 
 *  a conversion method to pcl::PCLPointCloud2. 
 *
 *  freely inspired by http://wiki.icub.org/yarpdoc/yarp_with_ros.html
 *
 *  email: v.varricchio@gmail.com 
 *
 */

#ifndef YARPMSG_TYPE_PointCloud2
#define YARPMSG_TYPE_PointCloud2

#include <string>
#include <vector>
#include <yarp/os/Portable.h>

#include <pcl/io/pcd_io.h>
#include "YARP_Header.h"
#include "YARP_PCLPointField.h"

class YARP_PointCloud2 : public pcl::PCLPointCloud2, public yarp::os::Portable {
public:
  YARP_Header yarp_header;
  std::vector<YARP_PCLPointField> yarp_fields;

  YARP_PointCloud2(){
  }
  
  YARP_PointCloud2(const pcl::PCLPointCloud2& pclpc2): pcl::PCLPointCloud2(pclpc2){
    update_yarp();
  }
  
  pcl::PCLPointCloud2& getPointCloud2(){
    return *(static_cast<pcl::PCLPointCloud2*>(this));
  }
  
  void update_yarp(){
    // yarp objects from pcl ones
    // *** header ***
    yarp_header = YARP_Header(header);	
    // *** fields ***
    uint len = fields.size();
    yarp_fields.resize(len);
    for (uint i=0; i<len; i++) {
      yarp_fields[i] = YARP_PCLPointField(fields[i]);
    }
  }
  
  void update_pcl(){
    // pcl objects from yarp ones
    // *** header ***
    header = *(static_cast<pcl::PCLHeader*>(&yarp_header));
    // *** fields ***
    uint len = yarp_fields.size();
    fields.resize(len);
    for (uint i=0; i<len; i++) {
      fields[i] = *(static_cast<pcl::PCLPointField*>(&(yarp_fields[i])));
    }	
  }
  
  bool read(yarp::os::ConnectionReader& connection) {
    // *** yarp_header ***
    if (!yarp_header.read(connection)) return false;
    
    // *** height ***
    height = connection.expectInt();

    // *** width ***
    width = connection.expectInt();

    // *** fields ***
    int len = connection.expectInt();
    yarp_fields.resize(len);
    for (int i=0; i<len; i++) {
      if (!yarp_fields[i].read(connection)) return false;
    }	
    
    // *** is_bigendian ***
    if (!connection.expectBlock((char*)&is_bigendian,1)) return false;

    // *** point_step ***
    point_step = connection.expectInt();

    // *** row_step ***
    row_step = connection.expectInt();

    // *** data ***
    len = connection.expectInt();
    data.resize(len);
    if (!connection.expectBlock((char*)&data[0],sizeof(char)*len)) return false;

    // *** is_dense ***
    if (!connection.expectBlock((char*)&is_dense,1)) return false;
    update_pcl();
    return !connection.isError();
  }

  bool write(yarp::os::ConnectionWriter& connection) {  
    update_yarp();
    
    // *** yarp_header ***
    if (!yarp_header.write(connection)) return false;

    // *** height ***
    connection.appendInt(height);

    // *** width ***
    connection.appendInt(width);

    // *** fields ***
    connection.appendInt(yarp_fields.size());
    for (int i=0; i<yarp_fields.size(); i++) {
      if (!yarp_fields[i].write(connection)) return false;
    }

    // *** is_bigendian ***
    const char isb(is_bigendian);
    connection.appendBlock(&isb,1);

    // *** point_step ***
    connection.appendInt(point_step);

    // *** row_step ***
    connection.appendInt(row_step);

    // *** data ***
    connection.appendInt(data.size());
    connection.appendExternalBlock((char*)&data[0],sizeof(char)*data.size());

    // *** is_dense ***
    const char isd(is_dense);
    connection.appendBlock(&isd,1);
    return !connection.isError();
  }
};

#endif