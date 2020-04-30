#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h>
#include <iostream>
#include <signal.h>
#include <thread>
#include <termios.h>
#define PORT 8080 
using namespace std;
bool close_socket = true;

char getch(void)
{
    char buf = 0;
    struct termios old = {0};
    fflush(stdout);
    if(tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if(tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if(read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if(tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return buf;
}

char check_press_key()
{
    char chk;
    chk=getch();
    return chk;
}

static void signal_close(int signal)
{
    printf("Disconnected\n");
    exit(EXIT_FAILURE);
}

void read_event(int socket, char* buffer)
{
    while(1)
    {
        read(socket , buffer, 1024);
        if(buffer[0] != '\0')
            printf("client: %s\n",buffer );
        memset(buffer,0,1024);   
    }
}

void write_event(int socket, char* message)
{
    char *strchar;
    while(1)
    {
        strchar[0] = check_press_key();
        cout<<"server: "<<strchar[0];
        scanf("%[^\n]%*c",message);
        strcat(strchar,message);
        send(socket , strchar, strlen(strchar) , 0);
        memset(strchar,0,strlen(strchar));
    }
}

int main(int argc, char const *argv[]) 
{ 
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0}; 
    char message[1024] = {0};
    signal(SIGINT, signal_close);
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
       
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                       (socklen_t*)&addrlen))<0) 
    { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    }
    bool close_socket = false;
    thread read_socket (read_event, new_socket, buffer);
    thread write_socket (write_event, new_socket, message);
    read_socket.join();
    write_socket.join();
    return 0; 
}