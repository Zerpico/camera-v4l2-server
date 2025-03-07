#pragma once

#include <liveMedia.hh>
#include "NtDeviceInterface.h"
#include <string>
#include <sstream>

class BaseServerMediaSubsession
{
public:
    BaseServerMediaSubsession(StreamReplicator *replicator) : m_replicator(replicator)
    {
        NtDeviceInterface *deviceSource = dynamic_cast<NtDeviceInterface *>(replicator->inputSource());
        if (deviceSource)
        {
            if (deviceSource->getVideoFormat() > NtDeviceFormat::FMT_NONE)
            {
                m_format = BaseServerMediaSubsession::getVideoRtpFormat(deviceSource->getVideoFormat());
            }
            else
            {
                m_format = BaseServerMediaSubsession::getAudioRtpFormat(deviceSource->getAudioFormat(), deviceSource->getSampleRate(), deviceSource->getChannels());
            }
            replicator->envir() << "RTP format:" << m_format.c_str();
        }
    }

    static std::string getVideoRtpFormat(NtDeviceFormat format)
    {
        switch (format)
        {
        case NtDeviceFormat::FMT_HEVC:
            return "video/H265";
        case NtDeviceFormat::FMT_H264:
            return "video/H264";
        case NtDeviceFormat::FMT_MJPEG:
            return "video/JPEG";
        case NtDeviceFormat::FMT_VP8:
            return "video/VP8";
        case NtDeviceFormat::FMT_VP9:
            return "video/VP9";
        case NtDeviceFormat::FMT_MPEGTS:
            return "video/MP2T";
        default:
            return "";
        }
    }

    static std::string getAudioRtpFormat(NtDeviceFormat format, int sampleRate, int channels)
    {
        std::ostringstream os;
        os << "audio/";
        switch (format)
        {
        case NtDeviceFormat::FMT_A_LAW:
            os << "PCMA";
            break;
        case NtDeviceFormat::FMT_MU_LAW:
            os << "PCMU";
            break;
        case NtDeviceFormat::FMT_S8:
            os << "L8";
            break;
        case NtDeviceFormat::FMT_S24_BE:
        case NtDeviceFormat::FMT_S24_LE:
            os << "L24";
            break;
        case NtDeviceFormat::FMT_S32_BE:
        case NtDeviceFormat::FMT_S32_LE:
            os << "L32";
            break;
        case NtDeviceFormat::FMT_MPEG:
            os << "MPEG";
            break;
        default:
            os << "L16";
            break;
        }
        os << "/" << sampleRate << "/" << channels;
        return os.str();
    }

public:
    static FramedSource *createSource(UsageEnvironment &env, FramedSource *videoES, const std::string &format);
    static RTPSink *createSink(UsageEnvironment &env, Groupsock *rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, const std::string &format, NtDeviceInterface *source);
    char const *getAuxLine(NtDeviceInterface *source, RTPSink *rtpSink);

protected:
    StreamReplicator *m_replicator;
    std::string m_format;
};