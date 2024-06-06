#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <map>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <cstdlib> 
#include <cstring>
#include <arpa/inet.h>
#include <poll.h>
#include <vector>
#include "client.hpp"
#include "channel.hpp"
# include "command/command.hpp"

class Server {
public:
    Server(int port, const std::string& password);
    ~Server();

private:
    void create_server();
    void accept_connections();

    int server_fd;
    int port;
    std::string password;
    struct sockaddr_in address;
    socklen_t addrlen;
    std::map<int, Client> clients;
    std::map<std::string, Channel> channels;
    void handle_client_message(int client_fd, const std::string& message);

};


#endif
