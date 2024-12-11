#include "NtDummyVideoDevice.h"
#include <memory>
#include <stdexcept>

NtDummyVideoDevice *NtDummyVideoDevice::createNew(const DummyVideoDeviceParameters &params)
{
    NtDummyVideoDevice *device = new NtDummyVideoDevice(params);
    if (device)
    {
        if (device->getFd() == -1)
        {
            delete device;
            device = NULL;
        }
    }
    return device;
}

NtDummyVideoDevice::NtDummyVideoDevice(const DummyVideoDeviceParameters &params)
    : m_params(params)
{
    // allocate avFrame and fill props and data
    m_buffer_frame = av_frame_alloc();
    m_buffer_frame->width = m_params.m_width;
    m_buffer_frame->height = m_params.m_height;
    m_buffer_frame->format = AVPixelFormat::AV_PIX_FMT_RGB24;
    int ret = av_frame_get_buffer(m_buffer_frame, 0);

    if (ret)
        throw std::runtime_error("Failed fill buffer frame");

    m_buffer_size = av_image_get_buffer_size((AVPixelFormat)m_buffer_frame->format, m_buffer_frame->width, m_buffer_frame->height, 4);
    m_buffer = std::make_unique<uint8_t[]>(m_buffer_size);

    m_encoder = new NtVideoEncoder("libx264");
    // fill_frame(*m_buffer_frame, m_buffer.get());
}

NtDummyVideoDevice::~NtDummyVideoDevice()
{
    if (m_buffer_frame != NULL)
        av_frame_free(&m_buffer_frame);
    m_buffer.reset();
}

inline void NtDummyVideoDevice::fill_frame(AVFrame &frame, uint8_t *buffer)
{
    // Generate complete frame
    unsigned columnWidth = frame.width / (sizeof bar_colours / sizeof bar_colours[0]); // width of column in bar_colour
    for (unsigned y = 0; y < frame.height; y++)
    {
        for (unsigned x = 0; x < frame.width; x += 3)
        {
            unsigned col_idx = x / columnWidth;
            buffer[y * frame.width + x + 0] = bar_colours[col_idx][0];
            buffer[y * frame.width + x + 1] = bar_colours[col_idx][1];
            buffer[y * frame.width + x + 2] = bar_colours[col_idx][2];
        }
    }
    // fill frame
    int ret = av_image_fill_arrays(frame.data, frame.linesize, buffer, (AVPixelFormat)frame.format, frame.width, frame.height, 1);
}
