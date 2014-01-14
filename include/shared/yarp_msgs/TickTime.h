// This is an automatically generated file.
// Generated from this TickTime.msg definition:
// Instances of this class can be read and written with YARP ports,
// using a ROS-compatible format.

#ifndef YARPMSG_TYPE_TickTime
#define YARPMSG_TYPE_TickTime

#include <string>
#include <vector>
#include <yarp/os/Portable.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/NetInt16.h>
#include <yarp/os/NetUint16.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/NetUint32.h>
#include <yarp/os/NetInt64.h>
#include <yarp/os/NetUint64.h>
#include <yarp/os/NetFloat32.h>
#include <yarp/os/NetFloat64.h>

class TickTime : public yarp::os::Portable {
public:
  yarp::os::ConstString getTypeName() const {
    return "TickTime";
  }

  yarp::os::NetUint32 sec;
  yarp::os::NetUint32 nsec;

  bool read(yarp::os::ConnectionReader& connection) {
    // *** sec ***
    sec = connection.expectInt();

    // *** nsec ***
    nsec = connection.expectInt();
    return !connection.isError();
  }

  bool write(yarp::os::ConnectionWriter& connection) {
    // *** sec ***
    connection.appendInt(sec);

    // *** nsec ***
    connection.appendInt(nsec);
    return !connection.isError();
  }
};

#endif
