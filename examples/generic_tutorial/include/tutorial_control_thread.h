#ifndef TUTORIAL_CONTROL_THREAD_H_
#define TUTORIAL_CONTROL_THREAD_H_

#include <yarp/os/RateThread.h>
#include <yarp/sig/Vector.h>

#include <idynutils/yarp_single_chain_interface.h>
#include <GYM/yarp_command_interface.hpp>
#include <GYM/generic_thread.hpp>

/**
 * @brief The tutorial_control_thread class inherit from a generic_thread
 */
class tutorial_control_thread: public generic_thread
{
private:   
    // left_arm chain interface
    walkman::yarp_single_chain_interface left_arm_chain_interface;
    // joints number
    int num_joints;
    // left_arm configuration vector
    yarp::sig::Vector left_arm_configuration;
    // max velocity 
    double max_vel;
    // ref speed vector
    yarp::sig::Vector ref_speed_vector;
    // command interface
    walkman::yarp_command_interface command_interface;

    // link the tutorial optional params
    void link_tutorial_params();
    
public:
    /**
     * @brief tutorial_control_thread constructor
     * @param module_prefix passed from the module
     * @param rf passed from the module
     * @param ph a param helper builted while generating the control thread
     */
    tutorial_control_thread(    std::string module_prefix, 
                                yarp::os::ResourceFinder rf, 
                                std::shared_ptr<paramHelp::ParamHelperServer> ph );

    /**
     * @brief custom_init we use this method when a "start" is sent to the module
     * @return true on success, false otherwise
     */
    virtual bool custom_init();

    /**
     * @brief run we use this one to compute our super control!
     */
    virtual void run();
    
    /**
     * @brief custom_pause we use this method when a "pause" is sent to the module
     * @return true on success, false otherwise
     */
    virtual bool custom_pause();

    /**
     * @brief custom_resume we use this method when a "resume" is sent to the module
     * @return true on success, false otherwise
     */
    virtual bool custom_resume();
    
};

#endif
