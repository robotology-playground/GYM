/* 
 *  YARP Implementation of a flat_walk_command message 
 *  acoording to the agreed interfaces. 
 *
 *  email: v.varricchio@gmail.com 
 */

#ifndef YARPMSG_TYPE_flat_walk_cmd
#define YARPMSG_TYPE_flat_walk_cmd

#include <string>
#include <map>
#include <utility>
#include <yarp/os/Portable.h>
#include <shared/yarp_msgs/YARP_Point.h>
#include "cmd_struct.h"

class flat_walk_cmd : public yarp::os::Portable{
public:
    
  std::string action; 	// currently ("fwd", "bwd", "right", "left", "rotr", "rotl")
  double amount;	// walking distance in meters and rotations in degrees
  int seq_num;
  static std::map<std::string, std::string> opposite;
  static std::map<std::string, double> el;

  yarp::os::ConstString getTypeName() const;
  flat_walk_cmd();
 
  bool read(yarp::os::ConnectionReader&);
  bool write(yarp::os::ConnectionWriter& connection);
  
  bool add(const flat_walk_cmd& cmd);

  void normalize();
  
  cmd_struct to_struct() const;
  bool from_struct(const cmd_struct& cmds); 

  static std::pair<flat_walk_cmd, size_t> split(const flat_walk_cmd&);
  
};

#endif
