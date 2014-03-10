#ifndef _YARP_INTERFACE_H_
#define _YARP_INTERFACE_H_

#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <vector>
#define FT_ENABLED false
#define FT_PORT true

typedef std::string command;

namespace walkman
{
namespace drc
{

class interface
{
protected:
    command command_i;
    int amount_i;
    int seq_num_i;
    bool start_i;
    bool stop_i;
public:
    virtual void getCommand(command& cmd, int& seq_num)=0;

    virtual bool getStart() {
        return start_i;
    }
    virtual bool getStop() {
        return stop_i;
    }

    virtual void setStatus(status status_o, int seq_num_o) = 0;
};


class yarp_interface: interface
{
public:
    yarp_interface();
    ~yarp_interface();
    
    void setMaxSpeed ( double max_speed );
    bool getStart();
    bool getStop();

    inline const std::vector<int>& getNumberOfJoints()
    {
        return joint_numbers;
    }


    void getCommand ( command& cmd, int& seq_num );
    void getCommand(command& cmd, double& amount, int& seq_num);
    void setStatus(status status_o, int seq_num_o);
    void setStatus(status status_o, int seq_num_o, int status_count_check);
    
    void sense(yarp::sig::Vector &left_leg_q,
               yarp::sig::Vector &right_leg_q,
               yarp::sig::Vector &left_foot_FT,
               yarp::sig::Vector &right_foot_FT);

    void move(const yarp::sig::Vector &left_leg_q_d,
              const yarp::sig::Vector &right_leg_q_d);
    void move(const yarp::sig::Vector &left_leg_q_d,
              const yarp::sig::Vector &right_leg_q_d,
              const double max_speed);
    void moveAndCheck(const yarp::sig::Vector &left_leg_q_d,
                      const yarp::sig::Vector &right_leg_q_d,
                      const double max_speed);

    bool sendTrj() {
        return send_trj;
    }
    void stop() {
        send_trj = false;
    }
    
private:
    
    bool send_trj;
    bool set_position_mode;
    yarp::os::BufferedPort<yarp::os::Bottle> port_send_trj;
    yarp::os::Port right_arm_configuration_ref_port;
    yarp::os::Port left_arm_configuration_ref_port;
    yarp::os::Port torso_configuration_ref_port;
    yarp::os::Port right_leg_configuration_ref_port;
    yarp::os::Port left_leg_configuration_ref_port;
    yarp::os::Port status_port;
    
private:
    int status_port_counter;
    yarp::os::Port status_port;
    yarp::os::BufferedPort<yarp::os::Bottle> command_port;
    
    int left_arm_dofs;
    int left_hand_dofs;
    int right_arm_dofs;
    int right_hand_dofs;
    int right_leg_dofs;
    int left_leg_dofs;
    int ft_left_leg_channels;
    int ft_right_leg_channels;
    
    static const char * kinematic_chains;
    bool isTorsoAvailable;
    bool isLeftArmAvailable;
    bool isRightArmAvailable;
    bool isLeftLegAvailable;
    bool isRightLegAvailable;

    void checkInput();

    void fillBottleAndSend(const yarp::sig::Vector& q_d, const std::string& kinematic_chain);
    void fillStatusBottleAndSend(const std::string& status);
    void setPositionControlModeKinematicChain(const std::string& kinematic_chain);
    void moveKinematicChain(const yarp::sig::Vector& q_d, const std::string& kinematic_chain);

private:

    std::vector<int> joint_numbers;
    yarp::os::Network yarp;
    
    yarp::dev::IEncodersTimed *encodersMotor_left_hand;
    yarp::dev::IEncodersTimed *encodersMotor_right_hand;
    yarp::dev::IEncodersTimed *encodersMotor_left_arm;
    yarp::dev::IEncodersTimed *encodersMotor_right_arm;
    yarp::dev::IEncodersTimed *encodersMotor_torso;
    yarp::dev::IEncodersTimed *encodersMotor_left_leg;
    yarp::dev::IEncodersTimed *encodersMotor_right_leg;
    
    yarp::dev::IPositionDirect *positionDirect_left_hand;
    yarp::dev::IPositionDirect *positionDirect_right_hand;
    yarp::dev::IPositionDirect *positionDirect_left_leg;
    yarp::dev::IPositionDirect *positionDirect_right_leg;
    yarp::dev::IPositionDirect *positionDirect_torso;
    yarp::dev::IPositionDirect *positionDirect_left_arm;
    yarp::dev::IPositionDirect *positionDirect_right_arm;
    
    yarp::dev::IControlMode *controlMode_torso;
    yarp::dev::IControlMode *controlMode_left_arm;
    yarp::dev::IControlMode *controlMode_right_arm;
    yarp::dev::IControlMode *controlMode_left_leg;
    yarp::dev::IControlMode *controlMode_right_leg;
    yarp::dev::IControlMode *controlMode_left_hand;
    yarp::dev::IControlMode *controlMode_right_hand;
    
    yarp::dev::PolyDriver polyDriver_torso;
    yarp::dev::PolyDriver polyDriver_left_arm;
    yarp::dev::PolyDriver polyDriver_right_arm;
    yarp::dev::PolyDriver polyDriver_left_leg;
    yarp::dev::PolyDriver polyDriver_right_leg;
    yarp::dev::PolyDriver polyDriver_left_hand;
    yarp::dev::PolyDriver polyDriver_right_hand;
    
    yarp::dev::IPositionControl2 *positionControl_left_leg;
    yarp::dev::IPositionControl2 *positionControl_right_leg;
    yarp::dev::IPositionControl2 *positionControl_left_hand;
    yarp::dev::IPositionControl2 *positionControl_right_hand;
    yarp::dev::IPositionControl2 *positionControl_left_arm;
    yarp::dev::IPositionControl2 *positionControl_right_arm;    

#ifdef FT_ENABLED

#if(FT_PORT)
    yarp::os::BufferedPort<yarp::os::Bottle> FT_left_arm_port;
    yarp::os::BufferedPort<yarp::os::Bottle> FT_right_arm_port;
#else
    yarp::dev::IAnalogSensor *FT_left_arm;
    yarp::dev::IAnalogSensor *FT_right_arm;

    yarp::dev::PolyDriver polyDriver_left_arm_FT;
    yarp::dev::PolyDriver polyDriver_right_arm_FT;
#endif
#if(FT_PORT)
    yarp::os::BufferedPort<yarp::os::Bottle> FT_left_leg_port;
    yarp::os::BufferedPort<yarp::os::Bottle> FT_right_leg_port;
#else
    yarp::dev::IAnalogSensor *FT_left_leg;
    yarp::dev::IAnalogSensor *FT_right_leg;
    yarp::dev::PolyDriver polyDriver_left_leg_FT;
    yarp::dev::PolyDriver polyDriver_right_leg_FT;
#endif
#endif

    yarp::os::Port status_port;

    yarp::os::BufferedPort<yarp::os::Bottle> command_port;
    yarp::os::BufferedPort<yarp::os::Bottle> command_KBD_port;
    yarp::os::BufferedPort<yarp::os::Bottle> start_port;
    yarp::os::BufferedPort<yarp::os::Bottle> stop_port;
    yarp::os::BufferedPort<yarp::os::Bottle> pause_port;

    bool createPolyDriver ( const std::string &kinematic_chain, yarp::dev::PolyDriver &polyDriver );

};

}
}

#endif
