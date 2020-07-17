# ROBO-CARE: Caring for the Sick at Home using Robots

**Project By:** Tanmay Agarwal, Kathan Shah and Muntaqim Mehtaz  
**Supervisor:** Dr. Junaed Sattar  
Final Project for CSCI 5551, University of Minnesota, Twin Cities

This project simulates a TiaGo humanoid robot as a "Home Caretaking Robot". 
Users can communicate with the robot through two modes:
1. Hand Gestures - Each room in the simulation is given a number. To tell the robot to go to room 1, hold up one finger; two fingers for room two and so forth
2. Voice Commands - The robot can communicate over Alexa. Simply say "Alexa, tell RoboCare to go to room <insert number>". Instructions to do so are below

Some preliminary code for manipulation is also present, but it does not have full capabilities at this point.
Runs on ROS-Melodic, Rviz and Gazebo

## To initialize: 

Run the `install_packages.sh` bash script to get all required packages
You will have to use `chmod` to give execution access to this script

### NOTE: 
The src folder of the carkin workspace is too large to upload on github. 
Please find it on google drive here: https://drive.google.com/drive/folders/10PZcTk0JXZnHjjIUpCupsAGWwbPgtYaD?usp=sharing
The install packages assume you have ROS, Gazebo and RViz installed.
If not, please run:
`sudo apt install ros-melodic-desktop-full

cd <the name of your catkin_ws>
catkin_make
source devel/setup.bash
`
Note: The catkin_make can take a few minutes

## RUNNING HRI COMPONENTS

### TO RUN NAVIGATION THROUGH HAND_GESTURES:

On 1 terminal:
`roslaunch tiago_2dnav_gazebo apartment_navigation.launch public_sim:=true`

NOTE: Before proceeding, please make sure that TiaGo has "tucked" its arm

Terminal 2:
`rosrun room_nav room_nav`

Terminal 3:
`rosrun hand_gestures gesture_detection.py`

Hand gestures output is on the /gesture_output channel
If vision algorithm is unstable, in order to observe navigation behavior, you can do:

`rostopic pub /gesture_output std_msgs/Int16 <number between 1 and 4>`


------------------------------------------------------

### TO RUN ALEXA COMMANDS:

NOTE: If you want to run navigation with alexa, before running the rosbridge server and nodejs scripts, you will need to run The first 2 terminals as described above for hand gestures. These nodes handle navigation and communication with HRI components.
----

There are two ways to test the Alexa functionality. Method one is easier as everything is running locally, method two is has few extra steps but it is cooler as you will be able to control the robot movement using your voice.

Method 1-

Once you have the package downloaded you can simply follow the steps below-

Install rosbridge-server (this should have been done through the shell script already)
`sudo apt-get install ros-melodic-rosbridge-server`
Running RoboCare
To begin, start the rosbridge websocket with the following:
`roslaunch rosbridge_server rosbridge_websocket.launch`
To start the alexa-app-server, navigate into the examples directory and run the server using node in a new terminal:
`cd alexa-node-ros/examples/
sudo node server.js`

If there are no errors, then you can navigate to http://localhost:8081/alexa/robo-care in your browser for the interactive Echo simulator built by alexa-js
In the drop down menu of the echo simulator, select “IntentRequest” and then select either of “MoveRoom”, “FindObj” or “MoveObj” depending on what you want to do.

Method 2-

Requirements - Amazon developer account

Start the rosbridge-server in the same manner as Method 1.

`sudo apt-get install ros-melodic-rosbridge-server`

Next open a new terminal and navigate to the robo-care directory

`cd alexa-node-ros/examples/apps/robo-care`

Now we need to run a proxy that forwards the local server to a public url (Make sure bespoken is installed before proceeding)

`sudo bst proxy lambda index.js`

This will generate a public URL which you will need soon

Now navigate to Alexa Skills console (https://developer.amazon.com/en-US/alexa)
Click on create Alexa Skill, then click on Console. Now click on Create Skill then follow the on screen instructions to create a custom skill, type skill name as “robo care” and choose “Provision your own” for the skill’s backend resources. Now click create skill and choose scratch template. This will take you to the developer console. Under the “Interaction Model” click on “JSON editor” and upload or copy paste the content of file “schema.json” which exists in the alexa-node-ros folder. Then click build model and move to “Endpoint”, here select HTTPS and paste the link you generated when you ran the bst command earlier. In the drop down menu select “My development endpoint is a sub-domain of a domain that has a wildcard certificate from a certificate authority”. Now save endpoints and you are good to go. 

Move to the test tab on the console, here you can either speak or type commands like - 
“Alexa, ask robo care to go to room {num}” or “Alexa, ask robo care to find {object}” or “Alexa, ask robo care to go to room {num} and find {object} and deliver to room {num}”.

This will generate the same results as Method 1.


If above alexa integration does note work, you can publish directly on the /alexa_msgs channel
to observe output behavior

`rostopic pub /alexa_msgs std_msgs/String "<source room num>, object_identifying_string, <destination_room_num>"`

Note that the room numbers must be between 1 and 4. Currently, for object string we only support one object having
an Aruco tag of marker_id = 582. Any string entered for object string apart from "empty" or "" will be interpreted
as a string referring to this object

-----------------------------------------------------
### TO ATTEMPT OBJECT PICK UP:

On 1 terminal:
`roslaunch tiago_2dnav_gazebo apartment_navigation.launch public_sim:=true`

NOTE: Before proceeding, please make sure that TiaGo has "tucked" its arm

Using RViz, send a 2DNav Goal to the location where the object (white Aruco Cube) is placed.
You can place it yourself in Gazebo by going to the Insert tab and choosing the aruco_cube_5cm model. By default, this is placed on the table by the person in the simulation environment. 

Once the robot is facing the white aruco cube (you can check so by looking at the camera stream in RViz), on a separate terminal, run:
`roslaunch tiago_pick_demo pick_demo.launch`

On another terminal, run:
`rosservice call /pick_gui`

Tiago will now attempt to pick up the object. You will see its attempts at detection and localization in the new RViz window
