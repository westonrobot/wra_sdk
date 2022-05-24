/*
 * video_streaming_client.cpp
 *
 * Created on: Oct 19, 2021 21:52
 * Description:
 *
 * Copyright (c) 2021 Ruixiang Du (rdu)
 */

#include <thread>
#include <chrono>
#include <array>
#include <iostream>

#include <grpc/grpc.h>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>

#include "wra_sdk/video_streaming/video_streaming_client.hpp"

#include "opencv2/opencv.hpp"

using namespace westonrobot;
using asio::ip::udp;

cv::Mat ToCvImage(unsigned char* data, size_t len) {
  std::vector<unsigned char> img_data;
  img_data.resize(len);
  std::memcpy(img_data.data(), data, len);

#if (CV_MAJOR_VERSION <= 3)
  return cv::imdecode(img_data, CV_LOAD_IMAGE_COLOR);
#else
  return cv::imdecode(img_data, cv::IMREAD_COLOR);
#endif
}

int main(int argc, char* argv[]) {
  std::string server_ip = "10.20.0.1";
  std::string server_port = "80056";

  std::string target_str = server_ip + ":" + server_port;
  VideoStreamingClient client(
      server_ip,
      grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));

  RpcCallResult rpc_call_result;
  bool ret = client.IsStreamingActive(rpc_call_result);
  if (ret && rpc_call_result == RpcCallResult::Success) {
    std::cout << "service online" << std::endl;
  } else {
    std::cout << "service offline" << std::endl;
    return -1;
  }

  std::vector<StreamDescriptor> stream_list =
      client.GetStreamList(rpc_call_result);
  if (rpc_call_result == RpcCallResult::Success) {
    for (const auto& desc : stream_list) {
      std::cout << desc << std::endl;
    }
  }

  client.InitStreamReceiver(stream_list);

  for (const auto& desc : stream_list) {
    client.StartStreamReceiver(desc.stream_id);
  }

  std::vector<cv::Mat> imgs;
  imgs.resize(stream_list.size());
  std::vector<unsigned char> buffer;
  while (true) {
    for (const auto& desc : stream_list) {
      if (client.GetStreamBuffer(desc.stream_id, buffer)) {
        std::cout << "buffer " << desc.stream_id << ": " << buffer.size()
                  << std::endl;
        if (!buffer.empty()) {
          imgs[desc.stream_id] = ToCvImage(buffer.data(), buffer.size());
          if (!imgs[desc.stream_id].empty()) {
            std::string name = "cam" + std::to_string(desc.stream_id);
            cv::imshow(name.c_str(), imgs[desc.stream_id]);
          }
        }
      }
      std::cout << "latency " << desc.stream_id << ": "
                << client.GetDataTransmissionLatency(desc.stream_id)
                << std::endl;
    }

    if (cv::waitKey(30) == 27) break;
  }

  return 0;
}