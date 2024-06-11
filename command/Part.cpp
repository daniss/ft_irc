#include "command.hpp"

void part_execute(std::vector<std::string> &params, int client_fd, std::map<int, Client> &clients, std::map<std::string, Channel> &channels)
{
    if (params.size() >= 1)
    {
        std::string channel_name = params[0];

        // Check if the user is registered
        if (!clients[client_fd].get_is_authenticated() || clients[client_fd].get_username().empty())
        {
            std::string err_msg = ":monserver 451 * :You have not registered\r\n";
            send(client_fd, err_msg.c_str(), err_msg.length(), 0);
            return;
        }

        // Check if the user is part of the channel
        if (is_in_channel(channel_name, clients, client_fd))
        {
            // Create the PART message
            std::string part_msg = ":" + clients[client_fd].get_username() + "!" + clients[client_fd].get_username() + "@localhost PART :" + channel_name + "\r\n";
            
            // Broadcast the PART message to other clients in the channel
            broadcast_message_to_channel(part_msg, channel_name, client_fd, clients);


            // Remove the client from the channel
            clients[client_fd].erase_channel(channel_name);
            
            // remove operator status
            channels[channel_name].eraseOperator(clients[client_fd].get_username());
            channels[channel_name].remove_client(clients[client_fd].get_username());

            // Check if the channel is empty by iterating through all clients and checking if they are in the channel
            if (channels[channel_name].getUsers().size() == 0)
            {
                std::cout << "Channel " << channel_name << " is empty, deleting it" << std::endl;
                channels.erase(channel_name);
            }
        }
        else
        {
            // Send an error message if the user is not in the channel
            std::string err_msg = ":monserver 442 " + clients[client_fd].get_username() + " " + channel_name + " :You're not on that channel\r\n";
            send(client_fd, err_msg.c_str(), err_msg.length(), 0);
        }
    }
    else
    {
        // Send an error message if no channel is specified
        std::string err_msg = ":monserver 461 " + clients[client_fd].get_username() + " PART :Not enough parameters\r\n";
        send(client_fd, err_msg.c_str(), err_msg.length(), 0);
    }
}