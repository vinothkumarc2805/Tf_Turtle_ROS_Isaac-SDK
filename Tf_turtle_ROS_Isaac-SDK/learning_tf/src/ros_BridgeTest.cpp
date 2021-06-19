#include <ros/ros.h>
// Include the appropriate message headers for your test case
#include <geometry_msgs/Pose2D.h>
#include <tf/transform_broadcaster.h>

#include <geometry_msgs/Pose.h>
// Sending random points for navigation testing
#include <random>

#include <tf/transform_listener.h>
#include <turtlesim/Spawn.h>
#include <geometry_msgs/Twist.h>


geometry_msgs::Twist turtle_cmd_vel;

// A simple call back to acknowledge receiving a message
 void navigationCallback(const geometry_msgs::Twist::ConstPtr& msg) {
  ROS_INFO("Hello-Heard: cmdvel from ISaac is Linear velocity:%f and Angular velocity:%f", msg->linear.x, msg->angular.z);

    turtle_cmd_vel.linear.x = msg->linear.x;
    turtle_cmd_vel.angular.z = msg->angular.z;

 }	

constexpr double pi = 3.14159265358979323846;

int main(int argc, char **argv) {
    // Select a random point to send as a goal pose
    std::random_device rd;
    std::mt19937_64 eng(rd());
    std::uniform_real_distribution<> dist_x(10, 30);
    std::uniform_real_distribution<> dist_y(10, 30);
    std::uniform_real_distribution<> dist_theta(-pi, pi);

    ros::init(argc, argv, "ros_BridgeTest");

    ros::NodeHandle ros_BridgeTestNode;

    // Channel to publish navigation requests to.
    // Replace with desired message and channel name for your test case
    ros::Publisher nav_pub = ros_BridgeTestNode.advertise<geometry_msgs::Pose>("isaac_navigation2D_request", 1000);


    ros::Publisher isaac_pub = ros_BridgeTestNode.advertise<geometry_msgs::Twist>("turtle1/cmd_vel", 10);
    // Listen for pose/navigation updates.
    // Replace with desired message and channel name for your test case
     ros::Subscriber nav_sub = ros_BridgeTestNode.subscribe("isaac_goal_status", 1000, &navigationCallback);


    //while (ros::ok()) {
        // Generate a random goal location

  tf::TransformListener listener;

      // Send a message and check updates every ten seconds
    ros::Rate rate(10.0);


//  ros::Rate rate(10.0);
  while (ros_BridgeTestNode.ok()){
    tf::StampedTransform transform;
    try{
      ros::Time now = ros::Time::now();
      listener.waitForTransform("/turtle1", "/world",
                              now, ros::Duration(3.0));
      listener.lookupTransform("/turtle1", "/world",
                             now, transform);
    }
    catch (tf::TransformException &ex) {
      ROS_ERROR("%s",ex.what());
      ros::Duration(1.0).sleep();
      continue;
    }

    geometry_msgs::Pose ROS_Isaac_pose;
    ROS_Isaac_pose.position.x = transform.getOrigin().x();
    ROS_Isaac_pose.position.y = transform.getOrigin().y();
    ROS_Isaac_pose.position.z = transform.getOrigin().z();
    ROS_Isaac_pose.orientation.x = transform.getRotation().x();
    ROS_Isaac_pose.orientation.y = transform.getRotation().y();
    ROS_Isaac_pose.orientation.z = transform.getRotation().z();
    ROS_Isaac_pose.orientation.w = transform.getRotation().w();

    nav_pub.publish(ROS_Isaac_pose);

//        geometry_msgs::Pose2D pose_msg;
//        pose_msg.x = dist_x(eng);
//        pose_msg.y = dist_y(eng);
//        pose_msg.theta = dist_theta(eng);

//        ROS_INFO("Sent: Move Isaac to  X:%f, Y:%f, Theta: %f", pose_msg.x, pose_msg.y, pose_msg.theta);
//        nav_pub.publish(pose_msg);


//    turtle_cmd_vel.linear.x = 2.0;
//   turtle_cmd_vel.angular.z = 1.8;


    isaac_pub.publish(turtle_cmd_vel);
        // Manually spin to check messages
    ros::spinOnce();

    rate.sleep();
    }
    return 0;
}
