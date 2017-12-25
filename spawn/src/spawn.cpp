#include"ros/ros.h"
#include "turtlesim/Spawn.h"
#include "geometry_msgs/Twist.h"
#include "turtlesim/SpawnRequest.h"
#include "turtlesim/SpawnResponse.h"

using namespace std;

ros::Publisher pub;


void move(double speed,int direction);

int main(int argc,char ** argv){

    ros::init(argc,argv,"myTurtle");
    ros::NodeHandle nh;
    ros::service::waitForService("spawn",20);
    ros::ServiceClient add_turtle = nh.serviceClient<turtlesim::Spawn>("spawn");
    

    turtlesim::Spawn srv;
    srv.request.x = atof(argv[1]); //added
    srv.request.y = atof(argv[2]); //added
    srv.request.theta = atof(argv[3]);
    srv.request.name=argv[4];
    
    ROS_INFO("%d  %s",argc,argv);
    add_turtle.call(srv);
   

}


