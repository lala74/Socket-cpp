#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <termios.h>
#include <unistd.h>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "utils.h"

using namespace std;

static void close_socket(int socketFD)
{
    if(socketFD < 0) {
        close(socketFD);
    }
}

static char getch(void)
{
    char buf = 0;
    struct termios old = {0};
    fflush(stdout);
    if(tcgetattr(0, &old) < 0) perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if(tcsetattr(0, TCSANOW, &old) < 0) perror("tcsetattr ICANON");
    if(read(0, &buf, 1) < 0) perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if(tcsetattr(0, TCSADRAIN, &old) < 0) perror("tcsetattr ~ICANON");
    return buf;
}

in_addr_t getAddr(char* host)
{
    in_addr_t addr;
    struct hostent* server = nullptr;
    // Convert address IP to in_addr_t
    if(inet_pton(AF_INET, host, &addr) <= 0) {
        server = gethostbyname(host);
        if(server == NULL) {
            return 0;
        } else {
            bcopy(server->h_addr_list[0], (char*)&addr, server->h_length);
        }
    }
    return addr;
}

void read_event(string name, int socketFD, int serverSocketFD)
{
    int iRet = 0;
    char msg[256];
    while(1) {
        bzero(msg, 256);
        iRet = read(socketFD, msg, 255);
        if(iRet < 0) {
            cerr << "ERROR reading from socket" << endl;
            throw - 1;
        }
        if(msg[0] != '\0') {
            cout << name << ": " << msg << endl;
        }
        if(strcmp(msg, "exit") == 0) {
            cout << "\nDisconnected" << endl;
            close_socket(socketFD);
            close_socket(serverSocketFD);
            exit(EXIT_SUCCESS);
        }
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

void write_event(string name, int socketFD, int serverSocketFD)
{
    int iRet = 0;
    char firstWord;
    string msg;
    while(1) {
        firstWord = getch();
        cout << name << ": " << firstWord;
        getline(cin, msg);
        msg = firstWord + msg;
        send(socketFD, msg.c_str(), msg.length(), 0);
        if(iRet < 0) {
            cerr << "ERROR sending to socket" << endl;
            throw - 1;
        }
        if(msg == "exit") {
            cout << "\nDisconnected" << endl;
            close_socket(socketFD);
            close_socket(serverSocketFD);
            exit(EXIT_SUCCESS);
        }
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}