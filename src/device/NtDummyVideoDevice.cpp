#include "NtDummyVideoDevice.h"
#include <memory>
#include <stdexcept>
#include <chrono>
#include "mioc/mioc.h"
#include <DataPackets.h>
#include <NtMediaChannels.h>

using namespace std::chrono;

NtDummyVideoDevice::NtDummyVideoDevice(const DummyVideoDeviceParameters &params)
    : m_params(params)
{
    _dispatcher = getContainer()->Resolve<CDispatcherBase>();

    // allocate avFrame and fill props and data
    m_buffer_frame = std::shared_ptr<AVFrame>(av_frame_alloc(), [](AVFrame *ptr)
                                              { av_frame_free(&ptr); });
    m_buffer_frame->width = m_params.m_width;
    m_buffer_frame->height = m_params.m_height;
    m_buffer_frame->format = AVPixelFormat::AV_PIX_FMT_YUV420P;
    auto dd = *m_buffer_frame;
    int ret = av_frame_get_buffer(m_buffer_frame.get(), 0);

    if (ret)
        throw std::runtime_error("Failed fill buffer frame");

    m_buffer_size = av_image_get_buffer_size((AVPixelFormat)m_buffer_frame->format, m_buffer_frame->width, m_buffer_frame->height, 4);
    m_buffer = std::make_unique<uint8_t[]>(m_buffer_size);

    m_encoder = std::make_shared<NtVideoEncoder>("libx264", m_params.m_width, m_params.m_height, AVPixelFormat::AV_PIX_FMT_YUV420P, m_params.m_fps);
}

NtDummyVideoDevice::~NtDummyVideoDevice()
{
    stop();
    // if (m_buffer_frame != NULL)
    //     av_frame_free(&(m_buffer_frame.get()));
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

void NtDummyVideoDevice::start()
{
    if (mStop)
        thread_capture = std::thread(&NtDummyVideoDevice::runThread, this);
}
void NtDummyVideoDevice::stop()
{
    mStop = 0;
    if (thread_capture.joinable())
        thread_capture.join();
}

bool NtDummyVideoDevice::update(void *userData)
{
    return false;
}

void NtDummyVideoDevice::runThread()
{
    mStop = 1;
    std::chrono::milliseconds maxElapsedMs = milliseconds((int)(1000 / m_params.m_fps));
    while (mStop)
    {
        auto start = std::chrono::high_resolution_clock::now();
        // fill frame and encode
        fill_frame(*m_buffer_frame.get(), m_buffer.get());
        m_encoder->Push(m_buffer_frame.get());
        auto end = std::chrono::high_resolution_clock::now(); // td::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        // packet
        auto packet = m_encoder->Pull();
        auto data = std::make_shared<PacketData>(m_encoder->getVideoFormat());
        data->copy(packet->data, packet->size);
        data->set_refId(m_params.m_id);
        auto sleepMs = maxElapsedMs - duration;
        if (duration < maxElapsedMs)
            std::this_thread::sleep_for(sleepMs);

        // send to bus
        _dispatcher->SendMessageLP(data);
    }
}