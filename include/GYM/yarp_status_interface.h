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
    void send();
    
public:
    std::string state;            // String description of the module's state (quite common)
    yarp::os::Bottle data;        // Optional additional data
        
        yarp_status_interface(const std::string& module_prefix, const int& period_ms_= 500, 
                            const std::string& init_state_ = "initializing");
    
    void setPort(const std::string& port_name_);
    
    void run();
    
    void setStatus(const std::string& new_status, const yarp::os::Bottle& data = yarp::os::Bottle());
    
    bool threadInit();
    
    void threadRelease();
    
}; 


class yarp_status_receiver_interface{

    yarp::os::BufferedPort<yarp::os::Bottle> port;
    std::mutex mtx; 
    std::string port_name;
        
public:
    yarp_status_receiver_interface(const std::string& module_prefix, yarp::os::Network* network);
    
    void setPort(const std::string& port_name_);
    
    bool getStatus(std::string& status, int& seq_num);
    
}; 


}
}
#endif
