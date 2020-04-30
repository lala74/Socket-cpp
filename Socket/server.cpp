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

void getMsgFromClient(int socketFD)
{
    string msg;
    while(1) {
        msg = getMsg(socketFD);
        cout << "Client: " << msg << endl;
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

void sendMsgToClient(int socketFD)
{
    string msg;
    while(1) {
        msg = getInput();
        sendMsg(socketFD, msg);
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

int main(int argc, char *argv[])
{
    int portNumber = 0;
    
    if (argc < 2) {
        cerr << "ERROR, no port provided\n" << endl;
        exit(EXIT_FAILURE);
    }
    portNumber = atoi(argv[1]);

    try {
        CServer server(portNumber);
        CSocket *socketClient = nullptr;
        socketClient = server.acceptConnection();
        cout << "server: got connection from "<< socketClient->getAddr() <<" port " << socketClient->getPort() << endl;

        /* Thread to read + write msg */
        thread read_socket  (getMsgFromClient, socketClient->getSocketFD());
        thread write_socket (sendMsgToClient, socketClient->getSocketFD());

        read_socket.join();
        write_socket.join();

    } catch (int e) {
        cout << "Error code " << e << endl;
    }
    return 0;
}