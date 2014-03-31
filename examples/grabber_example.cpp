/*
 *  yarp_message_grabber usage example
 *
 *  yarp_msg_grabber helps handling callbacks from yarp ports, preventing simultaneous 
 *  reading and writing of data and keeping track of time elpsed since last reception
 *  (port expiration implemented inside)
 * 
 *  You can implement your own grabber defining the update_data pure virtual method.
 *  update_data offers the the option to convert raw communication data 
 *  (msg_t, e.g. yarp::os::Bottle) into a personalized datatype (out_t).
 *  If no conversion is needed, you can just instantiate a yarp_msg_simple_grabber
 *  as shown below.
 *
 *  email: v.varricchio@gmail.com 
 */


#include <yarp/os/all.h>
#include <stdio.h>
#include <sstream>
#include <drc_shared/yarp_grabbers/YARP_grabber.hpp>

int main(int argc, char *argv[]) {

  yarp::os::Network yarp;

  yarp::os::BufferedPort<yarp::os::Bottle> inputPort;
  yarp::os::Bottle b;
  
  // Instantiating a simple grabber
  yarp_msg_simple_grabber <yarp::os::Bottle> g;
  
  // Associating a grabber to a yarp port, with a 4-seconds timeout
  g.grab_from_port(inputPort, 2);  // this can be even done before opening the port! :O
  
  inputPort.open("/test/input");

  std::cout << "Try to write sth on port /test/input" << std::endl;
  std::cout << "i.e. in a separate terminal, execute:" << std::endl;
  std::cout << "     yarp write ... /test/input" << std::endl;

  yarp::os::ContactStyle persist;
  persist.persistent = true;
  yarp.connect("/data", inputPort.getName(), persist);
  
  while (true){    
    if(g.expired()){	
      std::cout << " Input port expired. Data was not received for 2 secs." << std::endl;
    }
    
    if(g.get_updates(b)){
      std::cout << " Got \"" << b.toString() << "\" on input port." << std::endl;
    }
  }
  
  return 0;
}
