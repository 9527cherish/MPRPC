#pragma once 

#include <memory>
#include "google/protobuf/service.h"
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <google/protobuf/descriptor.h>
#include <unordered_map>

class RpcProvider
{
public:
    // 发布rpc方法
    void publishService(google::protobuf::Service* service);
    
    // 运行，供其他服务调用
    void run();

private:
    // 连接的回调函数
    void onConnection(const muduo::net::TcpConnectionPtr& conn);
    // 收到信息的回调函数
    void onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer*, muduo::Timestamp);

    std::unique_ptr<muduo::net::TcpServer> m_pServer;
    muduo::net::EventLoop m_loop;

    struct ServiceInfo
    {
        google::protobuf::Service* m_service;
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_methodMap;
    };

    // 存储所有服务的信息
    std::unordered_map<std::string, ServiceInfo> m_ServiceMap;
};