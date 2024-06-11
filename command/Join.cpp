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
        if (is_in_channel(channel, clients, it->first) == true)
            send(it->first, message.c_str(), message.length(), 0);
    }
}

void join_execute(int client_fd, std::vector<std::string> &params, std::map<std::string, Channel> &channels, std::map<int, Client> &clients)
{
    int i = 0;
    std::cout << clients[client_fd].get_username() << " is joining the channel " << params[0] << std::endl;
    if (params[0][0] != '#')
    {
        std::string err_msg = ":monserver 403 " + clients[client_fd].get_username() + " JOIN " + params[0] + " :No such channel\r\n";
        send(client_fd, err_msg.c_str(), err_msg.length(), 0);
        return;
    }
    if (params.size() >= 1 && params.size() <= 2)
    {
        std::string channel_name = params[0];

        int is_invited = 0;

        if (clients[client_fd].get_channels().size() == 10)
        {
            std::string response = ":monserver 405 " + clients[client_fd].get_username() + " " + channel_name + " :You're already on 10 channels\r\n";
            send(client_fd, response.c_str(), response.length(), 0);
            return;
        } 

        
        if (channels.find(channel_name) != channels.end() && channels[channel_name].getUserModes()["invite_only"] == true)
        {
            std::deque<std::string> invited_channels = clients[client_fd].get_invited_channels();
            for (std::deque<std::string>::iterator it = invited_channels.begin(); it != invited_channels.end(); ++it)
            {
                if (it->compare(channel_name) == 0)
                {
                    is_invited = 1;
                    break;
                }
            }
            if (is_invited == 0)
            {
                std::string response = ":monserver 473 " + clients[client_fd].get_username() + " " + channel_name + " :You're not invited to this channel\r\n";
                send(client_fd, response.c_str(), response.length(), 0);
                return;
            }
        }
        
        // Check if the channel exists, if not, create it

        if (channels.find(channel_name) == channels.end())
        {
            Channel cha;
            cha.setChannelName(channel_name);
            cha.addOperator(clients[client_fd].get_username());
            if (params.size() == 2)
            {
                std::cout << "password : " << params[1] << std::endl;
                cha.setKey(params[1]);
            }
            else {
                cha.setKey("");
            }
            channels[channel_name] = cha;
            std::cout << "Channel " << channel_name << " created by " << clients[client_fd].get_username() << std::endl;
        }
        if ((channels[channel_name].getKey().empty() == false && params.size() < 2) || (params.size() == 2 && channels[channel_name].getKey().compare(params[1]) != 0))
        {
            std::string response = ":monserver 475 " + clients[client_fd].get_username() + " " + channel_name + " :Wrong channel key\r\n";
            send(client_fd, response.c_str(), response.length(), 0);
            return;
        }
        if (clients[client_fd].get_channels().size() == 10)
        {
            std::string response = ":monserver 405 " + clients[client_fd].get_username() + " " + channel_name + " :You're already on 10 channels\r\n";
            send(client_fd, response.c_str(), response.length(), 0);
            return;
        }

        if (channels[channel_name].getUserLimit() != 0 && channels[channel_name].getUsers().size() >= channels[channel_name].getUserLimit())
        {
            std::string response = ":monserver 471 " + clients[client_fd].get_username() + " " + channel_name + " :Channel is full\r\n";
            send(client_fd, response.c_str(), response.length(), 0);
            return;
        }

        if (is_in_channel(channel_name, clients, client_fd) == true)
        {
            std::string response = ":monserver 443 " + clients[client_fd].get_username() + " " + channel_name + " :You're already in this channel\r\n";
            send(client_fd, response.c_str(), response.length(), 0);
            return;
        }
        clients[client_fd].join_channel(channel_name);

        std::cout << "Client " << clients[client_fd].get_username() << " joined channel " << channel_name << std::endl;
        // Broadcast the JOIN message to other clients in the channel
        std::string join_msg = ":" + clients[client_fd].get_username() + "!" + clients[client_fd].get_realname() + " JOIN :" + channel_name + "\r\n";
        channels[channel_name].addUser(clients[client_fd].get_username());
        std::cout << join_msg << std::endl;
        broadcast_message_join_to_channel(join_msg, channel_name, client_fd, clients);
        
        if (channels[channel_name].getTopic().empty() == false)
        {
            std::string topic_msg = ":monserver 332 " + clients[client_fd].get_username() + " " + channel_name + " :" + channels[channel_name].getTopic() + "\r\n";
            send(client_fd, topic_msg.c_str(), topic_msg.length(), 0);
        }
        if (channels[channel_name].getUsers().size() == 1)
        {
            std::vector<std::string> mode_params;
            mode_params.push_back(channel_name);
            mode_params.push_back("+o");
            mode_params.push_back(clients[client_fd].get_username());

            handle_mode_command(client_fd, mode_params, channels, clients);
        }
        // Send the list of users in the channel
        std::string response = ":monserver 353 " + clients[client_fd].get_username() + " = " + channel_name + " :";
        std::vector<std::string> users = channels[channel_name].getUsers();
        for (std::vector<std::string>::iterator it = users.begin(); it != users.end(); ++it)
        {
            if (whois_operator(clients, channel_name, find_fd_username(*it, clients), channels).compare(*it) == 0)
            {
                response += "@" + *it + " ";
                std::cout << "Operator : " << *it << std::endl;
            }
            else
            {
                std::cout << "User : " << *it << std::endl;
                response += *it + " ";
            }
        }
        response += "\r\n";
        send(client_fd, response.c_str(), response.length(), 0);
        response = ":monserver 366 " + clients[client_fd].get_username() + " " + channel_name + " :End of /NAMES list\r\n";
        send(client_fd, response.c_str(), response.length(), 0);
    }

    else {
        std::string err_msg = ":monserver 461 " + clients[client_fd].get_username() + " JOIN :Not enough parameters\r\n";
        send(client_fd, err_msg.c_str(), err_msg.length(), 0);
    }
}
