#include <yarp/os/all.h>
#include <cstdlib>

#include "tutorial_control_thread.h"
#include "tutorial_constants.h"

tutorial_control_thread::tutorial_control_thread(   std::string module_prefix, 
                                                    yarp::os::ResourceFinder rf, 
                                                    std::shared_ptr< paramHelp::ParamHelperServer > ph) :   left_arm_chain_interface( "left_arm", module_prefix, get_robot_name() ),
                                                                                                            num_joints( left_arm_chain_interface.getNumberOfJoints() ),
                                                                                                            left_arm_configuration( num_joints ),
                                                                                                            ref_speed_vector( num_joints ),
                                                                                                            command_interface( module_prefix ),
                                                                                                            generic_thread( module_prefix, rf, ph )
{
    // position mode on left arm chain interface
    left_arm_chain_interface.setPositionMode();
}

void tutorial_control_thread::link_tutorial_params()
{
    // get a shared pointer to param helper
    std::shared_ptr<paramHelp::ParamHelperServer> ph = get_param_helper();
    // link the left_arm configuration (vector linking)
    ph->linkParam( PARAM_ID_LEFT_ARM, left_arm_configuration.data() );
    // link the max_vel parameter (single value linking
    ph->linkParam( PARAM_ID_MAX_VEL, &max_vel );
}


bool tutorial_control_thread::custom_init()
{   
    // link the tutorial additional params to param helper
    link_tutorial_params(); 
    return true;
}

void tutorial_control_thread::run()
{   
    std::string cmd = command_interface.getCommand();
    // when we receive the string "test_cmd" through the command interface, go to the desired configuration
    if( cmd == "test_cmd" ) {
        
        // set the ref speed for all the joints
        left_arm_chain_interface.setReferenceSpeed( max_vel );
        
        // position move to desired configuration
        left_arm_chain_interface.move(left_arm_configuration);
    }
    else if( cmd != "" ) {
	std::cout << cmd <<  " -> command not valid" << std::endl;
    }
}

bool tutorial_control_thread::custom_pause()
{
    // set the ref speed to 0 for all the joints
    left_arm_chain_interface.setReferenceSpeed( 0 );
}

bool tutorial_control_thread::custom_resume()
{
    // set the ref speed to max_vel for all the joints
    left_arm_chain_interface.setReferenceSpeed( max_vel );
}


