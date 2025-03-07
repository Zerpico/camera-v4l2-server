#pragma once
#include <string>

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

extern "C"
{
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
}

#include <thread>
#include "ThreadsafeQueue.h"
#include "NtDeviceFormat.h"

class NtVideoEncoder
{
protected:
    // NtVideoEncoder() = default;
    NtVideoEncoder(NtVideoEncoder const &) = delete;
    void operator=(NtVideoEncoder const &x) = delete;

public:
    NtVideoEncoder(std::string codec_name, int width = 640, int height = 480, AVPixelFormat format = AV_PIX_FMT_YUV420P, double fps = 25.0);
    ~NtVideoEncoder();
    void Push(const AVFrame *frame);
    AVPacket *Pull();

    int getWidth()
    {
        return _codecCtx->width;
    }
    int getHeight()
    {
        return _codecCtx->height;
    }
    NtDeviceFormat getVideoFormat()
    {
        switch (_codecCtx->codec->id)
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
    double getFramerate()
    {
        return av_q2d(_codecCtx->framerate);
    }

private:
    std::shared_ptr<AVCodecContext> _codecCtx;
    std::shared_ptr<AVPacket> _outPacket;
    std::unique_ptr<ThreadsafeQueue<AVFrame>> frame_queue;
    std::unique_ptr<std::thread> frame_push_thread;
};
