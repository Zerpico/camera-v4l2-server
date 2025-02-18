#include "NtFrameConverter.h"
#include <typeinfo>

NtFrameConverter::NtFrameConverter(const int src_width, const int src_height, const AVPixelFormat src_format, const int dst_width, const int dst_height, const AVPixelFormat dst_format)
{
    m_src_width = src_width;
    m_src_height = src_height;
    m_src_format = src_format;
    m_dst_width = dst_width;
    m_dst_height = dst_height;
    m_dst_format = dst_format;
    m_swscontext = get_swscontext();
}

NtFrameConverter::NtFrameConverter(const AVFrame &src_frame, const AVCodecContext &dst_codec)
{
    m_src_width = src_frame.width;
    m_src_height = src_frame.height;
    m_src_format = (AVPixelFormat)src_frame.format;
    m_dst_width = dst_codec.width;
    m_dst_height = dst_codec.height;
    m_dst_format = dst_codec.pix_fmt;
    m_swscontext = get_swscontext();
}

NtFrameConverter::~NtFrameConverter()
{
    // auto name = typeid(*this).name();
    if (m_swscontext != NULL)
        sws_freeContext(m_swscontext);
}

AVFrame *NtFrameConverter::convert(const AVFrame &frame)
{
    return NULL;
}

SwsContext *NtFrameConverter::get_swscontext()
{
    if (m_swscontext == NULL)
    {
        m_swscontext = sws_getCachedContext(m_swscontext, m_src_width, m_src_height, m_src_format,
                                            m_dst_width, m_dst_height, m_dst_format,
                                            SWS_FAST_BILINEAR, NULL, NULL, NULL);
    }
    return m_swscontext;
}

SwsContext *NtFrameConverter::get_swscontext(const AVFrame &srcFrame)
{
    return get_swscontext();
}