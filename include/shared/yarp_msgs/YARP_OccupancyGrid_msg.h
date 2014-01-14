/* 
 *  Temporary YARP Implementation of a OccupancyGrid message
 *
 *  email: v.varricchio@gmail.com 
 *
 */


#ifndef YARPMSG_TYPE_OCCUPANCYGRID_MSG
#define YARPMSG_TYPE_OCCUPANCYGRID_MSG

#include <math.h>
#include <vector>
#include <stdio.h>
#include <yarp/os/all.h>
#include <assert.h>
#include "YARP_Header.h"
#include "YARP_Pose.h"

class YARP_OccupancyGrid_msg : public yarp::os::Portable {

public:
  YARP_Header header;
  
  // Size of the map
  double width;
  double height;
  
  // Coordinates of the origin w/r lower-left corner of map normalized by the width and height of map 
  // These default to 0.5 0.5, i.e. the center of the map   // (0,0 => lower-left corned; 1,1 => upper right corner)
  
  double u0;
  double v0;
  
  double resolution;
  std::vector<u_int32_t> data;
  
  YARP_Pose origin;
  
  YARP_OccupancyGrid_msg():width(0), height(0), u0(0), v0(0), resolution(0){
  }
  
  YARP_OccupancyGrid_msg(const double& _width, const double& _height, const double& _resolution, 
			 const double& _u0=0.5, const double& _v0=0.5): 
			 width(_width), height(_height), u0(_u0), v0(_v0), resolution(_resolution){
  }
  
public: 
  
  // YARP communication stuff
  
  bool read(yarp::os::ConnectionReader& connection) {
    // *** header ***
    if(!header.read(connection)) return false; 
    
    // *** height ***
    height = connection.expectDouble();

    // *** width ***
    width = connection.expectDouble();

    // *** u0 ***
    u0 = connection.expectDouble();

    // *** v0 ***
    v0 = connection.expectDouble();

    // *** resolution ***
    resolution = connection.expectDouble();
    
    // *** data ***
    int len = connection.expectInt();
    
    data.resize(len);

    for(uint i=0; i<len; i++)
      data[i] = connection.expectInt();

    origin.read(connection);
    
    return !connection.isError();
  }

  bool write(yarp::os::ConnectionWriter& connection) {  

    // *** header ***
    if(!header.write(connection)) return false;
    
    // *** height ***
    //std::cout << "writing height = " << height << std::endl;
    connection.appendDouble(height);

    // *** width ***
    //std::cout << "writing width = " << width << std::endl;
    connection.appendDouble(width);
      
    // *** u0 ***
    //std::cout << "writing u0 = " << u0 << std::endl;
    connection.appendDouble(u0);

    // *** v0 ***
    connection.appendDouble(v0);
    
    // *** resolution ***
    connection.appendDouble(resolution);

    // *** data ***
    connection.appendInt(data.size());
    
    for (const auto& cellvalue: data) {
      connection.appendInt(cellvalue);
    }
    
    origin.write(connection);

    return !connection.isError();
  } 
};

#endif