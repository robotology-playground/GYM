#ifndef control_tutorial_THREAD_H_
#define control_tutorial_THREAD_H_

#include <GYM/control_thread.hpp>

/**
 * @brief control_tutorial control thread
 * 
 **/
class control_tutorial_thread : public control_thread
{
private:
    unsigned int left_arm_joints;
    double omega;
    double phi;
    int tick;
    
public:
    
    /**
     * @brief constructor
     * 
     * @param module_prefix the prefix of the module
     * @param rf resource finderce
     * @param ph param helper
     */
     control_tutorial_thread( std::string module_prefix, yarp::os::ResourceFinder rf, std::shared_ptr<paramHelp::ParamHelperServer> ph );
    
    
    /**
     * @brief control_tutorial control thread initialization
     * 
     * @return true on succes, false otherwise
     */
    virtual bool custom_init();
    
    /**
     * @brief control_tutorial control thread main loop
     * 
     */
    virtual void run();
    
};

#endif
