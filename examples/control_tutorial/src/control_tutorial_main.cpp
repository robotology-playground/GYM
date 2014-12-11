#include <yarp/os/all.h>
#include <GYM/generic_module.hpp>
#include <cstdlib>

#include "control_tutorial_module.hpp"

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
    // set control_tutorial_initial_config.ini as default
    // to specify another config file, run with this arg: --from your_config_file.ini 
    rf.setDefaultConfigFile( "control_tutorial_initial_config.ini" ); 
    rf.setDefaultContext( "control_tutorial" );  
    rf.configure(argc, argv);
    // create my module
    control_tutorial_module control_tutorial_mod = control_tutorial_module( argc, argv, "control_tutorial", MODULE_PERIOD, rf );
        
    // yarp network deinitialization
    yarp.fini();
    
    // run the module
    control_tutorial_mod.runModule( rf );
    
    exit(EXIT_SUCCESS);
}
