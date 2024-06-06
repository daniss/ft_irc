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
#include "command.hpp"

void broadcast_message_join_to_channel(const std::string &message, const std::string &channel, int client_fd, std::map<int, Client> &clients)
{
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
            send(it->first, message.c_str(), message.length(), 0);
    }
}

void join_execute(int client_fd, std::vector<std::string> &params, std::map<std::string, Channel> &channels, std::map<int, Client> &clients)
{
    int i = 0;
    std::cout << clients[client_fd].username << " is joining the channel " << params[0] << std::endl;
    if (params[0][0] != '#')
    {
        std::string err_msg = ":monserver 403 " + clients[client_fd].username + " JOIN " + params[0] + " :No such channel\r\n";
        send(client_fd, err_msg.c_str(), err_msg.length(), 0);
        return;
    }
    if (params.size() >= 1 && params.size() <= 2)
    {
        std::string channel_name = params[0];

        int is_invited = 0;

        if (clients[client_fd].channels.size() == 10)
        {
            std::string response = ":monserver 405 " + clients[client_fd].username + " " + channel_name + " :You're already on 10 channels\r\n";
            send(client_fd, response.c_str(), response.length(), 0);
            return;
        } 

        
        if (channels.find(channel_name) != channels.end() && channels[channel_name].user_modes["invite_only"] == true)
        {
            for (std::deque<std::string>::iterator it = clients[client_fd].invited_channels.begin(); it != clients[client_fd].invited_channels.end(); ++it)
            {
                if (it->compare(channel_name) == 0)
                {
                    is_invited = 1;
                    break;
                }
            }
            if (is_invited == 0)
            {
                std::string response = ":monserver 473 " + clients[client_fd].username + " " + channel_name + " :You're not invited to this channel\r\n";
                send(client_fd, response.c_str(), response.length(), 0);
                return;
            }
        }
        
        // Check if the channel exists, if not, create it

        if (channels.find(channel_name) == channels.end())
        {
            Channel cha;
            cha.chan_name = channel_name;
            cha.operators.push_back(clients[client_fd].username);
            if (params.size() == 2)
            {
                
                std::cout << "password : " << params[1] << std::endl;
                cha.key = params[1];
            }
            else {
                cha.key = "";
            }
            channels[channel_name] = cha;
            std::cout << "Channel " << channel_name << " created by " << clients[client_fd].username << std::endl;
        }
        if ((channels[channel_name].key.empty() == false && params.size() < 2) || (params.size() == 2 && channels[channel_name].key.compare(params[1]) != 0))
        {
            std::string response = ":monserver 475 " + clients[client_fd].username + " " + channel_name + " :Wrong channel key\r\n";
            send(client_fd, response.c_str(), response.length(), 0);
            return;
        }
        if (clients[client_fd].channels.size() == 10)
        {
            std::string response = ":monserver 405 " + clients[client_fd].username + " " + channel_name + " :You're already on 10 channels\r\n";
            send(client_fd, response.c_str(), response.length(), 0);
            return;
        }

        if (channels[channel_name].user_limit != 0 && channels[channel_name].users.size() >= channels[channel_name].user_limit)
        {
            std::string response = ":monserver 471 " + clients[client_fd].username + " " + channel_name + " :Channel is full\r\n";
            send(client_fd, response.c_str(), response.length(), 0);
            return;
        }

        clients[client_fd].join_channel(channel_name);

        std::cout << "Client " << clients[client_fd].username << " joined channel " << channel_name << std::endl;
        // Broadcast the JOIN message to other clients in the channel
        std::string join_msg = ":" + clients[client_fd].username + "!" + clients[client_fd].realname + " JOIN :" + channel_name + "\r\n";
        std::cout << join_msg << std::endl;
        broadcast_message_join_to_channel(join_msg, channel_name, client_fd, clients);
        execute_topic(client_fd, params, clients, channels);
    }

    else {
        std::string err_msg = ":monserver 461 " + clients[client_fd].username + " JOIN :Not enough parameters\r\n";
        send(client_fd, err_msg.c_str(), err_msg.length(), 0);
    }
}
