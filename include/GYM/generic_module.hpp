#ifndef GENERIC_MODULE_HPP
#define GENERIC_MODULE_HPP

#include <yarp/os/all.h>
#include <boost/concept_check.hpp>
#include "generic_thread.hpp"
#include <drc_shared/yarp_status_interface.h>
#include <drc_shared/yarp_command_interface.hpp>

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
class generic_module: public yarp::os::RFModule {
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
        if( rf.check("dT") ) {
            actual_find_value = rf.find("dT");
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
        if( rf.check("robot") ) {
            actual_find_value = rf.find("robot");
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
    
    /**
     * @brief generic module standard configure: take the rf (custom or standard) and initialize the mandatory params. 
     *        It calls the custom_configure() at the end of the function. 
     * @param rf resource finder.
     * 
     * @return true if the rf (standard or custom) exists. False otherwise.
     **/
    bool configure( yarp::os::ResourceFinder &rf ) final
    {
        // check the rf and the mandatory params initialization
        if( initializeMandatoryParam() ) {
            //call the custom configure
            return custom_configure( rf );
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
        //call cutom_stop
        bool custom_close_ret = custom_close();
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
