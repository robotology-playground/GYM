#include <yarp/os/all.h>
#include <stdio.h>
#include <sstream>
#include "YARP_grabber.hpp"

int main(int argc, char *argv[]) {

  yarp::os::Network yarp;

  yarp::os::BufferedPort<yarp::os::Bottle> inputPort;
  yarp::os::Bottle b;
  
  // Instantiating a simple grabber
  yarp_msg_simple_grabber <yarp::os::Bottle> g;
  
  // Associating a grabber to a yarp port
  g.grab_from_port(inputPort, 4);  // this can be even done before opening the port! :O
  
  inputPort.open("/test/input");
  yarp::os::ContactStyle persist;
  persist.persistent = true;
  yarp.connect("/data", inputPort.getName(), persist);
  
  while (true){    
    if(g.expired()){	
      std::cout << " Port 1 expired." << std::endl;
    }
    
    if(g.get_updates(b)){
      std::cout << " Got \"" << b.toString() << "\" on input port." << std::endl;
    }
  }
  
  return 0;
}
