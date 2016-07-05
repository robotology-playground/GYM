/*
 * Copyright (C) 2014 Walkman
 * Author: Luca Muratore
 * email:  luca.muratore@iit.it
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>
*/

#ifndef CONTROL_THREAD_HPP
#define CONTROL_THREAD_HPP

// GYM
#include <GYM/generic_thread.hpp>
// YARP
#include <yarp/os/all.h>
// idynutils
#include <idynutils/RobotUtils.h>

// param helper
#include <paramHelp/paramHelperServer.h>
// C++11 smart pointers
#include <memory>

//Trj related stuffs
#include <kdl/path_circle.hpp>
#include <kdl/path_roundedcomposite.hpp>
#include <kdl/path_line.hpp>
#include <kdl/path_point.hpp>

#include <kdl/trajectory_composite.hpp>
#include <kdl/trajectory_segment.hpp>
#include <kdl/trajectory_stationary.hpp>

#include <kdl/velocityprofile_trap.hpp>
#include <kdl/velocityprofile_spline.hpp>

#include <kdl/rotational_interpolation_sa.hpp>
//

/**
 * @brief The trj_interface class is a simplified interface to kdl trajectories
 */
class trj_interface
{
public:
    trj_interface(const double dt):
        _dt(0.0),
        _time(0.0)
    {}

    /**
     * @brief initLinearTrj initialize a linear trajectory, with trapezoidal velocity profile, from start to end
     * @param start intial frame
     * @param end final frame
     * @param max_vel max velocity of the trajectory
     * @param max_acc max acceleration of the trajectory
     * @return false if with the specified max_vel and max_acc the bang phase does not exist
     */
    bool initLinearTrj(const KDL::Frame& start, const KDL::Frame& end, const double max_vel, const double max_acc)
    {
        trj.reset();
        _time = 0.0;

        boost::shared_ptr<KDL::Path_Line> linear_path;
        double eqradius = 1.0; //Check this...
        linear_path.reset(new KDL::Path_Line(start, end, new KDL::RotationalInterpolation_SingleAxis(), eqradius));

        boost::shared_ptr<KDL::VelocityProfile> velocity_profile;
        velocity_profile.reset(new KDL::VelocityProfile_Trap(max_vel, max_acc));
        velocity_profile->SetProfile(0, linear_path->PathLength());

        double L = linear_path->PathLength();
        if(L <= (max_vel*max_vel)/max_acc){
            std::cout<<"Too fast trajectory, no Coast phase exists!"<<std::endl;
            return false;
        }

        trj.reset(new KDL::Trajectory_Segment(linear_path.get(), velocity_profile.get()));
        return true;
    }

    /**
     * @brief initLinearTrj initialize a linear trajectory, with trapezoidal velocity profile, from start to end.
     * Here we assume that bang phases and coast phases have the same duration
     * @param start
     * @param end
     * @param trj_time total time of the trj
     * @return true
     */
    bool initLinearTrj(const KDL::Frame &start, const KDL::Frame &end, const double trj_time)
    {
        boost::shared_ptr<KDL::Path_Line> linear_path;
        double eqradius = 1.0; //Check this...
        linear_path.reset(new KDL::Path_Line(start, end, new KDL::RotationalInterpolation_SingleAxis(), eqradius));

        double L = linear_path->PathLength();
        double max_vel = (3.*L)/(2.*trj_time);
        double max_acc = (9.*L)/(2.*trj_time*trj_time);

        return initLinearTrj(start, end, max_vel, max_acc);
    }

    /**
     * @brief Pos
     * @return pose of the trajectory at actual time
     */
    KDL::Frame Pos()
    {
        return trj->Pos(_time);
    }

    /**
     * @brief Vel
     * @return velocity of the trajectory at actual time
     */
    KDL::Twist Vel()
    {
        return trj->Vel(_time);
    }

    /**
     * @brief updateTrj integrate the internal time of the trj
     */
    void updateTrj()
    {
        _time += _dt;
    }

    double getTime()
    {
        return _time;
    }

    double Duration()
    {
        return trj->Duration();
    }

    bool isFinished()
    {
        return _time >= trj->Duration();
    }

    boost::shared_ptr<KDL::Trajectory> trj;

private:
    double _time;
    double _dt;

};

/**
 * @brief control thread
 * 
 * @author Luca Muratore (luca.muratore@iit.it)
 **/
class control_thread : public generic_thread
{
protected:
    RobotUtils robot;
    iDynUtils& model;
    // urdf path
    std::string urdf_path;
    // srdf path
    std::string srdf_path;
    
    trj_interface trj;

public: 
    
    /**
     * @brief constructor of the generic thread.
     *
     * @param module_prefix module name.
     * @param thread_period period of the run thread in millisecond.
     * @param rf resource finder.
     **/
    control_thread( std::string module_prefix, 
                    yarp::os::ResourceFinder rf, 
		std::shared_ptr<paramHelp::ParamHelperServer> ph  ) :   robot( 	module_prefix,
										rf.find("robot_name").asString(), 
										rf.find("urdf_path").asString(),
										rf.find("srdf_path").asString() ),
										
									model( robot.idynutils ),
									urdf_path( rf.find("urdf_path").asString() ),
									srdf_path( rf.find("srdf_path").asString() ),
                                    trj( rf.find("thread_period").asDouble()/1000. ),
									generic_thread( module_prefix, rf, ph )
    {    
    }
    
    /**
     * @brief getter method for the urdf path
     * 
     * @return the urdf path
     */
    std::string get_urdf_path() 
    {
	return urdf_path;
    }
    
    /**
     * @brief getter method for the srdf path
     * 
     * @return the srdf path
     */
    std::string get_srdf_path() 
    {
	return srdf_path;
    }
    
  
};

#endif //CONTROL_THREAD_HPP
