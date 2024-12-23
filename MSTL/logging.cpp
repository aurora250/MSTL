#include "logging.h"
MSTL_BEGIN_NAMESPACE__

void Logger::output(FILE* f, char level, const char* tag, const char* format, va_list list) {
    if (not f) return;
    auto now = std::chrono::system_clock::now();
    time_t tt = std::chrono::system_clock::to_time_t(now);

    std::tm local_tm = {};
    std::tm* timeinfo;
    {
        std::unique_lock<std::mutex> lock(mtx);
#ifdef MSTL_COMPILE_MSVC__
        if (localtime_s(&local_tm, &tt) != 0) {
            std::cerr << "Error calling localtime_s" << std::endl;
            return;
        }
        timeinfo = &local_tm;
#elif MSTL_PLATFORM_POSIX__
        if (localtime_r(&tt, &local_tm) == NULL) {
            std::cerr << "Error calling localtime_r" << std::endl;
            return;
        }
        timeinfo = &local_tm;
#else 
        if ((timeinfo = localtime(&tt)) == NULL) {
            std::cerr << "Error calling localtime" << std::endl;
            return;
        }
#endif
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;
        fprintf(f, "[%02d-%02d %02d:%02d:%02d.%03d %c/%s] ",
            timeinfo->tm_mon + 1,
            timeinfo->tm_mday,
            timeinfo->tm_hour,
            timeinfo->tm_min,
            timeinfo->tm_sec,
            static_cast<int>(ms), level, tag);
        vfprintf(f, format, list);
        fprintf(f, "\n");
    }
}

void Logger::verbose(const char* tag, const char* format, va_list list) {
    if (mLevel > LOG_LEVEL_VERBOSE) {
        return;
    }
    output(stdout, 'V', tag, format, list);
}

void Logger::debug(const char* tag, const char* format, va_list list) {
    if (mLevel > LOG_LEVEL_DEBUG) {
        return;
    }
    output(stdout, 'D', tag, format, list);
}

void Logger::info(const char* tag, const char* format, va_list list) {
    if (mLevel > LOG_LEVEL_INFO) {
        return;
    }
    output(stdout, 'I', tag, format, list);
}

void Logger::warning(const char* tag, const char* format, va_list list) {
    if (mLevel > LOG_LEVEL_WARNING) {
        return;
    }
    output(stdout, 'W', tag, format, list);
}

void Logger::error(const char* tag, const char* format, va_list list) {
    if (mLevel > LOG_LEVEL_ERROR) {
        return;
    }
    output(stderr, 'E', tag, format, list);
}

Logger& Logger::get_logger() {
    static Logger instance;
    return instance;
}

Logger::Logger() : mLevel(LOG_LEVEL_VERBOSE) {}

void Logger::set_level(__log_level level) {
    mLevel = level;
}

void set_log_level(__log_level level) {
    Logger::get_logger().set_level(level);
}

void logv_tag(const char* format, ...) {
    va_list list;
    va_start(list, format);
    Logger::get_logger().verbose(TO_STRING(LOG_LEVEL_VERBOSE), format, list);
    va_end(list);
}

void logd_tag(const char* format, ...) {
    va_list list;
    va_start(list, format);
    Logger::get_logger().debug(TO_STRING(LOG_LEVEL_DEBUG), format, list);
    va_end(list);
}

void logi_tag(const char* format, ...) {
    va_list list;
    va_start(list, format);
    Logger::get_logger().info(TO_STRING(LOG_LEVEL_INFO), format, list);
    va_end(list);
}

void logw_tag(const char* format, ...) {
    va_list list;
    va_start(list, format);
    Logger::get_logger().warning(TO_STRING(LOG_LEVEL_WARNING), format, list);
    va_end(list);
}

void loge_tag(const char* format, ...) {
    va_list list;
    va_start(list, format);
    Logger::get_logger().error(TO_STRING(LOG_LEVEL_ERROR), format, list);
    va_end(list);
}


MSTL_END_NAMESPACE__
