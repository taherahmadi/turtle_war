#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include<turtlesim/Pose.h>
#include <signal.h>
#include <termios.h>
#include <stdio.h>
#include <exception>
#include <std_msgs/Int32.h>

#define KEYCODE_R 0x43 
#define KEYCODE_L 0x44
#define KEYCODE_U 0x41
#define KEYCODE_D 0x42
#define KEYCODE_Q 0x71
#define KEYCODE_W 0x77
#define KEYCODE_E 0x65

using namespace std;


int counter=0; 

class TeleopTurtle
{
public:
  TeleopTurtle();
  void keyLoop();
  

private:
  
  ros::NodeHandle nh_;
  ros::Publisher cmd_vel_publisher;
  double linear_, angular_, l_scale_, a_scale_;

  ros::Publisher recognize;
};

TeleopTurtle::TeleopTurtle():
  linear_(0),
  angular_(0),
  l_scale_(2.0),
  a_scale_(2.0)
{
  nh_.param("scale_angular", a_scale_, a_scale_);
  nh_.param("scale_linear", l_scale_, l_scale_);

 
  recognize=nh_.advertise<std_msgs::Int32>("predator/specifier",1); 
  cmd_vel_publisher = nh_.advertise<geometry_msgs::Twist>("teleop/cmd_vel", 1);

}

int kfd = 0;
struct termios cooked, raw;

void quit(int sig)
{
  (void)sig;
  tcsetattr(kfd, TCSANOW, &cooked);
  ros::shutdown();
  exit(0);
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "teleop_turtle");
  TeleopTurtle teleop_turtle;
  signal(SIGINT,quit);
try{
  teleop_turtle.keyLoop();}
    catch(exception& ourerror){
           cout<<ourerror.what();

      };


  return(0);
}


void TeleopTurtle::keyLoop()
{
  char c;
  bool dirty=false;


  // get the console in raw mode                                                              
  tcgetattr(kfd, &cooked);
  memcpy(&raw, &cooked, sizeof(struct termios));
  raw.c_lflag &=~ (ICANON | ECHO);
  // Setting a new line, then end of file 

  raw.c_cc[VEOL] = 1;
  raw.c_cc[VEOF] = 2;
  tcsetattr(kfd, TCSANOW, &raw);

  puts("Reading from keyboard");
  puts("---------------------------");
  puts("Use arrow keys to move the turtle.");


  for(;;)
  {
    // get the next event from the keyboard  
        ROS_INFO("i am waiting \n");

    if(read(kfd, &c, 1) < 0)
    {
      perror("read():");
      exit(-1);
    }

    linear_=angular_=0;
    ROS_INFO("i am running \n");
    ROS_ERROR("value: 0x%02X\n", c);
    
    switch(c)
    {
      case KEYCODE_L:
        ROS_DEBUG("LEFT");
        angular_ = 1.0;
        dirty = true;
        break;
      case KEYCODE_R:
        ROS_DEBUG("RIGHT");
        angular_ = -1.0;
        dirty = true;
        break;
      case KEYCODE_U:
        ROS_DEBUG("UP");
        linear_ = 1.0;
        dirty = true;
        break;
      case KEYCODE_D:
        ROS_DEBUG("DOWN");
        linear_ = -1.0;
        dirty = true;
        break;
        case KEYCODE_E:
        counter++;
        if(counter > 4){
          counter=0;
        }
        break;
        case KEYCODE_W:
        counter--;
        if(counter < 0){
        counter=4;
        }
        break;
    }
   

    geometry_msgs::Twist twist;
    twist.angular.z = a_scale_*angular_;
    twist.linear.x = l_scale_*linear_;
    if(dirty ==true)
    {

      //selfControl.publish(twist);
      std_msgs::Int32 temp_var;
      temp_var.data=counter;
      cmd_vel_publisher.publish(twist);
      
      recognize.publish(temp_var);
      ROS_DEBUG("predator: %d",counter);
      dirty=false;
      ROS_INFO("before spin");
      ros::spinOnce();   
    } 
      
    }
  }


  

