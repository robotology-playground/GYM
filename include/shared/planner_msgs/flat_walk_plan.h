/* 
 *  YARP Implementation of a flat_walk_plan message as a sequence of flat_walk_cmd
 *  with a few conversion utilities. 
 *
 *  email: v.varricchio@gmail.com 
 */

#ifndef YARPMSG_TYPE_flat_walk_plan
#define YARPMSG_TYPE_flat_walk_plan

#include <string.h>
#include <vector>
#include <shared/yarp_msgs/YARP_Point.h>
#include <yarp/os/Portable.h>
#include <shared/utils/Pose2D.h>

#include "flat_walk_cmd.h"

class flat_walk_plan : public yarp::os::Portable{
  
public:
  int status;
  int progress;
  std::vector<flat_walk_cmd> controls;
  YARP_Point init_pose;

  flat_walk_plan();
  
  yarp::os::ConstString getTypeName() const;
 
  bool read(yarp::os::ConnectionReader&);

  bool write(yarp::os::ConnectionWriter& connection);

  // Utils
  
  std::vector<YARP_Point> to_path();

  std::vector<Pose2D> to_traj();
 
  Pose2D next_pose(const Pose2D& ip, const flat_walk_cmd& cmd);
  
  void from_rrts_unicycle_controls(const float* init_state, std::vector<float*> traj_controls, int seq_num_offset = 0);
};

#endif