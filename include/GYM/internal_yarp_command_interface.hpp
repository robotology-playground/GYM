#ifndef _INTERNAL_YARP_COMMAND_INTERFACE_H_
#define _INTERNAL_YARP_COMMAND_INTERFACE_H_
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
        
        template<class command_type> class internal_yarp_command_sender_interface
        {
        public:
            internal_yarp_command_sender_interface(const std::string& module_prefix,const std::string& port_suffix,yarp::os::Network* network)
            {
                if (module_prefix[0]=='/') module_prefix=module_prefix.substr(1);
                std::string temp_o="/"+module_prefix+port_suffix+":o";
                std::string temp_i="/"+module_prefix+port_suffix+":i";
                
                command_port.open(temp_o.c_str());
                yarp::os::ContactStyle style;
                style.persistent = true;
                network->connect(temp_o.c_str(),temp_i.c_str());
            }
            
            bool sendCommand(command_type& cmd, int seq_num=0)
            {
                yarp::os::Bottle& b=command_port.prepare();
                b=cmd.toBottle();
                command_port.write();
            }
        private:
            yarp::os::BufferedPort<yarp::os::Bottle> command_port;
        };
        
        
        template<> class internal_yarp_command_sender_interface<std::string>
        {
        public:
            internal_yarp_command_sender_interface(const std::string& module_prefix,const std::string& port_suffix,yarp::os::Network* network)
            {
                std::string temp_o="/"+module_prefix+port_suffix+":o";
                std::string temp_i="/"+module_prefix+port_suffix+":i";
                
                command_port.open(temp_o.c_str());
                yarp::os::ContactStyle style;
                style.persistent = true;
                network->connect(temp_o.c_str(),temp_i.c_str());
            }
            
            bool sendCommand(const std::string& cmd, int seq_num=0)
            {
                yarp::os::Bottle& b=command_port.prepare();
                b.clear();
                b.addString(cmd);
                command_port.write();
            }
            
            bool sendCommand(int cmd, int seq_num=0)
            {
                yarp::os::Bottle& b=command_port.prepare();
                b.clear();
                b.addInt(cmd);
                command_port.write();
            }
            
        private:
            yarp::os::BufferedPort<yarp::os::Bottle> command_port;    
        };
        
        

        template<class command_type> class internal_yarp_command_interface
        {
            
        public:
            
            internal_yarp_command_interface(const std::string& module_prefix,const std::string& port_suffix)
            {
                std::string temp="/"+module_prefix+port_suffix;
                command_port.open(temp.c_str());
                
            }
            
            bool getCommand ( command_type& cmd, int seq_num )
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
            
            internal_yarp_command_interface(const std::string& module_prefix,const std::string& port_suffix)
            {
                command_port.open("/"+module_prefix+port_suffix);
                
            }
            
            bool getCommand(int& command)
            {
                yarp::os::Bottle* bot_command = command_port.read(false);
                
                if(bot_command != NULL) {
                    command= bot_command->get(0).asInt();
                    return true;
                }
                return false;
            }
            
            std::string getCommand()
            {
                
                yarp::os::Bottle* bot_command = command_port.read(false);
                
                if(bot_command != NULL) {
                    command_i= bot_command->get(0).asString();
                    return command_i;
                }
                return "";
            }
            
            bool getCommand (std::string & cmd)
            {
                yarp::os::Bottle* bot_command = command_port.read(false);
                
                if(bot_command != NULL) {
                    command_i= bot_command->get(0).asString();
                    return true;
                }
                cmd=command_i;
                return false;
            }
            
            bool getCommand ( std::string& cmd, int seq_num )
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
        
        
        
        
        
    }
}













#endif