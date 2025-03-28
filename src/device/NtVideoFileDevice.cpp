#include "NtVideoFileDevice.h"
#include <memory>
#include <stdexcept>
#include <chrono>
#include "mioc/mioc.h"
#include <DataPackets.h>
#include <NtMediaChannels.h>
#include <fstream>

NtVideoFileDevice::NtVideoFileDevice(const VideoFileParameters &param)
    : m_params(param)
{
    _dispatcher = getContainer()->Resolve<IObserverEvent>();

    m_buffer_packet = std::shared_ptr<AVPacket>(av_packet_alloc(), [](AVPacket *ptr)
                                                { av_packet_free(&ptr); });

    init();
}

NtVideoFileDevice::~NtVideoFileDevice()
{
    stop();
}

int NtVideoFileDevice::init()
{
    int ret;
    {
        AVFormatContext *ifmt_ctx = NULL;

        if ((ret = avformat_open_input(&ifmt_ctx, m_params.m_path.c_str(), NULL, NULL)) < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
            return ret;
        }

        m_format_ctx = std::shared_ptr<AVFormatContext>(ifmt_ctx, [](AVFormatContext *ptr)
                                                        { avformat_close_input(&ptr); });
    }

    if ((ret = avformat_find_stream_info(m_format_ctx.get(), NULL)) < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
        return ret;
    }

    videoStreamIndex = av_find_best_stream(m_format_ctx.get(), AVMediaType::AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Could not find %s stream in input file '%s'\n",
               av_get_media_type_string(AVMediaType::AVMEDIA_TYPE_VIDEO), m_params.m_path.c_str());
        return ret;
    }

    AVStream *videoStream = m_format_ctx->streams[videoStreamIndex];

    /* find decoder for the stream */
    const AVCodec *dec = avcodec_find_decoder(videoStream->codecpar->codec_id);
    if (!dec)
    {
        av_log(NULL, AV_LOG_ERROR, "Failed to find %s codec\n",
               av_get_media_type_string(AVMediaType::AVMEDIA_TYPE_VIDEO));
        return AVERROR(EINVAL);
    }

    /* Allocate a codec context for the decoder */
    m_codec_ctx = std::shared_ptr<AVCodecContext>(avcodec_alloc_context3(dec), [](AVCodecContext *pi)
                                                  { avcodec_free_context(&pi); });
    if (!m_codec_ctx)
    {
        av_log(NULL, AV_LOG_ERROR, "Failed to allocate the %s codec context\n",
               av_get_media_type_string(AVMediaType::AVMEDIA_TYPE_VIDEO));
        return AVERROR(ENOMEM);
    }

    /* Copy codec parameters from input stream to output codec context */
    if ((ret = avcodec_parameters_to_context(m_codec_ctx.get(), videoStream->codecpar)) < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Failed to copy %s codec parameters to decoder context\n",
               av_get_media_type_string(AVMediaType::AVMEDIA_TYPE_VIDEO));
        return ret;
    }

    /* Init the decoders */
    if ((ret = avcodec_open2(m_codec_ctx.get(), dec, NULL)) < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "Failed to open %s codec\n",
               av_get_media_type_string(AVMediaType::AVMEDIA_TYPE_VIDEO));
        return ret;
    }
    /*  Initializing the filter context */
    std::string filterName;
    if (videoStream->codecpar->codec_id == AVCodecID::AV_CODEC_ID_H264)
        filterName = "h264_mp4toannexb";
    else if (videoStream->codecpar->codec_id == AVCodecID::AV_CODEC_ID_HEVC)
        filterName = "hevc_mp4toannexb";
    if (!filterName.empty())
    {
        const AVBitStreamFilter *bsfilter = av_bsf_get_by_name("h264_mp4toannexb");
        AVBSFContext *bsfPtr;
        if (av_bsf_alloc(bsfilter, &bsfPtr) < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "Failed to allocate filter %s \n", filterName.c_str());
            return ret;
        }

        m_bsf_ctx = std::shared_ptr<AVBSFContext>(bsfPtr, [](AVBSFContext *ptr)
                                                  { if (ptr != NULL) av_bsf_free(&ptr); });

        if (avcodec_parameters_copy(m_bsf_ctx->par_in, m_format_ctx->streams[videoStreamIndex]->codecpar) < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "Error: Could not copy codec parameters to bitstream filter\n");
            av_bsf_free(&bsfPtr);
            return 0;
        }
        m_bsf_ctx->time_base_in = m_format_ctx->streams[videoStreamIndex]->time_base;
        m_bsf_ctx->par_in->codec_tag = 0; // Important: Must be set to zero for some codecs

        if (av_bsf_init(m_bsf_ctx.get()) < 0)
        {
            std::cerr << "Error: Could not initialize bitstream filter" << std::endl;
            av_bsf_free(&bsfPtr);
        }
    }

    return 0;
}

int NtVideoFileDevice::getWidth()
{
    return m_codec_ctx->width;
}

int NtVideoFileDevice::getHeight()
{
    return m_codec_ctx->height;
}

NtDeviceFormat NtVideoFileDevice::getVideoFormat()
{
    switch (m_codec_ctx->codec->id)
    {
    case AVCodecID::AV_CODEC_ID_H264:
        return NtDeviceFormat::FMT_H264;
        break;

    case AVCodecID::AV_CODEC_ID_HEVC:
        return NtDeviceFormat::FMT_HEVC;
        break;

    case AVCodecID::AV_CODEC_ID_VP8:
        return NtDeviceFormat::FMT_VP8;
        break;

    case AVCodecID::AV_CODEC_ID_VP9:
        return NtDeviceFormat::FMT_VP9;
        break;

    case AVCodecID::AV_CODEC_ID_MJPEG:
        return NtDeviceFormat::FMT_MJPEG;
        break;

    case AVCodecID::AV_CODEC_ID_MPEG2TS:
        return NtDeviceFormat::FMT_MPEGTS;
        break;

    default:
        return NtDeviceFormat::FMT_NONE;
        break;
    }
}

void NtVideoFileDevice::start()
{
    if (mStop)
        thread_capture = std::thread(&NtVideoFileDevice::runThread, this);
}
void NtVideoFileDevice::stop()
{
    mStop = 0;
    if (thread_capture.joinable())
        thread_capture.join();
}

bool NtVideoFileDevice::update(void *userData)
{
    return false;
}

void NtVideoFileDevice::runThread()
{
    mStop = 1;
    int index = 0;
    uint32_t nal_start_code_len_ = 0;

    /* Get frame rate */
    AVStream *videoStream = m_format_ctx->streams[videoStreamIndex];
    auto frame_rate_ = av_q2d(videoStream->r_frame_rate);
    if (frame_rate_ <= 0)
    {
        frame_rate_ = av_q2d(videoStream->avg_frame_rate); // Try avg_frame_rate
    }
    if (frame_rate_ <= 0)
    {
        frame_rate_ = 25.0; // Default if can't determine
        std::cerr << "Warning: Could not determine frame rate, using default: " << frame_rate_ << std::endl;
    }
    // auto frame_duration_ = std::chrono::duration<double, std::ratio<1>>(1.0 / frame_rate_);

    int ret = 0;
    std::chrono::steady_clock::time_point start;
    std::chrono::milliseconds maxElapsedMs = std::chrono::milliseconds((int)(1000 / frame_rate_));
    while (mStop)
    {
        if (ret == 0)
        {
            start = std::chrono::high_resolution_clock::now();
        }

        ret = av_read_frame(m_format_ctx.get(), m_buffer_packet.get());
        if (ret == AVERROR(EAGAIN))
        {
            continue;
        }
        if (ret == AVERROR_EOF)
        {
            // End of file - rewind
            av_seek_frame(m_format_ctx.get(), videoStreamIndex, 0, AVSEEK_FLAG_BACKWARD); // Rewind

            av_packet_unref(m_buffer_packet.get());
            continue; // Skip to the next iteration
        }

        if (m_buffer_packet->stream_index == videoStreamIndex)
        {
            if (m_bsf_ctx != NULL)
            {
                // BitStreamfilter to maintain memory space inside
                ret = av_bsf_send_packet(m_bsf_ctx.get(), m_buffer_packet.get());
                if (ret != 0)
                {
                    av_log(NULL, AV_LOG_WARNING, "Error on send packet to annexb filter \n");
                    av_packet_unref(m_buffer_packet.get()); //  You don't have to release resources.
                    continue;
                }

                // BitStreamfilter to maintain memory space inside
                ret = av_bsf_receive_packet(m_bsf_ctx.get(), m_buffer_packet.get());
                if (ret != 0)
                {
                    av_log(NULL, AV_LOG_WARNING, "Error on receive packet from annexb filter\n");
                    continue;
                }
            }

            auto data = std::make_shared<PacketData>(getVideoFormat());
            data->copy(m_buffer_packet->data, m_buffer_packet->size);
            data->set_refId(m_params.m_id);

            auto end = std::chrono::high_resolution_clock::now(); // td::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

            // Pause to simulate frame rate
            auto sleepMs = maxElapsedMs - duration;
            if (duration < maxElapsedMs)
                std::this_thread::sleep_for(sleepMs);

            av_packet_unref(m_buffer_packet.get());
            // send to bus
            _dispatcher->publishEvent(data);
        }
    }
}