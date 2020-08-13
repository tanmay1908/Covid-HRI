#define _GLIBCXX_USE_C99 1
#include <ros/ros.h>
#include <string>
#include <iostream>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include "std_msgs/Int16.h"
#include "std_msgs/String.h"
#include "geometry_msgs/PoseStamped.h"


typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

class Sub
{
private:
  ros::Subscriber room_num_sub; //gesture_output subscriber
  ros::Subscriber aruco_detected_checker; // checks if aruco tag was found or not
  ros::Subscriber alexa_sub;
  // add sub to alexa channel

  bool foundAruco;

public:
  Sub(ros::NodeHandle &n)
  {
    room_num_sub = n.subscribe("/gesture_output", 2 ,&Sub::manageGestures,this);
    aruco_detected_checker = n.subscribe("/pose",2, &Sub::manageAruco,this);
    alexa_sub = n.subscribe("/alexa_msgs", 2 ,&Sub::manageAlexaCmd,this);
    //initialize subscriber to alexa channel
    foundAruco = 0;
    ROS_INFO("Initialized Sub instance");
  }

  void manageAlexaCmd(const std_msgs::String::ConstPtr& msg)
  {
    ROS_INFO("In Alexa call back func");
      std::string cmd = msg->data;
     // ROS_INFO("%s\n", msg->data.c_str());
     std::vector<std::string> tokens;
     std::stringstream check1(cmd);
     std::string intermediate;

     while(getline(check1, intermediate, ','))
     {
       tokens.push_back(intermediate);
     }
     //ROS_INFO("added tokens");
     int src = std::stoi(tokens[0]);
     std::string obj = tokens[1];
     int dest = std::stoi(tokens[2]);
     //ROS_INFO("differentiation finished");
     //ROS_INFO(src);
     // ROS_INFO(obj);
      //ROS_INFO(dest);
     manageInputs(src,obj,dest);
     //ROS_INFO("process finished");
  }

  void manageAruco(const geometry_msgs::PoseStamped::ConstPtr& aruco_pose)
  {
    ROS_INFO("Aruco Tag detected!");
    foundAruco = 1;
  }

  void manageGestures(const std_msgs::Int16::ConstPtr& room_num)
  {
    int num = room_num->data;
    manageInputs(num,"",-1);
  }

  void room_dispatch(int room_num)
  {
    MoveBaseClient nav_client("move_base",true);

    while(!nav_client.waitForServer(ros::Duration(5.0)))
    {
      ROS_INFO("Waiting for the move_base action server to come up");
      
    }

    
	move_base_msgs::MoveBaseGoal goal;

	goal.target_pose.header.frame_id = "map";
	goal.target_pose.header.stamp = ros::Time::now();

	if (room_num == 0)
	  {
	    goal.target_pose.pose.position.x = 0.0;
	    goal.target_pose.pose.position.y = 0.0;
	    goal.target_pose.pose.orientation.w = 1.0;

	    ROS_INFO("Sending goal for Room 0");
	  }

	else if (room_num == 1)
	  {
	    goal.target_pose.pose.position.x = 6.0;
	    goal.target_pose.pose.position.y = -4.50;
	    goal.target_pose.pose.orientation.w = 1.0;

	    ROS_INFO("Sending goal for Room 1");
	  }
	    
	else if(room_num == 2)
	  {
	    goal.target_pose.pose.position.x = 6.0;
	    goal.target_pose.pose.position.y = -1.50;
	    goal.target_pose.pose.orientation.w = 1.0;

	    ROS_INFO("Sending goal for Room 2");
	  }

	else if(room_num == 3)
	  {
	    goal.target_pose.pose.position.x = 4.0;
	    goal.target_pose.pose.position.y = 8.0;
	    goal.target_pose.pose.orientation.w = 1.0;

	    ROS_INFO("Sending goal for Room 3");
	  }
	else if(room_num == 4)
	  {
	    goal.target_pose.pose.position.x = -4.0;
	    goal.target_pose.pose.position.y = 8.0;
	    goal.target_pose.pose.orientation.w = 1.0;

	    ROS_INFO("Sending goal for Room 4");
	  }
	else
	  {
	    ROS_INFO("Received illegal room number. No action taken");
	    return;
	  }

	    nav_client.sendGoal(goal);

	    nav_client.waitForResult();

	    if(nav_client.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
	      ROS_INFO("Robot reached the source room");
	    else
	      ROS_INFO("Robot failed to reach the source room for some reason");
  }

  // add callback function for alexa subscriber
  // this function will basically prse the string and call manageInputs

  void manageInputs(int source_room, std::string obj, int dest_room)
  {

    if(source_room > 0)
      {
	room_dispatch(source_room);
      }

    if(obj != "" && obj != "empty")
      {
	// object search code
	// currently supports looking for an object with an aruco tag with marker_id =  582

	MoveBaseClient nav_client("move_base",true);

         while(!nav_client.waitForServer(ros::Duration(5.0)))
         {
           ROS_INFO("Waiting for the move_base action server to come up");
         }

    
	move_base_msgs::MoveBaseGoal goal;

	goal.target_pose.header.frame_id = "map";
	goal.target_pose.header.stamp = ros::Time::now();

	goal.target_pose.pose.position.x = -7.0;
	goal.target_pose.pose.position.y = -0.25;
	goal.target_pose.pose.orientation.w = 0.709694;
	goal.target_pose.pose.orientation.z = 0.704509;

	nav_client.sendGoal(goal);

	nav_client.waitForResult();

	if(nav_client.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
	  ROS_INFO("Robot reached the search position");
	else
	  // ROS_INFO("Robot failed to reach the search position for some reason");

	ROS_INFO("Finding Aruco Tag");

	if(!foundAruco)
	  {
	     goal.target_pose.header.frame_id = "map";
	     goal.target_pose.header.stamp = ros::Time::now();

	     goal.target_pose.pose.position.x = -5.11;
	     goal.target_pose.pose.position.y = 1.5045;
	     goal.target_pose.pose.orientation.w = 0.03282;
	     goal.target_pose.pose.orientation.z = 0.99946;

	     nav_client.sendGoal(goal);

	     nav_client.waitForResult();

	     if(nav_client.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
	       ROS_INFO("Robot reached the search position");
	     else
	       //ROS_INFO("Robot failed to reach the search position for some reason");

	     ROS_INFO("Finding Aruco Tag");
	  }

	if(!foundAruco)
	  {
	     goal.target_pose.header.frame_id = "map";
	     goal.target_pose.header.stamp = ros::Time::now();

	     goal.target_pose.pose.position.x = -4.3543;
	     goal.target_pose.pose.position.y = -6.8173;
	     goal.target_pose.pose.orientation.w = 0.03224;
	     goal.target_pose.pose.orientation.z = 0.99480;

	     nav_client.sendGoal(goal);

	     nav_client.waitForResult();

	     if(nav_client.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
	       ROS_INFO("Robot reached the search position");
	     else
	     ROS_INFO("Robot failed to reach the search position for some reason");

	     ROS_INFO("Finding Aruco Tag");
	  }

	if(!foundAruco)
	  {
	     goal.target_pose.header.frame_id = "map";
	     goal.target_pose.header.stamp = ros::Time::now();

	     goal.target_pose.pose.position.x = -7.2625;
	     goal.target_pose.pose.position.y = -8.52249;
	     goal.target_pose.pose.orientation.w = 0.86508;
	     goal.target_pose.pose.orientation.z = 0.501622;

	     nav_client.sendGoal(goal);

	     nav_client.waitForResult();

	     if(nav_client.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
	       ROS_INFO("Robot reached the search position");
	     else
	     ROS_INFO("Robot failed to reach the search position for some reason");

	     ROS_INFO("Finding Aruco Tag");
	  }

	  if(!foundAruco)
	    {
	      ROS_INFO("Failed to find object in scene");
	    }
	  else
	    {
	      ROS_INFO("Found object. Pretend that I picked it up. Going to destination room now");
	    }
      }


    if(dest_room > 0)
      {
	room_dispatch(dest_room);
      }
  }

};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "room_nav");
  ros::NodeHandle n;
  Sub inst(n);
  ros::spin();
  return 0;
}
	

  
