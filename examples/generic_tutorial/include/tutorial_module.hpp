#ifndef TUTORIAL_MODULE_HPP_
#define TUTORIAL_MODULE_HPP_

// include generic module header
#include <GYM/generic_module.hpp>
// include control thread and constants header files
#include "tutorial_control_thread.h"
#include "tutorial_constants.h"

/**
 * @brief The tutorial_module class inherit from generic_module.
 */
class tutorial_module : public generic_module<tutorial_control_thread> {
public:
    /**
     * @brief tutorial_module constructor
     * @param argc main argc
     * @param argv main argv
     * @param module_prefix prefix of the module
     * @param module_period period of the module (ex: 1000 [ms])
     * @param rf resource finder
     */
    tutorial_module(    int argc, 
                        char* argv[],
                        std::string module_prefix, 
                        int module_period, 
                        yarp::os::ResourceFinder rf ) : generic_module<tutorial_control_thread>(    argc, 
                                                                                                    argv, 
                                                                                                    module_prefix, 
                                                                                                    module_period,
                                                                                                    rf )
    {
    }
    
    /**
     * @brief custom_get_ph_parameters inherit from generic module, we reimplement it since we have more parameters in the
     * param_help (tutorial_configuration.ini file) than the default ones.
     * @return a vector of the custom parameters for the param helper
     */
    virtual std::vector< paramHelp::ParamProxyInterface* > custom_get_ph_parameters() 
    {   
        // custom param helper parameters vector
        std::vector<paramHelp::ParamProxyInterface *> custom_params;
        // insert left_arm param
        /// NOTE: "left_arm" id has to be the same of the one in the tutorial_configuration.ini
        custom_params.push_back( new paramHelp::ParamProxyBasic<double>(   "left_arm", 
                                                                            PARAM_ID_LEFT_ARM, 
                                                                            PARAM_SIZE_LEFT_ARM, 
                                                                            paramHelp::PARAM_IN_OUT, 
                                                                            NULL, 
                                                                            "left_arm configuration in [degree]" ) );
        // insert max_vel param
        custom_params.push_back( new paramHelp::ParamProxyBasic<double>(    "max_vel", 
                                                                            PARAM_ID_MAX_VEL, 
                                                                            PARAM_SIZE_MAX_VEL, 
                                                                            paramHelp::PARAM_IN_OUT, 
                                                                            NULL, 
                                                                            "maximum velocity in [degree/second]" ) );
        
        return custom_params;
    }
};

#endif
