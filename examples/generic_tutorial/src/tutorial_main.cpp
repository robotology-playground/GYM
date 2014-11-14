// include yarp and generic_module headers
#include <yarp/os/all.h>
#include <GYM/generic_module.hpp>
// include tutorial module and control thread headers 
#include "tutorial_module.hpp"
#include "tutorial_control_thread.h"

// define representing the period of the module in [milliseconds]
#define MODULE_PERIOD_MILLISEC 1000

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

    // create the resource finder for the tutorial module
    yarp::os::ResourceFinder tutorial_rf;
    tutorial_rf.setVerbose(true);
    tutorial_rf.setDefaultConfigFile( "tutorial_configuration.ini" );
    tutorial_rf.setDefaultContext( "generic_tutorial" );  
    tutorial_rf.configure(argc, argv);

    // create tutorial module
    tutorial_module tutorial_mod = tutorial_module( argc, 
                                                    argv, 
                                                    "generic_tutorial", 
                                                    MODULE_PERIOD_MILLISEC, 
                                                    tutorial_rf );
        
    // yarp network deinitialization
    yarp.fini();
    
    // run the module
    tutorial_mod.runModule( tutorial_rf );
    
    exit(EXIT_SUCCESS);
}
