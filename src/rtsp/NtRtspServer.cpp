#include "NtRtspServer.h"

#include <sstream>
#include <fstream>
#include <algorithm>
#include <string>
#include <cstdio>
#include <time.h>
#include <ByteStreamMemoryBufferSource.hh>

u_int32_t NtRTSPServer::NtClientConnection::m_ClientSessionCount = 0;

// NtRTSPServer implementation

NtRTSPServer *NtRTSPServer::createNew(UsageEnvironment &env, Port rtspPort, UserAuthenticationDatabase *authDatabase, unsigned reclamationTestSeconds)
{
    int ourSocketIPv4 = setUpOurSocket(env, rtspPort, AF_INET);
    int ourSocketIPv6 = setUpOurSocket(env, rtspPort, AF_INET6);

    if (ourSocketIPv4 != -1)
    {
        return new NtRTSPServer(env, ourSocketIPv4, ourSocketIPv6, rtspPort, authDatabase, reclamationTestSeconds);
    }
    return NULL;
}

NtRTSPServer::NtRTSPServer(UsageEnvironment &env, int ourSocketIPv4, int ourSocketIPv6, Port rtspPort, UserAuthenticationDatabase *authDatabase, unsigned reclamationTestSeconds)
    : RTSPServer(env, ourSocketIPv4, ourSocketIPv6, rtspPort, authDatabase, reclamationTestSeconds)
{
}

NtRTSPServer::~NtRTSPServer() {}

char const *NtRTSPServer::allowedCommandNames()
{
    return "OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE, GET_PARAMETER";
}

// NtClientConnection implementation

NtRTSPServer::NtClientConnection::NtClientConnection(RTSPServer &ourServer, int clientSocket, struct sockaddr_storage const &clientAddr)
    : RTSPServer::RTSPClientConnection(ourServer, clientSocket, clientAddr)
{
    envir() << "new client connected\n";
    m_ClientSessionCount++;
}

NtRTSPServer::NtClientConnection::~NtClientConnection()
{
    envir() << "client disconnect\n";
    m_ClientSessionCount--;
}

void NtRTSPServer::NtClientConnection::handleCmd_notFound()
{
    std::ostringstream os;
    ServerMediaSessionIterator it(fOurServer);
    ServerMediaSession *serverSession = NULL;
    while ((serverSession = it.next()) != NULL)
    {
        os << serverSession->streamName() << "\n";
    }

    setRTSPResponse("404 Stream Not Found", os.str().c_str());
}

void NtRTSPServer::NtClientConnection::afterStreaming(void *clientData)
{
    NtRTSPServer::NtClientConnection *clientConnection = (NtRTSPServer::NtClientConnection *)clientData;

    // Arrange to delete the 'client connection' object:
    if (clientConnection->fRecursionCount > 0)
    {
        // We're still in the midst of handling a request
        clientConnection->fIsActive = False; // will cause the object to get deleted at the end of handling the request
    }
    else
    {
        // We're no longer handling a request; delete the object now:
        delete clientConnection;
    }
}

void NtRTSPServer::NtClientConnection::handleCmd_GET_PARAMETER(char const *t_fullRequestStr)
{
    envir() << "handleCmd_GET_PARAMETER:" << t_fullRequestStr;
    RTSPServer::RTSPClientConnection::handleCmd_GET_PARAMETER(t_fullRequestStr);
}

void NtRTSPServer::NtClientConnection::handleCmd_OPTIONS()
{
    handleLogRequest((const struct sockaddr_in &)getClientAddr(), "OPTIONS");
    RTSPServer::RTSPClientConnection::handleCmd_OPTIONS();
}
void NtRTSPServer::NtClientConnection::handleCmd_DESCRIBE(char const *t_urlPreSuffix, char const *t_urlSuffix, char const *t_fullRequestStr)
{
    handleLogRequest((const struct sockaddr_in &)getClientAddr(), t_fullRequestStr);
    RTSPServer::RTSPClientConnection::handleCmd_DESCRIBE(t_urlPreSuffix, t_urlSuffix, t_fullRequestStr);
}

void NtRTSPServer::NtClientConnection::handleLogRequest(const struct sockaddr_in &addr, char const *request)
{
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);

    std::string line(request);
    auto start_position_to_erase = line.find("RTSP/1.0");
    if (start_position_to_erase != std::string::npos)
        line.erase(start_position_to_erase, line.length() - start_position_to_erase);

    envir() << "Client '" << ip << "' Request " << line.c_str() << "\n";
}

// NtClientSession implementation
NtRTSPServer::NtClientSession::NtClientSession(NtRTSPServer &ourServer, u_int32_t sessionId)
    : RTSPServer::RTSPClientSession(ourServer, sessionId), m_sessionId(sessionId)
{
}

NtRTSPServer::NtClientSession::~NtClientSession()
{
}

void NtRTSPServer::NtClientSession::handleCmd_TEARDOWN(RTSPServer::RTSPClientConnection *t_ourClientConnection, ServerMediaSubsession *t_subsession)
{
    const sockaddr_storage &clientAddr = reinterpret_cast<NtRTSPServer::NtClientConnection *>(t_ourClientConnection)->getClientAddr();
    handleLogRequest((const struct sockaddr_in &)clientAddr, "TEARDOWN");
    RTSPServer::RTSPClientSession::handleCmd_TEARDOWN(t_ourClientConnection, t_subsession);
}

void NtRTSPServer::NtClientSession::handleCmd_PLAY(RTSPServer::RTSPClientConnection *t_ourClientConnection, ServerMediaSubsession *t_subsession, char const *t_fullRequestStr)
{
    auto ourClientConnection = reinterpret_cast<NtRTSPServer::NtClientConnection *>(t_ourClientConnection);
    const sockaddr_storage &clientAddr = ourClientConnection->getClientAddr();
    handleLogRequest((const struct sockaddr_in &)clientAddr, t_fullRequestStr);
    // RTSPServer::RTSPClientSession::handleCmd_PLAY(t_ourClientConnection, t_subsession, t_fullRequestStr);

    char const *fullRequestStr = t_fullRequestStr;
    ServerMediaSubsession *subsession = t_subsession;

    char *rtspURL = fOurRTSPServer.rtspURL(fOurServerMediaSession, ourClientConnection->fClientInputSocket);
    size_t rtspURLSize = strlen(rtspURL);

    // Parse the client's "Scale:" header, if any:
    float scale;
    Boolean sawScaleHeader = parseScaleHeader(fullRequestStr, scale);

    // Try to set the stream's scale factor to this value:
    if (subsession == NULL /*aggregate op*/)
    {
        fOurServerMediaSession->testScaleFactor(scale);
    }
    else
    {
        subsession->testScaleFactor(scale);
    }

    char buf[100];
    char *scaleHeader;
    if (!sawScaleHeader)
    {
        buf[0] = '\0'; // Because we didn't see a Scale: header, don't send one back
    }
    else
    {
        sprintf(buf, "Scale: %f\r\n", scale);
    }
    scaleHeader = strDup(buf);

    // Parse the client's "Range:" header, if any:
    float duration = 0.0;
    double rangeStart = 0.0, rangeEnd = 0.0;
    char *absStart = NULL;
    char *absEnd = NULL;
    Boolean startTimeIsNow;
    Boolean sawRangeHeader = parseRangeHeader(fullRequestStr, rangeStart, rangeEnd, absStart, absEnd, startTimeIsNow);

    if (sawRangeHeader && absStart == NULL /*not seeking by 'absolute' time*/)
    {
        // Use this information, plus the stream's duration (if known), to create our own "Range:" header, for the response:
        duration = subsession == NULL /*aggregate op*/
                       ? fOurServerMediaSession->duration()
                       : subsession->duration();
        if (duration < 0.0)
        {
            // We're an aggregate PLAY, but the subsessions have different durations.
            duration = -duration;
        }

        // Make sure that "rangeStart" and "rangeEnd" (from the client's "Range:" header)
        // have sane values, before we send back our own "Range:" header in our response:
        if (rangeStart < 0.0)
            rangeStart = 0.0;
        else if (rangeStart > duration)
            rangeStart = duration;
        if (rangeEnd < 0.0)
            rangeEnd = 0.0;
        else if (rangeEnd > duration)
            rangeEnd = duration;
        if ((scale > 0.0 && rangeStart > rangeEnd && rangeEnd > 0.0) ||
            (scale < 0.0 && rangeStart < rangeEnd))
        {
            // "rangeStart" and "rangeEnd" were the wrong way around; swap them:
            double tmp = rangeStart;
            rangeStart = rangeEnd;
            rangeEnd = tmp;
        }
    }

    // Do any required seeking/scaling on each subsession, before starting streaming.
    // (However, we don't do this if the "PLAY" request was for just a single subsession
    // of a multiple-subsession stream; for such streams, seeking/scaling can be done
    // only with an aggregate "PLAY".)
    for (unsigned i = 0; i < fNumStreamStates; ++i)
    {
        if (subsession == NULL /* means: aggregated operation */ || fNumStreamStates == 1)
        {
            if (fStreamStates[i].subsession != NULL)
            {
                if (sawScaleHeader)
                {
                    fStreamStates[i].subsession->setStreamScale(fOurSessionId, fStreamStates[i].streamToken, scale);
                }
                if (absStart != NULL)
                {
                    // Special case handling for seeking by 'absolute' time:

                    fStreamStates[i].subsession->seekStream(fOurSessionId, fStreamStates[i].streamToken, absStart, absEnd);
                }
                else
                {
                    // Seeking by relative (NPT) time:

                    u_int64_t numBytes;
                    if (!sawRangeHeader || startTimeIsNow)
                    {
                        // We're resuming streaming without seeking, so we just do a 'null' seek
                        // (to get our NPT, and to specify when to end streaming):
                        fStreamStates[i].subsession->nullSeekStream(fOurSessionId, fStreamStates[i].streamToken,
                                                                    rangeEnd, numBytes);
                    }
                    else
                    {
                        // We do a real 'seek':
                        double streamDuration = 0.0; // by default; means: stream until the end of the media
                        if (rangeEnd > 0.0 && (rangeEnd + 0.001) < duration)
                        {
                            // the 0.001 is because we limited the values to 3 decimal places
                            // We want the stream to end early.  Set the duration we want:
                            streamDuration = rangeEnd - rangeStart;
                            if (streamDuration < 0.0)
                                streamDuration = -streamDuration;
                            // should happen only if scale < 0.0
                        }
                        fStreamStates[i].subsession->seekStream(fOurSessionId, fStreamStates[i].streamToken,
                                                                rangeStart, streamDuration, numBytes);
                    }
                }
            }
        }
    }

    // Create the "Range:" header that we'll send back in our response.
    // (Note that we do this after seeking, in case the seeking operation changed the range start time.)
    char *rangeHeader;
    if (absStart != NULL)
    {
        // We're seeking by 'absolute' time:
        char *rangeHeaderBuf;

        if (absEnd == NULL)
        {
            rangeHeaderBuf = new char[100 + strlen(absStart)]; // ample space
            sprintf(rangeHeaderBuf, "Range: clock=%s-\r\n", absStart);
        }
        else
        {
            rangeHeaderBuf = new char[100 + strlen(absStart) + strlen(absEnd)]; // ample space
            sprintf(rangeHeaderBuf, "Range: clock=%s-%s\r\n", absStart, absEnd);
        }
        delete[] absStart;
        delete[] absEnd;
        rangeHeader = strDup(rangeHeaderBuf);
        delete[] rangeHeaderBuf;
    }
    else
    {
        // We're seeking by relative (NPT) time:
        if (!sawRangeHeader || startTimeIsNow)
        {
            // We didn't seek, so in our response, begin the range with the current NPT (normal play time):
            float curNPT = 0.0;
            for (unsigned i = 0; i < fNumStreamStates; ++i)
            {
                if (subsession == NULL /* means: aggregated operation */
                    || subsession == fStreamStates[i].subsession)
                {
                    if (fStreamStates[i].subsession == NULL)
                        continue;
                    float npt = fStreamStates[i].subsession->getCurrentNPT(fStreamStates[i].streamToken);
                    if (npt > curNPT)
                        curNPT = npt;
                    // Note: If this is an aggregate "PLAY" on a multi-subsession stream,
                    // then it's conceivable that the NPTs of each subsession may differ
                    // (if there has been a previous seek on just one subsession).
                    // In this (unusual) case, we just return the largest NPT; I hope that turns out OK...
                }
            }
            rangeStart = curNPT;
        }

        if (rangeEnd == 0.0 && scale >= 0.0)
        {
            sprintf(buf, "Range: npt=%.3f-\r\n", rangeStart);
        }
        else
        {
            sprintf(buf, "Range: npt=%.3f-%.3f\r\n", rangeStart, rangeEnd);
        }
        rangeHeader = strDup(buf);
    }

    // Create a "RTP-Info:" line.  It will get filled in from each subsession's state:
    char const *rtpInfoFmt =
        "%s" // "RTP-Info:", plus any preceding rtpInfo items
        "%s" // comma separator, if needed
        "url=%s/%s"
        ";seq=%d"
        ";rtptime=%u";
    unsigned rtpInfoFmtSize = strlen(rtpInfoFmt);
    char *rtpInfo = strDup("RTP-Info: ");

    unsigned numRTPInfoItems = 0;
    unsigned short rtpSeqNum = 0;
    unsigned rtpTimestamp = 0;

    // prepare response
    for (unsigned i = 0; i < fNumStreamStates; ++i)
    {
        if (subsession == NULL || subsession == fStreamStates[i].subsession)
        {
            if (fStreamStates[i].subsession == NULL)
                continue;

            // get Timestamp from rtpSink
            RTPSink *t_rtpSink = NULL;
            RTCPInstance *t_rtcp = NULL;
            fStreamStates[i].subsession->getRTPSinkandRTCP(fStreamStates[i].streamToken, t_rtpSink, t_rtcp);

            if (t_rtpSink != NULL)
            {
                rtpSeqNum = t_rtpSink->currentSeqNo();
                rtpTimestamp = t_rtpSink->presetNextTimestamp();
            }

            const char *urlSuffix = fStreamStates[i].subsession->trackId();
            char *prevRTPInfo = rtpInfo;
            unsigned rtpInfoSize = rtpInfoFmtSize + strlen(prevRTPInfo) + 1 + rtspURLSize + strlen(urlSuffix) + 5 /*max unsigned short len*/
                                   + 10 /*max unsigned (32-bit) len*/ + 2 /*for trailing \r\n at end*/;

            rtpInfo = new char[rtpInfoSize];
            sprintf(rtpInfo, rtpInfoFmt,
                    prevRTPInfo,
                    numRTPInfoItems++ == 0 ? "" : ",",
                    rtspURL, urlSuffix,
                    rtpSeqNum,
                    rtpTimestamp);
            delete[] prevRTPInfo;
        }
    }
    if (numRTPInfoItems == 0)
    {
        rtpInfo[0] = '\0';
    }
    else
    {
        size_t rtpInfoLen = strlen(rtpInfo);
        rtpInfo[rtpInfoLen] = '\r';
        rtpInfo[rtpInfoLen + 1] = '\n';
        rtpInfo[rtpInfoLen + 2] = '\0';
    }

    // Fill in the response:
    snprintf((char *)ourClientConnection->fResponseBuffer, sizeof ourClientConnection->fResponseBuffer,
             "RTSP/1.0 200 OK\r\n"
             "CSeq: %s\r\n"
             "%s"
             "%s"
             "%s"
             "Session: %08X\r\n"
             "%s\r\n",
             ourClientConnection->fCurrentCSeq,
             dateHeader(),
             scaleHeader,
             rangeHeader,
             fOurSessionId,
             rtpInfo);

    // Now, start streaming:
    for (unsigned i = 0; i < fNumStreamStates; ++i)
    {
        if (subsession == NULL || subsession == fStreamStates[i].subsession)
        {
            if (fStreamStates[i].subsession == NULL)
                continue;

            fStreamStates[i].subsession->startStream(fOurSessionId,
                                                     fStreamStates[i].streamToken,
                                                     (TaskFunc *)noteClientLiveness, this,
                                                     rtpSeqNum, rtpTimestamp,
                                                     NtRTSPServer::NtClientConnection::handleAlternativeRequestByte, ourClientConnection);
        }
    }

    delete[] rtpInfo;
    delete[] rangeHeader;
    delete[] scaleHeader;
    delete[] rtspURL;
}

void NtRTSPServer::NtClientSession::handleCmd_PAUSE(RTSPServer::RTSPClientConnection *t_ourClientConnection, ServerMediaSubsession *t_subsession)
{
    const sockaddr_storage &clientAddr = reinterpret_cast<NtRTSPServer::NtClientConnection *>(t_ourClientConnection)->getClientAddr();
    handleLogRequest((const struct sockaddr_in &)clientAddr, "PAUSE");
    RTSPServer::RTSPClientSession::handleCmd_PAUSE(t_ourClientConnection, t_subsession);
}

void NtRTSPServer::NtClientSession::handleCmd_SETUP(RTSPServer::RTSPClientConnection *t_ourClientConnection, char const *t_urlPreSuffix, char const *t_urlSuffix, char const *t_fullRequestStr)
{
    const sockaddr_storage &clientAddr = reinterpret_cast<NtRTSPServer::NtClientConnection *>(t_ourClientConnection)->getClientAddr();
    handleLogRequest((const struct sockaddr_in &)clientAddr, t_fullRequestStr);
    RTSPServer::RTSPClientSession::handleCmd_SETUP(t_ourClientConnection, t_urlPreSuffix, t_urlSuffix, t_fullRequestStr);
}

void NtRTSPServer::NtClientSession::handleCmd_GET_PARAMETER(RTSPClientConnection *t_ourClientConnection, ServerMediaSubsession *t_subsession, char const *t_fullRequestStr)
{
    const sockaddr_storage &clientAddr = reinterpret_cast<NtRTSPServer::NtClientConnection *>(t_ourClientConnection)->getClientAddr();
    handleLogRequest((const struct sockaddr_in &)clientAddr, t_fullRequestStr);
    setRTSPResponse(t_ourClientConnection, "200 OK", fOurSessionId);
}

void NtRTSPServer::NtClientSession::handleLogRequest(const struct sockaddr_in &addr, char const *request)
{
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);

    std::string line(request);
    auto start_position_to_erase = line.find("RTSP/1.0");
    if (start_position_to_erase != std::string::npos)
        line.erase(start_position_to_erase, line.length() - start_position_to_erase);

    envir() << "Client '" << ip << "' Request " << line.c_str() << "\n";
}
