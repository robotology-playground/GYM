#ifndef _MODULE_PREFIX__MODULE_HPP_
#define _MODULE_PREFIX__MODULE_HPP_

#include <GYM/generic_module.hpp>

#include "_MODULE_PREFIX__thread.h"
#include "_MODULE_PREFIX__constants.h"

/**
 * @brief _MODULE_PREFIX module derived from generic_module
 * 
 * @author 
 */
class _MODULE_PREFIX__module : public generic_module<_MODULE_PREFIX__thread> {
public:
    
    /**
     * @brief constructor: do nothing but construct the superclass
     * 
     */
    _MODULE_PREFIX__module(   int argc, 
                               char* argv[],
                               std::string module_prefix, 
                               int module_period, 
                               yarp::os::ResourceFinder rf ) : generic_module<_MODULE_PREFIX__thread>(  argc, 
                                                                                            		argv, 
                                                                                            		module_prefix, 
                                                                                            		module_period,
                                                                                            		rf )
    {
    }
    
    /**
     * @brief overriden function to specify the custom params for the param helper
     * 
     * @return a vector of the custom params for the param helper
     */
    virtual std::vector< paramHelp::ParamProxyInterface* > custom_get_ph_parameters() 
    {
	// TODO: function skeleton
        std::vector<paramHelp::ParamProxyInterface *> custom_params;
        return custom_params;
    }
    
    
};

#endif
