#include "command.hpp"

void broadcast_privmsg_to_channel(const std::string &message, const std::string &channel, int client_fd, std::map<int, Client> &clients)
{

    if (!is_in_channel(channel, clients, client_fd)) {
        std::string response = ":monserver 442 " + clients[client_fd].get_username() + " " + channel + " :You're not on that channel\r\n";
        std::cout << response << std::endl;
        send(client_fd, response.c_str(), response.length(), 0);
        return;
    }
    // Broadcast the message to all clients in the channel
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        int other_client_fd = it->first;
        const Client &other_client = it->second;

        if (other_client_fd != client_fd) {
            for (std::deque<std::string>::const_iterator it2 = other_client.get_channels().begin(); it2 != other_client.get_channels().end(); ++it2) {
                if (*it2 == channel) {
                    send(other_client_fd, message.c_str(), message.length(), 0);
                    break;
                }
            }
        }
    }
}

void handle_privmsg(int client_fd, std::string &command, std::vector<std::string> &params, std::map<int, Client> &clients)
{
    std::cout << "handle_privmsg" << std::endl;
    if (params.size() < 2) {
        std::string err_msg = ":monserver 461 " + clients[client_fd].get_username() + " PRIVMSG :Not enough parameters\r\n";
        send(client_fd, err_msg.c_str(), err_msg.length(), 0);
        return;
    }

    if (params[0][0] != '#' || params[1][0] != ':') {
        std::string err_msg = ":monserver 401 " + clients[client_fd].get_username() + " PRIVMSG " + params[0] + " :No such nick/channel\r\n";
        send(client_fd, err_msg.c_str(), err_msg.length(), 0);
        return;
    }
    //erase first lettre from command
    params[1].erase(0, 1);

    std::string target = params[0];

    std::string message = params[1];
    for (size_t i = 2; i < params.size(); ++i) {
        message += " " + params[i];
    }

    std::string full_message = ":" + clients[client_fd].get_username() + "!" + clients[client_fd].get_realname() + " PRIVMSG " + target + " :" + message + "\r\n";

    if (target[0] == '#') {
        broadcast_privmsg_to_channel(full_message, target, client_fd, clients);
    } else {
        // Send a message to the target client
        bool target_found = false;
        for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
            if (it->second.get_username() == target) {
                std::cout << "message : " << full_message << " to : " << it->second.get_username() << std::endl;
                send(it->first, full_message.c_str(), full_message.length(), 0);
                target_found = true;
                break;
            }
        }

        if (!target_found) {
            std::string err_msg = ":monserver 401 " + clients[client_fd].get_username() + " " + target + " :No such nick/channel\r\n";
            send(client_fd, err_msg.c_str(), err_msg.length(), 0);
        }
    }
}