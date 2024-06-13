
#include "command.hpp"

void user_execute(std::vector<std::string> &params, int client_fd, std::map<int, Client> &clients)
{
    if (clients[client_fd].get_is_registered())
    {
        const char *response = ":monserver 462 * :You may not reregister\r\n";
        send(client_fd, response, strlen(response), 0);
        return;
    }
    if (params.size() >= 4)
    {
        std::string realname = params[3];
        if (realname[0] == ':') {
            realname.erase(0, 1);
        }
        // parcours clients to check if username already exists
        if (clients[client_fd].get_realname().empty())
        {
            
        }
        clients[client_fd].set_realname(realname);
    }
}