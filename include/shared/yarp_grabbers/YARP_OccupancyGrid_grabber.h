/* 
 *  Grabber for a YARP_OccupancyGrid_msg that outputs an OccupancyGrid object.
 *
 *  email: v.varricchio@gmail.com 
 */

#include <shared/utils/OccupancyGrid.h>
#include "YARP_grabber.hpp"

class YARP_OccupancyGrid_grabber : public yarp_msg_grabber<YARP_OccupancyGrid_msg, OccupancyGrid>  {
         
public: 
  void update_data(YARP_OccupancyGrid_msg& msg){
    //std::cout << "YARP_OccupancyGrid_grabber.h: msg frame_id: '" << msg.header.frame_id << "' " << std::endl;
    this->data = OccupancyGrid(msg);
    //std::cout << "YARP_OccupancyGrid_grabber.h: this->data frame_id: '" << this->data.header.frame_id << "' " << std::endl;
  }
};