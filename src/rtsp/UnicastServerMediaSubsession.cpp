#include "UnicastServerMediaSubsession.h"

UnicastServerMediaSubsession *UnicastServerMediaSubsession::createNew(UsageEnvironment &env, StreamReplicator *replicator)
{
    return new UnicastServerMediaSubsession(env, replicator);
}

FramedSource *UnicastServerMediaSubsession::createNewStreamSource(unsigned clientSessionId, unsigned &estBitrate)
{
    estBitrate = 500;
    FramedSource *source = m_replicator->createStreamReplica();
    return createSource(envir(), source, m_format);
}

RTPSink *UnicastServerMediaSubsession::createNewRTPSink(Groupsock *rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource *inputSource)
{
    return createSink(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic, m_format, dynamic_cast<NtDeviceInterface *>(m_replicator->inputSource()));
}

char const *UnicastServerMediaSubsession::getAuxSDPLine(RTPSink *rtpSink, FramedSource *inputSource)
{
    return this->getAuxLine(dynamic_cast<NtDeviceInterface *>(m_replicator->inputSource()), rtpSink);
}