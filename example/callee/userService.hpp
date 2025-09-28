#include <iostream>
#include <string>
#include "user.pb.h"

class UserService : public fixbug::UserServiceRpc
{

    bool Login(std::string name, std::string pwd)
    {
        std::cout << "" << std::endl;
        std::cout << "name:" << name << "pwd:" << pwd << std::endl;
        return true;
    }

    void Login(::google::protobuf::RpcController* controller,
                const ::fixbug::LoginRequest* request,
                ::fixbug::LoginResponse* response,
                ::google::protobuf::Closure* done) override
    {
        std::string name = request->name();
        std::string pwd = request->pwd();
        fixbug::ResultCode* code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        bool loginResult = Login(name, pwd);
        response->set_success(loginResult);
        done->Run();
    }
};  