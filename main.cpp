#include "server.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h> // Add missing include directive
#include <netinet/in.h>
#include <errno.h>


int check_args(int ac, char **av)
{
    if (ac != 3)
    {
        std::cerr << "You Need To Provide 2 Arguments" << std::endl;
        return 1;
    }
    try {
        if (std::atoi(av[1]) < 1024 || std::atoi(av[1]) > 65535)
        {
            std::cerr << "You are Not On the Right Range Of Allowed Port" << std::endl;
            return 1;
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "You Need To Provide a Number" << std::endl;
        return 1;
    }
    return 0;
}

int main(int ac, char **av)
{
    if (check_args(ac, av) == 1)
    {
        return 1;
    }
    std::string password = av[2];
    Server server(std::atoi(av[1]), password);

}

