#pragma once
enum class NtDeviceFormat
{
    FMT_NONE = -1,
    FMT_JPEG,
    FMT_MJPEG,
    FMT_MPEGTS,
    FMT_H264,
    FMT_HEVC,
    FMT_VP8,
    FMT_VP9,

    FMT_A_LAW = 1000,
    FMT_MU_LAW,
    FMT_S8,
    FMT_S16,
    FMT_S24_BE,
    FMT_S24_LE,
    FMT_S32_BE,
    FMT_S32_LE,
    FMT_MPEG,
    FMT_AAC
};