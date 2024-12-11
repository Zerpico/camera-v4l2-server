extern "C"
{
#include "libavutil/avutil.h"
}

#include <string>
#include <spdlog/spdlog.h>
#include <algorithm>

const char *ws = " \t\n\r\f\v";
inline int current_av_log_level = AV_LOG_QUIET;
inline int get_avlog_level(int spdlog_level);
inline spdlog::level::level_enum get_spdlog_level(int av_log_level);

// trim from end of string (right)
inline std::string &
rtrim(std::string &s, const char *t = ws)
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

void redirect_avlog_outputs(void *ptr, int level, const char *fmt, va_list vargs)
{
    if (level > current_av_log_level)
        return;
    std::string message;
    va_list ap_copy;
    va_copy(ap_copy, vargs);
    size_t len = vsnprintf(0, 0, fmt, ap_copy);
    message.resize(len + 1); // need space for NUL
    vsnprintf(&message[0], len + 1, fmt, vargs);
    message.resize(len); // remove the NUL
    spdlog::log(get_spdlog_level(level), rtrim(message));
}

void set_external_avlogger(std::shared_ptr<spdlog::logger> logger)
{
    int av_log_level = get_avlog_level(logger->level());
    av_log_set_level(av_log_level);
    av_log_set_callback(redirect_avlog_outputs);
    current_av_log_level = av_log_level;
}

inline int get_avlog_level(int spdlog_level)
{
    int av_log_level = AV_LOG_INFO;
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
    return av_log_level;
}

inline spdlog::level::level_enum get_spdlog_level(int av_log_level)
{
    auto spd_log_level = SPDLOG_LEVEL_INFO;
    switch (av_log_level)
    {
    case AV_LOG_TRACE:
        spd_log_level = SPDLOG_LEVEL_TRACE;
        break;
    case AV_LOG_DEBUG:
        spd_log_level = SPDLOG_LEVEL_DEBUG;
        break;
    case AV_LOG_INFO:
        spd_log_level = SPDLOG_LEVEL_INFO;
        break;
    case AV_LOG_WARNING:
        spd_log_level = SPDLOG_LEVEL_WARN;
        break;
    case AV_LOG_ERROR:
        spd_log_level = SPDLOG_LEVEL_ERROR;
        break;
    case AV_LOG_PANIC:
        spd_log_level = SPDLOG_LEVEL_CRITICAL;
        break;
    case AV_LOG_QUIET:
        spd_log_level = SPDLOG_LEVEL_OFF;
        break;
    default:
        spd_log_level = SPDLOG_LEVEL_INFO;
        break;
    }
    return (spdlog::level::level_enum)spd_log_level;
}