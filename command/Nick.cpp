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
#include "command.hpp"

int check_if_already_exists(std::string &username, std::map<int, Client> &clients)
{
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second.get_username().compare(username) == 0) {
            return 1;
        }
    }
    return 0;
}


void nick_execute(std::vector<std::string> &params, int client_fd, std::map<int, Client> &clients, std::map<std::string, Channel> &channels)
{
    if (params.empty())
    {
        const char *response = ":monserver 431 * :No nickname given\r\n";
        send(client_fd, response, strlen(response), 0);
        return;
    }

    if (!check_if_already_exists(params[0], clients))
    {
        std::string nickname = params[0];
        if (nickname[0] == '#' || nickname[0] == ':' || nickname.find(' ') != std::string::npos)
        {
            const char *response = ":monserver 432 * :Erroneous nickname\r\n";
            send(client_fd, response, strlen(response), 0);
            return;
        }
        if (!clients[client_fd].get_username().empty()) {
            std::string nick_change_msg = ":" + clients[client_fd].get_username() + "!new_nick@" + "monserver" + " NICK :" + params[0] + "\r\n";
            send(client_fd, nick_change_msg.c_str(), nick_change_msg.length(), 0);
        }
        // parcours map chanel et change le nom de l'opérateur et change vector user
        for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it)
        {

            std::vector<std::string> users = it->second.getUsers();
            for (std::vector<std::string>::iterator it2 = users.begin(); it2 != users.end(); ++it2)
            {
                if (it2->compare(clients[client_fd].get_username()) == 0)
                {
                    //*it2 = params[0];
                    it->second.changeUser(clients[client_fd].get_username(), params[0]);
                    std::string chan = it->first;
                    if (whois_operator(clients, chan, client_fd, channels).compare(clients[client_fd].get_username()) == 0)
                    {
                        it->second.changeOperator(clients[client_fd].get_username(), params[0]);
                    }
                }
            }
        }
        clients[client_fd].set_username(params[0]);

        
    } 
    else
    {
        const char *response = ":monserver 433 * :Nickname is already in use\r\n";
        send(client_fd, response, strlen(response), 0);
    }
}



