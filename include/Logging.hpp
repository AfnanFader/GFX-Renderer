#ifndef LOGGING_HPP
#define LOGGING_HPP
#pragma once

#include <spdlog/spdlog.h>
#include <string.h>

//----------------------------------------------------------------------------//

#ifdef _WIN32
    #define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
    #define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

//----------------------------------------------------------------------------//

#define LOG_INFO(...) SPDLOG_LOGGER_CALL(spdlog::default_logger(), spdlog::level::info, __VA_ARGS__)

#define LOG_WARN(...) SPDLOG_LOGGER_CALL(spdlog::default_logger_raw(), spdlog::level::warn, __VA_ARGS__)

#define LOG_ERROR(...) SPDLOG_LOGGER_CALL(spdlog::default_logger_raw(), spdlog::level::err, __VA_ARGS__)

#define LOG_ERROR_EXIT(...) \
    SPDLOG_LOGGER_CALL(spdlog::default_logger_raw(), spdlog::level::err, __VA_ARGS__); \
    std::exit(EXIT_FAILURE); \

#endif