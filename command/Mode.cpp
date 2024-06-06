#include "command.hpp"

void handle_mode_command(int client_fd, const std::vector<std::string>& params, std::map<std::string, Channel>& channels, std::map<int, Client>& clients) {
    if (params.size() < 2) {
        // Not enough parameters
        std::string response = ":monserver 461 " + clients[client_fd].get_username() + " MODE :Not enough parameters\r\n";
        send(client_fd, response.c_str(), response.length(), 0);
        return;
    }

    std::string channel_name = params[0];
    std::string mode_changes = params[1];
    std::string mode_param = (params.size() > 2) ? params[2] : "";

    Channel& channel = channels[channel_name];
    bool adding_mode = (mode_changes[0] == '+');
    for (size_t i = 1; i < mode_changes.length(); ++i) {
        char mode = mode_changes[i];
        switch (mode) {
            case 'i':
                std::cout << "mode i" << std::endl;
                channel.addUsermode("invite_only", adding_mode);
                break;
            case 't':
                channel.addUsermode("topic_lock", adding_mode);
                break;
            case 'k':
                if (adding_mode) {
                    channel.setKey(mode_param);
                } else {
                    channel.setKey("");
                }
                break;
            case 'o':
                if (adding_mode) {
                    channel.addOperator(mode_param);
                } else {
                    channel.eraseOperator(mode_param);
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
                std::string response = ":monserver 472 " + clients[client_fd].get_username() + " " + mode + " :is unknown mode char to me\r\n";
                send(client_fd, response.c_str(), response.length(), 0);
                break;
        }
    }

}