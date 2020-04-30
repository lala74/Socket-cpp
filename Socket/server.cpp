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
#include "utils.h"

using namespace std;

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
        thread read_socket  (read_event,  "client", socketClient->getSocketFD(), server.getSocketFD());
        thread write_socket (write_event, "server", socketClient->getSocketFD(), server.getSocketFD());

        read_socket.join();
        write_socket.join();

    } catch (int e) {
        cout << "Error code " << e << endl;
    }
    return 0;
}