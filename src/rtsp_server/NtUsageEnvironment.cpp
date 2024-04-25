#include "NtUsageEnvironment.h"
#include "Logger.h"

NtUsageEnvironment::NtUsageEnvironment(TaskScheduler &taskScheduler)
    : BasicUsageEnvironment(taskScheduler)
{
}

NtUsageEnvironment::~NtUsageEnvironment()
{
}

NtUsageEnvironment *NtUsageEnvironment::createNew(TaskScheduler &taskScheduler)
{
    NtUsageEnvironment *env = new NtUsageEnvironment(taskScheduler);

    if (env)
    {
        env->ptr = env->buffer;
        LOG_DEBUG("NtUsageEnvironment initialized");
    }

    return env;
}

UsageEnvironment &NtUsageEnvironment::operator<<(char const *str)
{
    int num = 0;

    if (str == NULL)
        str = "(NULL)"; // sanity check

    while (str[num] != '\0')
    {
        if (str[num] == '\n')
        {
            flush();
        }
        else
        {
            *ptr++ = str[num];
            check();
        }
        num++;
    }

    return *this;
}

void NtUsageEnvironment::flush()
{
    *ptr = '\0';
    LOG_DEBUG(buffer);
    ptr = buffer;
}

void NtUsageEnvironment::check()
{
    if ((ptr - buffer) > (RS_MAX_LOG_MSG_SIZE - RS_MAX_LOG_MSG_THLD))
    {
        flush();
    }
}

UsageEnvironment &NtUsageEnvironment::operator<<(int i)
{
    ptr += sprintf(ptr, "%d", i);
    check();
    return *this;
}

UsageEnvironment &NtUsageEnvironment::operator<<(unsigned u)
{
    ptr += sprintf(ptr, "%u", u);
    check();
    return *this;
}

UsageEnvironment &NtUsageEnvironment::operator<<(double d)
{
    ptr += sprintf(ptr, "%f", d);
    check();
    return *this;
}

UsageEnvironment &NtUsageEnvironment::operator<<(void *p)
{
    ptr += sprintf(ptr, "%p", p);
    check();
    return *this;
}