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

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "usage: ./mobile_base_client \"ip:port\"\n";
    return 0;
  }
  MobileBaseClient client(
      grpc::CreateChannel(argv[1], grpc::InsecureChannelCredentials()));
  HandshakeResultType feedback;

  std::cout << "Connection: " << client.IsRobotBaseAlive() << std::endl;
  feedback = client.RequestControl();

  if (feedback.code != HANDSHAKE_RESULT_CONTROL_ACQUIRED &&
      feedback.code != HANDSHAKE_RESULT_ALREADY_GAINED_CONTROL) {
    std::cout << "Failed to acquire control of the robot." << std::endl;
    return -1;
  }

  ZVector3 linear, angular;
  linear.y = 0;
  linear.z = 0;
  angular.x = 0;
  angular.y = 0;

  linear.x = 0.5;
  angular.z = 0.0;

  MotionStateMsg state;

  while (client.SdkHasControlToken()) {
    client.SetMotionCommand(linear, angular);
    // std::cout << "Connection: " << client.IsRobotBaseAlive() << std::endl;
    std::cout << "motion command: " << linear.x << " , " << angular.z
              << std::endl;
    state = client.GetMotionState();
    
    std::cout<<"motion state:" << state.actual_motion.linear.x << " , "<< state.actual_motion.angular.z<< std::endl;
    auto data = client.GetActuatorState();
    std::cout<< data[0].motor.rpm<<"\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  return 0;
}