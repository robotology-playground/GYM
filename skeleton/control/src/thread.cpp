#include <yarp/os/all.h>

#include "_MODULE_PREFIX__thread.h"
#include "_MODULE_PREFIX__constants.h"

_MODULE_PREFIX__thread::_MODULE_PREFIX__thread( std::string module_prefix, 
                             			yarp::os::ResourceFinder rf, 
                             			std::shared_ptr< paramHelp::ParamHelperServer > ph) :
    generic_thread( module_prefix, rf, ph )
{
    // TODO: skeleton constructor
}

bool _MODULE_PREFIX__thread::custom_init()
{
    // TODO: skeleton function   
    return true;
}

void _MODULE_PREFIX__thread::run()
{   
    // TODO: skeleton function
}    
