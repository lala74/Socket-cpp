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
#include <thread>

#include "CSocket.h"
#include "utils.h"

using namespace std;

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
        thread read_socket  (read_event,  "server", client.getSocketFD());
        thread write_socket (write_event, "client", client.getSocketFD());

        read_socket.join();
        write_socket.join();

    } catch (int e) {
        cout << "Error code " << e << endl;
    }
    return 0;
}