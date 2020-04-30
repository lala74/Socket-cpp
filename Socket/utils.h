#ifndef ___UTILS_H__
#define ___UTILS_H__

#include <netinet/in.h>
#include <iostream>
#include <string>

in_addr_t getAddr(char *host);
void read_event(std::string name, int socketFD);
void write_event(int socketFD);

#endif