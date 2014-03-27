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


namespace walkman
{
    namespace drc
    {


template<class command_type> class internal_yarp_command_interface
{
    
public:
    
    internal_yarp_command_interface(std::string module_prefix,std::string port_suffix)
    {
        std::string temp="/"+module_prefix+port_suffix;
        command_port.open(temp.c_str());
        
    }
    
    bool getCommand ( command_type& cmd, int& seq_num )
    {
        yarp::os::Bottle* bot_command = command_port.read(false);
        
        int seq_num_i = -1;
        
        if(bot_command != NULL) {
            seq_num_i = bot_command->get(0).asInt();
            command_i.fromBottle(bot_command);
            return true;
        }
        cmd=command_i;
        seq_num=seq_num_i;
        return false;
    }
private:
    command_type command_i;
    yarp::os::BufferedPort<yarp::os::Bottle> command_port;
};

template<> class internal_yarp_command_interface<std::string>
{
public:
    
    internal_yarp_command_interface(std::string module_prefix,std::string port_suffix)
    {
        command_port.open("/"+module_prefix+port_suffix);
        
    }
    
    bool getCommand ( std::string& cmd, int& seq_num )
    {
        yarp::os::Bottle* bot_command = command_port.read(false);
        
        int seq_num_i = -1;
        
        if(bot_command != NULL) {
            seq_num_i = bot_command->get(0).asInt();
            command_i= bot_command->get(1).asString();
            return true;
        }
        cmd=command_i;
        seq_num=seq_num_i;
        return false;
    }
private:
    std::string command_i;
    yarp::os::BufferedPort<yarp::os::Bottle> command_port;
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