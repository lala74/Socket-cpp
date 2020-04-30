#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

#include "utils.h"

using namespace std;

static string getInput()
{
    string msg;
    cin >> msg;
    return msg;
}

static void sendMsg(int socketFD, string msg)
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

void read_event(string name, int socketFD)
{
    int iRet;
    char msg[256];
    while(1) {
        bzero(msg, 256);
        iRet = read(socketFD, msg, 255);
        if (iRet < 0) {
            cerr << "ERROR reading from socket" << endl;
            throw -1;
        }
        if (msg[0] != '\0') {
            cout << name << ": " << msg << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

void write_event(int socketFD)
{
    string msg;
    while(1) {
        msg = getInput();
        sendMsg(socketFD, msg);
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}