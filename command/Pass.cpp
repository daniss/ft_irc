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
#include "../client.hpp"
#include "../channel.hpp"

void pass_execute(std::vector<std::string> &params, int client_fd, std::map<int, Client> &clients, std::string &password)
{
    if (clients[client_fd].is_authenticated) {
        const char *response = ":monserver 462 * :You may not reregister\r\n";
        send(client_fd, response, strlen(response), 0);
        return;
    }
    if (params.empty())
    {
        const char *response = ":monserver 461 * PASS :Not enough parameters\r\n";
        send(client_fd, response, strlen(response), 0);
        return;
    }
    params[0].erase(params[0].find_last_not_of(" \n\r\t") + 1);
    if (!params.empty() && params[0] == password)
    {
        clients[client_fd].is_authenticated = true;
    }
    else
    {
        std::cout << "params[0] = " << params[0] << " password = " << password << std::endl;
        const char *response = ":monserver 464 * :Password incorrect\r\n";
        send(client_fd, response, strlen(response), 0);
        close(client_fd);
        clients.erase(client_fd);
    }
}