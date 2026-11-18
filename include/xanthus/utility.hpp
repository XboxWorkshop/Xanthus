#pragma once

#include <string>

#if XANTHUS_LOG
#include <fmt/format.h>
#include <utility>
#endif

namespace xanthus::utility {
#if XANTHUS_LOG
enum class ELogLevel {
    Verbose = 0,
    Debug   = 1,
    Info    = 2,
    Warn    = 3,
    Error   = 4
};

void log(ELogLevel level, const char* tag, const std::string& message);

template <typename... Args>
void logf(ELogLevel level, const char* tag, fmt::format_string<Args...> fmtStr, Args&&... args) {
    log(level, tag, fmt::format(fmtStr, std::forward<Args>(args)...));
}
#endif
}

#if XANTHUS_LOG
#define XANTHUS_VERBOSE(...) xanthus::utility::logf(xanthus::utility::ELogLevel::Verbose, __FILE__, __VA_ARGS__)
#define XANTHUS_DBG(...)     xanthus::utility::logf(xanthus::utility::ELogLevel::Debug,   __FILE__, __VA_ARGS__)
#define XANTHUS_INFO(...)    xanthus::utility::logf(xanthus::utility::ELogLevel::Info,    __FILE__, __VA_ARGS__)
#define XANTHUS_WARN(...)    xanthus::utility::logf(xanthus::utility::ELogLevel::Warn,    __FILE__, __VA_ARGS__)
#define XANTHUS_ERROR(...)   xanthus::utility::logf(xanthus::utility::ELogLevel::Error,   __FILE__, __VA_ARGS__)
#else
#define XANTHUS_VERBOSE(...) ((void)0)
#define XANTHUS_DBG(...)     ((void)0)
#define XANTHUS_INFO(...)    ((void)0)
#define XANTHUS_WARN(...)    ((void)0)
#define XANTHUS_ERROR(...)   ((void)0)
#endif
