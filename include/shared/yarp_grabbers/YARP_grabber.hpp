/* 
 *  Abstract datatype to handle callbacks from yarp ports, while preventing
 *  simultaneous reading and writing of data and keeping track of time elpsed
 *  since last reception.
 * 
 *  The pure virtual method update_data needs be implemented to make your 
 *  own grabber. 
 *
 *  email: v.varricchio@gmail.com 
 * 
 */

#ifndef YARP_MSG_GRABBER
#define YARP_MSG_GRABBER

#include <mutex>
#include <yarp/os/all.h>
#include <time.h>

template <class msg_t, class out_t>
class yarp_msg_grabber : public yarp::os::TypedReaderCallback<msg_t>  {
private:
  std::mutex mtx; 
  bool is_new;
  float timeout_sec;
  int last_stamp;
  
protected:  
  out_t data;

public:
    
  yarp_msg_grabber(){
    is_new = false;
  };

  void grab_from_port(yarp::os::BufferedPort<msg_t>& b, int timeout_sec_=-1){
    timeout_sec = timeout_sec_;
    if(timeout_sec>0) last_stamp = time(nullptr);
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
    last_stamp = time(nullptr);
    return data;
  }
  
  bool has_new_data(){
    std::unique_lock<std::mutex>lck(mtx);
    return is_new;
  }
  
  bool get_updates(out_t& var){ // considering how timeout is handled, maybe var shoud be spcified in the constructor ?
    if(has_new_data()){
      var = get_data();
      return true;
    }
    return false;
  }
  
  float get_timer(){
    return ((float) (time(nullptr)-last_stamp));
  }    
  
  bool expired(){
    if(timeout_sec<0) return false;
    return get_timer() > timeout_sec;
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