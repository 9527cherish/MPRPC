#pragma once 
#include <fstream>
#include <string>
#include <unordered_map>

class MprpcCommonFunc
{
public:

    static MprpcCommonFunc& getInstance();

    // 初始化日志
    void init_logger(const std::string& log_name);


    // 去除字符串前后的空格
    std::string trim(const std::string &s);
    // 从配置文件读取键值对
    bool readConfig();


private:

    std::string logPath;
    std::string logName;

    std::unordered_map<std::string, std::string> m_config;

    MprpcCommonFunc();

    // 刪除左值和右值  拷貝构造和赋值
    MprpcCommonFunc(const MprpcCommonFunc&) = delete;
    MprpcCommonFunc(const MprpcCommonFunc&&) = delete;
    MprpcCommonFunc& operator=(const MprpcCommonFunc&) = delete;
    MprpcCommonFunc& operator=(MprpcCommonFunc&&) = delete;
};