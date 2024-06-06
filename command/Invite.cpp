#include "command.hpp"

int find_fd_username(std::string &username, std::map<int, Client> &clients)
{
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second.get_username().compare(username) == 0)
        {
            int fd = it->second.get_client_fd();
            return fd;
        }
    }
    return -1;
}


void invite_execute(std::vector<std::string> &params, int fd, std::map<int, Client> &clients, std::map<std::string, Channel> &channels)
{
    if (params.size() < 2)
    {
        std::string err_msg = ":monserver 461 " + clients[fd].get_username() + " INVITE :Not enough parameters\r\n";
        // print param
        std::cout << "params size : " << params.size() << std::endl;
        send(fd, err_msg.c_str(), err_msg.length(), 0);
        return;
    }
    std::string target = params[0];
    std::string channel = params[1];

    if (!is_in_channel(params[1], clients, fd))
    {
        std::string err_msg = ":monserver 442 " + clients[fd].get_username() + " INVITE :You're not on this channel\r\n";
        send(fd, err_msg.c_str(), err_msg.length(), 0);
        return;
    }

    if (whois_operator(clients, params[1], fd, channels).compare(clients[fd].get_username()) != 0)
    {
        std::string err_msg = ":monserver 482 " + clients[fd].get_username() + " INVITE :You're not channel operator\r\n";
        send(fd, err_msg.c_str(), err_msg.length(), 0);
        return;
    }

    if (channels[params[1]].getUserModes()["invite_only"] == false)
    {
        std::string err_msg = ":monserver 482 " + clients[fd].get_username() + " INVITE :Channel is not invite only\r\n";
        send(fd, err_msg.c_str(), err_msg.length(), 0);
        return;
    }

    int player_fd = find_fd_username(target, clients);

    if (player_fd == -1)
    {
        std::string err_msg = ":monserver 401 " + clients[fd].get_username() + " INVITE :No such nick/channel\r\n";
        send(fd, err_msg.c_str(), err_msg.length(), 0);
        return;
    }
 
    if (is_in_channel(params[1], clients, player_fd))
    {
        std::cout << "already in channel" << std::endl;
        std::string err_msg = ":monserver 443 " + clients[fd].get_username() + " INVITE :User is already in channel\r\n";
        send(fd, err_msg.c_str(), err_msg.length(), 0);
        return;
    }

    
    

    int exist = 0;
    for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it)
    {
        if (it->first.compare(channel) == 0)
        {
            exist = 1;
            break;
        }
    }

    if (exist == 0)
    {
        std::string err_msg = ":monserver 403 " + clients[fd].get_username() + " INVITE :No such channel\r\n";
        send(fd, err_msg.c_str(), err_msg.length(), 0);
        return;
    }

    
    int invited = 0;
    //std::string invite_msg = ":" + clients[fd].username + "!" + clients[fd].realname + " INVITE " + target + " " + channel + "\r\n";
    std::string msg = ":monserver 341 " + clients[fd].get_username() + " INVITE " + target + " " + channel + "\r\n";
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        std::cout << "client username : " << target << std::endl;
        if (it->second.get_username().compare(target) == 0)
        {
            it->second.add_invited_channel(channel);
            send(it->first, msg.c_str(), msg.length(), 0);
            std::cout << "invite message sent to " << it->second.get_username() << "for the channel " << channel << " by " << clients[fd].get_username() << std::endl;
            return;
        }
    }
    
}