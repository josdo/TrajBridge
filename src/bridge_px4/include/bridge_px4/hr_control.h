/**************************************************************************
  File Name    : hr_control.h
  Author       : JunEn Low
                 Multi-Robot Systems Lab (MSL), Stanford University
  Contact      : jelow@stanford.edu
  Create Time  : Jan 27, 2020.
  Description  : Publisher to Handle Setpoint by Error Over Mavlink
**************************************************************************/

#ifndef __HR_CONTROL_H__
#define __HR_CONTROL_H__

#include "ros/ros.h"
#include "bridge_px4/TrajTransfer.h"
#include "bridge_px4/TrajUpdate.h"

#include <std_msgs/Float32MultiArray.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/TwistStamped.h>
#include <mavros_msgs/AttitudeTarget.h>
#include <Eigen/Dense>
#include <numeric>

using namespace Eigen;
using namespace std;
using namespace std_msgs;

using OneD = vector<double>;
using TwoD = vector<vector<double>>;
using ThreeD = vector<vector<vector<double>>>;


class HR_Control
{
public:
  // Constructor
  HR_Control();
  virtual ~HR_Control();

  bool transfer(bridge_px4::TrajTransfer::Request& req, bridge_px4::TrajTransfer::Response& res);

protected:
  ros::NodeHandle nh;

private:
  // Limit Check variables
  float pxy_slim_;
  float pz_slim_;
  float v_slim_;
  float q_slim_;
  float ep_lim_;
  float eq_lim_;
  Matrix<float,10,1> del_slim;
  Matrix<float,7,1> err_lim;

  static const string states[];

  // ROS variables
  ros::Subscriber    pose_curr_sub;
  ros::Subscriber    vel_curr_sub;
  ros::Publisher     att_sp_pub;
  ros::ServiceServer traj_server;
  ros::Subscriber    traj_sub;

  // ROS Timers
  ros::Timer closedLoop;      // closed-loop control update timer

  // Trajectory Variables
  vector<float> x_arr;
  vector<float> u_arr;
  vector<float> L_arr;

  Matrix<float,4,1>  u_curr;
  Matrix<float,4,17> L_curr;
  Matrix<float,17,1> x_bar;
  Matrix<float,17,1> x_curr;  // Current State
  Matrix<float,17,1> del_x;   // Current State Error Relative to Nominal
  Matrix<float,4,1>  u_br;    // Current Input
  Matrix<float,7,1>  del_z;   // Integral Delta

  // Counter and Time Variables
  double t_dt;
  int k_main;
  int N;

  // Quad Setpoints
  mavros_msgs::AttitudeTarget att_sp_out;      // Setpoint Attitude (body rate, orientation, thrust) Out

  // Function(s)
  bool policy_update();
  void delta_update();
  void controller();
  bool limit_check();
  void pose_curr_cb(const geometry_msgs::PoseStamped::ConstPtr& msg);
  void vel_curr_cb(const geometry_msgs::TwistStamped::ConstPtr& msg);
  void clc_cb(const ros::TimerEvent& event);
  void traj_upd_cb(const bridge_px4::TrajUpdate::ConstPtr& msg);

};

#endif