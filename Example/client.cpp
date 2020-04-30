#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <iostream>
#include <signal.h>
#include <thread>
#include <termios.h>

#define PORT 8080 

using namespace std;

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
            printf("server: %s\n",buffer );
        memset(buffer,0,1024);
    }
}

void write_event(int socket, char* message)
{
    char *strchar;
    while(1)
    {
        strchar[0] = check_press_key();
        cout<<"client: "<<strchar[0];
        scanf("%[^\n]%*c",message);
        strcat(strchar,message);
        send(socket , strchar , strlen(strchar), 0 );
        memset(strchar,0,strlen(strchar));
    }
}

int main(int argc, char const *argv[]) 
{ 
    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 
    char message[1024] = {0}; 
    char buffer[1024] = {0};
    signal(SIGINT, signal_close);
 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    }

    thread read_socket (read_event, sock, buffer);
    thread write_socket (write_event, sock, message);
/*
    while(1)
    {
        cout<<"client: ";
        scanf("%[^\n]%*c",message);
        //cin>>message;
        send(sock , message , strlen(message) , 0 ); 
        valread = read( sock , buffer, 1024); 
        printf("server: %s\n",buffer);
        memset(buffer,0,1024);
    }*/
    read_socket.join();
    write_socket.join();
    return 0; 
} 