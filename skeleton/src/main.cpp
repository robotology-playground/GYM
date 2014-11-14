#include <yarp/os/all.h>
#include <GYM/generic_module.hpp>
#include <cstdlib>

#include "_MODULE_PREFIX__module.hpp"

// default module period
#define MODULE_PERIOD 1000 //[millisec]

int main(int argc, char* argv[])
{
    // yarp network declaration and check
    yarp::os::Network yarp;
    if(!yarp.checkNetwork()){
        std::cerr <<"yarpserver not running - run yarpserver"<< std::endl;
        exit(EXIT_FAILURE);
    }
    // yarp network initialization
    yarp.init();

    // create rf
    yarp::os::ResourceFinder rf;
    rf.setVerbose(true);
    // set _MODULE_PREFIX__initial_config.ini as default
    // to specify another config file, run with this arg: --from your_config_file.ini 
    rf.setDefaultConfigFile( "_MODULE_PREFIX__initial_config.ini" ); 
    rf.setDefaultContext( "_MODULE_PREFIX_" );  
    rf.configure(argc, argv);
    // create my module
    _MODULE_PREFIX__module _MODULE_PREFIX__mod = _MODULE_PREFIX__module( argc, argv, "_MODULE_PREFIX_", MODULE_PERIOD, rf );
        
    // yarp network deinitialization
    yarp.fini();
    
    // run the module
    _MODULE_PREFIX__mod.runModule( rf );
    
    exit(EXIT_SUCCESS);
}
