#ifndef _YARP_COMMAND_INTERFACE_H_
#define _YARP_COMMAND_INTERFACE_H_
#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <vector>
#include <iostream>
#include <yarp/os/RateThread.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <mutex>
#include "internal_yarp_command_interface.hpp"

namespace walkman
{
namespace drc
{

    

template<class command_type>
class yarp_custom_command_sender_interface:public internal_yarp_command_sender_interface<command_type>
{
public:
    yarp_custom_command_sender_interface(std::string module_prefix,yarp::os::Network* network):internal_yarp_command_sender_interface< command_type >(module_prefix,"/command",network)
    {
    }
};

class yarp_command_sender_interface:public yarp_custom_command_sender_interface<std::string>
{
public:
    yarp_command_sender_interface(std::string module_prefix,yarp::os::Network* network):yarp_custom_command_sender_interface<std::string>(module_prefix,network)
    {
    }
};

class yarp_switch_sender_interface:public internal_yarp_command_sender_interface<std::string>
{
public:
    yarp_switch_sender_interface(std::string module_prefix,yarp::os::Network* network):internal_yarp_command_sender_interface< std::string >(module_prefix,"/switch",network)
    {
        
    }
};    

template<class command_type>
class yarp_custom_command_interface:public internal_yarp_command_interface<command_type>
{
public:
    yarp_custom_command_interface(std::string module_prefix):internal_yarp_command_interface< command_type >(module_prefix,"/command:i")
    {
    }
};

class yarp_command_interface:public yarp_custom_command_interface<std::string>
{
public:
    yarp_command_interface(std::string module_prefix):yarp_custom_command_interface<std::string>(module_prefix)
    {
    }
};

class yarp_switch_interface:public internal_yarp_command_interface<std::string>
{
public:
    yarp_switch_interface(std::string module_prefix):internal_yarp_command_interface< std::string >(module_prefix,"/switch:i")
    {

    }
};


}
}
#endif
