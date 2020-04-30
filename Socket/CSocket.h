#ifndef ___C_SOCKET_H__
#define ___C_SOCKET_H__

#include <netinet/in.h>
#include <string>

class CSocket
{
    public:
        CSocket();
        CSocket(const CSocket &soc);
        ~CSocket();

        int getSocketFD();
        int getPort();
        std::string getAddr();

    protected:
        int m_socketFD;
        sockaddr_in m_socketAddr;

    friend class CServer;
    friend class CClient;
};

class CServer: public CSocket
{
    public:
        CServer(int port);
        CSocket *acceptConnection();
};

class CClient: public CSocket
{
    public:
        CClient();
        void connectAddr(in_addr_t addr, int port);
};

#endif