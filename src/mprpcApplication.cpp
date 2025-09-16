#include "mprpcApplication.hpp"

void MprpcApplication::init()
{
}

MprpcApplication &MprpcApplication::getInstance()
{
    static MprpcApplication ins;
    return ins;
}
