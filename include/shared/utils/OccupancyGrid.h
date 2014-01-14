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
  double width;
  double height;
  
  // Coordinates of the origin w/r lower-left corner of map normalized by the width and height of map 
  // These default to 0.5 0.5, i.e. the center of the map   // (0,0 => lower-left corned; 1,1 => upper right corner)
  
  double u0;
  double v0;
  
  uint cols;
  uint rows;
  
  double resolution;
  std::vector<u_int32_t> data;
  
  // Pose
  YARP_Pose origin;
    
  OccupancyGrid();
  
  OccupancyGrid(const double& _width, const double& _height, const double& _resolution, 
                     const double& _u0=0.5, const double& _v0=0.5);
  
  OccupancyGrid(const YARP_OccupancyGrid_msg&);

  bool set_value_rc(uint row, uint col, const double& value);
  
  bool get_value_rc(uint row, uint col, double &value) const;
  
  double value_rc(uint row, uint col) const;
      
  bool set_value_xy(double x, double y, const double& value);
  
  bool get_value_xy(double x, double y, double& value) const;
  
  double value_xy(double x, double y) const;
      
  bool is_in_xy(double x, double y) const;
    
  bool is_in_rc(uint row, uint col) const;

  void print();

  void fill_msg(YARP_OccupancyGrid_msg&);
  
  uint get_col(double x) const;
  
  uint get_row(double y) const;
    
  double get_x(uint col) const;
  
  double get_y(uint row) const;
private:
  
  size_t get_idx_rc(uint row, uint col) const;
  
  uint get_idx_xy(double x, double y) const;

  void init(const double& _width, const double& _height, const double& _resolution, 
            const double& _u0, const double& _v0);
  
  void init(bool zero=true);
  
};

#endif