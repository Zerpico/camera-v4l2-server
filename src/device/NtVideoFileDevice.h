#pragma once

#include <memory>
#include <ctime>
#include <thread>
#include <unordered_map>
#include <string>
#include "utils.h"
#include <Observer.h>
#include "NtDeviceInterface.h"
#include "NtMediaChannels.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libavutil/intreadwrite.h"
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
#include <libavcodec/bsf.h>
}

struct VideoFileParameters
{
public:
    VideoFileParameters(std::string id, std::string path)
        : m_id(id), m_path(path)
    {
    }

    std::string m_id{};
    std::string m_path{};

    static VideoFileParameters Create(const NtChannel &channel)
    {
        VideoFileParameters param{channel.id, channel.source};
        return param;
    }

private:
    template <typename T>
    static void parseParam(std::string &paramStr, T &param)
    {
        T val = 0;
        if (tryParseString<T>(paramStr, val))
            param = val;
        else
            paramStr = std::format("{}", param);
    }
};

class NtVideoFileDevice : public NtDeviceInterface
{

public:
    NtVideoFileDevice(const VideoFileParameters &param);
    ~NtVideoFileDevice();

    virtual std::string getFd() { return m_params.m_id; };

    virtual int getWidth();
    virtual int getHeight();
    virtual NtDeviceFormat getVideoFormat();

    virtual void start();
    virtual void stop();
    virtual bool update(void *userData);

private:
    int init();

private:
    VideoFileParameters m_params;
    std::thread thread_capture{};
    std::shared_ptr<IObserverEvent> _dispatcher = NULL;
    std::shared_ptr<AVPacket> m_buffer_packet;
    std::shared_ptr<AVFormatContext> m_format_ctx;
    std::shared_ptr<AVCodecContext> m_codec_ctx;
    std::shared_ptr<AVBSFContext> m_bsf_ctx;
    int videoStreamIndex;
    void runThread();
    int mStop = 1;

    std::vector<uint8_t> prependStartCode(const uint8_t *data, int size)
    {
        std::vector<uint8_t> nal_unit;
        nal_unit.push_back(0x00);
        nal_unit.push_back(0x00);
        nal_unit.push_back(0x00);
        nal_unit.push_back(0x01);
        nal_unit.insert(nal_unit.end(), data, data + size);
        return nal_unit;
    }

    uint8_t *avpriv_find_start_code(const uint8_t *p, const uint8_t *end, uint32_t *state)
    {
        int i;

        if (p >= end)
            return (uint8_t *)end;

        for (i = 0; i < 3; i++)
        {
            uint32_t tmp = *state << 8;
            *state = tmp + *(p++);
            if (tmp == 0x100 || p == end)
                return (uint8_t *)p;
        }

        while (p < end)
        {
            if (p[-1] > 1)
                p += 3;
            else if (p[-2])
                p += 2;
            else if (p[-3] | (p[-1] - 1))
                p++;
            else
            {
                p++;
                break;
            }
        }

        p = FFMIN(p, end) - 4;
        *state = AV_RB32(p);
        return (uint8_t *)(p + 4);
    }
};
