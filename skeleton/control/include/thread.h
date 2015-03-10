#ifndef _MODULE_PREFIX__THREAD_H_
#define _MODULE_PREFIX__THREAD_H_

#include <GYM/control_thread.hpp>

/**
 * @brief _MODULE_PREFIX_ control thread
 * 
 **/
class _MODULE_PREFIX__thread : public control_thread
{
private:   
    
public:
    
    /**
     * @brief constructor
     * 
     * @param module_prefix the prefix of the module
     * @param rf resource finderce
     * @param ph param helper
     */
     _MODULE_PREFIX__thread( std::string module_prefix, yarp::os::ResourceFinder rf, std::shared_ptr<paramHelp::ParamHelperServer> ph );
    
    
    /**
     * @brief _MODULE_PREFIX_ control thread initialization
     * 
     * @return true on succes, false otherwise
     */
    virtual bool custom_init();
    
    /**
     * @brief _MODULE_PREFIX_ control thread main loop
     * 
     */
    virtual void run();
    
};

#endif
