#pragma once 


class MprpcApplication
{

public:
    static void init();
    static MprpcApplication& getInstance();

private:
    MprpcApplication() = default;

    // 刪除左值和右值  拷貝构造和赋值
    MprpcApplication(const MprpcApplication&) = delete;
    MprpcApplication(const MprpcApplication&&) = delete;
    MprpcApplication& operator=(const MprpcApplication&) = delete;
    MprpcApplication& operator=(MprpcApplication&&) = delete;
};