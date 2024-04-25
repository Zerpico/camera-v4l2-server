#include "Logger.h"

CLogger *CLogger::m_pThis = NULL;

CLogger::CLogger()
{
}

CLogger *CLogger::GetLogger()
{
    if (m_pThis == NULL)
    {
        m_pThis = new CLogger();
    }
    return m_pThis;
}

void CLogger::Log(log_level level, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    const int nLength = 1 + vsnprintf(NULL, 0, format, args);
    char *sMessage = new char[nLength];
    vsnprintf(sMessage, nLength, format, args);
    va_end(args);

    printf("[%s %s] %s \n", CurrentDateTime().c_str(), GetFormattedLogLevel(level).c_str(), sMessage);

    delete[] sMessage;
}

const std::string CLogger::CurrentDateTime()
{
    time_t now = time(NULL);
    struct tm tstruct;
    char buf[80];
#if defined(WIN32)
    localtime_s(&tstruct, &now);
#else
    localtime_r(&now, &tstruct);
#endif
    strftime(buf, sizeof(buf), "%H:%M:%S", &tstruct);
    return buf;
}

const std::string CLogger::GetFormattedLogLevel(log_level level)
{
    static char _retval[5];
    switch (level)
    {
    case LOGLEVEL_TRACE:
        strcpy(_retval, "TRC");
        break;
    case LOGLEVEL_DEBUG:
        strcpy(_retval, "DBG");
        break;
    case LOGLEVEL_INFO:
        strcpy(_retval, "INF");
        break;
    case LOGLEVEL_WARN:
        strcpy(_retval, "WRN");
        break;
    case LOGLEVEL_ERR:
        strcpy(_retval, "ERR");
        break;
    case LOGLEVEL_CRIT:
        strcpy(_retval, "CRT");
        break;

    default:
        break;
    }
    return _retval;
}
