/*  Copyright
 *    http://www.linuxhowtos.org/C_C++/socket.htm
 */
/* The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>
#include <chrono>
#include <thread>
#include <iostream>

#include "CSocket.h"

using namespace std;

struct thread_data {
   int thread_id;
   int socketFD;
};

string getInput()
{
    string msg;
    cout << "Server: ";
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

void *getMsgFromClient(void *threadArg)
{
    string msg;
    struct thread_data *data;
    data = (struct thread_data *) threadArg;
    while(1) {
        msg = getMsg(data->socketFD);
        cout << "Client: " << msg << endl;
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

void *sendMsgToClient(void *threadArg)
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

int main(int argc, char *argv[])
{
    pthread_t thread_getMsg, thread_sendMsg;
    struct thread_data data_getMsg, data_sendMsg;
    int portNumber = 0;
    int iRet = 0;
    
    if (argc < 2) {
        cerr << "ERROR, no port provided\n" << endl;
        exit(1);
    }
    try {
        portNumber = atoi(argv[1]);
        CServer server(portNumber);
        CSocket *socketClient = nullptr;
        socketClient = server.acceptConnection();
        cout << "server: got connection from "<< socketClient->getAddr() <<" port " << socketClient->getPort() << endl;

        /* Thread to read + write msg */
        data_getMsg.thread_id = 1;
        data_getMsg.socketFD  = socketClient->getSocketFD();
        iRet = pthread_create(&thread_getMsg, NULL, getMsgFromClient, (void *)&data_getMsg);
        if (iRet < 0) {
            cerr << "Error creating thread" << endl;
            throw -1;
        }

        data_sendMsg.thread_id = 2;
        data_sendMsg.socketFD = socketClient->getSocketFD();
        iRet = pthread_create(&thread_sendMsg, NULL, sendMsgToClient, (void *)&data_sendMsg);
        if (iRet < 0) {
            cerr << "Error creating thread" << endl;
            throw -1;
        }
        while (1) {

        }

        close(socketClient->getSocketFD());
        close(server.getSocketFD());
    } catch (int e) {
        cout << "Error code " << e << endl;
    }
    return 0;
}