/* 
 *  YARP Implementation of the ros message std_msgs/Header, with 
 *  a conversion method to pcl::PCLHeader. 
 *
 *  freely inspired by http://wiki.icub.org/yarpdoc/yarp_with_ros.html
 *
 *  email: v.varricchio@gmail.com 
 */

#ifndef YARPMSG_TYPE_std_msgs_Header
#define YARPMSG_TYPE_std_msgs_Header

#include <string>
#include <vector>
#include <yarp/os/Portable.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/NetUint32.h>
#include "TickTime.h"
#include <pcl/PCLHeader.h>

class YARP_Header : public yarp::os::Portable, public pcl::PCLHeader {
public:
  yarp::os::ConstString getTypeName() const {
    return "YARP_Header";
  }

  yarp::os::NetUint32 yarp_seq;
  TickTime yarp_stamp;
  //std::string frame_id; // already defined in pcl::PCLHeader

  YARP_Header(){
  };
  
  YARP_Header(const pcl::PCLHeader& pclh): pcl::PCLHeader(pclh){
    update_yarp();    
  }
  
  /*YARP_Header(const YARP_Header& pclh): pcl::PCLHeader(pclh){
    update_yarp();    
  }*/
  
  void update_yarp(){
    // yarp objects from pcl ones
    yarp_seq = seq;
    yarp_stamp.nsec = stamp%1000000000;
    yarp_stamp.sec = stamp/1000000000;
  }
  
  void update_pcl(){
    // pcl objects from yarp ones
    seq = yarp_seq;
    stamp = yarp_stamp.nsec+yarp_stamp.sec*1000000000; // I'm assuming that nsec and sec mean freaking nanoseconds and seconds
  }
  
  bool read(yarp::os::ConnectionReader& connection) {
    // *** yarp_seq ***
    yarp_seq = connection.expectInt();

    // *** yarp_stamp ***
    if (!yarp_stamp.read(connection)) return false;

    // *** frame_id ***
    //int len = connection.expectInt();
    //frame_id.resize(len);
    //if (!connection.expectBlock((char*)frame_id.c_str(),len)) return false;
    
    this->frame_id = connection.expectText();
    //std::cout << "YARP_Header.h: received string '" << frame_id << "'" << std::endl;
    update_pcl();
    return !connection.isError();
  }

  bool write(yarp::os::ConnectionWriter& connection) {
    //update_yarp();
    
    // *** yarp_seq ***
    connection.appendInt(yarp_seq);

    // *** yarp_stamp ***
    if (!yarp_stamp.write(connection)) return false;

    // *** frame_id ***
    //connection.appendInt(frame_id.length());
    connection.appendString(frame_id.c_str());
    //connection.appendExternalBlock((char*)frame_id.c_str(),frame_id.length());
    return !connection.isError();
  }

  pcl::PCLHeader& get_PCL_Header(){
  	update_pcl();
  	return *static_cast<pcl::PCLHeader*>(this);
  }
};

#endif
