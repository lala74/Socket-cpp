/* C libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
/* C++ libraries */
#include <pthread.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <string>

#include "CSocket.h"

using namespace std;

CSocket::CSocket()
{
    m_socketFD = 0;
    // clear address structure
    bzero((char *)&m_socketAddr, sizeof(m_socketAddr));
}

CSocket::CSocket(const CSocket &soc)
{   
    m_socketFD = soc.m_socketFD;
    // clear address structure
    bzero((char *)&m_socketAddr, sizeof(m_socketAddr));
    m_socketAddr.sin_family         = soc.m_socketAddr.sin_family;
    m_socketAddr.sin_addr.s_addr    = soc.m_socketAddr.sin_addr.s_addr;
    m_socketAddr.sin_port           = soc.m_socketAddr.sin_port;
}

CSocket::~CSocket()
{

}

int CSocket::getSocketFD()
{
    return m_socketFD;
}

int CSocket::getPort()
{
    return ntohs(m_socketAddr.sin_port);
}

string CSocket::getAddr()
{
    return inet_ntoa(m_socketAddr.sin_addr);
}


CServer::CServer(int port)
{
    int iRet;
    char hostname[256];
    iRet = gethostname(hostname, sizeof(hostname));
    cout << "hostname: " << hostname << endl;

    bzero((char *)&m_socketAddr, sizeof(m_socketAddr));
    /* setup the host_addr structure for use in bind call */
    m_socketAddr.sin_family = AF_INET;              // server byte order
    m_socketAddr.sin_addr.s_addr = INADDR_ANY;      // automatically be filled with current host's IP address
    m_socketAddr.sin_port = htons(port);            // convert short integer value for port must be converted into network byte order
    /* create a socket
     * socket(int domain, int type, int protocol)
     *  - domain        :
     *      - AF_UNIX: unix domain for two processes which share a common file system  
     *      - AF_INET: IPv4 or AF_INET6:IPv6 - Internet domain for any two hosts on the Internet
     *  - protocol type : SOCK_STREAM:TCP/IP or SOCK_DGRAM:UDP/IP
     *  - protocol      : 0 to use default protocol
    */
    m_socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socketFD < 0) {
        cerr << "Error opening socket" << endl;
        throw m_socketFD;
    }
    /* bind(int fd, struct sockaddr *local_addr, socklen_t addr_length)
     * bind() passes file descriptor, the address structure,
     * and the length of the address structure
     * This bind() call will bind  the socket to the current IP address on port, portno
     */
    iRet = bind(m_socketFD, (struct sockaddr *)&m_socketAddr, sizeof(m_socketAddr));
    if (iRet < 0) {
        cerr << "Error on binding" << endl;
        throw iRet;
    }
    /* This listen() call tells the socket to listen to the incoming connections.
     * The listen() function places all incoming connection into a backlog queue
     * until accept() call accepts the connection.
     * Here, we set the maximum size for the backlog queue to 5.
     */
    iRet = listen(m_socketFD, 5);
    if (iRet < 0) {
        cerr << "Error on listening" << endl;
        throw iRet;
    }
}

CSocket *CServer::acceptConnection()
{
    CSocket socketClient;
    socklen_t addrLen = sizeof(socketClient.m_socketAddr);
    /* This accept() function will write the connecting client's address info
     * into the the address structure and the size of that structure is clilen.
     * The accept() returns a new socket file descriptor for the accepted connection.
     * So, the original socket file descriptor can continue to be used
     * for accepting new connections while the new socker file descriptor is used for
     * communicating with the connected client.
     */
    socketClient.m_socketFD = accept(m_socketFD, (struct sockaddr *)&socketClient.m_socketAddr, &addrLen);
    if (socketClient.m_socketFD < 0) {
        cerr << "Error on accepting" << endl;
        throw socketClient.m_socketFD;
    }
    CSocket *pSocket = new CSocket(socketClient);
    return pSocket;
}

CClient::CClient()
{
    m_socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socketFD < 0) {
        cerr << "Error opening socket" << endl;
        throw m_socketFD;
    }
}

void CClient::connectAddr(char *addr, int addrLen, int port)
{
    int iRet = 0;
    sockaddr_in serverAddr;
    bzero((char *)&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    bcopy(  addr,
            (char *)&serverAddr.sin_addr.s_addr,
            addrLen);
    cout << "Connection to " << inet_ntoa(serverAddr.sin_addr) << " port " << port << endl;
    iRet = connect(m_socketFD, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (iRet < 0) {
        cerr << "ERROR connecting" << endl;
        throw -iRet;
    }
}
