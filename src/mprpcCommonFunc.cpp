#include "mprpcCommonFunc.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include "spdlog/sinks/rotating_file_sink.h"
#include <filesystem>


MprpcCommonFunc::MprpcCommonFunc()
{
    logName = "../logs/MPRPC/logfile";
    logPath = "../logs/MPRPC";
    
}

MprpcCommonFunc &MprpcCommonFunc::getInstance()
{
    static MprpcCommonFunc ins;
    return ins;
}

void MprpcCommonFunc::init()
{
    init_logger(logName);
    readConfig();
}

void MprpcCommonFunc::init_logger(const std::string &log_name)
{
        // 自动创建日志目录
    std::filesystem::create_directories(logPath);

    // 控制台彩色输出   
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::debug);

    // 文件输出（支持每天一个文件）
    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        log_name, 1024 * 1024 * 5, 3);
    file_sink->set_level(spdlog::level::debug);;

    // 日志器合并
    std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};
    auto logger = std::make_shared<spdlog::logger>("main_logger", sinks.begin(), sinks.end());

    // 设置为默认 logger
    spdlog::set_default_logger(logger);
    spdlog::flush_on(spdlog::level::info); // 自动 flush

    // 日志等级与格式
    spdlog::set_level(spdlog::level::debug); // 显示全部
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v");
}

std::string MprpcCommonFunc::trim(const std::string &s)
{
    auto start = s.begin();
    while (start != s.end() && std::isspace(*start)) {
        start++;
    }
    
    auto end = s.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(*end));
    
    return std::string(start, end + 1);
}

bool MprpcCommonFunc::readConfig()
{
    std::string filename = "../config.ini";
    std::ifstream file(filename);
    if (!file.is_open()) {
        spdlog::error ("无法打开配置文件: " + filename);
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // 处理空行
        line = trim(line);
        if (line.empty()) {
            continue;
        }
        
        // 忽略注释行
        if (line[0] == '#') {
            continue;
        }
        
        // 查找等号位置
        size_t eqPos = line.find('=');
        if (eqPos == std::string::npos) {
            continue; // 跳过没有等号的行
        }
        
        // 提取键和值
        std::string key = trim(line.substr(0, eqPos));
        std::string value = trim(line.substr(eqPos + 1));
        
        if (!key.empty() && !value.empty()) {
            m_config[key] = value;
        }
    }
    
    file.close();
    return true;
}

int MprpcCommonFunc::getValueFromConfig(const std::string key, std::string &value)
{
    auto iter = m_config.find(key);
    if(iter == m_config.end())
    {
        spdlog::error("配置文件不存在关键字:" + key);
        value = "";
        return 0;
    }
    
    value = iter->second;
    return 1;
}
