#include "rpcProvider.hpp"
#include "mprpcCommonFunc.hpp"
#include <functional>

void RpcProvider::publishService(google::protobuf::Service *service)
{
    ServiceInfo serviceInfo;
    const google::protobuf::ServiceDescriptor* serviceDescriotor = service->GetDescriptor();
    spdlog::info("service name:" + serviceDescriotor->name());

    for(int i = 0; i < serviceDescriotor->method_count(); i++)
    {
        const google::protobuf::MethodDescriptor* methodDescriotor = serviceDescriotor->method(i);   
        serviceInfo.m_methodMap[methodDescriotor->name()] = methodDescriotor;
    }

    serviceInfo.m_service = service;
    m_ServiceMap[serviceDescriotor->name()] = serviceInfo;
}

void RpcProvider::run()
{

    MprpcCommonFunc::getInstance().init();
    std::string ip, port;
    if(0 == MprpcCommonFunc::getInstance().getValueFromConfig("rpcServerIP", ip)
        || 0 == MprpcCommonFunc::getInstance().getValueFromConfig("rpcServerPort", port))
    {
        spdlog::error("rpcServerIP:" + ip + "   rpcServerPort:" + port);
        return;
    }
    muduo::net::InetAddress addr(ip, uint16_t(atoi(port.c_str())));
    muduo::net::TcpServer server(&m_loop, addr, "chatserver");

        // 注册连接回调
    server.setConnectionCallback(std::bind(&RpcProvider::onConnection, this,  std::placeholders::_1));
    // 注册消息回调
    server.setMessageCallback(std::bind(&RpcProvider::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    // 设置线程数
    server.setThreadNum(4);

    spdlog::info("rpcServerIP:" + ip + "   rpcServerPort:" + port);
    server.start();
    m_loop.loop();

}

void RpcProvider::onConnection(const muduo::net::TcpConnectionPtr &)
{
}

void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr & conn, muduo::net::Buffer *, muduo::Timestamp)
{
    if(!conn->connected())
    {
        conn->shutdown();
        spdlog::info(conn->peerAddress().toIpPort() + "断开连接！");
    }
    else
    {
        spdlog::info("接收到来自" + conn->peerAddress().toIpPort() + "的请求");
    }
}
