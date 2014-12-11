#include <yarp/os/all.h>
#include <yarp/math/Math.h>

#include "control_tutorial_thread.h"
#include "control_tutorial_constants.h"

using namespace yarp::math;

control_tutorial_thread::control_tutorial_thread( std::string module_prefix, 
                             			yarp::os::ResourceFinder rf, 
                             			std::shared_ptr< paramHelp::ParamHelperServer > ph) :
    control_thread( module_prefix, rf, ph )
{
    left_arm_joints = robot.left_arm.getNumberOfJoints();
    omega = 0.1;
    phi = 10;
    tick = 0;
}

bool control_tutorial_thread::custom_init()
{
    robot.setPositionMode();
    robot.setReferenceSpeed(0.2);
    return true;
}

void control_tutorial_thread::run()
{   
    yarp::sig::Vector q_sensed = robot.left_arm.sensePosition();
    std::cout << "Sense : " << q_sensed.toString() << std::endl;
    
    yarp::sig::Vector q_ref(left_arm_joints, 0.0);
    for( int i = 0; i < left_arm_joints; i++) {
	q_ref[i] = q_sensed[i] + sin( (omega * tick / 5) + phi );
    }
    
    std::cout << "Rererence : " << q_ref.toString() << std::endl;
    
    robot.left_arm.move(q_ref);
    
    tick++;
}    
