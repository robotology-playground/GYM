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
#include <kdl/path_line.hpp>
#include <kdl/path_point.hpp>
#include <kdl/path_composite.hpp>

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
        _dt(dt),
        _time(0.0),
        _eq_radius(0.01),
        _is_inited(false)
    {}

//    bool initCircleTrj(const KDL::Frame& start_pose, const KDL::Rotation& final_rotation,
//                       const double angle_of_rotation,
//                       const KDL::Vector& circle_center, const KDL::Vector& plane_normal)
//    {
//        trj.reset();
//        _time = 0.0;

//        _circle_path.reset(new KDL::Path_Circle());
//    }


    /**
     * @brief initCompositeTrj intialize a trajectory composed by sub-linear trajectories specified
     * by waypoints. Here we assume that bang phases and coast phases have the same duration and
     * each sub trajectory have the same duration
     * @param way_points vector, note that the first waypoint should be the actual pose
     * @param trj_time
     * @return true
     */
    bool initCompositeTrj(const std::vector<KDL::Frame>& way_points, const double trj_time)
    {
        std::vector<double> trj_times;
        for(unsigned int i = 0; i < way_points.size()-1; ++i)
            trj_times.push_back(trj_time);

        return initCompositeTrj(way_points, trj_times);
    }

    /**
     * @brief initCompositeTrj intialize a trajectory composed by sub-linear trajectories specified
     * by waypoints. Here we assume that bang phases and coast phases have the same duration
     * @param way_points vector, note that the first waypoint should be the actual pose
     * @param trj_times vector of time for each sub trajecotry
     * @return true
     */
    bool initCompositeTrj(const std::vector<KDL::Frame>& way_points,
                          const std::vector<double> trj_times)
    {
        if(trj_times.size() != way_points.size()-1){
            std::cout<<"trj_time.size() != way_points.size()-1"<<std::endl;
            return false;
        }

        std::vector<boost::shared_ptr<KDL::Path_Line> > vector_of_line_paths;
        for(unsigned int i = 0; i < way_points.size()-1; ++i)
            vector_of_line_paths.push_back(boost::shared_ptr<KDL::Path_Line>(
                new KDL::Path_Line(way_points[i], way_points[i+1],
                    new KDL::RotationalInterpolation_SingleAxis(), _eq_radius)));

        std::vector<double> max_vels;
        std::vector<double> max_accs;
        for(unsigned int i = 0; i < vector_of_line_paths.size(); ++i)
        {
            double L = vector_of_line_paths[i]->PathLength();
            double max_vel = (3.*L)/(2.*trj_times[i]);
            double max_acc = (9.*L)/(2.*trj_times[i]*trj_times[i]);

            max_vels.push_back(max_vel);
            max_accs.push_back(max_acc);
        }

        return initCompositeTrj(way_points, max_vels, max_accs);
    }

    /**
     * @brief initCompositeTrj intialize a trajectory composed by sub-linear trajectories specified
     * by waypoints. For each sub-trajectory we have the same max velocity and max acceleration
     * @param way_points vector, note that the first waypoint should be the actual pose
     * @param max_vel
     * @param max_acc
     * @return false if for one sub-trajecotry the coast phase does not exist
     */
    bool initCompositeTrj(const std::vector<KDL::Frame>& way_points,
                          const double max_vel, const double max_acc)
    {
        std::vector<double> max_vels;
        std::vector<double> max_accs;
        for(unsigned int i = 0; i < way_points.size()-1; ++i){
            max_vels.push_back(max_vel);
            max_accs.push_back(max_acc);
        }

        return initCompositeTrj(way_points, max_vels, max_accs);
    }


    /**
     * @brief initCompositeTrj intialize a trajectory composed by sub-linear trajectories specified
     * by waypoints
     * @param way_points vector, note that the first waypoint should be the actual pose
     * @param max_vels vector of max velocities for each sub-trajectory
     * @param max_accs vector of max accelerations for each sub-trajectory
     * @return false if for one sub-trajecotry the coast phase does not exist
     */
    bool initCompositeTrj(const std::vector<KDL::Frame>& way_points,
                          const std::vector<double> max_vels, const std::vector<double> max_accs)
    {
        if(max_vels.size() != max_accs.size()){
            std::cout<<"max_vels.size() != max_accs.size()"<<std::endl;
            return false;}
        if(max_vels.size() != way_points.size()-1){
            std::cout<<"max_vels.size() != way_points.size()-1"<<std::endl;
            return false;}

        trj.reset();
        _time = 0.0;

        _vector_of_line_paths.clear();
        for(unsigned int i = 0; i < way_points.size()-1; ++i)
            _vector_of_line_paths.push_back(boost::shared_ptr<KDL::Path_Line>(
                new KDL::Path_Line(way_points[i], way_points[i+1],
                    new KDL::RotationalInterpolation_SingleAxis(), _eq_radius)));

        _vector_of_velocity_profiles.clear();
        _vector_of_trj_segment.clear();
        trj.reset(new KDL::Trajectory_Composite());
        for(unsigned int i = 0; i < _vector_of_line_paths.size(); ++i)
        {
            _vector_of_velocity_profiles.push_back(boost::shared_ptr<KDL::VelocityProfile>(
                new KDL::VelocityProfile_Trap(max_vels[i], max_accs[i])));
            _vector_of_velocity_profiles[i]->SetProfile(0, _vector_of_line_paths[i]->PathLength());

            double L = _vector_of_line_paths[i]->PathLength();
            if(L <= (max_vels[i]*max_vels[i])/max_accs[i]){
                std::cout<<"Too fast trajectory, no Coast phase exists! For trj "<< i <<std::endl;
                return false;
            }

            _vector_of_trj_segment.push_back(boost::shared_ptr<KDL::Trajectory_Segment>(
                new KDL::Trajectory_Segment(_vector_of_line_paths[i].get(),
                                            _vector_of_velocity_profiles[i].get())));

            trj->Add(_vector_of_trj_segment[i].get());
        }

        _is_inited = true;
        return _is_inited;
    }

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

        _linear_path.reset(new KDL::Path_Line(start, end, new KDL::RotationalInterpolation_SingleAxis(), _eq_radius));

        _velocity_profile.reset(new KDL::VelocityProfile_Trap(max_vel, max_acc));
        _velocity_profile->SetProfile(0, _linear_path->PathLength());

        double L = _linear_path->PathLength();
        if(L <= (max_vel*max_vel)/max_acc){
            std::cout<<"Too fast trajectory, no Coast phase exists!"<<std::endl;
            return false;
        }

        _trj_seg.reset(new KDL::Trajectory_Segment(_linear_path.get(), _velocity_profile.get()));

        trj.reset(new KDL::Trajectory_Composite());
        trj->Add(_trj_seg.get());

        _is_inited = true;
        return _is_inited;
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
        linear_path.reset(new KDL::Path_Line(start, end, new KDL::RotationalInterpolation_SingleAxis(), _eq_radius));

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

        if(isFinished())
            _is_inited = false;
    }

    /**
     * @brief getTime
     * @return actual time of the trajectory
     */
    double getTime()
    {
        return _time;
    }

    /**
     * @brief Duration
     * @return return the total time of the trajectory
     */
    double Duration()
    {
        return trj->Duration();
    }

    /**
     * @brief isFinished
     * @return true if the internal time reached the duration of the trajectory
     */
    bool isFinished()
    {
        return _time >= trj->Duration();
    }

    /**
     * @brief isStarted
     * @return true if the trajectory is playing
     */
    bool isStarted()
    {
        return _time > 0.0;
    }

    /**
     * @brief isRunning
     * @return true if the trajectory is started and is not finished
     */
    bool isRunning()
    {
        return isStarted() && !isFinished();
    }

    /**
     * @brief isInited
     * @return true if trajectory is inited, false if is not inited or finished
     */
    bool isInited()
    {
        return _is_inited;
    }

    boost::shared_ptr<KDL::Trajectory_Composite> trj;

private:
    double _time;
    double _dt;
    double _eq_radius;
    bool _is_inited;
    boost::shared_ptr<KDL::VelocityProfile> _velocity_profile;

    std::vector<boost::shared_ptr<KDL::Path_Line> > _vector_of_line_paths;
    std::vector<boost::shared_ptr<KDL::VelocityProfile> > _vector_of_velocity_profiles;
    std::vector<boost::shared_ptr<KDL::Trajectory_Segment> > _vector_of_trj_segment;

    boost::shared_ptr<KDL::Path_Line> _linear_path;
    boost::shared_ptr<KDL::Path_Circle> _circle_path;
    boost::shared_ptr<KDL::Trajectory_Segment> _trj_seg;


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
                std::shared_ptr<paramHelp::ParamHelperServer> ph  ) :   robot(  module_prefix,
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
