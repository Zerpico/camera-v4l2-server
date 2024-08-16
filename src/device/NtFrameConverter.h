#pragma once

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

class NtFrameConverter
{
protected:
    NtFrameConverter() = default;
    NtFrameConverter(NtFrameConverter const &) = delete;
    void operator=(NtFrameConverter const &x) = delete;

public:
    NtFrameConverter(const int src_width, const int src_height, const AVPixelFormat src_format, const int dst_width, const int dst_height, const AVPixelFormat dst_format);
    NtFrameConverter(const AVFrame &src_frame, const AVCodecContext &dst_codec);
    ~NtFrameConverter();
    AVFrame *convert(const AVFrame &frame);

private:
    SwsContext *get_swscontext(const AVFrame &frame);
    SwsContext *get_swscontext();
    SwsContext *m_swscontext = NULL;

    int m_src_width;
    int m_src_height;
    AVPixelFormat m_src_format;

    int m_dst_width;
    int m_dst_height;
    AVPixelFormat m_dst_format;
};
