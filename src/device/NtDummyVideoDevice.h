#pragma once

#include "NtDeviceInterface.h"
#include "NtVideoEncoder.h"
#include <memory>
#include <ctime>
#include <thread>
#include <unordered_map>
#include <string>
#include "utils.h"
#include <format>
#include <Observer.h>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include <libavutil/imgutils.h>
}

// ---------------------------------
// struct for NtDummyVideoDevice
// ---------------------------------
struct DummyVideoDeviceParameters
{
public:
    DummyVideoDeviceParameters(std::string id, const int width = 640, const int height = 480, const double fps = 25.0, const char *title_text = NULL, const bool add_timer = false)
        : m_id("dummy_video"), m_width(width), m_height(height), m_fps(fps)
    {
        if (title_text)
            m_title = title_text;
        m_add_timer = add_timer;
    }

    std::string m_id{};
    int m_width;
    int m_height;
    double m_fps;
    std::string m_title;
    bool m_add_timer;

    static DummyVideoDeviceParameters Create(std::string id, std::unordered_map<std::string, std::string> &metadata)
    {
        DummyVideoDeviceParameters param{id};
        parseParam(metadata["width"], param.m_width);
        parseParam(metadata["height"], param.m_height);
        parseParam(metadata["fps"], param.m_fps);

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

// ---------------------------------
//  NtDummyVideoDevice
// ---------------------------------
class NtDummyVideoDevice : public NtDeviceInterface
{

public:
    NtDummyVideoDevice(const DummyVideoDeviceParameters &params);
    virtual ~NtDummyVideoDevice();
    void close();

public:
    virtual std::string getFd() { return m_params.m_id; }

    virtual int getWidth() { return m_encoder->getWidth(); }
    virtual int getHeight() { return m_encoder->getHeight(); }
    virtual NtDeviceFormat getVideoFormat() { return (NtDeviceFormat)m_encoder->getVideoFormat(); }

    inline void fill_frame(AVFrame &frame, uint8_t *dst);

    virtual void start();
    virtual void stop();
    virtual bool update(void *userData);

private:
    std::shared_ptr<CDispatcherBase> _dispatcher = NULL;
    DummyVideoDeviceParameters m_params;
    std::shared_ptr<AVFrame> m_buffer_frame = NULL;
    size_t m_buffer_size = 0;
    std::unique_ptr<uint8_t[]> m_buffer;
    std::thread thread_capture{};
    void runThread();
    int mStop = 1;

    const uint8_t bar_colours[8][3] =
        {
            {255, 255, 255}, // White
            {255, 255, 0},   // Yellow
            {0, 255, 255},   // Cyan
            {0, 255, 0},     // Green
            {255, 0, 255},   // Magenta
            {255, 0, 0},     // Red
            {0, 0, 255},     // Blue
            {0, 0, 0},       // Black
        };

    std::shared_ptr<NtVideoEncoder> m_encoder = NULL;
    long _fd = -1;
};
