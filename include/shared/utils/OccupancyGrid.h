#ifndef __OCCUPANCY_GRID_H__
#define __OCCUPANCY_GRID_H__

#include <math.h>
#include <vector>
#include <stdio.h>
#include <yarp/os/all.h>
#include <assert.h>
#include "../yarp_msgs/YARP_OccupancyGrid_msg.h"
#include "../yarp_msgs/YARP_Pose.h"

/*  Quick (re)implementation of an OccupancyGrid
 *                
 *                         y ^
 *               .           |
 *   v0*height   . . . . . . +---> x
 *               .           .
 *               ^           . 
 *               |           .
 *               +---> . . . . . ,
 *                       u0*width 
 * 
 *      
 *  email: v.varricchio@gmail.com
 * 
 */


class OccupancyGrid{

public:
  YARP_Header header;
  
  // Size of the map
  float width;
  float height;
  
  // Coordinates of the origin w/r lower-left corner of map normalized by the width and height of map 
  // These default to 0.5 0.5, i.e. the center of the map   // (0,0 => lower-left corned; 1,1 => upper right corner)
  
  float u0;
  float v0;
  
  uint cols;
  uint rows;
  
  float resolution;
  std::vector<u_int32_t> data;
  
  // Pose
  YARP_Pose origin;
    
  OccupancyGrid();
  
  OccupancyGrid(const float& _width, const float& _height, const float& _resolution, 
                     const float& _u0=0.5, const float& _v0=0.5);
  
  OccupancyGrid(const YARP_OccupancyGrid_msg&);

  bool set_value_rc(uint row, uint col, const float& value);
  
  bool get_value_rc(uint row, uint col, float &value) const;
  
  float value_rc(uint row, uint col) const;
      
  bool set_value_xy(float x, float y, const float& value);
  
  bool get_value_xy(float x, float y, float& value) const;
  
  float value_xy(float x, float y) const;
      
  bool is_in_xy(float x, float y) const;
    
  bool is_in_rc(uint row, uint col) const;

  void print();

  void fill_msg(YARP_OccupancyGrid_msg&);
  
  uint get_col(float x) const;
  
  uint get_row(float y) const;
    
  float get_x(uint col) const;
  
  float get_y(uint row) const;
private:
  
  size_t get_idx_rc(uint row, uint col) const;
  
  uint get_idx_xy(float x, float y) const;

  void init(const float& _width, const float& _height, const float& _resolution, 
            const float& _u0, const float& _v0);
  
  void init(bool zero=true);
  
};

#endif