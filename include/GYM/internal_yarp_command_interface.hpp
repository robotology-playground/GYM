/*
 * Copyright (C) 2014 Walkman
 * Author: Mirko Ferrati
 * email:  mirko.ferrati@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>
*/

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
    template<class command_type> class internal_yarp_command_sender_interface
    {
    public:
        internal_yarp_command_sender_interface(const std::string& module_prefix_,const std::string& port_suffix)
        {
        this->module_prefix=module_prefix_;
            if (module_prefix[0]=='/') module_prefix=module_prefix.substr(1);
            std::string temp_o="/"+module_prefix+port_suffix+":o";
            std::string temp_i="/"+module_prefix+port_suffix+":i";
            std::string temp_o_num=temp_o;
            if (yarp::os::NetworkBase::exists(temp_o_num))
                temp_o_num=temp_o+std::to_string(1);
            if (yarp::os::NetworkBase::exists(temp_o_num))
                temp_o_num=temp_o+std::to_string(2);
            if (yarp::os::NetworkBase::exists(temp_o_num))
                temp_o_num=temp_o+std::to_string(3);

            /*
             * ALTERNATIVE VERSION WITH ONLY 1 QUERY BUT MORE RESULTS IN THE MESSAGE
            yarp::os::ContactStyle style;
            style.quiet = true;
            yarp::os::Bottle cmd, reply;
            cmd.add("name");
            cmd.add("list");
            for (int i=0; i<argc; i++) {
                Value v;
                v.fromString(argv[i]);
                cmd.add(v);
            }
            bool ok = NetworkBase::writeToNameServer(cmd,
                                                     reply,
                                                     style);
            if (!ok) {
                ACE_OS::fprintf(stderr, "Failed to reach name server\n");
                return 1;
            }
            if (reply.size()==1&&reply.get(0).isString()) {
                printf("%s", reply.get(0).asString().c_str());
            } else if (reply.get(0).isVocab() && reply.get(0).asVocab()==VOCAB4('m','a','n','y')) {
                for (int i=1; i<reply.size(); i++) {
                    Value& v = reply.get(i);
                    if (v.isString()) {
                        printf("  %s\n", v.asString().c_str());
                    } else {
                        printf("  %s\n", v.toString().c_str());
                    }
                }
            } else {
                printf("%s\n", reply.toString().c_str());
            }
            grep registration | awk '{print $3 }'| grep ^\/
            */

            command_port.open(temp_o_num.c_str());
            yarp::os::ContactStyle style;
            style.persistent = true;
            yarp::os::Network::connect(temp_o_num.c_str(),temp_i.c_str(), style);
        }

        bool sendCommand(command_type& cmd, int seq_num=0)
        {
            yarp::os::Bottle& b=command_port.prepare();
    b.clear();
            b.append(cmd.toBottle());
    b.addInt(seq_num);
            command_port.write();
        }
    private:
        yarp::os::BufferedPort<yarp::os::Bottle> command_port;
        std::string module_prefix;
    };


    template<> class internal_yarp_command_sender_interface<std::string>
    {
    public:
        internal_yarp_command_sender_interface(const std::string& module_prefix,const std::string& port_suffix)
        {
            this->module_prefix=module_prefix;
            std::string temp_o="/"+module_prefix+port_suffix+":o";
            std::string temp_i="/"+module_prefix+port_suffix+":i";
            std::string temp_o_num=temp_o;
            if (yarp::os::NetworkBase::exists(temp_o_num))
                temp_o_num=temp_o+std::to_string(1);
            if (yarp::os::NetworkBase::exists(temp_o_num))
                temp_o_num=temp_o+std::to_string(2);
            if (yarp::os::NetworkBase::exists(temp_o_num))
                temp_o_num=temp_o+std::to_string(3);

            command_port.open(temp_o_num.c_str());
            yarp::os::ContactStyle style;
            style.persistent = true;
            yarp::os::Network::connect(temp_o_num.c_str(),temp_i.c_str(), style);
        }

        bool sendCommand(const std::string& cmd, int seq_num=0)
        {
            yarp::os::Bottle& b=command_port.prepare();
            b.clear();
            b.addString(cmd);
    b.addInt(seq_num);
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
        std::string module_prefix;

    };

    template<class command_type> class internal_yarp_command_interface
    {
    public:
        internal_yarp_command_interface(const std::string& module_prefix,const std::string& port_suffix)
        {
            this->module_prefix=module_prefix;
            std::string temp="/"+module_prefix+port_suffix;
            command_port.open(temp.c_str());
        }

        bool getCommand ( command_type& cmd, int& seq_num )
        {
            yarp::os::Bottle* bot_command = command_port.read(false);
            int seq_num_i = -1;
            if(bot_command != NULL) {
                seq_num_i = bot_command->pop().asInt();
                std::cout<<"received message"<<seq_num_i<<std::endl;
        command_i.fromBottle(bot_command);
        cmd=command_i;
        seq_num=seq_num_i;
        return true;
            }
            seq_num=seq_num_i;
            return false;
        }
    private:
        command_type command_i;
        yarp::os::BufferedPort<yarp::os::Bottle> command_port;
        std::string module_prefix;

    };

    template<> class internal_yarp_command_interface<std::string>
    {
    public:

        internal_yarp_command_interface(const std::string& module_prefix,const std::string& port_suffix)
        {
            this->module_prefix=module_prefix;
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
                std::string command_i= bot_command->get(0).asString();
                return command_i;
            }
            return "";
        }

        bool getCommand (std::string & cmd)
        {
            yarp::os::Bottle* bot_command = command_port.read(false);

            if(bot_command != NULL) {
                cmd = bot_command->get(0).asString();
                return true;
            }
            else
            {
                cmd="";
                return false;
            }
        }

        bool getCommand ( std::string& cmd, int& seq_num )
        {
            yarp::os::Bottle* bot_command = command_port.read(false);
            if(bot_command != NULL) {
                seq_num = bot_command->pop().asInt();
                cmd= bot_command->get(0).asString();
                return true;
            }
            else
            {
                cmd="";
                seq_num=-1;
                return false;
            }
        }
    private:
        yarp::os::BufferedPort<yarp::os::Bottle> command_port;
        std::string module_prefix;

    };
}

#endif
