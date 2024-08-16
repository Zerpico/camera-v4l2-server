#include "NtVideoEncoder.h"
#include <string>
#include <spdlog/spdlog.h>
#include <memory>

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

struct AVCodecContext_deleter
{
    void operator()(AVCodecContext *p) const
    {
        if (p)
            avcodec_free_context(&p);
    }
};

struct AVCodecContext_deleter2
{
    void operator()(AVCodecContext *ctx)
    {
        avcodec_free_context(&ctx);
    }
};

NtVideoEncoder::NtVideoEncoder(/* args */)
{
    std::string codec_name = "libx264";
    int width = 640;
    int height = 480;
    double fps = 25.0;
    AVPixelFormat format = AVPixelFormat::AV_PIX_FMT_YUV420P;
    int gop_size = fps;
    {
        int ret = 0;
        const AVCodec *codec = NULL;

        codec = avcodec_find_encoder_by_name(codec_name.c_str());

        if (codec)
        {
            spdlog::error("Can't open codec by name '{0}'", codec_name);
            return;
        }

        auto codec_ctx = std::shared_ptr<AVCodecContext>(avcodec_alloc_context3(codec), [](AVCodecContext *pi)
                                                         { avcodec_free_context(&pi); });
        if (codec_ctx)
        {
            spdlog::error("Can't allocate codec context");
            return;
        }

        auto framerate = av_d2q(fps, 100);

        codec_ctx->width = width;
        codec_ctx->height = height;
        codec_ctx->framerate = framerate;
        codec_ctx->pix_fmt = format;
        codec_ctx->time_base = av_inv_q(framerate); // Frames per second
        if (gop_size > 0)
            codec_ctx->gop_size = gop_size; // Intra frames per x P frames
        // CodecCtx->bit_rate = config.BitRate;     // average bit_rate

        if (codec->id == AVCodecID::AV_CODEC_ID_H264 || codec->id == AVCodecID::AV_CODEC_ID_HEVC ||
            codec->id == AVCodecID::AV_CODEC_ID_MPEG4 || codec->id == AVCodecID::AV_CODEC_ID_MPEG2VIDEO)
        {
            codec_ctx->thread_count = 1;
        }

        if (codec->id == AVCodecID::AV_CODEC_ID_MPEG4 || codec->id == AVCodecID::AV_CODEC_ID_MPEG2VIDEO)
        {
            codec_ctx->max_b_frames = 1;
        }

        // добавляем словарь метаданные для кодировщика
        if (config.Metadata != null)
            fixed(AVDictionary **optPtr = &_encodingOpt)
            {
                foreach (var opt in config.Metadata)
                {
                    if (opt.Value is string a)
                        av_dict_set(optPtr, opt.Key, a, 0);
                    else if (opt.Value is int b)
                        av_dict_set_int(optPtr, opt.Key, b, 0);
                    else if (opt.Value is System.Text.Json.JsonElement j)
                    {
                        if (j.ValueKind == System.Text.Json.JsonValueKind.String)
                            av_dict_set(optPtr, opt.Key, j.GetString(), 0);
                        if (j.ValueKind == System.Text.Json.JsonValueKind.Number)
                            av_dict_set_int(optPtr, opt.Key, j.GetInt32(), 0);
                    }
                }
            }

        // CodecCtx->flags &= ~AV_CODEC_FLAG_GLOBAL_HEADER;

        // fixed(AVDictionary **optPtr = &_encodingOpt)
        //     ret = avcodec_open2(CodecCtx, codec, optPtr);
        // if (ret != 0)
        //     return false;

        // OutPacket = av_packet_alloc();

        // // указываем что не все метаданные кодировщика использованы
        // if (_encodingOpt != null)
        // {
        //     AVDictionaryEntry *dictEntry = null;
        //     dictEntry = av_dict_get(_encodingOpt, "", dictEntry, AV_DICT_IGNORE_SUFFIX);
        //     do
        //     {
        //         Log.Warn($ "Codec option not used: '{PtrHelpers.BytePtrToStringUTF8(dictEntry->key)}' = '{PtrHelpers.BytePtrToStringUTF8(dictEntry->value)}'");
        //     } while ((dictEntry = av_dict_get(_encodingOpt, "", dictEntry, AV_DICT_IGNORE_SUFFIX)) != null);
        // }

        // return true;
    }
}

NtVideoEncoder::~NtVideoEncoder()
{
}