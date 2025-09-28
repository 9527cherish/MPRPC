#include "userService.hpp"
#include "rpcProvider.hpp"

int main()   // int argc, const char** argv
{
    
    RpcProvider rpcProvider;
    rpcProvider.publishService(new UserService());
    rpcProvider.run();
    return 0;
}