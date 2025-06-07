#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <iostream>
#include <spdlog/sinks/stdout_color_sinks.h>
std::shared_ptr<spdlog::logger> g_default_logger;
namespace ns_log
{
   
    void init_log(bool mode, const std::string &file_name, int level)
    {
        if (mode == false)
        {

            g_default_logger = spdlog::stdout_color_mt("default_logger");
            g_default_logger->set_level(spdlog::level::trace);
            g_default_logger->flush_on(spdlog::level::trace);
        }

        else
        {

            g_default_logger = spdlog::basic_logger_mt("default_logger", file_name);
            g_default_logger->set_level((spdlog::level::level_enum)level);
            g_default_logger->flush_on((spdlog::level::level_enum)level);
        }
        g_default_logger->set_pattern("[%n][%H:%M:%S][%t][%-8l]%v");
    }
};
#define SLOG_TRACE(format, ...) g_default_logger->trace(std::string("[{}:{}] ") + format, __FILE__, __LINE__, ##__VA_ARGS__)
#define SLOG_DEBUG(format, ...) g_default_logger->debug(std::string("[{}:{}] ") + format, __FILE__, __LINE__, ##__VA_ARGS__)
#define SLOG_INFO(format, ...) g_default_logger->info(std::string("[{}:{}] ") + format, __FILE__, __LINE__, ##__VA_ARGS__)
#define SLOG_WARN(format, ...) g_default_logger->warn(std::string("[{}:{}] ") + format, __FILE__, __LINE__, ##__VA_ARGS__)
#define SLOG_ERROR(format, ...) g_default_logger->error(std::string("[{}:{}] ") + format, __FILE__, __LINE__, ##__VA_ARGS__)
#define SLOG_FATAL(format, ...) g_default_logger->critical(std::string("[{}:{}] ") + format, __FILE__, __LINE__, ##__VA_ARGS__)
