#pragma once
#include <string>

extern "C"
{
#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
}

#include "ThreadsafeQueue.h"

class NtVideoEncoder
{
protected:
    // NtVideoEncoder() = default;
    NtVideoEncoder(NtVideoEncoder const &) = delete;
    void operator=(NtVideoEncoder const &x) = delete;

public:
    NtVideoEncoder(std::string codec_name, int width = 640, int height = 480, AVPixelFormat format = AV_PIX_FMT_YUV420P, double fps = 25.0);
    ~NtVideoEncoder();

private:
    std::shared_ptr<AVCodecContext> _codecCtx;
    // std::unique_ptr<ThreadsafeQueue<queue_frame>> frame_queue;
    std::unique_ptr<std::thread> frame_push_thread;
};
