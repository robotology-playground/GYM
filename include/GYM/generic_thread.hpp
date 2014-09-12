#ifndef GENERIC_THREAD_HPP
#define GENERIC_THREAD_HPP

#include <yarp/os/all.h>


/**
 * @brief generic thread
 * 
 * @author Luca Muratore (luca.muratore@iit.it)
 **/
class generic_thread : public yarp::os::RateThread
{
protected:
    
    std::string module_prefix;
    double thread_period;
    std::string robot;
    yarp::os::ResourceFinder rf;

public: 
    
    /**
     * @brief constructor of the generic thread.
     *
     * @param module_prefix module name.
     * @param thread_period period of the run thread in millisecond.
     * @param rf resource finder.
     **/
    generic_thread( std::string module_prefix, 
                    double thread_period,
                    yarp::os::ResourceFinder rf) : module_prefix( module_prefix),
                                                   thread_period( thread_period ),
                                                   rf( rf ),
                                                   RateThread( thread_period )
    {    
        thread_period = rf.find("thread_period").asInt();
        robot = rf.find("robot").asString();
        std::cout << "Thread Period : " << thread_period << std::endl; 
        std::cout << "Robot Name : " << robot << std::endl; 
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
     * @brief custom initialization function: must be overrided by sub-classes.
     * 
     * @return true on success, false otherwise.
     */
    virtual bool custom_init() = 0;
    
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
    * @brief custom release function: could be overrided by sub-classes.
    * 
    */
    virtual void custom_release() 
    {
    }
    
     
  
};

#endif
