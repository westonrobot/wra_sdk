/* 
 * mobile_base_client.cpp
 * 
 * Created on: Oct 19, 2021 21:52
 * Description: 
 * 
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */ 

#include <memory.h>
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

#include <grpc/grpc.h>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>

#include "wra_sdk/robot_control/mobile_base_client.hpp"

using namespace westonrobot;

int main() {
  std::string target_str = "10.20.0.1:80086";
  
  MobileBaseClient client(
      grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));

  RpcCallResult rpc_call_result;
  if (!client.HasControlToken(rpc_call_result, 50) &&
      rpc_call_result == RpcCallResult::Success) {
    auto response = client.RequestControl(rpc_call_result, 50);
    if (rpc_call_result == RpcCallResult::Success) {
      if (response.feedback() ==
              rpc_msgs::mobile_base::ControlRequestFeedback::CONTROL_ACQUIRED ||
          response.feedback() == rpc_msgs::mobile_base::ControlRequestFeedback::
                                     ALREADY_GAINED_CONTROL) {
        std::cout << "Acquired control of the robot." << std::endl;
      } else {
        std::cout << "Failed to acquire control of the robot: "
                  << response.feedback() << std::endl;
        return -1;
      }
    }
  }

  rpc_msgs::mobile_base::MotionCommand motion_cmd;
  motion_cmd.mutable_linear()->set_x(0.5);
  motion_cmd.mutable_linear()->set_y(0);
  motion_cmd.mutable_linear()->set_z(0);
  motion_cmd.mutable_angular()->set_x(0);
  motion_cmd.mutable_angular()->set_y(0);
  motion_cmd.mutable_angular()->set_z(0.5);

  while (client.HasControlToken(rpc_call_result, 50) &&
         rpc_call_result == RpcCallResult::Success) {
    RpcCallResult ret;
    client.SetMotionCommand(motion_cmd, ret, 50);

    // std::cout << "motion command: " << motion_cmd.linear().x() << " , "
    //           << motion_cmd.angular().z() << std::endl;

    auto robot_state = client.GetRobotState(ret, 50);
    if (ret == RpcCallResult::Success) {
      printf("linear: %f, angular: %f\n", robot_state.odometry().linear().x(),
             robot_state.odometry().angular().z());
    }

    auto sensor_data = client.GetSensorData(ret, 50);
    if (ret == RpcCallResult::Success) {
      auto us_data = sensor_data.ultrasonic().data();
      std::cout << "Ultrasonic: ";
      for (auto it = us_data.begin(); it != us_data.end(); ++it) {
        std::cout << it->range() << ",";
      }
      std::cout << std::endl;

      auto tof_data = sensor_data.tof().data();
      std::cout << "Tof: ";
      for (auto it = tof_data.begin(); it != tof_data.end(); ++it) {
        std::cout << it->range() << ",";
      }
      std::cout << std::endl;
    }
  }

  return 0;
}