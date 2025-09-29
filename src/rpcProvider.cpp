#include "rpcProvider.hpp"
#include "mprpcCommonFunc.hpp"
#include <functional>
#include "rpcHeader.pb.h"

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
    m_serviceMap[serviceDescriotor->name()] = serviceInfo;
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

void RpcProvider::onConnection(const muduo::net::TcpConnectionPtr& conn)
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

void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr & conn, muduo::net::Buffer* buffer, muduo::Timestamp)
{
    std::string receiveBuffer = buffer->retrieveAllAsString();

    // headerSize 中存放头的长度 serviceName +  methodName + argsSize
    uint32_t headerSize = 0;
    receiveBuffer.copy((char*)&headerSize, 4, 0);

    mprpc::RpcHeader rpcHeader;
    std::string serviceName ;
    std::string methodName;
    uint32_t argsSize;

    std::string rpcHeaderStr = receiveBuffer.substr(4, headerSize);
    if(rpcHeader.ParseFromString(rpcHeaderStr))
    {
        serviceName = rpcHeader.servicename();
        methodName = rpcHeader.methodname();
        argsSize = rpcHeader.argssize();
    }
    else{
        spdlog::error(std::string("rpcHeaderStr") + "反序列化失败");  
        return;
    }

    // 获取头中的参数之后，再获取解析args
    std::string argsStr = receiveBuffer.substr(4+headerSize, argsSize);
    spdlog::info(std::string("serviceName:") + serviceName);  
    spdlog::info(std::string("methodName:") + methodName);  
    spdlog::info(std::string("argsSize:") + std::to_string(argsSize));  
    spdlog::info(std::string("argsStr:") + argsStr);  


    google::protobuf::Service* service = nullptr;
    google::protobuf::MethodDescriptor* methodDescri = nullptr;

    if(1 != getMethod(serviceName, methodName, service, methodDescri)
        || nullptr == service || nullptr == methodDescri){
            return;
    }

    // 将方法转化成对应request、response请求
    google::protobuf::Message* request = service->GetRequestPrototype(methodDescri).New();
    google::protobuf::Message* response = service->GetResponsePrototype(methodDescri).New();

    if(nullptr == request || nullptr == response)
        return;

    // 看客户端是如何序列化args的
    if(!request->ParseFromString(argsStr))
    {
        spdlog::error(std::string("argsStr") + "反序列化失败");  
        return;
    }

    // 给下面的method方法调用，定义一个闭包
    google::protobuf::Closure* done = google::protobuf::NewCallback<RpcProvider, const muduo::net::TcpConnectionPtr&, google::protobuf::Message*>
                                    (this, &RpcProvider::sendRpcResponse, conn, response);

    service->CallMethod(methodDescri, nullptr, request, response, done);
}

int RpcProvider::getMethod(std::string serviceName, std::string methodName, google::protobuf::Service *service, const google::protobuf::MethodDescriptor *methodDescri)
{
    auto serviceIter = m_serviceMap.find(serviceName);
    if(serviceIter == m_serviceMap.end())
    {
        spdlog::info("该rpc服务没有提供:" + serviceName);
        return -2;
    }

    ServiceInfo serviceInfo = serviceIter->second;

    auto methodIter = serviceInfo.m_methodMap.find(methodName);
    if(methodIter == serviceInfo.m_methodMap.end())
    {
        spdlog::info("该rpc服务没有提供:" + methodName);
        return -1;
    }

    service = serviceInfo.m_service;
    methodDescri = methodIter->second;
    return 1;
}

void RpcProvider::sendRpcResponse(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *response)
{
    std::string responseStr;
    if(response->SerializeToString(&responseStr))
    {
        conn->send(responseStr);
    }
    else
    {
        spdlog::error("序列化response失败");  
    }
    conn->shutdown();
}
