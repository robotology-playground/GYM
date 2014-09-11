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
    double dT;
    std::string robot;
    yarp::os::ResourceFinder* rf;

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
                    yarp::os::ResourceFinder* rf) : module_prefix( module_prefix),
                                                   dT( thread_period ),
                                                   RateThread( thread_period )
    {    
        // rf
        this->rf = new yarp::os::ResourceFinder( *rf );
        dT = rf->find("dT").asInt();
        robot = rf->find("robot").asString();
        std::cout << "Thread Period : " << dT << std::endl; 
        std::cout << "Robot Name : " << robot << std::endl; 
    }
    
    
    bool threadInit() final
    {        
        return custom_init();
    }
    
    virtual bool custom_init() = 0;
    
    void threadRelease() final 
    { 
        // resource finder
        if( rf ) {   
            delete rf;
            rf = NULL;
        }
        // custom release
        custom_release();
    }
       
    virtual void custom_release() 
    {
    }
     
  
};

#endif
