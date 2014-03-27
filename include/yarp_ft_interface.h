#ifndef YARP_FT_INTERFACE_H
#define YARP_FT_INTERFACE_H
#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <yarp/dev/IAnalogSensor.h>
#include <vector>
#include <iostream>
#include <yarp/os/RateThread.h>
#include <yarp/os/BufferedPort.h>
#include <mutex>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/BufferedPort.h>
#define FT_ENABLED TRUE

class yarp_ft_interface
{
public:
    yarp_ft_interface(std::string kinematic_chain);
    yarp::sig::Vector sense();
    void sense(yarp::sig::Vector& q_sensed);
    
    
private:
    int ft_channels;
    yarp::sig::Vector input;
    
    #if (FT_ENABLED == TRUE)
    yarp::dev::IAnalogSensor *FT_sensor;    
    yarp::dev::PolyDriver polyDriver_FT;
    #endif
    
};

#endif // YARP_FT_INTERFACE_H
