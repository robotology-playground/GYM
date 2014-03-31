/* 
 *  module_state_streamer usage example
 *
 *  module_state_streamer is a small utility to stream a module's
 *  state (a string + a Bottle)  at a costant rate independently 
 *  from the module's processing time.
 *
 *  A dedicated yarp::os::RateThread is wrapped inside.
 * 
 *  email: v.varricchio@gmail.com 
 */

#include <yarp/os/all.h>
#include <stdio.h>
#include <unistd.h>
#include <drc_shared/utils/module_state_streamer.h>

#define DT 0.2

class my_module:public yarp::os::RFModule{
  yarp::os::Network yarp;
  
  module_state_streamer state_streamer;
  
  std::string state_string;
  yarp::os::Bottle more_details;   
  
  double t0;
  int count;
  
  public:

  my_module(){
    yarp_setup();
    
    // Setup state broadcaster
    state_streamer.setPort("/my_module/state:o"); // port is opened and handled inside state_streamer
    state_streamer.setRate(100); // constant rate at which I want to send the state independently from the processing
    state_streamer.start();
    
    instructions();
    
    // more stuff
    t0 = yarp::os::Time::now();
    count = 0;
  }

  bool updateModule(){
    count++;
    
    // This changes the state only at certain time istants...
    // ...and the module_state_streamer will take care of streaming the state
    // on its port at a different and fixed rate specified in the constructor (here 100 ms)
    
    if(!(count%15)){
      state_string = std::to_string(count)+" iterations done.";
      more_details.clear();
      more_details.addString("time elapsed: ");
      more_details.addDouble(yarp::os::Time::now()-t0);
      state_streamer.setState(state_string, more_details); // more_details is optional
    }
    return true;
  }

  
  double getPeriod(){
    return DT;
  }
  
  bool interruptModule(){
    return true;
  }

  bool close(){
    state_streamer.stop();
    return true;
  }
  
  void yarp_setup(){
    while(!yarp.checkNetwork()){
      std::cout << "\r Waiting for yarp server..." << std::flush;
      sleep(1);
    } 
    std::cout << "Connected" << std::endl;
  }
  
  void instructions(){
    std::cout << "Please listen YARP port /my_module/state:o" << std::endl;
    std::cout << "  i.e. in a separate terminal, run:" << std::endl;
    std::cout << "    yarp read ... /my_module/state:o" << std::endl;
  }
};


int main(int argc, char *argv[]) {
  my_module whatever; 
  whatever.runModule();
  return 0;
}
