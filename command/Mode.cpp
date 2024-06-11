#include "command.hpp"

void handle_mode_command(int client_fd, const std::vector<std::string>& params, std::map<std::string, Channel>& channels, std::map<int, Client>& clients) {
    if (params.size() < 2) {
        // Not enough parameters
        std::string response = ":monserver 461 " + clients[client_fd].get_username() + " MODE :Not enough parameters\r\n";
        send(client_fd, response.c_str(), response.length(), 0);
        return;
    }

    int exists = 0;
    for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
        if (it->first == params[0]) {
            exists = 1;
            break;
        }
    }

    if (!exists) {
        // No such nick/channel
        std::string response = ":monserver 401 " + clients[client_fd].get_username() + " " + params[0] + " :No such channel\r\n";
        send(client_fd, response.c_str(), response.length(), 0);
        return;
    }

    

    std::string channel_name = params[0];
    std::string mode_changes = params[1];
    std::string mode_param = (params.size() > 2) ? params[2] : "";

    if (whois_operator(clients, channel_name, client_fd, channels).compare(clients[client_fd].get_username()) != 0 && channels[channel_name].getUsers().size() != 1) {
        // You're not an operator
        std::string response = ":monserver 482 " + clients[client_fd].get_username() + " " + params[0] + " :You're not a channel operator\r\n";
        send(client_fd, response.c_str(), response.length(), 0);
        return;
    }

    Channel& channel = channels[channel_name];
    bool adding_mode = (mode_changes[0] == '+');
    std::string response_mode;
    for (size_t i = 1; i < mode_changes.length(); ++i) {
        char mode = mode_changes[i];
        switch (mode) {
            case 'i':
                if (params.size() != 2) {
                    // Not enough parameters
                    std::string response = ":monserver 461 " + clients[client_fd].get_username() + " MODE :Not enough parameters\r\n";
                    send(client_fd, response.c_str(), response.length(), 0);
                    return;
                }
                if (adding_mode) {
                    channel.addUsermode("invite_only", adding_mode);
                    response_mode = ":monserver 324 " + clients[client_fd].get_username() + " " + channel_name + " +i\r\n";
                    send(client_fd, response_mode.c_str(), response_mode.length(), 0);
                } else {
                    channel.addUsermode("invite_only", adding_mode);
                    response_mode = ":monserver 324 " + clients[client_fd].get_username() + " " + channel_name + " -i\r\n";
                    send(client_fd, response_mode.c_str(), response_mode.length(), 0);
                }
                break;
            case 't':
                if (params.size() != 2) {
                    // Not enough parameters
                    std::string response = ":monserver 461 " + clients[client_fd].get_username() + " MODE :Not enough parameters\r\n";
                    send(client_fd, response.c_str(), response.length(), 0);
                    return;
                }
                if (adding_mode) {
                    channel.addUsermode("topic_lock", adding_mode);
                    response_mode = ":monserver 324 " + clients[client_fd].get_username() + " " + channel_name + " +t\r\n";
                    send(client_fd, response_mode.c_str(), response_mode.length(), 0);
                } else {
                    channel.addUsermode("topic_lock", adding_mode);
                    response_mode = ":monserver 324 " + clients[client_fd].get_username() + " " + channel_name + " -t\r\n";
                    send(client_fd, response_mode.c_str(), response_mode.length(), 0);
                }
                break;
            case 'k':
                if (params.size() < 3) {
                    // Not enough parameters
                    std::string response = ":monserver 461 " + clients[client_fd].get_username() + " MODE :Not enough parameters\r\n";
                    send(client_fd, response.c_str(), response.length(), 0);
                    return;
                }
                if (adding_mode) {
                    channel.setKey(mode_param);
                } else {
                    channel.setKey("");
                }
                break;
            case 'o':
                if (params.size() < 3) {
                    // Not enough parameters
                    std::string response = ":monserver 461 " + clients[client_fd].get_username() + " MODE :Not enough parameters\r\n";
                    send(client_fd, response.c_str(), response.length(), 0);
                    return;
                }
                if (adding_mode) {
                    std::cout << "Adding operator" << std::endl;
                    channel.addOperator(mode_param);
                    std::string response = ":" + clients[client_fd].get_username() + "!" + clients[client_fd].get_realname() + "@localhost" + " MODE " + channel_name + " +o " + params[2] + "\r\n";
                    broadcast_message_to_channel(response, channel_name, client_fd, clients);
                } else {
                    channel.eraseOperator(mode_param);
                    std::string response = ":" + clients[client_fd].get_username() + "!" + clients[client_fd].get_realname() + "@localhost" + " MODE " + channel_name + " -o " + params[2] + "\r\n";
                    broadcast_message_to_channel(response, channel_name, client_fd, clients);
                    std::cout << response << std::endl;
                }
                break;
            case 'l':
                if (adding_mode) {
                    channel.setUserLimit(std::atoi(mode_param.c_str()));
                } else {
                    channel.setUserLimit(0);
                }
                break;
            default:
                // Unknown mode
                response_mode = ":monserver 472 " + clients[client_fd].get_username() + " " + mode + " :is unknown mode char to me\r\n";
                send(client_fd, response_mode.c_str(), response_mode.length(), 0);
                break;
        }
    }

}