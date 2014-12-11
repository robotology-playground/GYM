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

/**
 * @brief control thread
 * 
 * @author Luca Muratore (luca.muratore@iit.it)
 **/
class control_thread : public generic_thread
{
protected:
    RobotUtils robot;

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
									generic_thread( module_prefix, rf, ph )
    {    
    }
    
  
};

#endif //CONTROL_THREAD_HPP
