
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
        if (clients[client_fd].get_username().empty())
        {
            for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
            {
                if (it->second.get_username().compare(params[0]) == 0)
                {
                    const char *response = ":monserver 462 * :You may not reregister\r\n";
                    send(client_fd, response, strlen(response), 0);
                    return;
                }

                if (it->second.get_realname().compare(realname) == 0)
                {
                    const char *response = ":monserver 462 * :You may not reregister\r\n";
                    send(client_fd, response, strlen(response), 0);
                    return;
                }
            }
        }
        clients[client_fd].set_username(params[0]);
        clients[client_fd].set_realname(realname);
        if (clients[client_fd].get_is_authenticated() && !clients[client_fd].get_username().empty()) {
            std::string welcome = ":monserver 001 " + clients[client_fd].get_username() + " :Welcome to the IRC Server\r\n";
            std::cout << "welcome message sent" << std::endl;
            send(client_fd, welcome.c_str(), welcome.length(), 0);
        }
    }
}