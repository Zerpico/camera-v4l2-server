#pragma once

#include <BasicUsageEnvironment.hh>
#include "spdlog/spdlog.h"

#define RS_MAX_LOG_MSG_SIZE 1024
#define RS_MAX_LOG_MSG_THLD 128

class NtUsageEnvironment : public BasicUsageEnvironment
{
public:
    static NtUsageEnvironment *createNew(TaskScheduler &taskScheduler, spdlog::level::level_enum logLevel);

    virtual UsageEnvironment &operator<<(char const *str);
    virtual UsageEnvironment &operator<<(int i);
    virtual UsageEnvironment &operator<<(unsigned u);
    virtual UsageEnvironment &operator<<(double d);
    virtual UsageEnvironment &operator<<(void *p);

protected:
    NtUsageEnvironment(TaskScheduler &taskScheduler, spdlog::level::level_enum logLevel);
    // called only by "createNew()" (or subclass constructors)
    virtual ~NtUsageEnvironment();

private:
    void flush();
    void check();

private:
    char buffer[RS_MAX_LOG_MSG_SIZE];
    char *ptr;
    spdlog::level::level_enum _logLevel;
};
