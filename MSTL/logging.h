#ifndef MSTL_LOGGING_H__
#define MSTL_LOGGING_H__
#include "basiclib.h"
#include <stdarg.h>
#include <stdio.h>
#include <chrono>
#include <mutex>
MSTL_BEGIN_NAMESPACE__

enum __log_level { // ERROR > WARNING > INFO > DEBUG > VERBOSE
    LOG_LEVEL_VERBOSE,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_CLOSE
};

class Logger {
public:
    static Logger& get_logger();

    void set_level(__log_level level);

    void verbose(const char* tag, const char* format, va_list list);
    void debug(const char* tag, const char* format, va_list list);
    void info(const char* tag, const char* format, va_list list);
    void warning(const char* tag, const char* format, va_list list);
    void error(const char* tag, const char* format, va_list list);
private:
    Logger();
    ~Logger() = default;
    Logger(const Logger& rhs);
    void output(FILE* f, char level, const char* tag, const char* format, va_list list);

    __log_level mLevel;
    std::mutex mtx;
};

void set_log_level(__log_level level);

void logv_tag(const char* format, ...);
void logd_tag(const char* format, ...);
void logi_tag(const char* format, ...);
void logw_tag(const char* format, ...);
void loge_tag(const char* format, ...);


#ifdef MSTL_PLATFORM_WIN64__
#define __FILENAME__ (strrchr(__FILE__, '\\') + 1)
#elif MSTL_PLATFORM_LINUX__
#define __FILENAME__ (strrchr(__FILE__, '/') + 1)
#else
#define __FILENAME__ (OPT)
#endif

#define LOG_TRACE(func, format, ...) \
    func("[%s][%s][%d]: " format, __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define LOG_VERBOSE(format, ...) LOG_TRACE(logv_tag, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) LOG_TRACE(logd_tag, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) LOG_TRACE(logi_tag, format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) LOG_TRACE(logw_tag, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) LOG_TRACE(loge_tag, format, ##__VA_ARGS__)

#define TIMERLOG_START(V) auto time_##V##_start = std::chrono::high_resolution_clock::now()
#define TIMERLOG_END(V) auto time_##V##_end = std::chrono::high_resolution_clock::now(); \
                   auto time_##V##_span = std::chrono::duration_cast<std::chrono::duration<double>> \
                       (time_##V##_end - time_##V##_start); \
                   LOG_DEBUG("%s elapse time: %.3f ms", #V, time_##V##_span.count() * 1000)

MSTL_END_NAMESPACE__
#endif // MSTL_LOGGING_H__
