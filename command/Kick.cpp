#include "command.hpp"

void execute_kick(std::vector<std::string> &params, int client_fd, std::map<int, Client> &clients, std::map<std::string, Channel> &channels)
{
    if (params.size() < 2)
    {
        std::string err_msg = ":monserver 461 " + clients[client_fd].get_username() + " KICK :Not enough parameters\r\n";
        send(client_fd, err_msg.c_str(), err_msg.length(), 0);
        return;
    }
    if (channels.find(params[0]) == channels.end())
    {
        std::string err_msg = ":monserver 403 " + clients[client_fd].get_username() + " KICK " + params[0] + " :No such channel\r\n";
        send(client_fd, err_msg.c_str(), err_msg.length(), 0);
        return;
    }
    if (is_in_channel(params[0], clients, client_fd) == false)
    {
        std::string err_msg = ":monserver 442 " + clients[client_fd].get_username() + " KICK " + params[0] + " :You're not on that channel\r\n";
        send(client_fd, err_msg.c_str(), err_msg.length(), 0);
        return;
    }
    size_t fd_tokick = find_fd_username(params[1], clients);
    if (clients.find(fd_tokick) != clients.end() && is_in_channel(params[1], clients, fd_tokick == false))
    {
        std::string err_msg = ":monserver 441 " + clients[client_fd].get_username() + " KICK " + params[0] + " " + params[1] + " :They aren't on that channel\r\n";
        send(client_fd, err_msg.c_str(), err_msg.length(), 0);
        return;
    }
    if (whois_operator(clients, params[0], client_fd, channels).compare(clients[client_fd].get_username()) != 0)
    {
        std::string err_msg = ":monserver 482 " + clients[client_fd].get_username() + " KICK :You're not channel operator\r\n";
        send(client_fd, err_msg.c_str(), err_msg.length(), 0);
        return;
    }
    std::string channel = params[0];
    std::string target = params[1];
    std::string kick_msg = ":" + clients[client_fd].get_username() + "!" + clients[client_fd].get_realname() + " KICK " + channel + " " + target + "\r\n";
    broadcast_message_to_channel(kick_msg, channel, client_fd, clients);
}