#ifndef GENERIC_MODULE_HPP
#define GENERIC_MODULE_HPP

// YARP
#include <yarp/os/all.h>
// status interface
#include <drc_shared/yarp_status_interface.h>
// command interface
#include <drc_shared/yarp_command_interface.hpp>
// param helper
#include <paramHelp/paramHelperServer.h>
#include <paramHelp/paramProxyBasic.h>
#include <paramHelp/paramProxyInterface.h>
//generic thread
#include "generic_thread.hpp"

// param helper define for params
#define PARAM_ID_DT 0
#define PARAM_ID_ROBOT 1
#define PARAM_SIZE_DT 1
#define PARAM_SIZE_ROBOT 1

// param helper define for commands
#define COMMAND_ID_HELP 0
#define COMMAND_ID_SAVE_PARAMS 1

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
 * @brief generic module template with a standard switch interface, a standard status interface and a generic thread. 
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
    // switch and status interface of the module
    walkman::drc::yarp_switch_interface* switch_interface;
    walkman::drc::yarp_status_interface* status_interface;
    int actual_num_seq;
    // resource finder
    yarp::os::ResourceFinder rf;
    // param helper
    paramHelp::ParamHelperServer* ph;
    yarp::os::Port rpc_port;
    std::vector<paramHelp::ParamProxyInterface *> actual_ph_parameters;
    std::vector<paramHelp::CommandDescription> actual_ph_commands;
    
    
    std::vector<paramHelp::ParamProxyInterface *> create_standard_ph_parameters()
    {
        std::vector<paramHelp::ParamProxyInterface *> standard_ph_parameters;
        // insert dT param
        standard_ph_parameters.push_back( new paramHelp::ParamProxyBasic<double>(   "thread_period", 
                                                                                    PARAM_ID_DT, 
                                                                                    PARAM_SIZE_DT, 
                                                                                    paramHelp::ParamLowerBound<double>(1e-3), 
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
    
    std::vector<paramHelp::CommandDescription> create_standard_ph_commands()
    {
        std::vector<paramHelp::CommandDescription> standard_ph_commands;
        // insert help command
        standard_ph_commands.push_back(paramHelp::CommandDescription(   "help",
                                                                        COMMAND_ID_HELP,
                                                                        "Get instructions about how to communicate with this module") );
        // insert saveParams command
        standard_ph_commands.push_back(paramHelp::CommandDescription(   "saveParams",
                                                                        COMMAND_ID_SAVE_PARAMS,
                                                                        "saveParams(string fileName) # Save the actual configuration parameters to file, inside the sot context folder") );
        return standard_ph_commands;
    }
    
    std::vector<paramHelp::ParamProxyInterface *> get_ph_parameters()
    {
        // standard params
        std::vector<paramHelp::ParamProxyInterface *> ph_parameters = std::vector<paramHelp::ParamProxyInterface *>( create_standard_ph_parameters() );
        // custom params
        std::vector<paramHelp::ParamProxyInterface *> custom_ph_parameters = std::vector<paramHelp::ParamProxyInterface *>( custom_get_ph_parameters() );
        // concat the two vectors
        ph_parameters.insert(ph_parameters.end(), custom_ph_parameters.begin(), custom_ph_parameters.end() );
    
        return ph_parameters;
    }
    
    std::vector<paramHelp::CommandDescription> get_ph_commands()
    {
        // standard params
        std::vector<paramHelp::CommandDescription> ph_commands = std::vector<paramHelp::CommandDescription>( create_standard_ph_commands() );
        // custom params
        std::vector<paramHelp::CommandDescription> custom_ph_commands = std::vector<paramHelp::CommandDescription>( custom_get_ph_commands() );
        // concat the two vectors
        ph_commands.insert(ph_commands.end(), custom_ph_commands.begin(), custom_ph_commands.end() );
        
        return ph_commands;
    }
    
     void ph_link_parameters() 
    {
        ph->linkParam(PARAM_ID_DT, &thread_period);
        ph->linkParam(PARAM_ID_ROBOT, &robot_name);
        custom_ph_link_parameters();
    }
    
    void ph_register_commands() 
    {
        YARP_ASSERT(ph->registerCommandCallback(COMMAND_ID_HELP,           this));
        YARP_ASSERT(ph->registerCommandCallback(COMMAND_ID_SAVE_PARAMS,    this));
        custom_ph_register_commands();
    }
    
    
    
    
    
    

    /**
     * @brief initializer for the mandatory params that are: 
     *        - the thread period expressed in millisec.
     *        - the robot name.
     * 
     * @return true if the initialization has success. False otherwise.
     */
    bool initializeMandatoryParam()
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
                return false;
            }
        }
        else {
            //thread period does not exist
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
                return false;
            }
        }
        else {
            //robot name does not exist
            return false;
        }
        
        //intizializaions had success
        return true;
    }

    
    
public: 
    int argc;
    char** argv;
    
    /**
     * @brief constructor of the generic module. 
     *        It creates a standard switch interface and a status interface for the module at /module_prefix/module/status:o
     *
     * @param argc: argc
     * @param argv: argv
     * @param module_prefix module name.
     * @param module_period period of the module in milliseconds.
     * @param rf resource finder: optional param.
     **/
    generic_module( int argc, 
                    char* argv[],
                    std::string module_prefix, 
                    int module_period, 
                    yarp::os::ResourceFinder rf) :  argc( argc ),
                                                    argv( argv ),
                                                    module_prefix( module_prefix ),
                                                    rf( rf )
    {
        // check that T is a generic_thread subclass (at compile time)
        derived_constraint<T, generic_thread>();
        // create the switch and the status interface and make it starts
        switch_interface = new walkman::drc::yarp_switch_interface( module_prefix );
        // status rate setted at the half of the module period
        status_interface = new walkman::drc::yarp_status_interface( module_prefix + "/module" );
        status_interface->setRate( module_period / 2 );
        status_interface->start();
        // not alive
        alive = false;
        // initialize actual sequence number
        actual_num_seq = 0;
        // set the module period in second for the RFModule
        this->module_period = (double)module_period / 1000;


    }
    
    virtual std::vector<paramHelp::ParamProxyInterface *> custom_get_ph_parameters()
    {
        std::vector<paramHelp::ParamProxyInterface *> empty_vector;
        return empty_vector;
    }
    
    virtual std::vector<paramHelp::CommandDescription> custom_get_ph_commands()
    {
        std::vector<paramHelp::CommandDescription> empty_vector;
        return empty_vector;
    }
    
    virtual void custom_ph_link_parameters() 
    {
    }
    
     virtual void custom_ph_register_commands() 
    {
    }
    
    virtual bool respond(const yarp::os::Bottle& command,
                         yarp::os::Bottle& reply)
    {
        std::cout << "cmd : " << command.toString() << std::endl;
        bool respond_ok = true;
        ph->lock();
        if( !ph->processRpcCommand( command, reply ) ) {
            reply.addString( ( std::string( "Command " ) + command.toString().c_str() + " not recognized." ).c_str() );
            respond_ok = false;
        }
        ph->unlock();
        
         // if reply is empty put something into it, otherwise the rpc communication gets stuck
        if( reply.size() == 0 )
            reply.addString( ( std::string( "Command " ) + command.toString().c_str()+" received." ).c_str() );
        return respond_ok;
    }
    
    virtual void parameterUpdated(const paramHelp::ParamProxyInterface *pd)
    {
        return;
    }


    virtual void commandReceived(const paramHelp::CommandDescription &cd, const yarp::os::Bottle &params, yarp::os::Bottle &reply)
    {
        switch(cd.id)
        {
        case COMMAND_ID_HELP:
            ph->getHelpMessage(reply);
            break;
        case COMMAND_ID_SAVE_PARAMS:
            {
                /*std::string fileName = rf->find( "from" ).asString();
                
                std::string folderName = rf.getContextPath() + "/";
                std::string confPath = folderName + fileName;
                std::vector<int> configIds;
                for(unsigned int i = 0; i < PARAM_ID_SIZE; ++i)
                    if( sot_VelKinCon_ParamDescr[i]->ioType.value == paramHelp::PARAM_IN_OUT ||
                        sot_VelKinCon_ParamDescr[i]->ioType.value == paramHelp::PARAM_INPUT  ||
                        sot_VelKinCon_ParamDescr[i]->ioType.value == paramHelp::PARAM_CONFIG )
                        configIds.push_back(i);

                std::cout << "Saving to " << confPath;

                std::stringstream ss;
                boost::posix_time::ptime pt = boost::posix_time::second_clock::local_time();
                boost::posix_time::time_facet* output_facet = new boost::posix_time::time_facet("%Y%m%dT%H%M%S%F%q");
                ss.imbue(std::locale(ss.getloc(), output_facet));
                ss << pt;
                std::string confPathWithTimestamp = confPath + "." + ss.str();

                std::cout << " and " << confPathWithTimestamp;
                reply.addString("saving...");

                if( ph->writeParamsOnFile( confPathWithTimestamp,
                                                    configIds.data(),
                                                    configIds.size())) {
                    if(boost::filesystem::exists(confPath))
                            boost::filesystem::remove(confPath);
                    ph->writeParamsOnFile( confPath,
                                                    configIds.data(),
                                                    configIds.size());
                    reply.addString("ok");
                } else
                    reply.addString("failed!");*/
            }
            break;
        }
    }
    
    
    
    /**
     * @brief generic module standard configure: take the rf (custom or standard) and initialize the mandatory params. 
     *        It calls the custom_configure() at the end of the function. 
     * @param rf resource finder.
     * 
     * @return true if the rf (standard or custom) exists and the param helper is initialized with success. False otherwise.
     **/
    bool configure( yarp::os::ResourceFinder &rf ) final
    {
        // set the name of the module
        setName( module_prefix.c_str() );
        
        

        // get the data for the param heleper using copy constructor to avoid problems on delete
        actual_ph_parameters = std::vector<paramHelp::ParamProxyInterface *>( get_ph_parameters() );
        actual_ph_commands = std::vector<paramHelp::CommandDescription>( get_ph_commands() );
        // switch to standard c const vector
        const paramHelp::ParamProxyInterface * const* ph_parameters =  &actual_ph_parameters[0];
        const paramHelp::CommandDescription* ph_commands =  &actual_ph_commands[0];
        // create the param helper
        ph = new paramHelp::ParamHelperServer(  ph_parameters, actual_ph_parameters.size(),
                                                ph_commands , actual_ph_commands.size() );
        // link parameters
        ph_link_parameters();
        // register commands
        ph_register_commands();
        
        
        
        
        // check the rf and the mandatory params initialization
        if( initializeMandatoryParam() ) {
            // param helper param init
            yarp::os::Bottle init_msg;
            ph->initializeParams( rf, init_msg );
            paramHelp::printBottle( init_msg );
            // attach to the module the rpc port for the param helper
            attach( rpc_port );
            // open the rpc port for the param helper
            rpc_port.open( "/"+ module_prefix +"/rpc" );
            // call the init on the param helper
            if( ph->init( module_prefix ) ) {
                //call the custom configure
                return custom_configure( rf );
            }
            else {
                // error on the param helper initialization
                std::cout << "Error while initializing parameter helper." << std::endl;
                return false;
            }
        }
        else {
            return false;
        }
    }
    
    /**
     * @brief cutom configure function: could be redefined in subclasses.
     *
     * @return true on succes. False otherwise.
     **/
    virtual bool custom_configure( yarp::os::ResourceFinder &rf )
    {
        return true;
    }
    
    /**
     * @brief call configure, create a new custom thread and make it start .
     *
     * @return true if the thread correctly starts and the configure has success. False otherwise.
     **/
    bool start()
    {
        //call configure - if it has success create the thread and make it start
        if( configure( rf ) ) {
            // create the thread 
            thread = new T( module_prefix, thread_period, rf );
            // start the thread 
            if( !thread->start() )
            {   // error starting the thread
                delete thread;
                thread = NULL;
                return false;
            }
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
        //delete param helper
        if( ph ) {
            ph->close();
            delete ph;
            ph = NULL;
        }
        // could happend that alive is false here -> close called in automatic after updateModule return false
        if( alive ){
            thread->stop();
            delete thread;
            thread = NULL;
        } 
        alive = false;
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
     * @brief suspend the thread.
     *
     * @return always true
     **/
    bool pause() 
    {
        thread->suspend();
        return true;
    }
    
    /**
     * @brief resume the thread.
     *
     * @return always true
     **/
    bool resume() 
    {
        thread->resume();
        return true;
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
     * @return the period of the thread.
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
        status_interface->setStatus( "Module alive at updateModule : #" + std::to_string(actual_num_seq), actual_num_seq);
        actual_num_seq++;
        // get the command
        std::string switch_command;
        if(switch_interface->getCommand(switch_command)) {
            std::cout<<"Switch Interface received: "<<switch_command<<std::endl;
            //stop command
            if(switch_command == "stop") {
                if(this->isAlive()) {
                      std::cout<<"Stopping module"<<std::endl;
                      this->close();
                }
            }
            //start command
            else if(switch_command == "start") {
                if(this->isAlive()) {
                    std::cout<<"Starting module"<<std::endl;
                    this->close();
                }
                std::cout<<"Starting module"<<std::endl;       
                if(this->start()) {
                    std::cout<<"Module is started"<<std::endl;
                }
                else {
                    std::cout<<"Error starting Module"<<std::endl;
                }
            }
            // pause command
            else if(switch_command == "pause") {
                if(this->isAlive()) {
                std::cout<<"Module Suspended"<<std::endl;
                this->pause();
                }
            }
            // resume command
            else if(switch_command == "resume") {
                if(this->isAlive()) {
                    std::cout<<"Module Resumed"<<std::endl;
                    this->resume();
                }
            }
            // quit command
            else if(switch_command == "quit") {
                std::cout<<"Quit"<<std::endl;
                std::cout<<"Closing the module ... "<<std::endl;
                return false;
            }
            else {
                std::cout<<switch_command<<" is not vaild"<<std::endl;
            }
        }
        return true;
    }
    
    virtual ~generic_module()
    {
        // delete switch interface
        if ( switch_interface ) {
            delete switch_interface;
            switch_interface = NULL;
        }
        // delete status interface
        if ( status_interface ) {
            status_interface->stop();
            delete status_interface;
            status_interface = NULL;
        }
    }
  
};

#endif
