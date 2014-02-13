/* 
 *  Abstract datatype that handles callbacks from yarp ports, while preventing
 *  simultaneous reading and writing of data and keeping track of time elpsed
 *  since last reception.
 * 
 *  Within the pure virtual method update_data you can implement your own grabber,
 *  i.e. the code that should be executed every time new data is received.
 *
 *  Inside update_data you have the option to convert raw communication data 
 *  (msg_t, e.g. yarp::os::Bottle) into a personalized datatype (out_t).
 *  If no conversion is needed, you can instantiate a yarp_msg_simple_grabber.
 *
 *	Please check grabber_hello_world.cpp for an example! :)
 *
 *  email: v.varricchio@gmail.com 
 */

#ifndef YARP_MSG_GRABBER
#define YARP_MSG_GRABBER

#include <mutex>
#include <yarp/os/all.h>
#include <yarp/os/Time.h>

template <class msg_t, class out_t>
class yarp_msg_grabber : public yarp::os::TypedReaderCallback<msg_t>  {
private:
  std::mutex mtx; 
  bool is_new;
  double timeout_sec;
  double last_stamp;
  
protected:  
  out_t data;

public:
    
  yarp_msg_grabber(){
    is_new = false;
  };

  void grab_from_port(yarp::os::BufferedPort<msg_t>& b, double timeout_sec_=-1){
    timeout_sec = timeout_sec_;
    last_stamp = yarp::os::Time::now();
    b.useCallback(*this);
    is_new = false;
  }

  void onRead(msg_t& msg){
    std::unique_lock<std::mutex>lck(mtx);
    update_data(msg);
    is_new = true;
  }

  virtual void update_data(msg_t& msg)=0;

  out_t get_data(){
    std::unique_lock<std::mutex>lck(mtx);
    is_new = false;
    last_stamp = yarp::os::Time::now();
    return data;
  }
  
  bool has_new_data(){
    std::unique_lock<std::mutex>lck(mtx);
    return is_new;
  }
  
  bool get_updates(out_t& var){
    if(has_new_data()){
      var = get_data();
      return true;
    }
    return false;
  }
  
  double get_timer(){
    return yarp::os::Time::now()-last_stamp;
  }    
  
  bool expired(){
    if(timeout_sec<0) return false;
    if(get_timer() > timeout_sec){
      last_stamp = yarp::os::Time::now();
      return true;
    }else{
      return false;
    }
  }
};

template <class T>
class yarp_msg_simple_grabber: public yarp_msg_grabber<T,T>{
public: 
  yarp_msg_simple_grabber(){}

  void update_data(T& msg){
    this->data = msg;
  }
};

#endif