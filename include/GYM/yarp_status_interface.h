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

#ifndef _YARP_STATUS_INTERFACE_H_
#define _YARP_STATUS_INTERFACE_H_

#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <vector>
#include <iostream>
#include <yarp/os/RateThread.h>
#include <yarp/os/BufferedPort.h>
#include <mutex>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/BufferedPort.h>


namespace walkman
{
namespace drc
{

class yarp_status_interface : public yarp::os::RateThread{
    
    yarp::os::BufferedPort<yarp::os::Bottle> port;
    std::mutex mtx; 
    std::string port_name;
    yarp::os::ConstString status_i;
    bool data_set;
    int seq_num;
    void send();
    std::string module_prefix;
    
    
public:
    std::string state;            // String description of the module's state (quite common)
    yarp::os::Bottle data;        // Optional additional data
        
        yarp_status_interface(const std::string& module_prefix, const int& period_ms_= 500, 
                            const std::string& init_state_ = "initializing");
    
    void setPort(const std::string& port_name_);
    
    void run();
    
    void setStatus(const std::string& new_status, const yarp::os::Bottle& data, int seq_num=0);

    void setStatus(const std::string& new_status, int seq_num=0);
    
    bool threadInit();
    
    void threadRelease();
    
}; 


class yarp_status_receiver_interface{

    yarp::os::BufferedPort<yarp::os::Bottle> port;
    std::mutex mtx; 
    std::string port_name;
    std::string module_prefix;
    
public:
    yarp_status_receiver_interface(const std::string& module_prefix);
    
    void setPort(const std::string& port_name_);
    
    bool getStatus(std::string& status, int& seq_num, yarp::os::Bottle* bottle_out=0);
    
}; 


}
}
#endif
