
#include "command.hpp"

void user_execute(std::vector<std::string> &params, int client_fd, std::map<int, Client> &clients)
{
    if (!clients[client_fd].get_realname().empty())
    {
        const char *response = ":monserver 462 * :You may not reregister\r\n";
        send(client_fd, response, strlen(response), 0);
        return;
    }
    if (params.size() ==4)
    {
        std::string realname = params[3];
        if (realname[0] == ':') {
            realname.erase(0, 1);
        }
        if (params[0].empty() || params[1].empty() || params[2].empty() || realname.empty())
        {
            const char *response = ":monserver 461 * USER :Not enough parameters\r\n";
            send(client_fd, response, strlen(response), 0);
            return;
        }
        if (params[1].find_first_not_of("0123456789") != std::string::npos)
        {
            const char *response = ":monserver 461 * USER :Not enough parameters\r\n";
            send(client_fd, response, strlen(response), 0);
            return;
        }
        if (realname.empty())
        {
            const char *response = ":monserver 461 * USER :Not enough parameters\r\n";
            send(client_fd, response, strlen(response), 0);
            return;
        }
        clients[client_fd].set_realname(realname);
    }
    else 
    {
        const char *response = ":monserver 461 * USER :Not enough parameters\r\n";
        send(client_fd, response, strlen(response), 0);
    }
}