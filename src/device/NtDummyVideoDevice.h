#pragma once

#include "NtDeviceInterface.h"
#include "NtVideoEncoder.h"
#include <string>
#include <memory>
#include <ctime>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include <libavutil/imgutils.h>
}

struct DummyVideoDeviceParameters
{
    DummyVideoDeviceParameters(const int width = 640, const int height = 480, const double fps = 25.0, const char *title_text = NULL, const bool add_timer = false)
        : m_devName("dummy_video"), m_width(width), m_height(height), m_fps(fps)
    {
        if (title_text)
            m_title = title_text;
        m_add_timer = add_timer;
    }

    std::string m_devName{};
    int m_width;
    int m_height;
    double m_fps;
    std::string m_title;
    bool m_add_timer;
};

class NtDummyVideoDevice : public NtDeviceInterface
{

public:
    static NtDummyVideoDevice *createNew(const DummyVideoDeviceParameters &params);
    virtual ~NtDummyVideoDevice();
    void close();

protected:
    NtDummyVideoDevice(const DummyVideoDeviceParameters &params);

public:
    virtual size_t read(char *buffer, size_t bufferSize) { return 0; }
    virtual long getFd()
    {
        std::time_t result = std::time(nullptr);
        std::localtime(&result);
        return result;
    }
    virtual unsigned long getBufferSize() { return 0; }

    virtual int getWidth() { return m_encoder->getWidth(); }
    virtual int getHeight() { return m_encoder->getHeight(); }
    virtual int getVideoFormat() { return m_encoder->getVideoFormat(); }

    inline void fill_frame(AVFrame &frame, uint8_t *dst);

private:
    DummyVideoDeviceParameters m_params;
    AVFrame *m_buffer_frame;
    size_t m_buffer_size = 0;
    std::unique_ptr<uint8_t[]> m_buffer;

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

    NtVideoEncoder *m_encoder = NULL;
};
