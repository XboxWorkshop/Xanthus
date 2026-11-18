#include <fmt/format.h>
#include <mutex>

#ifdef __ANDROID__
#include <android/log.h>
#endif

#include "xanthus/utility.hpp"

namespace xanthus::utility {
namespace {

std::mutex s_logMutex;

const char* basename(const char* path) {
    if (!path)
        return "Xanthus";

    const char* name = path;

    for (const char* p = path; *p; ++p) {
        if (*p == '/' || *p == '\\')
            name = p + 1;
    }

    return *name ? name : "Xanthus";
}

#ifdef __ANDROID__
android_LogPriority toAndroidPriority(ELogLevel level) {
    switch (level) {
        case ELogLevel::Verbose: return ANDROID_LOG_VERBOSE;
        case ELogLevel::Debug:   return ANDROID_LOG_DEBUG;
        case ELogLevel::Info:    return ANDROID_LOG_INFO;
        case ELogLevel::Warn:    return ANDROID_LOG_WARN;
        case ELogLevel::Error:   return ANDROID_LOG_ERROR;
        default:                 return ANDROID_LOG_INFO;
    }
}
#endif

const char* toString(ELogLevel level) {
    switch (level) {
        case ELogLevel::Verbose: return "VERBOSE";
        case ELogLevel::Debug:   return "DEBUG";
        case ELogLevel::Info:    return "INFO";
        case ELogLevel::Warn:    return "WARN";
        case ELogLevel::Error:   return "ERROR";
        default:                 return "UNKNOWN";
    }
}

} // namespace

void log(ELogLevel level, const char* tag, const std::string& message) {
    tag = basename(tag);

#ifdef __ANDROID__
    __android_log_write(
        toAndroidPriority(level),
        tag,
        message.c_str()
    );
#else
    std::lock_guard lock(s_logMutex);

    fmt::print(stderr, "[{}] [{}] {}\n",
        toString(level),
        tag,
        message
    );
#endif
}

} // namespace xanthus::utility
