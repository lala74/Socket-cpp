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

struct thread_data {
   int thread_id;
   int socketFD;
};

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

void *getMsgFromServer(void *threadArg)
{
    string msg;
    struct thread_data *data;
    data = (struct thread_data *) threadArg;
    while(1) {
        msg = getMsg(data->socketFD);
        cout << "Server: " << msg << endl;
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

void *sendMsgToServer(void *threadArg)
{
    string msg;
    struct thread_data *data;
    data = (struct thread_data *) threadArg;
    while(1) {
        msg = getInput();
        sendMsg(data->socketFD, msg);
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
    pthread_t thread_getMsg, thread_sendMsg;
    struct thread_data data_getMsg, data_sendMsg;
    int portNumber = 0;
    int iRet = 0;

    if (argc < 3) {
        cerr << "usage hostname port " << endl;
        exit(0);
    }
    portNumber = atoi(argv[2]);
    try {
        serverAddr = getAddr(argv[1]);
        if (serverAddr == 0) {
            cerr << "Error no such host" << endl;
            throw -1;
        }
        CClient client;
        client.connectAddr(serverAddr, portNumber);
        
        /* Thread to read + write msg */
        data_getMsg.thread_id = 1;
        data_getMsg.socketFD  = client.getSocketFD();
        iRet = pthread_create(&thread_getMsg, NULL, getMsgFromServer, (void *)&data_getMsg);
        if (iRet < 0) {
            cerr << "Error creating thread" << endl;
            throw -1;
        }

        data_sendMsg.thread_id = 2;
        data_sendMsg.socketFD = client.getSocketFD();
        iRet = pthread_create(&thread_sendMsg, NULL, sendMsgToServer, (void *)&data_sendMsg);
        if (iRet < 0) {
            cerr << "Error creating thread" << endl;
            throw -1;
        }
        while (1) {

        }
    } catch (int e) {
        cout << "Error code " << e << endl;
    }
    return 0;
}