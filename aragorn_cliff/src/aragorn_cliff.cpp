#include <iostream>
#include <stdio.h>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Range.h>
#include <std_msgs/Float64.h>
#include <std_msgs/String.h>


class cliffDetector
{
  private:
    ros::NodeHandle nh;
    ros::Publisher pub_max_vel;
    ros::Subscriber sub_cliff_front, sub_cliff_left, sub_cliff_right;
    float c_f_state, c_l_state, c_r_state;
    float range_straingcase = 0.18;

  public:
    cliffDetector()
    {
      pub_max_vel = nh.advertise<std_msgs::Float64>("/max_vel", 1);

      sub_cliff_front = nh.subscribe("/sensor/cliff_front", 1, &cliffDetector::cbC_Front, this);
      sub_cliff_left  = nh.subscribe("/sensor/cliff_left", 1, &cliffDetector::cbC_Left, this);
      sub_cliff_right = nh.subscribe("/sensor/cliff_right", 1, &cliffDetector::cbC_Right, this);

      ros::Rate loop_rate(20);

      printf ("Cliff detection already");

      while(ros::ok())
      {
        sensor_check();

        ros::spinOnce();
        loop_rate.sleep();
      }

    }

    void cbC_Front(const sensor_msgs::Range::ConstPtr& data)
    {
      c_f_state = data->range;
      printf("CF = %f  | ", c_f_state);

    }

    void cbC_Left(const sensor_msgs::Range::ConstPtr& data)
    {
      c_l_state = data->range;
      printf("CL = %f  | ", c_f_state);
    }

    void cbC_Right(const sensor_msgs::Range::ConstPtr& data)
    {
      c_r_state = data->range;
      printf("CR = %f  \n", c_f_state);
    }

    void sensor_check(void)
    {
      if( ( c_f_state > range_straingcase ) || ( c_r_state > range_straingcase ) || ( c_l_state > range_straingcase ) )
      {
        printf("Detec Straingcase\n");
        fnStop();
      }
      else
      {
        printf("Go\n");
        fnGo();
      }
      
    }

    void fnStop(void)
    {
      std_msgs::Float64 max_vel_msg;
      max_vel_msg.data = 0.0;
      pub_max_vel.publish(max_vel_msg);
    }

    void fnGo(void)
    {
      std_msgs::Float64 max_vel_msg;
      max_vel_msg.data = 1.5;
      pub_max_vel.publish(max_vel_msg);
    }

};//End of class SubscribeAndPublish






int main(int argc, char **argv)
{
  //Initiate ROS
  ros::init(argc, argv, "subscribe_and_publish");

  //Create an object of class SubscribeAndPublish that will take care of everything
  cliffDetector cliff;

  ros::spin();

  return 0;
}