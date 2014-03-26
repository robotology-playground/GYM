#include <yarp_interface.h>

#include "yarp_interface.h"
#include <iterator>
#include <algorithm>
#include <vector>
#include <iostream>
#include <assert.h>

using namespace walkman::drc;


yarp_single_chain_interface::yarp_single_chain_interface(std::string kinematic_chain,std::string module_prefix):module_prefix(module_prefix),kinematic_chain(kinematic_chain)
{
    if(createPolyDriver(kinematic_chain.c_str(), polyDriver))
    {
        polyDriver.view(encodersMotor);
        polyDriver.view(positionDirect);
        polyDriver.view(controlMode);
        polyDriver.view(positionControl);
        isAvailable = true;
    }
    
}

yarp::sig::Vector yarp_single_chain_interface::sense(){
    
}

void yarp_single_chain_interface::sense(yarp::sig::Vector& q_sensed){
    
}

void yarp_single_chain_interface::move(const yarp::sig::Vector& q_d){
    
}

bool yarp_single_chain_interface::createPolyDriver(const std::string& kinematic_chain, yarp::dev::PolyDriver& polyDriver)
{
    yarp::os::Property options;
    options.put("robot", "coman");
    options.put("device", "remote_controlboard");
    
    yarp::os::ConstString s;
    s = module_prefix + kinematic_chain;
    
    options.put("local", s.c_str());
    
    yarp::os::ConstString ss;
    ss = "/coman/" + kinematic_chain;
    options.put("remote", ss.c_str());
    
    polyDriver.open(options);
    if (!polyDriver.isValid()) {
        std::cout<<"Device "<<kinematic_chain<<" not available."<<std::endl;
        return false;
    }
    else {
        std::cout<<"Device "<<kinematic_chain<<" available."<<std::endl;
        return true;
    }
}



/**
yarp_interface::yarp_interface()
{
    isTorsoAvailable = false;
    isLeftArmAvailable = false;
    isRightArmAvailable = false;
    isLeftLegAvailable = false;
    isRightLegAvailable = false;

    if(createPolyDriver("torso", polyDriver_torso))
    {
        polyDriver_torso.view(encodersMotor_torso);
        polyDriver_torso.view(positionDirect_torso);
        polyDriver_torso.view(controlMode_torso);
        torso_configuration_ref_port.open("/simple_homing/torso/reference:o");
        isTorsoAvailable = true;
    }
    if(createPolyDriver("left_arm", polyDriver_left_arm))
    {
        polyDriver_left_arm.view(encodersMotor_left_arm);
        polyDriver_left_arm.view(positionControl_left_arm);
        polyDriver_left_arm.view(controlMode_left_arm);
        left_arm_configuration_ref_port.open("/simple_homing/left_arm/reference:o");
        isLeftArmAvailable = true;
    }
    if(createPolyDriver("right_arm", polyDriver_right_arm))
    {
        polyDriver_right_arm.view(encodersMotor_right_arm);
        polyDriver_right_arm.view(positionControl_right_arm);
        polyDriver_right_arm.view(controlMode_right_arm);
        right_arm_configuration_ref_port.open("/simple_homing/right_arm/reference:o");
        isRightArmAvailable = true;
    }
    if(createPolyDriver("left_leg", polyDriver_left_leg))
    {
        polyDriver_left_leg.view(encodersMotor_left_leg);
        polyDriver_left_leg.view(positionControl_left_leg);
        polyDriver_left_leg.view(controlMode_left_leg);
        left_leg_configuration_ref_port.open("/simple_homing/left_leg/reference:o");
        isLeftLegAvailable = true;
    }
    if(createPolyDriver("right_leg", polyDriver_right_leg))
    {
        polyDriver_right_leg.view(encodersMotor_right_leg);
        polyDriver_right_leg.view(positionControl_right_leg);
        polyDriver_right_leg.view(controlMode_right_leg);
        right_leg_configuration_ref_port.open("/simple_homing/right_leg/reference:o");
        isRightLegAvailable = true;
    }

    send_trj = false;
    set_position_mode = false;

    port_send_trj.open("/simple_homing/do_homing:i");
    status_port.open("/simple_homing/status:o");
    
    
    if(createPolyDriver("left_leg", polyDriver_left_leg))
    {
        polyDriver_left_leg.view(encodersMotor_left_leg);
        polyDriver_left_leg.view(directControl_left_leg);
        polyDriver_left_leg.view(positionControl_left_leg);
        polyDriver_left_leg.view(controlMode_left_leg);
    }
    if(createPolyDriver("right_leg", polyDriver_right_leg))
    {
        polyDriver_right_leg.view(encodersMotor_right_leg);
        polyDriver_right_leg.view(directControl_right_leg);
        polyDriver_right_leg.view(positionControl_right_leg);
        polyDriver_right_leg.view(controlMode_right_leg);
    }
    
    #if (FT_PORT==TRUE)
    FT_left_leg_port.open("/coman/left_leg/analog:o/forceTorque");
    FT_right_leg_port.open("/coman/right_leg/analog:o/forceTorque");
    #else
    yarp::os::Property FT_left_prop;
    FT_left_prop.put("device", "analogsensorclient");
    FT_left_prop.put("robotName", "coman");
    FT_left_prop.put("remote", "/coman/left_leg/analog:o/forceTorque");
    FT_left_prop.put("local", "/FT_client_left_leg:o");
    FT_left_prop.put("rate", 1);
    
    polyDriver_left_leg_FT.open(FT_left_prop);
    if(!polyDriver_left_leg_FT.isValid())
        printf("error opening analogSensor");
    
    polyDriver_left_leg_FT.view(this->FT_left_leg);
    
    
    yarp::os::Property FT_right_prop;
    FT_right_prop.put("device", "analogsensorclient");
    FT_right_prop.put("robotName", "coman");
    FT_right_prop.put("remote", "/coman/right_leg/analog:o/forceTorque");
    FT_right_prop.put("local", "/FT_client_right_leg:o");
    FT_right_prop.put("rate", 1);
    
    polyDriver_right_leg_FT.open(FT_right_prop);
    if(!polyDriver_right_leg_FT.isValid())
        printf("error opening analogServer");
    
    polyDriver_right_leg_FT.view(this->FT_right_leg);
    #endif
    
    encodersMotor_left_leg->getAxes(&left_leg_dofs);
    encodersMotor_right_leg->getAxes(&right_leg_dofs);
    
    #if (FT_PORT==TRUE)
    ft_left_leg_channels = 6; //Here we consider classic FT sensors
    ft_right_leg_channels = 6;
    #else
    ft_left_leg_channels = FT_left_leg->getChannels();
    ft_right_leg_channels = FT_right_leg->getChannels();
    #endif
    
    
    for(unsigned int i = 0; i < left_leg_dofs; ++i)
    {
        controlMode_left_leg->setPositionMode(i);
    }
    
    for(unsigned int i = 0; i < right_leg_dofs; ++i)
    {
        controlMode_right_leg->setPositionMode(i);
    }
    
    command_port.open("/flat_walk/command:i");
    status_port_counter = 0;
    status_port.open("/flat_walk/status:o");


    command_i = WALKMAN_DRC_VALVE_COMMAND_NONE;
    amount_i = 0;
    seq_num_i = -1;
    start_i = false;
    stop_i = false;
    
    if(createPolyDriver("left_arm", polyDriver_left_arm))
    {
        polyDriver_left_arm.view(encodersMotor_left_arm);
        polyDriver_left_arm.view(positionControl_left_arm);
        polyDriver_left_arm.view(controlMode_left_arm);
        polyDriver_left_arm.view(positionDirect_left_arm);
    }
    if(createPolyDriver("right_arm", polyDriver_right_arm))
    {
        polyDriver_right_arm.view(encodersMotor_right_arm);
        polyDriver_right_arm.view(positionControl_right_arm);
        polyDriver_right_arm.view(controlMode_right_arm);
        polyDriver_right_arm.view(positionDirect_right_arm);
    }
    
    if(createPolyDriver("left_hand", polyDriver_left_hand))
    {
        polyDriver_left_hand.view(encodersMotor_left_hand);
        polyDriver_left_hand.view(positionControl_left_hand);
        polyDriver_left_hand.view(controlMode_left_hand);
        polyDriver_left_hand.view(positionDirect_left_hand);
    }
    if(createPolyDriver("right_hand", polyDriver_right_hand))
    {
        polyDriver_right_hand.view(encodersMotor_right_hand);
        polyDriver_right_hand.view(positionControl_right_hand);
        polyDriver_right_hand.view(controlMode_right_hand);
        polyDriver_right_hand.view(positionDirect_right_hand);
    }
    
    #if(FT_PORT)
    FT_left_arm_port.open("/coman/left_arm/analog:o/forceTorque");
    FT_right_arm_port.open("/coman/right_arm/analog:o/forceTorque");
    #else
    yarp::os::Property FT_left_prop;
    FT_left_prop.put("device", "analogsensorclient");
    FT_left_prop.put("robotName", "coman");
    FT_left_prop.put("remote", "/coman/left_arm/analog:o/forceTorque");
    FT_left_prop.put("local", "/FT_client_left_arm:o");
    FT_left_prop.put("rate", 1);
    
    polyDriver_left_arm_FT.open(FT_left_prop);
    if(!polyDriver_left_arm_FT.isValid())
        printf("error opening analogSensor");
    
    polyDriver_left_arm_FT.view(this->FT_left_arm);
    
    
    yarp::os::Property FT_right_prop;
    FT_right_prop.put("device", "analogsensorclient");
    FT_right_prop.put("robotName", "coman");
    FT_right_prop.put("remote", "/coman/right_arm/analog:o/forceTorque");
    FT_right_prop.put("local", "/FT_client_right_arm:o");
    FT_right_prop.put("rate", 1);
    
    polyDriver_right_arm_FT.open(FT_right_prop);
    if(!polyDriver_right_arm_FT.isValid())
        printf("error opening analogServer");
    
    polyDriver_right_arm_FT.view(this->FT_right_arm);
    
    #endif
    
    command_KBD_port.open("/turn_valve/cmd_kbd:i");
    
    valve_data_port.open("/turn_valve/valve_data:i");
    command_port.open("/turn_valve/control:i");
    status_port.open("/turn_valve/status:o");
    
    left_hand_dofs = -1;
    right_hand_dofs = -1;
    
    left_arm_dofs = -1;
    right_arm_dofs = -1;
    int temp_storage=0;
    if(left_arm_dofs == -1) {
        positionControl_left_arm->getAxes(&temp_storage);
    }
    left_arm_dofs=temp_storage;
    
    if(right_arm_dofs == -1) {
        positionControl_right_arm->getAxes(&temp_storage);
    }
    right_arm_dofs=temp_storage;
    
    
    temp_storage=0;
    if(left_hand_dofs == -1) {
        positionControl_left_hand->getAxes(&temp_storage);
    }
    left_hand_dofs=temp_storage;
    
    if(right_hand_dofs == -1) {
        positionControl_right_hand->getAxes(&temp_storage);
    }
    right_hand_dofs=temp_storage;
    
    
    joint_numbers.push_back(left_arm_dofs);
    joint_numbers.push_back(right_arm_dofs);
    joint_numbers.push_back(left_hand_dofs);
    joint_numbers.push_back(right_hand_dofs);
    
    int total_dofs =    left_arm_dofs +
    left_hand_dofs +
    right_arm_dofs +
    right_hand_dofs;
    int left_arm_total_dofs =   left_arm_dofs +
    left_hand_dofs;
    int right_arm_total_dofs =  right_arm_dofs +
    right_hand_dofs;
    if(input.q.size()!= total_dofs) {
        input.q.resize(total_dofs, 0.0);
    }
    
    if(outputs.q.size()!=   total_dofs) {
        outputs.q.resize(total_dofs,
                         0.0);
    }
    
    if(input.q_dot.size()!=total_dofs) {
        input.q_dot.resize(total_dofs,
                           0.0);
    }
    input.tau_left.resize(left_arm_total_dofs);
    input.tau_right.resize(right_arm_total_dofs);
    //TODO place in the right piece of code (flat_walk_yarp) in the state machine
    positionControl_left_arm->setPositionMode();
    positionControl_right_arm->setPositionMode();
    
    positionControl_left_hand->setPositionMode();
    positionControl_right_hand->setPositionMode();
    
    
}

void yarp_interface::checkInput()
{
    yarp::os::Bottle* bot_send_trj = port_send_trj.read(false);
    if(bot_send_trj != NULL) {
        send_trj = (bool)bot_send_trj->get(0).asInt();
        set_position_mode = false;
    }

    if(send_trj && !set_position_mode)
    {
        setPositionControlModeKinematicChain("torso");
        setPositionControlModeKinematicChain("left_arm");
        setPositionControlModeKinematicChain("right_arm");
        setPositionControlModeKinematicChain("left_leg");
        setPositionControlModeKinematicChain("right_leg");
        set_position_mode = true;
    }
}

yarp_interface::~yarp_interface()
{
    if(polyDriver_torso.isValid())
        polyDriver_torso.close();
    if(polyDriver_left_arm.isValid())
        polyDriver_left_arm.close();
    if(polyDriver_right_arm.isValid())
        polyDriver_right_arm.close();
    if(polyDriver_left_leg.isValid())
        polyDriver_left_leg.close();
    if(polyDriver_right_leg.isValid())
        polyDriver_right_leg.close();
    port_send_trj.close();
    right_arm_configuration_ref_port.close();
    left_arm_configuration_ref_port.close();
    torso_configuration_ref_port.close();
    right_leg_configuration_ref_port.close();
    left_leg_configuration_ref_port.close();
    status_port.close();
    command_port.close();
    status_port.close();
    command_port.close();
    start_port.close();
    stop_port.close();
    pause_port.close();
    polyDriver_left_arm.close();
    polyDriver_right_arm.close();
    polyDriver_left_hand.close();
    polyDriver_right_hand.close();
    
}

bool yarp_interface::createPolyDriver(const std::string& kinematic_chain, yarp::dev::PolyDriver& polyDriver)
{
    yarp::os::Property options;
    options.put("robot", "coman");
    options.put("device", "remote_controlboard");

    yarp::os::ConstString s;
    s = "/simple_homing/" + kinematic_chain;
    s = "/flat_walk/" + kinematic_chain;
    s = "/turn_valve/" + kinematic_chain;
    
    options.put("local", s.c_str());

    yarp::os::ConstString ss;
    ss = "/coman/" + kinematic_chain;
    options.put("remote", ss.c_str());

    polyDriver.open(options);
    if (!polyDriver.isValid()) {
        std::cout<<"Device "<<kinematic_chain<<" not available."<<std::endl;
        return false;
    }
    else {
        std::cout<<"Device "<<kinematic_chain<<" available."<<std::endl;
        return true;
    }
}

void yarp_interface::fillBottleAndSend(const yarp::sig::Vector &q_d, const std::string &kinematic_chain)
{
    yarp::os::Bottle bot;
    for(unsigned int i = 0; i < q_d.size(); ++i)
        bot.addDouble(q_d[i]);

    if(kinematic_chain.compare("torso") == 0)
        torso_configuration_ref_port.write(bot);
    else if(kinematic_chain.compare("left_arm") == 0)
        left_arm_configuration_ref_port.write(bot);
    else if(kinematic_chain.compare("right_arm") == 0)
        right_arm_configuration_ref_port.write(bot);
    else if(kinematic_chain.compare("left_leg") == 0)
        left_leg_configuration_ref_port.write(bot);
    else if(kinematic_chain.compare("right_leg") == 0)
        right_leg_configuration_ref_port.write(bot);
}

void yarp_interface::fillStatusBottleAndSend(const std::string &status)
{
    yarp::os::Bottle bot;
    bot.addString(status.c_str());
    status_port.write(bot);
}

void yarp_interface::moveKinematicChain(const yarp::sig::Vector &q_d, const std::string &kinematic_chain)
{
    if(kinematic_chain.compare("torso") == 0)
        if(!positionDirect_torso->setPositions(q_d.data()))
            std::cout<<"Cannot move torso using Direct Position Ctrl"<<std::cout;
#if USE_POSITION_CONTROL_LEFT_ARM
    if(kinematic_chain.compare("left_arm") == 0)
        if(!positionControl_left_arm->setPositions(q_d.data()))
            std::cout<<"Cannot move left_arm using Direct Position Ctrl"<<std::cout;
#endif
#if USE_POSITION_CONTROL_RIGHT_ARM
    if(kinematic_chain.compare("right_arm") == 0)
        if(!positionControl_right_arm->setPositions(q_d.data()))
            std::cout<<"Cannot move right_arm using Direct Position Ctrl"<<std::cout;
#endif
    if(kinematic_chain.compare("left_leg") == 0)
        if(!positionDirect_left_leg->setPositions(q_d.data()))
            std::cout<<"Cannot move left_leg using Direct Position Ctrl"<<std::cout;
    if(kinematic_chain.compare("right_leg") == 0)
        if(!positionDirect_right_leg->setPositions(q_d.data()))
            std::cout<<"Cannot move right_leg using Direct Position Ctrl"<<std::cout;
}

void yarp_interface::setPositionControlModeKinematicChain(const std::string &kinematic_chain)
{
    int number_of_joints = 0;
    if(kinematic_chain.compare("torso") == 0)
    {
        encodersMotor_torso->getAxes(&number_of_joints);
        for(unsigned int i = 0; i < number_of_joints; ++i)
            controlMode_torso->setPositionMode(i);
    }
#if USE_POSITION_CONTROL_LEFT_ARM
    else if(kinematic_chain.compare("left_arm") == 0)
    {
        encodersMotor_left_arm->getAxes(&number_of_joints);
        for(unsigned int i = 0; i < number_of_joints; ++i)
            controlMode_left_arm->setPositionMode(i);
    }
#endif
#if USE_POSITION_CONTROL_RIGHT_ARM
    else if(kinematic_chain.compare("right_arm") == 0)
    {
        encodersMotor_right_arm->getAxes(&number_of_joints);
        for(unsigned int i = 0; i < number_of_joints; ++i)
            controlMode_right_arm->setPositionMode(i);
    }
#endif
    else if(kinematic_chain.compare("left_leg") == 0)
    {
        encodersMotor_left_leg->getAxes(&number_of_joints);
        for(unsigned int i = 0; i < number_of_joints; ++i)
            controlMode_left_leg->setPositionMode(i);
    }
    else if(kinematic_chain.compare("right_leg") == 0)
    {
        encodersMotor_right_leg->getAxes(&number_of_joints);
        for(unsigned int i = 0; i < number_of_joints; ++i)
            controlMode_right_leg->setPositionMode(i);
    }
}

void yarp_interface::getCommand(command& cmd, double& amount, int& seq_num) {
    yarp::os::Bottle* bot_command = command_port.read(false);

    command command_i = NONE;
    double amount_i = 0.0;
    int seq_num_i = -1;

    if(bot_command != NULL) {

        std::string command_string = bot_command->get(0).asString();
        if(command_string.compare(WALKMAN_DRC_FLAT_WALK_COMMAND_FORWARD)==0) {
            std::cout<<"Get Forward..."<<std::endl;
            command_i = FORWARD;
        } else if(command_string.compare(WALKMAN_DRC_FLAT_WALK_COMMAND_BACKWARDS)==0) {
            command_i = BACKWARDS;
        } else if(command_string.compare(WALKMAN_DRC_FLAT_WALK_COMMAND_RIGHT)==0) {
            command_i = RIGHT;
        } else if(command_string.compare(WALKMAN_DRC_FLAT_WALK_COMMAND_LEFT)==0) {
            command_i = LEFT;
        } else if(command_string.compare(WALKMAN_DRC_FLAT_WALK_COMMAND_ROTATE_LEFT)==0) {
            command_i = ROTATE_LEFT;
        } else if(command_string.compare(WALKMAN_DRC_FLAT_WALK_COMMAND_ROTATE_RIGHT)==0) {
            command_i = ROTATE_RIGHT;
        }
        amount_i = bot_command->get(1).asDouble();
        seq_num_i = bot_command->get(2).asInt();
    }
    yarp::os::Bottle* bot_command = command_port.read(false);
    if(bot_command!=NULL) {
        std::string command_string = bot_command->get(0).asString();
        cmd=command_string;
        seq_num_i = bot_command->get(1).asInt();
    }
    else
    {
        cmd=WALKMAN_DRC_VALVE_COMMAND_NONE;
        seq_num=seq_num_i;
    }
    
    cmd = command_i;
    amount = amount_i;
    seq_num = seq_num_i;
}


void yarp_interface::setStatus(status status_o, int seq_num_o)
{
    yarp::os::Bottle bot;
    std::string status_string;
    switch(status_o) {
    case STOPPED:
        status_string = WALKMAN_DRC_FLAT_WALK_STATUS_STOPPED;
        break;
    case MOVING:
        status_string = WALKMAN_DRC_FLAT_WALK_STATUS_MOVING;
        break;
    case INVALID:
        status_string = WALKMAN_DRC_FLAT_WALK_STATUS_INVALID;
        break;
    case ABORT:
        status_string = WALKMAN_DRC_FLAT_WALK_STATUS_ABORTING;
        break;
    case READY:
    default:
        status_string = WALKMAN_DRC_FLAT_WALK_STATUS_READY;
        break;
    }

    bot.addString(status_string.c_str());
    bot.addInt(seq_num_o);
    status_port.write(bot);
    status_port_counter = 0;
}

void yarp_interface::setStatus(status status_o, int seq_num_o, int status_count_check)
{
    if(status_port_counter==status_count_check) {
        setStatus(status_o, seq_num_o);
        status_port_counter = 0;
    }

    status_port_counter++;
}

void yarp_interface::setStatus(status status_o, int seq_num_o)
{
    yarp::os::Bottle bot;
    std::string status_string;
    switch(status_o) {
        case STOPPED:
            status_string = WALKMAN_DRC_VALVE_STATUS_WAITING;
            break;
        case MOVING:
            status_string = WALKMAN_DRC_VALVE_STATUS_REACHING;
            break;
        case READY:
        default:
            status_string = WALKMAN_DRC_VALVE_STATUS_READY;
            break;
    }
    
    bot.addString(status_string.c_str());
    bot.addInt(seq_num_o);
    status_port.write(bot);
}

//void yarp_interface::setSteps(int n_steps) {
//    yarp::os::Bottle bot;
//    bot.addInt(n_steps);
//    steps_port.write(bot);
//}


void yarp_interface::sense(yarp::sig::Vector &left_leg_q,
                           yarp::sig::Vector &right_leg_q,
                           yarp::sig::Vector &left_foot_FT,
                           yarp::sig::Vector &right_foot_FT) {
    if(left_leg_q.size()!=left_leg_dofs) {
        left_leg_q.resize(left_leg_dofs,0.0);
    }

    if(right_leg_q.size()!=right_leg_dofs) {
        right_leg_q.resize(right_leg_dofs,0.0);
    }
    if(left_foot_FT.size()!= ft_left_leg_channels) {
        left_foot_FT.resize(ft_left_leg_channels,0.0);
    }
    if(right_foot_FT.size()!=ft_right_leg_channels) {
        right_foot_FT.resize(ft_right_leg_channels,0.0);
    }

    encodersMotor_left_leg->getEncoders(left_leg_q.data());
    encodersMotor_right_leg->getEncoders(right_leg_q.data());
    //std::cout<<"l_leg_conf_read: "<<left_leg_q.toString()<<std::endl;

#if(FT_PORT)
    yarp::os::Bottle* ft_leg_bot = NULL;
    ft_leg_bot = FT_left_leg_port.read(false);
    if(!(ft_leg_bot == NULL) && !ft_leg_bot->isNull())
    {
        for(unsigned int i = 0; i < ft_leg_bot->size(); ++i)
            left_foot_FT[i] = ft_leg_bot->get(i).asDouble();
    }
    ft_leg_bot = FT_right_leg_port.read(false);
    if(!(ft_leg_bot == NULL) && !ft_leg_bot->isNull())
    {
        for(unsigned int i = 0; i < ft_leg_bot->size(); ++i)
            right_foot_FT[i] = ft_leg_bot->get(i).asDouble();
    }
#else
    FT_left_leg->read(left_foot_FT);
    FT_right_leg->read(right_foot_FT);
#endif
}

const robot_state_input& yarp_interface::sense()
{
    encodersMotor_left_arm->getEncoders(input.q.data());
    encodersMotor_left_hand->getEncoders(&input.q[  left_arm_dofs]);
    
    encodersMotor_right_arm->getEncoders(&input.q[  left_arm_dofs +
    left_hand_dofs]);
    encodersMotor_right_hand->getEncoders(&input.q[ left_arm_dofs +
    left_hand_dofs +
    right_arm_dofs]);
    
    encodersMotor_left_arm->getEncoderSpeeds(input.q_dot.data());
    encodersMotor_left_hand->getEncoderSpeeds(&input.q_dot[  left_arm_dofs]);
    encodersMotor_right_arm->getEncoderSpeeds(&input.q_dot[  left_arm_dofs +
    left_hand_dofs]);
    encodersMotor_right_hand->getEncoderSpeeds(&input.q_dot[ left_arm_dofs +
    left_hand_dofs +
    right_arm_dofs]);
    //    std::cout<<"q:     "<<input.q.toString()<<std::endl;
    //    std::cout<<"q_dot: "<<input.q_dot.toString()<<std::endl;
    
    #ifdef FT_ENABLED
    #if(FT_PORT)
    yarp::os::Bottle* ft_arm_bot = FT_left_arm_port.read(false);
    if(!(ft_arm_bot == NULL) && !ft_arm_bot->isNull())
    {
        for(unsigned int i = 0; i < ft_arm_bot->size(); ++i)
            input.tau_left[i] = ft_arm_bot->get(i).asDouble();
    }
    ft_arm_bot = FT_right_arm_port.read(false);
    if(!(ft_arm_bot == NULL) && !ft_arm_bot->isNull())
    {
        for(unsigned int i = 0; i < ft_arm_bot->size(); ++i)
            input.tau_right[i] = ft_arm_bot->get(i).asDouble();
    }
    #else
    FT_left_arm->read(input.tau_left);
    FT_right_arm->read(input.tau_right);
    #endif
    #endif
    return input;
}

void yarp_interface::move(const yarp::sig::Vector &left_leg_q_d,
                          const yarp::sig::Vector &right_leg_q_d)
{
    if(!directControl_right_leg->setPositions(right_leg_q_d.data()))
        std::cout << "Error moving right leg with directControl" << std::endl;
    if(!directControl_left_leg->setPositions(left_leg_q_d.data()))
        std::cout << "Error moving left leg with directControl" << std::endl;
}

void yarp_interface::move(const robot_joints_output& outputs) {
    positionDirect_left_arm->setPositions(outputs.q.data());
    positionDirect_left_hand->setPositions(&outputs.q[left_arm_dofs]);
    
    positionDirect_right_arm->setPositions(&outputs.q[  left_arm_dofs +
    left_hand_dofs]);
    positionDirect_right_hand->setPositions(&outputs.q[ left_arm_dofs +
    left_hand_dofs +
    right_arm_dofs]);
    //    positionControl_left_arm->positionMove(outputs.q.data());
    //    positionControl_right_arm->positionMove(&outputs.q[left_arm_dofs]);
}

void yarp_interface::move(const yarp::sig::Vector &left_leg_q_d,
                          const yarp::sig::Vector &right_leg_q_d,
                          const double max_speed)
{
    double speeds[6];
    for(int i = 0; i < 6; ++i)
        speeds[i] = max_speed;

    positionControl_left_leg->setRefSpeeds(speeds);
    if(!positionControl_left_leg->positionMove(left_leg_q_d.data()))
        std::cout << "Error moving left leg"  << std::endl;

    positionControl_right_leg->setRefSpeeds(speeds);
    if(!positionControl_right_leg->positionMove(right_leg_q_d.data()))
        std::cout << "Error moving right leg"  << std::endl;
}

void yarp_interface::moveAndCheck(  const yarp::sig::Vector &left_leg_q_d,
                                    const yarp::sig::Vector &right_leg_q_d,
                                    const double max_speed)
{
    move(left_leg_q_d,right_leg_q_d, max_speed);
    bool flag_l, flag_r;
    do {
        yarp::os::Time::delay(.1);
        positionControl_left_leg->checkMotionDone(&flag_l);
        positionControl_right_leg->checkMotionDone(&flag_r);
    } while(flag_l != true || flag_r != true);
}


void yarp_interface::setMaxSpeed(double max_speed)
{
    yarp::sig::Vector max_speeds_l,max_speeds_r;
    max_speeds_l.resize(joint_numbers[0],max_speed);
    max_speeds_r.resize(joint_numbers[1],max_speed);
    positionControl_left_arm->setRefSpeeds(max_speeds_l.data());
    positionControl_right_arm->setRefSpeeds(max_speeds_r.data());
    max_speeds_l.resize(joint_numbers[2],max_speed);
    max_speeds_r.resize(joint_numbers[3],max_speed);
    positionControl_left_hand->setRefSpeeds(max_speeds_l.data());
    positionControl_right_hand->setRefSpeeds(max_speeds_r.data());
}



bool yarp_interface::getStart()
{
    yarp::os::Bottle* bot_start = start_port.read(false);
    if(!bot_start->isNull()) {
        start_i = bot_start->get(0).asBool();
        if(start_i == true)
            stop_i = false;
    }
    return start_i;
}

bool yarp_interface::getStop()
{
    yarp::os::Bottle* bot_stop = stop_port.read(false);
    if(!bot_stop->isNull()) {
        stop_i = bot_stop->get(0).asBool();
        if(stop_i == true)
            start_i = false;
    }
}


bool yarp_interface::getKBDCommand(char& cmd) {
    yarp::os::Bottle* bot_command = NULL;
    bot_command = command_KBD_port.read(false);
    if(bot_command != NULL && bot_command->size() > 0) {
        std::cout << "Reiceved command from keyboard: ";
        std::string command_string = bot_command->get(0).asString();
        if( command_string.size() > 0 &&
                command_string.at(0) != 0) {
            cmd = command_string.at(0);
            std::cout << cmd << std::endl;
            return true;
        }
    }
    return false;
}


*/
// #endif






