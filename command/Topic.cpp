#include "command.hpp"

void broadcast_message_topic_to_channel(const std::string &message, const std::string &channel, std::map<int, Client> &clients)
{
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (is_in_channel(channel, clients, it->first))
        {
                send(it->first, message.c_str(), message.length(), 0);
        }
    }
}

int is_in_channel(const std::string &channel, std::map<int, Client> &clients, int client_fd)
{
    std::deque<std::string> channels = clients[client_fd].get_channels();
    for (std::deque<std::string>::iterator it = channels.begin(); it != channels.end(); ++it)
    {
        if (it->compare(channel) == 0)
        {
            return 1;
        }
    }
    return 0;
}

void broadcast_message_to_channel(const std::string &message, const std::string &channel, int client_fd, std::map<int, Client> &clients)
{
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (is_in_channel(channel, clients, it->first))
        {
            if (it->first != client_fd)
                send(it->first, message.c_str(), message.length(), 0);
        }
    }
}

std::string whois_operator(std::map<int, Client> &clients, std::string &channel, int client_fd, std::map<std::string, Channel> &channels)
{
    std::string response = "";
    if (clients[client_fd].get_channels().empty())
    {
        std::cout << "empty channels" << std::endl;
        return response;
    }

    std::vector<std::string> operators = channels[channel].getOperators();
    for (std::vector<std::string>::iterator it = operators.begin(); it != operators.end(); ++it)
    {
        if (it->compare(clients[client_fd].get_username()) == 0)
        {
            response = clients[client_fd].get_username();
            break;
        }
    }
    return response;
}


void execute_topic(int client_fd, std::vector<std::string> &params, std::map<int, Client> &clients, std::map<std::string, Channel> &channels)
{
        if (params.size() < 1)
        {
            std::cout << "not enough params" << std::endl;
            std::string err_msg = ":monserver 461 " + clients[client_fd].get_username() + " TOPIC :Not enough parameters\r\n";
            send(client_fd, err_msg.c_str(), err_msg.length(), 0);
            return;
        }
        // parcours map to see if channel exists
        int exist = 0;
        for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it)
        {
            if (it->first.compare(params[0]) == 0)
            {
                exist = 1;
                break;
            }
        }
        if (exist == 0)
        {
            std::string err_msg = ":monserver 403 " + clients[client_fd].get_username() + " TOPIC :No such channel\r\n";
            send(client_fd, err_msg.c_str(), err_msg.length(), 0);
            return;
        }

        std::string channel = params[0];
        if (!is_in_channel(channel, clients, client_fd))
        {
            std::string err_msg = ":monserver 442 " + clients[client_fd].get_username() + " " + channel + " :You're not on that channel\r\n";
            send(client_fd, err_msg.c_str(), err_msg.length(), 0);
            return;
        }

        if (params.size() == 1 && channels[channel].getTopic().empty())
        {
            std::string topic_msg = ":monserver 331 " + clients[client_fd].get_username() + " " + channel + " :No topic is set\r\n";
            send(client_fd, topic_msg.c_str(), topic_msg.length(), 0);
            return;
        }
        if (params.size() == 1 && !channels[channel].getTopic().empty())
        {
            std::string topic_msg = ":monserver 332 " + clients[client_fd].get_username() + " " + channel + " :" + channels[channel].getTopic() + "\r\n";
            send(client_fd, topic_msg.c_str(), topic_msg.length(), 0);
            return;
        }

        if (params.size() > 1)
        {

            if (params[1][0] == ':')
            {
                params[1].erase(0, 1);
            }
            else {
                std::string err_msg = ":monserver 461 " + clients[client_fd].get_username() + " TOPIC :Not enough parameters\r\n";
                send(client_fd, err_msg.c_str(), err_msg.length(), 0);
                return;
            }
            if (channels[channel].getUserModes()["topic_lock"] == 1 && whois_operator(clients, channel, client_fd, channels).compare(clients[client_fd].get_username()) != 0)
            {
                std::string err_msg = ":monserver 482 " + clients[client_fd].get_username() + " TOPIC :You're not channel operator\r\n";
                send(client_fd, err_msg.c_str(), err_msg.length(), 0);
                return;
            }
            if (params[1].empty() || (params[1].length() == 1 && params[1][0] == ':'))
            {
                channels[channel].setTopic("");
                return;
            }
            std::string topic = params[1];
            for (size_t i = 2; i < params.size(); ++i)
            {
                topic += " " + params[i];
            }
            std::string topic_msg = ":" + clients[client_fd].get_username() + "!" + clients[client_fd].get_realname() + " TOPIC " + channel + " :" + topic + "\r\n";
            channels[channel].setTopic(topic);
            broadcast_message_topic_to_channel(topic_msg, channel, clients);
        }
    
}