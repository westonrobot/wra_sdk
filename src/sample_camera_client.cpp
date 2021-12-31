/* 
 * sample_camera_client.cpp
 * 
 * Created on: Jun 28, 2021 16:11
 * Description: 
 * 
 * Copyright (c) 2021 Weston Robot Pte. Ltd.
 */ 

#include <iostream>
#include <unistd.h>

#include "wra_sdk/video_streaming/camera_streaming_client.hpp"

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "usage: ./sample_camera_client \"ip:port\"\n";
    return 0;
  }
  std::cout << "Creating channel ptr.\n";
  auto channel_ptr =
      grpc::CreateChannel(argv[1], grpc::InsecureChannelCredentials());
  std::cout << "Instantiating client object.\n";
  westonrobot::CameraStreamingClient client(channel_ptr);
  std::cout << "Entering connect loop...\n";
  if (!client.ConnectLoop(3)) {
    std::cout << "Failed to connect to server!\n";
    return 0;
  }

  // Start a Ping thread. This serves as a heartbeat signal to keep the
  // connection alive.
  std::thread ping_thread = std::thread([&client]() {
    while (true) {
      client.QueryServerStatus(1);  // Ping server with 1s interval
      std::cout << "Latency: " << client.GetLatency()
                << std::endl;  // Get latency in ms
      std::cout << "-----------------------\n\n";
    }
  });

  // Read and show frames
  while (client.GetStreamerStatus() == westonrobot::StreamerStatus::Up) {
    int buf_size = 0;
    cv::imshow("Sample Stream", client.GetFrame(buf_size));
    cv::waitKey(33);
    std::cout << "Data size: " << buf_size << std::endl;
  }
  ping_thread.join();

  /* Note: Streaming can be paused and resumed with: 
     client.EnableStreamer();
     client.DisableStreamer();
  */

  return 0;
}