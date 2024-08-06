#pragma once

#include <RTSPServer.hh>
#include <RTSPCommon.hh>
#include <GroupsockHelper.hh>
#include <string>

class NtRTSPServer : public RTSPServer
{
public:
    static NtRTSPServer *createNew(UsageEnvironment &env, Port rtspPort, UserAuthenticationDatabase *authDatabase, unsigned reclamationTestSeconds = 20);

protected:
    NtRTSPServer(UsageEnvironment &env, int ourSocketIPv4, int ourSocketIPv6, Port rtspPort, UserAuthenticationDatabase *authDatabase, unsigned reclamationTestSeconds);
    virtual ~NtRTSPServer();

    char const *allowedCommandNames();

public:
    class NtRTSPClientSession; // forward
    // Implementation of custom RTSPClientConnection
    class NtClientConnection : public RTSPServer::RTSPClientConnection // forward
    {
    public:
        static NtClientConnection *createNew(RTSPServer &ourServer, int clientSocket, struct sockaddr_storage const &clientAddr)
        {
            return new NtClientConnection(ourServer, clientSocket, clientAddr);
        }

        const sockaddr_storage &getClientAddr() const { return fClientAddr; }
        const int &getClientSocket() const { return fClientInputSocket; }

        static void handleAlternativeRequestByte2(void *s, u_int8_t requestByte)
        {
            handleAlternativeRequestByte(s, requestByte);
        }

        unsigned char *getResponseBuffer()
        {
            return fResponseBuffer;
        }

    protected:
        NtClientConnection(RTSPServer &ourServer, int clientSocket, struct sockaddr_storage const &clientAddr);
        virtual ~NtClientConnection();

        virtual void handleCmd_OPTIONS();
        virtual void handleCmd_DESCRIBE(char const *urlPreSuffix, char const *urlSuffix, char const *fullRequestStr);
        virtual void handleCmd_GET_PARAMETER(char const *fullRequestStr);
        friend class NtRTSPServer;
        friend class NtClientSession;

    private:
        ServerMediaSubsession *getSubsesion(const char *urlSuffix);
        virtual void handleCmd_notFound();
        static void afterStreaming(void *clientData);

    private:
        static u_int32_t m_ClientSessionCount;
        void handleLogRequest(const struct sockaddr_in &addr, char const *request);
    };

    // Implementation of custom RTSPClientSession
    class NtClientSession : public RTSPServer::RTSPClientSession // forward
    {
    protected:
        NtClientSession(NtRTSPServer &ourServer, u_int32_t sessionId);
        virtual ~NtClientSession();

        friend class NtRTSPServer;
        friend class NtClientConnection;

        // request methods
        virtual void handleCmd_TEARDOWN(RTSPServer::RTSPClientConnection *t_ourClientConnection, ServerMediaSubsession *t_subsession);
        virtual void handleCmd_PLAY(RTSPServer::RTSPClientConnection *t_ourClientConnection, ServerMediaSubsession *t_subsession, char const *t_fullRequestStr);
        virtual void handleCmd_PAUSE(RTSPServer::RTSPClientConnection *t_ourClientConnection, ServerMediaSubsession *t_subsession);
        virtual void handleCmd_GET_PARAMETER(RTSPServer::RTSPClientConnection *t_ourClientConnection, ServerMediaSubsession *t_subsession, char const *t_fullRequestStr);

        virtual void handleCmd_SETUP(RTSPServer::RTSPClientConnection *ourClientConnection, char const *urlPreSuffix, char const *urlSuffix, char const *fullRequestStr);

    private:
        const u_int32_t m_sessionId;
        void handleLogRequest(const struct sockaddr_in &addr, char const *request);
    };

protected:
    virtual RTSPServer::ClientConnection *createNewClientConnection(int clientSocket, struct sockaddr_storage const &clientAddr)
    {
        return NtRTSPServer::NtClientConnection::createNew(*this, clientSocket, clientAddr);
    }
    virtual RTSPServer::ClientSession *createNewClientSession(u_int32_t sessionId)
    {
        return new NtClientSession(*this, sessionId);
    }

private:
    friend class NtClientConnection;
    friend class NtClientSession;
};
