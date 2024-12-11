extern "C"
{
#include "libavutil/avutil.h"
}

#include <string>
#include <spdlog/spdlog.h>
#include <algorithm>

const char *ws = " \t\n\r\f\v";

// trim from end of string (right)
inline std::string &rtrim(std::string &s, const char *t = ws)
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

void redirect_avlog_outputs(void *ptr, int level, const char *fmt, va_list vargs)
{
    std::string message;
    va_list ap_copy;
    va_copy(ap_copy, vargs);
    size_t len = vsnprintf(0, 0, fmt, ap_copy);
    message.resize(len + 1); // need space for NUL
    vsnprintf(&message[0], len + 1, fmt, vargs);
    message.resize(len); // remove the NUL
    spdlog::log(spdlog::level::info, rtrim(message));
}

void set_external_avlogger(std::shared_ptr<spdlog::logger> logger)
{
    int av_log_level = AV_LOG_INFO;
    auto spdlog_level = logger->level();
    switch (spdlog_level)
    {
    case SPDLOG_LEVEL_TRACE:
        av_log_level = AV_LOG_TRACE;
        break;
    case SPDLOG_LEVEL_DEBUG:
        av_log_level = AV_LOG_DEBUG;
        break;
    case SPDLOG_LEVEL_INFO:
        av_log_level = AV_LOG_INFO;
        break;
    case SPDLOG_LEVEL_WARN:
        av_log_level = AV_LOG_WARNING;
        break;
    case SPDLOG_LEVEL_ERROR:
        av_log_level = AV_LOG_ERROR;
        break;
    case SPDLOG_LEVEL_CRITICAL:
        av_log_level = AV_LOG_PANIC;
        break;
    case SPDLOG_LEVEL_OFF:
        av_log_level = AV_LOG_QUIET;
        break;
    default:
        break;
    }
    av_log_set_level(av_log_level);
    av_log_set_callback(redirect_avlog_outputs);
}