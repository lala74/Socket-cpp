# Socket-cpp

Class socket in C++

## Compile

```bash
cd Socket/
make all
```

## Execute

```bash
# In terminal 1
./server <port>
# In terminal 2
./client <hostname|IP> <port>
# Start chatting
```

Example

```bash
./server 8080
./client localhost 8080
```

We use `localhost` or `127.0.0.1` to chat between 2 terminals in the same laptop.

If we want to chat between 2 laptop, 2 laptop have to connect to the same wifi network and replace `<hostname> = computer_name | IP`
