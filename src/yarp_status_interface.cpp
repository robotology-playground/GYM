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


#include <GYM/yarp_status_interface.h>

#include <iterator>
#include <algorithm>
#include <vector>
#include <iostream>
#include <assert.h>

using namespace walkman;

yarp_status_interface::yarp_status_interface(const std::string& module_prefix, const int& period_ms_,
        const std::string& init_state_) :
    port_name("/"+module_prefix+"/status:o"), state(init_state_), RateThread(period_ms_) {
        this->module_prefix=module_prefix;
}

void yarp_status_interface::run() {
    std::unique_lock<std::mutex>lck(mtx);
    send();
}

void yarp_status_interface::setStatus(const std::string& new_status, const yarp::os::Bottle& data_, int seq_num_i) {
    std::unique_lock<std::mutex>lck(mtx);
    seq_num=seq_num_i;
    state = new_status;
    data = data_;
    data_set=true;
//     send();
}

void yarp_status_interface::setStatus(const std::string& new_status, int seq_num_i) {
    std::unique_lock<std::mutex>lck(mtx);
    seq_num=seq_num_i;
    state = new_status;
    data_set=false;
//     send();
}

void yarp_status_interface::send() {
    yarp::os::Bottle& b = port.prepare();
    b.clear();
    b.addInt(seq_num);
    if(state.length()==1) b.addInt((int)state[0]);
    else b.addString(state);
    if (data_set)
        b.append(data);
    port.write();
}

bool yarp_status_interface::threadInit() {
    if(port_name == "") {
        assert(false && "State streaming port not specified.");
    }
    bool result = port.open(port_name);
    if (!yarp::os::NetworkBase::isConnected(port_name,"/"+module_prefix+"/status:i"))
    {
        yarp::os::ContactStyle style;
        style.persistent=true;
        std::cout<<"connecting "<<port_name <<" to " <<"/"+module_prefix+"/status:i"<<std::endl;
        yarp::os::Network::connect(port_name,"/"+module_prefix+"/status:i",style);
    }
    return result;
}

void yarp_status_interface::threadRelease() {
    port.close();
}

void yarp_status_interface::setPort(const std::string& port_name_) {
    port_name = port_name_;
}

yarp_status_receiver_interface::yarp_status_receiver_interface(const std::string& module_prefix) :
    port_name("/"+module_prefix+"/status:i"){
    this->module_prefix=module_prefix;
    port.open(port_name);
    if (!yarp::os::NetworkBase::isConnected("/"+module_prefix+"/status:o",port_name))
    {
        yarp::os::ContactStyle style;
        style.persistent=true;
        std::cout<<"connecting "<<"/"+module_prefix+"/status:o"<<" to "<<port_name<<std::endl;
        yarp::os::Network::connect("/"+module_prefix+"/status:o",port_name,style);
    }
}

bool yarp_status_receiver_interface::getStatus(std::string& status, int& seq_num, yarp::os::Bottle* bottle_out) {
    yarp::os::Bottle* bot_status = port.read(false);

    bottle_out=0;
    if(bot_status != NULL) {
        seq_num = bot_status->get(0).asInt();
	if(bot_status->get(1).isInt()) status = std::string(1,(char)(bot_status->get(1).asInt()));
        else status = bot_status->get(1).asString();
        if (bot_status->size()>2)
            bottle_out=bot_status;
        return true;
    }
    seq_num=-1;
    status="no new status received";
    return false;
}

void yarp_status_receiver_interface::setPort(const std::string& port_name_) {
    port_name = port_name_;
}




