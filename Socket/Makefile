# Compile under Linux desktop
CC = gcc
CXX = g++
INC = ./
INC_PARAMS=$(INC:%=-I%) 
# INC_PARAMS=$(INC:%=-I%) $(shell pkg-config --cflags glib-2.0 gio-unix-2.0)
CXXFLAGS += -c -Wall -std=c++11 -O2
CFLAGS += -c -Wall -O2
LDLIBS = -pthread
# LDLIBS = -pthread $(shell pkg-config --libs glib-2.0 gio-unix-2.0)


BASE_OBJECTS 	=			CSocket.o \

TARGET_SERVER 	=			server
TARGET_CLIENT 	=			client

.PHONY: all all_
all_: $(TARGET_SERVER) $(TARGET_CLIENT); $(info Compiling with ${CXX})

all:
	make -j $(shell grep -c "^processor" /proc/cpuinfo) all_

$(TARGET_SERVER): $(TARGET_SERVER).o $(BASE_OBJECTS)
	$(CXX) $^ -Llib -o $(TARGET_SERVER) $(LDLIBS)

$(TARGET_CLIENT): $(TARGET_CLIENT).o $(BASE_OBJECTS)
	$(CXX) $^ -Llib -o $(TARGET_CLIENT) $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INC_PARAMS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(INC_PARAMS) $< -o $@

.PHONY: clean
clean:
	rm -f server.o server
	rm -f client.o client
	rm -f $(BASE_OBJECTS)

run: all;
	./server 8080