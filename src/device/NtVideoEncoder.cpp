#include "NtVideoEncoder.h"
#include <string>
#include <spdlog/spdlog.h>
#include <memory>
#include <DataPackets.h>

NtVideoEncoder::NtVideoEncoder(std::string codec_name, int width, int height, AVPixelFormat format, double fps)
    : _codecCtx(nullptr)
{
    int gop_size = fps;
    int ret = 0;
    AVDictionary *optPtr = NULL;

    const AVCodec *codec = avcodec_find_encoder_by_name(codec_name.c_str());

    if (!codec)
    {
        spdlog::error("Can't open codec by name '{0}'", codec_name);
        return;
    }

    auto dst_format = avcodec_find_best_pix_fmt_of_list(codec->pix_fmts, AVPixelFormat::AV_PIX_FMT_YUV420P, 1, &ret);
    if (format != dst_format)
    {
        spdlog::warn("Codec format redefined to: {}", (int)dst_format);
    }
    format = dst_format;

    _codecCtx = std::shared_ptr<AVCodecContext>(avcodec_alloc_context3(codec), [](AVCodecContext *pi)
                                                { avcodec_free_context(&pi); });
    if (!_codecCtx)
    {
        spdlog::error("Can't allocate codec context");
        return;
    }

    auto framerate = av_d2q(fps, 100);
    _codecCtx->width = width;
    _codecCtx->height = height;
    _codecCtx->framerate = framerate;
    _codecCtx->pix_fmt = format;
    _codecCtx->time_base = av_inv_q(framerate); // Frames per second
    if (gop_size > 0)
        _codecCtx->gop_size = gop_size; // Intra frames per x P frames

    if (codec->id == AVCodecID::AV_CODEC_ID_H264 || codec->id == AVCodecID::AV_CODEC_ID_HEVC ||
        codec->id == AVCodecID::AV_CODEC_ID_MPEG4 || codec->id == AVCodecID::AV_CODEC_ID_MPEG2VIDEO)
    {
        _codecCtx->thread_count = 1;
        av_dict_set(&optPtr, "profile", "baseline", 0);
        av_dict_set(&optPtr, "preset", "fast", 0);
        av_dict_set(&optPtr, "tune", "zerolatency", 0);
    }

    if (codec->id == AVCodecID::AV_CODEC_ID_MPEG4 || codec->id == AVCodecID::AV_CODEC_ID_MPEG2VIDEO)
    {
        _codecCtx->max_b_frames = 1;
    }

    _codecCtx->flags &= ~AV_CODEC_FLAG_GLOBAL_HEADER;

    ret = avcodec_open2(_codecCtx.get(), codec, &optPtr);
    if (ret != 0)
        return;

    if (optPtr != NULL)
        spdlog::warn("Some codec option not used");

    _outPacket = std::shared_ptr<AVPacket>(av_packet_alloc(), [](AVPacket *ptr)
                                           { av_packet_free(&ptr); });
}

NtVideoEncoder::~NtVideoEncoder()
{
}

void NtVideoEncoder::Push(const AVFrame *frame)
{
    int ret = avcodec_send_frame(_codecCtx.get(), frame);
    ret = avcodec_receive_packet(_codecCtx.get(), _outPacket.get());
}

AVPacket *NtVideoEncoder::Pull()
{
    // PacketData data(_outPacket->size);
    // data.copy(_outPacket->data, _outPacket->size);
    return _outPacket.get();
}