#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <iostream>
#include <pthread.h>
#include <chrono>
#include <thread>

#include "CSocket.h"

using namespace std;

string getInput()
{
    string msg;
    cout << "Client: ";
    cin >> msg;
    return msg;
}

void sendMsg(int socketFD, string msg)
{
    int n = 0;
    char bufferWrite[256];
    bzero(bufferWrite, 256);
    strcpy(bufferWrite, msg.c_str());
    n = write(socketFD, bufferWrite, strlen(bufferWrite));
    if (n < 0) {
        cerr << "ERROR writing to socket" << endl;
        throw -1;
    }
}

string getMsg(int socketFD)
{
    int n = 0;
    char buffer[256];
    string msg;
    bzero(buffer, 256);
    n = read(socketFD, buffer, 255);
    if (n < 0) {
        cerr << "ERROR reading from socket" << endl;
        throw -1;
    }
    msg += buffer;
    return msg;
}

void getMsgFromServer(int socketFD)
{
    string msg;
    while(1) {
        msg = getMsg(socketFD);
        cout << "Server: " << msg << endl;
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

void sendMsgToServer(int socketFD)
{
    string msg;
    while(1) {
        msg = getInput();
        sendMsg(socketFD, msg);
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

in_addr_t getAddr(char *host)
{
    in_addr_t addr;
    struct hostent *server = nullptr;
    // Convert address IP to in_addr_t
    if(inet_pton(AF_INET, host, &addr) <= 0) { 
        server = gethostbyname(host);
        if (server == NULL) {
            return 0;
        } else {
            bcopy(  server->h_addr_list[0],
                    (char *)&addr,
                    server->h_length);   
        }
    }
    return addr;
}

int main(int argc, char *argv[])
{
    in_addr_t serverAddr;
    int portNumber = 0;
    int iRet = 0;

    if (argc < 3) {
        cerr << "usage hostname port " << endl;
        exit(EXIT_FAILURE);
    }
    portNumber = atoi(argv[2]);

    try {
        serverAddr = getAddr(argv[1]);
        if (serverAddr == 0) {
            cerr << "Error no such host" << endl;
            throw -1;
        }
        CClient client;
        iRet = client.connectAddr(serverAddr, portNumber);
        if (iRet < 0) {
            cerr << "Error while connecting" << endl;
            throw -1;
        }
        
        /* Thread to read + write msg */
        thread read_socket  (getMsgFromServer, client.getSocketFD());
        thread write_socket (sendMsgToServer, client.getSocketFD());

        read_socket.join();
        write_socket.join();

    } catch (int e) {
        cout << "Error code " << e << endl;
    }
    return 0;
}