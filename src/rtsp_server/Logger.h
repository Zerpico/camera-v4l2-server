#pragma once

#include <iostream>
#include <cstdarg>
#include <string>
#include <cstring>
#include <stdio.h>
#include <mutex>

// #include "Common.h"

using namespace std;

#define LOGGER CLogger::GetLogger()

#define LOG_INFO(...) CLogger::GetLogger()->Log(LOGLEVEL_INFO, __VA_ARGS__)
#define LOG_DEBUG(...) CLogger::GetLogger()->Log(LOGLEVEL_DEBUG, __VA_ARGS__)
#define LOG_WARN(...) CLogger::GetLogger()->Log(LOGLEVEL_WARN, __VA_ARGS__)
#define LOG_ERR(...) CLogger::GetLogger()->Log(LOGLEVEL_ERR, __VA_ARGS__)
#define LOG(...) CLogger::GetLogger()->Log(LOGLEVEL_INFO, __VA_ARGS__)

typedef enum log_level
{
    LOGLEVEL_TRACE = 0x00, // Logs most detailed messages
    LOGLEVEL_DEBUG = 0x01, // Logs debug messages
    LOGLEVEL_INFO = 0x02,  // Logs general information
    LOGLEVEL_WARN = 0x03,  // Logs abnormal or unexpected event
    LOGLEVEL_ERR = 0x04,   // Logs failure or error event
    LOGLEVEL_CRIT = 0x05,  // Logs that describe an unrecoverable application or system crash
    LOGLEVEL_NONE = 0x06,  // Not used for writing log messages
} log_level_t;

/**
 *   Singleton Logger Class.
 */
class CLogger
{
public:
    /**
     *   Variable Length Logger function
     *   @param level level of message for prefix
     *   @param format string for the message to be logged.
     */
    void Log(log_level level, const char *format, ...);
    /**
     *   Funtion to create the instance of logger class.
     *   @return singleton object of Clogger class..
     */
    static CLogger *GetLogger();

private:
    /**
     *    Default constructor for the Logger class.
     */
    CLogger();
    /**
     *   copy constructor for the Logger class.
     */
    CLogger(const CLogger &); // copy constructor is private
    /**
     *   assignment operator for the Logger class.
     */
    CLogger &operator=(const CLogger &);
    ; // assignment operator is private
    /**
     *   Singleton logger class object pointer.
     **/
    static CLogger *m_pThis;

    const std::string CurrentDateTime();

    const std::string GetFormattedLogLevel(log_level level);

    std::mutex mutex_;
};
