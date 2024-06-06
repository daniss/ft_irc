#include <vector>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include "command.hpp"


void ping_execute(std::vector<std::string> &params, int client_fd, std::string &message)
{
    if (params.size() < 1)
    {
        const char *response = ":monserver 409 * :No origin specified\r\n";
        send(client_fd, response, strlen(response), 0);
        return;
    }
    
    std::string ping_data = message.substr(5);
    std::string pong_response = "PONG :" + ping_data + "\r\n";
    send(client_fd, pong_response.c_str(), pong_response.length(), 0);
    return;
}
