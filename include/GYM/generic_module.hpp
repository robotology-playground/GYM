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

#ifndef GENERIC_MODULE_HPP
#define GENERIC_MODULE_HPP

// YARP
#include <yarp/os/all.h>
// status interface
#include <GYM/yarp_status_interface.h>
// command interface
#include <GYM/yarp_command_interface.hpp>
// switch interface
#include <GYM/yarp_switch_interface.hpp>
// param helper
#include <paramHelp/paramHelperServer.h>
#include <paramHelp/paramProxyBasic.h>
#include <paramHelp/paramProxyInterface.h>
// C++11 smart pointers
#include <memory>
//generic thread
#include <GYM/generic_thread.hpp>

// param helper define for params
#define PARAM_ID_DT 1000
#define PARAM_ID_ROBOT 1001
#define PARAM_SIZE_DT 1
#define PARAM_SIZE_ROBOT 1

// param helper define for commands
#define COMMAND_ID_HELP 1000
#define COMMAND_ID_SAVE_PARAMS 1001

/**
 * @brief auxiliary struct to specify a constraint between T and B.
 **/
template<class T, class B> 
struct derived_constraint {
    
    /**
     * @brief constraint: T must be a subclass of B.
     *
     * @param ps a pointer to a T.
     * @return void
     **/
    static void constraints(T* ps) 
    {
        B* pb = ps; 
    }
    
    /**
     * @brief constraint trigger at compile time.
     *
     **/
    derived_constraint() 
    {
        void(*fp)(T*) = constraints; 
    }
};


/**
 * @brief generic module template with a switch interface, a status interface and a paramHelper a generic thread. 
 * The template type T must be a subclass of a generic_thread.
 * 
 * @author Luca Muratore (luca.muratore@iit.it)
 **/
template<class T> 
class generic_module :  public yarp::os::RFModule,
                        public paramHelp::ParamValueObserver,
                        public paramHelp::CommandObserver {
private:
    // generic module related variables
    std::string module_prefix;
    double module_period;
    bool alive;
    // thread controlled by the generic module
    T* thread;
    double thread_period;
    // name of the robot
    std::string robot_name;
    // switch interface of the module
    std::shared_ptr<walkman::yarp_switch_interface> switch_interface;
    // status interface of the module
    std::shared_ptr<walkman::yarp_status_interface> status_interface;
    std::string actual_status;
    int actual_num_seq;
    // resource finder
    yarp::os::ResourceFinder rf;  
    // param helper
    std::shared_ptr<paramHelp::ParamHelperServer> ph;
    yarp::os::Port rpc_port;
    std::vector<paramHelp::ParamProxyInterface *> actual_ph_parameters;
    std::vector<paramHelp::CommandDescription> actual_ph_commands;
    bool to_configure;
    bool to_open_port;
    
    
    /**
     * @brief create the standard commands for the param helper:
     *            - help : get instructions about how to communicate with this module
     *            - save_params <file_name>:  Save the actual configuration parameters to file, inside the resource finder context folder
     * 
     * @return a std::vector with the CommandDescription objects the standard parameters
     */
    std::vector<paramHelp::CommandDescription> create_standard_ph_commands()
    {
        std::vector<paramHelp::CommandDescription> standard_ph_commands;
        // insert help command
        standard_ph_commands.push_back(paramHelp::CommandDescription(   "help",
                                                                        COMMAND_ID_HELP,
                                                                        "Get instructions about how to communicate with this module") );
        // insert save_params command
        standard_ph_commands.push_back(paramHelp::CommandDescription(   "save_params",
                                                                        COMMAND_ID_SAVE_PARAMS,
                                                                        "Usage -> save_params <file_name>. Save the actual configuration parameters to file, inside the resource finder context folder") );
        return standard_ph_commands;
    }
    
    /**
     * @brief get the parameters for the param helper: standard parameters + custom(user defined) parameters calling custom_get_ph_commands()
     * 
     * @return a std::vector the ParamProxyInterface* of all the parameters: standard + custom 
     */
    std::vector<paramHelp::ParamProxyInterface *> get_ph_parameters()
    {
        // standard params
        std::vector<paramHelp::ParamProxyInterface *> ph_parameters( create_standard_ph_parameters() );
        // custom params
        std::vector<paramHelp::ParamProxyInterface *> custom_ph_parameters( custom_get_ph_parameters() );
        // concat the two vectors
        ph_parameters.insert( ph_parameters.end(), custom_ph_parameters.begin(), custom_ph_parameters.end() );
    
        return ph_parameters;
    }
    
    /**
     * @brief get the commands for the param helper: standard commands + custom(user defined) commands calling custom_get_ph_commands()
     * 
     * @return a std::vector the CommandDescription objects of all the commands: standard + custom 
     */
    std::vector<paramHelp::CommandDescription> get_ph_commands()
    {
        // standard params
        std::vector<paramHelp::CommandDescription> ph_commands( create_standard_ph_commands() );
        // custom params
        std::vector<paramHelp::CommandDescription> custom_ph_commands( custom_get_ph_commands() );
        // concat the two vectors
        ph_commands.insert(ph_commands.end(), custom_ph_commands.begin(), custom_ph_commands.end() );
        
        return ph_commands;
    }
    
    /**
     * @brief link the standard parameters to the generic module and call custom_ph_link_parameters() for custom link params for the module
     */ 
    void ph_link_parameters() 
    {
        ph->linkParam(PARAM_ID_DT, &thread_period);
        ph->linkParam(PARAM_ID_ROBOT, &robot_name);
        custom_ph_link_parameters();
    }
    
    /**
     * @brief register paramValueChangedCallback and call custom_ph_param_value_changed_callback()
     */ 
    void ph_param_value_changed_callback() 
    {
        custom_ph_param_value_changed_callback();
    }
    
    /**
     * @brief register the standard commands and call custom_ph_register_commands() for custom commands registration
     */ 
    void ph_register_commands() 
    {
        ph->registerCommandCallback(COMMAND_ID_HELP,           this);
        ph->registerCommandCallback(COMMAND_ID_SAVE_PARAMS,    this);
        custom_ph_register_commands();
    }

    

    /**
     * @brief utility function for checking a suffix in a std::string - TODO: to move outside this class
     * 
     * @return true if str ends with the specified suffix
     */
    bool has_suffix(const std::string &str, const std::string &suffix)
    {
        return  str.size() >= suffix.size() &&
                str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

protected:
    /**
     * @brief create the standard parameters for the param helper:
     *            - thread_period : an int that represents the period of the controlled thread in millisec
     *            - robot_name : a string that represents the name of the robot
     * 
     * @return a std::vector with the ParamProxyInterface* of the standard parameters
     */
    virtual std::vector<paramHelp::ParamProxyInterface *> create_standard_ph_parameters()
    {
        std::vector<paramHelp::ParamProxyInterface *> standard_ph_parameters;
        // insert thread_period param
        standard_ph_parameters.push_back( new paramHelp::ParamProxyBasic<int>(  "thread_period", 
                                                                                PARAM_ID_DT, 
                                                                                PARAM_SIZE_DT, 
                                                                                paramHelp::ParamLowerBound<int>(1), 
                                                                                paramHelp::PARAM_CONFIG, 
                                                                                NULL, 
                                                                                "control thread period [milliseconds]" ) );
        
        // insert robot name param
        standard_ph_parameters.push_back( new paramHelp::ParamProxyBasic<std::string>(  "robot_name", 
                                                                                        PARAM_ID_ROBOT, 
                                                                                        PARAM_SIZE_ROBOT,  
                                                                                        paramHelp::PARAM_CONFIG, 
                                                                                        NULL, 
                                                                                        "robot name" ) );
        return standard_ph_parameters;
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
        yarp::os::Value actual_find_value;
        //thread period in millisec as an int
        if( rf.check("thread_period") ) {
            actual_find_value = rf.find("thread_period");
            if ( actual_find_value.isInt() ) {
                thread_period = actual_find_value.asInt();
            }
            else {
                //thread period is not an int
		std::cerr << "Error: thread_period param found but NOT an int" << std::endl;
                return false;
            }
        }
        else {
            //thread period does not exist
	    std::cerr << "Error: thread period param NOT found" << std::endl;
            return false;
        }
        
        //robot name as a string
        if( rf.check("robot_name") ) {
            actual_find_value = rf.find("robot_name");
            if ( actual_find_value.isString() ) {
                robot_name = actual_find_value.asString();
            }
            else {
                //robot name is not a string
		std::cerr << "Error: robot_name param found but NOT a string" << std::endl;
                return false;
            }
        }
        else {
            //robot name does not exist
	    std::cerr << "Error: robot_name param NOT found" << std::endl;
            return false;
        }
        
        //intizializaions had success
        return true;
    }
    
public:
    // command line args
    int argc;
    char** argv;
    
    /**
     * @brief constructor of the generic module. 
     *        It creates a standard switch interface at /module_prefix/switch:i and a status interface for the module at /module_prefix/module/status:o
     *
     * @param argc: argc
     * @param argv: argv
     * @param module_prefix module name
     * @param module_period period of the module in milliseconds
     * @param rf resource finder
     **/
    generic_module( int argc, 
                    char* argv[],
                    std::string module_prefix, 
                    int module_period, 
                    yarp::os::ResourceFinder rf ) : argc( argc ),
                                                    argv( argv ),
                                                    module_prefix( module_prefix ),
                                                    module_period( static_cast<double>(module_period) / 1000 ),
                                                    rf( rf ),
                                                    alive( false ),
                                                    actual_num_seq( 0 ),
                                                    actual_status(),
                                                    thread( NULL ),
                                                    to_configure( true ),
                                                    to_open_port( true )
    {
        // check that T is a generic_thread subclass (at compile time)
        derived_constraint<T, generic_thread>();
    }
    
    /**
     * @brief cutom get parameters for param helper: could be redefined in subclasses
     * 
     * @return an empty std::vector if not redefined
     */
    virtual std::vector<paramHelp::ParamProxyInterface *> custom_get_ph_parameters()
    {
        std::vector<paramHelp::ParamProxyInterface *> empty_vector;
        return empty_vector;
    }
    
    /**
     * @brief cutom get commands for param helper: could be redefined in subclasses
     * 
     * @return an empty std::vector if not redefined
     */
    virtual std::vector<paramHelp::CommandDescription> custom_get_ph_commands()
    {
        std::vector<paramHelp::CommandDescription> empty_vector;
        return empty_vector;
    }
    
    /**
     * @brief custom link parameters for param helper function: should be redefined if custom parameters are present
     */
    virtual void custom_ph_link_parameters() 
    {
    }
    
    /**
     * @brief custom param value changed callbacks for param helper: could be redefined if needed
     */
    virtual void custom_ph_param_value_changed_callback() 
    {
    }

    
    /**
     * @brief custom register command for param helper function: should be redefined if custom commands are present
     */
     virtual void custom_ph_register_commands() 
    {
    }
    
    /**
     * @brief respond to the rpc port attached to the generic module
     * 
     * @param command command received
     * @param reply reply of the module
     * @return true on rpc process success, false otherwise
     */
    virtual bool respond(const yarp::os::Bottle& command,  yarp::os::Bottle& reply) final
    {
        bool respond_ok = true;
        // ph locked
        ph->lock();
        // try to process the command received
        if( !ph->processRpcCommand( command, reply ) ) {
            reply.addString( ( std::string( "Command " ) + command.toString().c_str() + " not recognized." ).c_str() );
            respond_ok = false;
        }
        ph->unlock();   
        // ph unlocked 
        
        // if reply is empty put something into it, otherwise the rpc communication gets stuck
        if( reply.size() == 0 )
            reply.addString( ( std::string( "Command " ) + command.toString().c_str()+" received." ).c_str() );
        return respond_ok;
    }
    
    /**
     * @brief custom parameter updated handler: could be redefined in subclasses
     * 
     * @param pd the ParamProxyInterface* of the updated param
     */
    virtual void custom_parameterUpdated(const paramHelp::ParamProxyInterface *pd)
    {
    }
    
    /**
     * @brief custom command received handler: could be redefined in subclasses
     * 
     * @param cd the CommandDescription object related to the command received
     * @param params the parameters of the command received
     * @param reply the reply of the module
     */
    virtual void custom_commandReceived(const paramHelp::CommandDescription &cd, const yarp::os::Bottle &params, yarp::os::Bottle &reply)
    {
    }
    
    /**
     * @brief handler called when a parameter is updated: it calls the custom_parameterUpdated() function
     * 
     * @param pd the ParamProxyInterface* of the updated param
     */
    void parameterUpdated(const paramHelp::ParamProxyInterface *pd) final
    {
        // call custom parameterUpdated
        custom_parameterUpdated( pd );
    }

    /**
     * @brief handler called when a command is received: it handles the standard commmand as specified in their descriptions and it calls the custom_commandReceived() function
     * 
     * @param cd the CommandDescription object related to the command received
     * @param params the parameters of the command received
     * @param reply the reply of the module
     */
    void commandReceived(const paramHelp::CommandDescription &cd, const yarp::os::Bottle &params, yarp::os::Bottle &reply) final
    {
        switch(cd.id)
        {
        // help command
        case COMMAND_ID_HELP:
            {
                ph->getHelpMessage(reply);
            }
            break;
        // save parameters command
        case COMMAND_ID_SAVE_PARAMS:
            {
                // get the filename from the params of the save command
                std::string fileName = params.get( 0 ).toString();
                // get the folder name 
                std::string folderName = rf.getHomeContextPath() + "/";
                // complete conf path
                std::string confPath = has_suffix( fileName, ".ini" ) ? folderName + fileName : folderName + fileName + ".ini";
                // create the params to save array
                std::vector<int> configIds;
                int param_size = actual_ph_parameters.size();
                for(unsigned int i = 0; i < param_size; ++i) {
                    std::cout << actual_ph_parameters[i]->name << std::endl;
                    if( actual_ph_parameters[i]->ioType.value == paramHelp::PARAM_IN_OUT ||
                        actual_ph_parameters[i]->ioType.value == paramHelp::PARAM_INPUT  ||
                        actual_ph_parameters[i]->ioType.value == paramHelp::PARAM_CONFIG ) {
                        //add the parameter if the condition is verified
                        configIds.push_back( actual_ph_parameters[i]->id );
                    }
                }

                // write on the file specified by the confPath
                reply.addString( "Saving to " + confPath + " ... " );         
                ph->writeParamsOnFile(  confPath,
                                        configIds.data(),
                                        configIds.size());
                reply.addString( "ok"  );
            }
            break;
        }
        // call custom commandReceived
        custom_commandReceived( cd, params, reply );
    }

    /**
     * @brief open the generic YARP module standard ports: a switch interface, a status interface and a rpc for the paramHelper
     * 
     * @return void
     */
    void open_gym_ports() 
    {
	if( to_open_port ) {
	    std::cout << "Opening the generic ports ..." << std::endl;
	    // update the flag : open port only at the first configure
	    to_open_port = false;
	    // attach to the module the rpc port for the param helper
	    attach( rpc_port );
	    // open the rpc port for the param helper
	    rpc_port.open( "/" + module_prefix +"/rpc" );
	    // open standard switch interface
	    switch_interface = std::make_shared<walkman::yarp_switch_interface>( module_prefix ) ;
	    // open status interface
	    status_interface = std::make_shared<walkman::yarp_status_interface>( module_prefix + "/module" ) ;
	    // status rate setted at the half of the module period
	    status_interface->setRate( module_period*1000.0 / 2.0 );//HACK //TODO change double to int everywhere
	    status_interface->start();
	}
    }

    /**
     * @brief generic module standard configure: take the rf and initialize the mandatory params. 
     *        It calls the custom_configure() at the end of the function. 
     * @param rf resource finder.
     * 
     * @return true if the rf (standard or custom) exists and the param helper is initialized with success. False otherwise.
     **/
    bool configure( yarp::os::ResourceFinder &rf ) final
    {
        // TBD to be removed: will be handled by YARP
        yarp::os::Time::isValid();
        
	if( to_configure ) {
	    std::cout << "Configuring the module ..." << std::endl;
	    // update the flag
	    to_configure = false;
	    
        // set the name of the module
        setName( module_prefix.c_str() );
       
        // get the data for the param heleper using copy constructor to avoid problems on delete
        actual_ph_parameters =  get_ph_parameters();
        actual_ph_commands =  get_ph_commands();
        // switch to standard c const vector
        const paramHelp::ParamProxyInterface * const* ph_parameters =  &actual_ph_parameters[0];
        const paramHelp::CommandDescription* ph_commands =  &actual_ph_commands[0];
        // create the param helper
        ph = std::make_shared<paramHelp::ParamHelperServer>( ph_parameters, actual_ph_parameters.size(),
                                                            ph_commands , actual_ph_commands.size() );
        // link parameters
        ph_link_parameters();
	// register callbacks for param value changed
	ph_param_value_changed_callback();
        // register commands
        ph_register_commands();
        
        // check the rf and the mandatory params initialization
        if( initializeMandatoryParam() ) {
            
            // param helper params init
            yarp::os::Bottle init_msg;
            if( ph->initializeParams( rf, init_msg ) ) {
                // print the bottle
                paramHelp::printBottle( init_msg );
		    // opend standard module port
		    open_gym_ports();
            }
            else {
                // error on the parameters initialization
                std::cerr << "Error while initializing param helper parameters from resource finder: further details below" << std::endl;
                // print the bottle
                paramHelp::printBottle( init_msg );
                return false;
            }
            
            // call the init on the param helper
            if( ph->init( module_prefix ) ) {
                //call the custom configure
                return custom_configure( rf );
            }
            else {
                // error on the param helper initialization
                std::cerr << "Error while opening the param helper YARP info and stream ports." << std::endl;
                return false;
            }
        }
        else {
	    // error on param helper mandatory params
	    std::cerr << "Error while initializing param helper mandatory param" << std::endl;
            return false;
        }
    }
	else {
	    std::cout << "Module already configured. Skipping configure ..." << std::endl;
	    return true;
	}
    }
    
    /**
     * @brief custom configure function: could be redefined in subclasses
     *
     * @return true on succes. False otherwise.
     **/
    virtual bool custom_configure( yarp::os::ResourceFinder &rf )
    {
        return true;
    }
    
    /**
     * @brief call configure, create a new custom thread and make it start 
     *
     * @return true if the thread correctly starts and the configure has success. False otherwise
     **/
    bool start()
    {
        if( configure( rf ) ) {
	
            // create the thread 
            thread = new T( module_prefix, rf, ph );
            // start the thread 
            if( !thread->start() )
            {   // error starting the thread
                delete thread;
                thread = NULL;
                return false;
            }

            // thread correctly started
            alive = true;
            return true;
	}
	// configure error
	else {
	    return false;
	}
    }

    /**
     * @brief call cutom_stop, stop and delete the thread.
     *
     * @return true if custom_stop has succes. False otherwise.
     **/
    bool close() final
    {
        //call cutom_close
        bool custom_close_ret = custom_close();
        // could happend that alive is false here -> close called in automatic after updateModule return false
        if( alive ){
            thread->stop();
            delete thread;
            thread = NULL;
        } 
        alive = false;
	to_configure = true;
        return custom_close_ret;
    }
    
    /**
     * @brief cutom stop function: could be redefined in subclasses
     *
     * @return true on succes. False otherwise.
     **/
    virtual bool custom_close()
    {
        return true;
    }
    
    /**
     * @brief call custom_pause on the thread and suspend the thread.
     *
     * @return true if custom_pause has succes. False otherwise.
     **/
    virtual bool pause() final
    {
        bool custom_pause_ret = thread->custom_pause();
        thread->suspend();
        return custom_pause_ret;
    }
    
    /**
     * @brief call custom_resume on the thread and resume the thread.
     *
     * @return true if custom_resume has succes. False otherwise.
     **/
    virtual bool resume() final
    {
        bool custom_resume_ret = thread->custom_resume();
        thread->resume();
        return custom_resume_ret;
    }
    
    /**
     * @brief check if the module is alive.
     *
     * @return true if the module is alive. False otherwise.
     **/
    bool isAlive() 
    {
        return alive;
    }
    
    /**
     * @brief getter for the period of the thread.
     *
     * @return the period of the thread in [milliseconds].
     **/
    double getThreadPeriod() 
    {
        return thread_period;
    }
    
    /**
     * @brief getter for the name of the robot.
     *
     * @return the name of the robot.
     **/
    std::string getRobotName() 
    {
        return robot_name;
    }
    
    /**
     * @brief getter for the period of the module.
     *
     * @return the period of the module.
     **/
    double getPeriod() final
    {
        return module_period;
    }
    
    /**
     * @brief getter for the prefix of the module.
     *
     * @return the prefix of the module.
     **/
    std::string getModulePrefix() 
    {
        return module_prefix;
    }
    
    /**
     * @brief update module function, called every module_period as specified in the constructor.
     * Gets the command from the switch interface and executes it.
     * 
     * Possible commands are: start, stop, pause, resume and quit.
     * 
     *
     * @return false after a quit command. True otherwise.
     **/
    bool updateModule() 
    {
        // status update
        if( this->isAlive() ) {
            actual_status =  "Control Thread alive at updateModule : #";
        }
        else {
            actual_status =  "Control Thread NOT alive at updateModule : #";
        }
        status_interface->setStatus( actual_status + std::to_string(actual_num_seq), actual_num_seq);
        actual_num_seq++;
        // get the command
        std::string switch_command;
        if( switch_interface->getCommand( switch_command ) ) {
            std::cout << "Switch Interface cmd received: " << switch_command << std::endl;
            
            //stop command
            if( switch_command == "stop" ) {
                if( this->isAlive() ) {
                    std::cout << "Stopping control thread" << std::endl;
                    this->close();
                }
            }
            
            //start command
            else if( switch_command == "start" ) {
                if( this->isAlive() ) {
                    std::cout << "Stopping control thread" << std::endl;
                    this->close();
                }
                std::cout << "Starting module" << std::endl;       
                if( this->start() ) {
                    std::cout << "Control thread is started" << std::endl;
                }
                else {
                    std::cout << "Error starting control thread" << std::endl;
                }
            }
            
            // pause command
            else if( switch_command == "pause" ) {
                if( this->isAlive() ) {
                    std::cout << "Control thread paused" << std::endl;
                    this->pause();
                }
            }
            
            // resume command
            else if( switch_command == "resume" ) {
                if( this->isAlive() ) {
                    std::cout << "Control thread resumed" << std::endl;
                    this->resume();
                }
            }
            
            // quit command
            else if( switch_command == "quit" ) {
                std::cout << "Quit" << std::endl;
                std::cout << "Closing the module ... " << std::endl;
                return false;
            }
            else {
                std::cout << switch_command << " is not vaild" << std::endl;
            }
        }
        // true if the module is not quitted
        return true;
    }
    
    /**
     * @brief getter method for the ResourceFinder
     * 
     * @return the resource finder
     */
    yarp::os::ResourceFinder get_resource_finder()
    {
        return rf;
    }
    
    /**
     * @brief getter method for the param helper
     * 
     * @return the param helper
     */
    std::shared_ptr<paramHelp::ParamHelperServer> get_param_helper()
    {
        return ph;
    }
    
    /**
     * @brief TODO
     * 
     * @return T*
     */
    T* get_thread() 
    {
	return thread;
    }
    
    /**
     * @brief virtual generic module destructor
     */
    virtual ~generic_module()
    {
    }
  
};

#endif
