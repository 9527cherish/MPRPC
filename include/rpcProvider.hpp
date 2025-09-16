#pragma once 

#include "google/protobuf/service.h"

class RpcProvider
{
public:
    // 发布rpc方法
    void publishService(google::protobuf::Service* service);
    
    // 运行，供其他服务调用
    void run();
};