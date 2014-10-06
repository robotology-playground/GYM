#ifndef GENERIC_THREAD_HPP
#define GENERIC_THREAD_HPP

// YARP
#include <yarp/os/all.h>
// param helper
#include <paramHelp/paramHelperServer.h>
// C++11 smart pointers
#include <memory>

/**
 * @brief generic thread
 * 
 * @author Luca Muratore (luca.muratore@iit.it)
 **/
class generic_thread : public yarp::os::RateThread
{
private:
    
    std::string module_prefix;
    double thread_period;
    std::string robot_name;
    yarp::os::ResourceFinder rf;
    std::shared_ptr<paramHelp::ParamHelperServer> ph;
    

public: 
    
    /**
     * @brief constructor of the generic thread.
     *
     * @param module_prefix module name.
     * @param thread_period period of the run thread in millisecond.
     * @param rf resource finder.
     **/
    generic_thread( std::string module_prefix, 
                    yarp::os::ResourceFinder rf, 
                    std::shared_ptr<paramHelp::ParamHelperServer> ph  ):    module_prefix( module_prefix ),
                                                                            thread_period( rf.find("thread_period").asInt() ),
                                                                            robot_name( rf.find("robot_name").asString() ),
                                                                            rf( rf ),
                                                                            ph( ph ),
                                                                            RateThread( thread_period )
    {    
    }
    
    
    /**
     * @brief initialize the thread and call custom_init.
     * 
     * @return true if custom_init has success, false otherwise.
     */
    bool threadInit() final
    {        
        return custom_init();
    }
    
    /**
     * @brief custom initialization function: called before run(), must be overrided by sub-classes
     * 
     * @return true on success, false otherwise.
     */
    virtual bool custom_init() = 0;
    
    
    /**
     * @brief custom pause function: called before pausing the thread
     *
     * @return true on succes. False otherwise.
     **/
    virtual bool custom_pause()
    {
        return true;
    }
    
    /**
     * @brief custom resume function: called before resuming the thread
     *
     * @return true on succes. False otherwise.
     **/
    virtual bool custom_resume()
    {
        return true;
    }
    
    
    /**
     * @brief release method of the thread.
     * 
     */
    void threadRelease() final 
    { 
        // custom release
        custom_release();
    }
       
    /**
    * @brief custom release function: called befire releasing the thread
    * 
    */
    virtual void custom_release() 
    {
    }
    
    /**
     * @brief getter method for the module prefix of the module that controls that thread
     * 
     * @return the module prefix of the module that controls that thread
     */
    std::string get_module_prefix() 
    {
        return module_prefix;
    }
    
    /**
     * @brief getter method for the period of the thread
     * 
     * @return the period of the thread
     */
    double get_thread_period()
    {
        return thread_period;
    }
    
    /**
     * @brief getter method for the name of the robot
     * 
     * @return the name of the robot
     */
    std::string get_robot_name()
    {
        return robot_name;
    }
    
    /**
     * @brief getter method for the resource finder
     * 
     * @return the resource finder
     */
    yarp::os::ResourceFinder get_resource_finder()
    {
        return rf;
    }
    
    /**
     * @brief getter method for the param helper
     * 
     * @return the param helper
     */
    std::shared_ptr<paramHelp::ParamHelperServer> get_param_helper()
    {
        return ph;
    }
    
    
  
};

#endif
