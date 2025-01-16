#pragma once
#include "BaseServerMediaSubsession.h"
#include "liveMedia.hh"

class UnicastServerMediaSubsession : public BaseServerMediaSubsession, public OnDemandServerMediaSubsession
{
public:
    static UnicastServerMediaSubsession *createNew(UsageEnvironment &env, StreamReplicator *replicator);

protected:
    UnicastServerMediaSubsession(UsageEnvironment &env, StreamReplicator *replicator)
        : BaseServerMediaSubsession(replicator), OnDemandServerMediaSubsession(env, False) {}

    virtual FramedSource *createNewStreamSource(unsigned clientSessionId, unsigned &estBitrate);
    virtual RTPSink *createNewRTPSink(Groupsock *rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource *inputSource);
    virtual char const *getAuxSDPLine(RTPSink *rtpSink, FramedSource *inputSource);
};