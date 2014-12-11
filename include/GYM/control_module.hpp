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

#ifndef CONTROL_MODULE_HPP
#define CONTROL_MODULE_HPP

// incldue GYM
#include <GYM/generic_module.hpp>
#include <GYM/control_thread.hpp>

// param helper define for params
#define PARAM_ID_URDF_PATH 1002
#define PARAM_ID_SRDF_PATH 1003
#define PARAM_SIZE_URDF_PATH 1
#define PARAM_SIZE_SRDF_PATH 1

template<class T> 
class control_module : public generic_module<T>  {
private:
    // urdf path
    std::string urdf_path;
    // srdf path
    std::string srdf_path;
    
protected:
    
    /**
     * @brief create the standard parameters for the control module param helper:
     *            - thread_period : an int that represents the period of the controlled thread in millisec
     *            - robot_name : a string that represents the name of the robot
     * 		  - urdf_path : path to the .urdf path
     * 		  - srdf_path : path to the .srdf path
     * @return a std::vector with the ParamProxyInterface* of the control module standard parameters
     */
    virtual std::vector<paramHelp::ParamProxyInterface *> create_standard_ph_parameters() 
    {
	std::vector<paramHelp::ParamProxyInterface *> standard_ph_control_parameters;
	// insert generic_module standard parameters
	standard_ph_control_parameters = generic_module<T>::create_standard_ph_parameters();
	// insert urdf_path param
        standard_ph_control_parameters.push_back( new paramHelp::ParamProxyBasic<std::string>(  "urdf_path", 
												PARAM_ID_URDF_PATH, 
												PARAM_SIZE_URDF_PATH, 
												paramHelp::PARAM_CONFIG, 
												NULL, 
												"path to the .urdf file" ) );
        
        // insert srdf_path param
        standard_ph_control_parameters.push_back( new paramHelp::ParamProxyBasic<std::string>(  "srdf_path", 
												PARAM_ID_SRDF_PATH, 
												PARAM_SIZE_SRDF_PATH,  
												paramHelp::PARAM_CONFIG, 
												NULL, 
												"path to the .srdf path" ) );
	return standard_ph_control_parameters;
    }
    
    /**
     * @brief initializer for the mandatory params that are: 
     *        - the thread period expressed in millisec (int).
     *        - the robot name (std::string).
     * 
     * @return true if the initialization has success. False otherwise.
     */
    virtual bool initializeMandatoryParam()
    {
	// initialize the mandatory params of the generic module
	bool generic_module_init_success = generic_module<T>::initializeMandatoryParam();
	// check if it has success
	if( !generic_module_init_success ) {
	    return false;
	}
	// get the resource finder
	yarp::os::ResourceFinder rf = generic_module<T>::get_resource_finder();
        yarp::os::Value actual_find_value;
        //urdf path as string
        if( rf.check("urdf_path") ) {
            actual_find_value = rf.find("urdf_path");
            if ( actual_find_value.isString() ) {
                urdf_path = actual_find_value.asString();
            }
            else {
                //urdf_path is not a string
		std::cerr << "Error: urdf_path param found but NOT a String" << std::endl;
                return false;
            }
        }
        else {
            //urdf_path does not exist
	    std::cerr << "Error: urdf_path param NOT found" << std::endl;
            return false;
        }
        
        //srdf_path as a string
        if( rf.check("srdf_path") ) {
            actual_find_value = rf.find("srdf_path");
            if ( actual_find_value.isString() ) {
                srdf_path = actual_find_value.asString();
            }
            else {
                //srdf_path is not a string
		std::cerr << "Error: srdf_path param found but NOT a string" << std::endl;
                return false;
            }
        }
        else {
            //srdf_path does not exist
	    std::cerr << "Error: srdf_path param NOT found" << std::endl;
            return false;
        }
        
        //intizializaions had success
        return true;
    }
    
public:
    /**
     * @brief control_module constructor
     * @param argc main argc
     * @param argv main argv
     * @param module_prefix prefix of the module
     * @param module_period period of the module (ex: 1000 [ms])
     * @param rf resource finder
     */
    control_module( int argc,
		    char* argv[],
		    std::string module_prefix, 
		    int module_period, 
		    yarp::os::ResourceFinder rf ) : generic_module<T>(  argc,
									argv, 
									module_prefix, 
									module_period,
									rf )
    {
	// check that T is a generic_thread subclass (at compile time)
        derived_constraint<T, control_thread>();
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

#endif //CONTROL_MODULE_HPP