#include <drc_shared/yarp_status_interface.h>

#include <iterator>
#include <algorithm>
#include <vector>
#include <iostream>
#include <assert.h>

using namespace walkman::drc;

yarp_status_interface::yarp_status_interface(const std::string& module_prefix, const int& period_ms_,
        const std::string& init_state_) :
    port_name("/"+module_prefix+"/status:o"), state(init_state_), RateThread(period_ms_) {
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
    send();
}

void yarp_status_interface::setStatus(const std::string& new_status, int seq_num_i) {
    std::unique_lock<std::mutex>lck(mtx);
    seq_num=seq_num_i;
    state = new_status;
    data_set=false;
    send();
}

void yarp_status_interface::send() {
    std::unique_lock<std::mutex>lck(mtx);
    yarp::os::Bottle& b = port.prepare();
    b.clear();
    b.addInt(seq_num);
    b.addString(state);
    if (data_set)
        b.append(data);
    port.write();
}

bool yarp_status_interface::threadInit() {
    if(port_name == "") {
        assert(false && "State streaming port not specified.");
    }
    return port.open(port_name);
}

void yarp_status_interface::threadRelease() {
    port.close();
}

void yarp_status_interface::setPort(const std::string& port_name_) {
    port_name = port_name_;
}

yarp_status_receiver_interface::yarp_status_receiver_interface(const std::string& module_prefix,yarp::os::Network* network) :
    port_name("/"+module_prefix+"/status:i"){
    port.open(port_name);
    yarp::os::ContactStyle style;
    style.persistent=true;
    network->connect("/"+module_prefix+"/status:o",port_name,style);
}

bool yarp_status_receiver_interface::getStatus(std::string& status, int& seq_num, yarp::os::Bottle* bottle_out) {
    yarp::os::Bottle* bot_status = port.read(false);

    int seq_num_i = -1;
    bottle_out=0;
    if(bot_status != NULL) {
        seq_num_i = bot_status->get(0).asInt();
        status = bot_status->get(1).asString();
        if (bot_status->size()>2)
            bottle_out=bot_status;
        return true;
    }
    seq_num=seq_num_i;
    status="no new status received";
    return false;
}

void yarp_status_receiver_interface::setPort(const std::string& port_name_) {
    port_name = port_name_;
}




