#ifndef control_tutorial_MODULE_HPP_
#define control_tutorial_MODULE_HPP_

#include <GYM/control_module.hpp>

#include "control_tutorial_thread.h"
#include "control_tutorial_constants.h"

/**
 * @brief control_tutorial_module module derived from control_module
 * 
 * @author 
 */
class control_tutorial_module : public control_module<control_tutorial_thread> {
public:
    
    /**
     * @brief constructor: do nothing but construct the superclass
     * 
     */
    control_tutorial_module(   int argc, 
                               char* argv[],
                               std::string module_prefix, 
                               int module_period, 
                               yarp::os::ResourceFinder rf ) : control_module<control_tutorial_thread>(  argc, 
                                                                                            		argv, 
                                                                                            		module_prefix, 
                                                                                            		module_period,
                                                                                            		rf )
    {
    }
    
    
};

#endif
