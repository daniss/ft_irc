#include "server.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <signal.h>

bool	interrupted = false;

void	interupte(int sig)
{
    interrupted = true;
}

Server::Server(int port, const std::string& password) : port(port), password(password) {
    create_server();

    std::vector<pollfd> fds;
    pollfd server_pollfd;
    server_pollfd.fd = this->server_fd;
    server_pollfd.events = POLLIN;
    server_pollfd.revents = 0;
    fds.push_back(server_pollfd);
    signal(SIGINT, interupte);

    while (!interrupted) {
        int poll_count = poll(&fds[0], fds.size(), 0);
        if (poll_count < 0) {
            if (interrupted)
            {
                std::cout << "Server Stopped by CTRL+C" << std::endl;
                break;
            }
            std::cerr << "Poll Failed" << std::endl;
            break;
        }

        for (size_t i = 0; i < fds.size(); ++i) {
            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == this->server_fd)
                {
                    int new_socket = accept(this->server_fd, (struct sockaddr *)&this->address, &this->addrlen);
                    if (new_socket < 0)
                    {
                        std::cerr << "Accept Failed" << std::endl;
                    }
                    else
                    {
                        std::cout << "Connection Accepted" << std::endl;
                        pollfd client_pollfd;
                        client_pollfd.fd = new_socket;
                        client_pollfd.events = POLLIN;
                        client_pollfd.revents = 0;
                        fds.push_back(client_pollfd);
                        clients[new_socket] = Client(); // Initialize client
                        clients[new_socket].set_client_fd(new_socket);
                        
                        struct sockaddr_in addr;
                        socklen_t addr_len = sizeof(addr);
                        // getpeername(new_socket, (struct sockaddr*)&addr, &addr_len);
                        // clients[new_socket].set_hostname(inet_ntoa(addr.sin_addr));
                    }
                }
                else
                {
                    char buffer[1024];
                    int stoped = 0;
                    std::string received_data;
                    while (true) {
                        int valread = recv(fds[i].fd, buffer, 1024, 0);
                        if (valread <= 0)
                        {
                            if (valread == 0) {
                                std::cout << "Client disconnected" << std::endl;
                            }
                            else
                            {
                                std::cerr << "Read Failed" << std::endl;
                            }
                            close(fds[i].fd);
                            clients.erase(fds[i].fd); // Remove client from map
                            fds.erase(fds.begin() + i);
                            --i;
                            stoped = 1;
                            break;
                        } else {
                            buffer[valread] = '\0';
                            received_data.append(buffer, valread);
                            if (strlen(buffer) < 1024) {
                                break;
                            }
                            
                        }
                    }
                    if (stoped == 1) {
                        continue;
                    }
                    handle_client_message(fds[i].fd, received_data);
                    }
                }
            }
        
    }

    for (std::vector<pollfd>::iterator it = fds.begin(); it != fds.end(); ++it) {
        close(it->fd);
    }
}
Server::~Server() {
    close(this->server_fd);
}

void Server::accept_connections() {
    this->addrlen = sizeof(this->address);
    while (true) {
        int new_socket = accept(this->server_fd, (struct sockaddr *)&this->address, &this->addrlen);
        if (new_socket < 0) {
            std::cerr << "Accept Failed" << std::endl;
            continue;
        }
        std::cout << "Connection Accepted" << std::endl;
        close(new_socket);  // Close the new socket immediately as we are not handling messages
    }
}

void Server::create_server() {
    this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->server_fd == -1) {
        std::cerr << "Socket Creation Failed" << std::endl;
        return;
    }

    int opt = 1;
    if (setsockopt(this->server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(int)) < 0) {
        std::cerr << "Socket Options Failed" << std::endl;
        close(this->server_fd);
        return;
    }

    this->address.sin_family = AF_INET;
    this->address.sin_addr.s_addr = INADDR_ANY;
    this->address.sin_port = htons(this->port);

    if (bind(this->server_fd, (struct sockaddr *)&this->address, sizeof(this->address)) < 0) {
        std::cerr << "Bind Failed" << std::endl;
        close(this->server_fd);
        return;
    }

    if (listen(this->server_fd, SOMAXCONN) < 0) {
        std::cerr << "Listen Failed" << std::endl;
        close(this->server_fd);
        return;
    }

    std::cout << "Server Started" << std::endl;

    if (fcntl(this->server_fd, F_SETFL, O_NONBLOCK) < 0) {
        std::cerr << "Socket Non-Blocking Failed" << std::endl;
        close(this->server_fd);
        return;
    }

    this->addrlen = sizeof(this->address);
}



void Server::handle_client_message(int client_fd, const std::string& message)
{
    // Append received data to the client's buffer
    clients[client_fd].set_recv_buffer(clients[client_fd].get_recv_buffer() + message);
    std::cout << "Received: " << message << std::endl;
    size_t pos;
    while ((pos = clients[client_fd].get_recv_buffer().find('\n')) != std::string::npos) {
        // Extract complete command from the buffer
        std::string command_line = clients[client_fd].get_recv_buffer().substr(0, pos);
        //[client_fd].recv_buffer.erase(0, pos + 1); // Remove the processed command from the buffer
        clients[client_fd].erase_recv_buffer(pos + 1); // Remove the processed command from the buffer

        // Parse the command and parameters
        std::string command;
        std::vector<std::string> params;
        size_t space_pos = command_line.find(' ');
        if (space_pos != std::string::npos) {
            command = command_line.substr(0, space_pos);
            std::string param_str = command_line.substr(space_pos + 1);
            
            // Split the parameters
            size_t param_pos;
            while ((param_pos = param_str.find(' ')) != std::string::npos) {
                params.push_back(param_str.substr(0, param_pos));
                param_str.erase(0, param_pos + 1);
            }
            if (!param_str.empty()) {
                params.push_back(param_str);
            }
        } else {
            command = command_line;
        }

        // Trim whitespace characters from params
        for (size_t i = 0; i < params.size(); ++i) {
            params[i].erase(params[i].find_last_not_of(" \n\r\t") + 1);
        }

        // Execute the command
        if (!clients[client_fd].get_username().empty() && !clients[client_fd].get_realname().empty() && clients[client_fd].get_is_authenticated()) {
            clients[client_fd].set_is_registered(true);
        }
        std::cout << "command : " << command << std::endl;
        execute_command(command, client_fd, params, command_line);
    }
}


void Server::broadcast_left_message_to_channel(const std::string &message, const std::string &channel, int client_fd)
{
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        std::deque<std::string> channels = it->second.get_channels();
        for (std::deque<std::string>::iterator it2 = channels.begin(); it2 != channels.end(); ++it2)
        {
            if (it2->compare(channel) == 0 && it->first != client_fd)
                send(it->first, message.c_str(), message.length(), 0);
        }
    }
}

void Server::cleanup_user(int client_fd)
{
    std::deque<std::string> channels_client = clients[client_fd].get_channels();
    for (std::deque<std::string>::iterator it = channels_client.begin(); it != channels_client.end(); ++it)
    {
        std::string part_msg = ":" + clients[client_fd].get_username() + "!" + clients[client_fd].get_realname() + " PART :" + *it + "\r\n";
        broadcast_left_message_to_channel(part_msg, *it, client_fd);
        if (channels[*it].getUsers().empty()) 
        {
            channels.erase(*it);
        }
    }
    clients.erase(client_fd);
}




void Server::execute_command(const std::string &command, int client_fd, std::vector<std::string> &params, std::string &message)
{
    std::string command_upper = command;
    command_upper.erase(command_upper.find_last_not_of(" \n\r\t") + 1);
    if (command_upper == "PASS") {
        pass_execute(params, client_fd, this->clients, this->password);
    } else if (command_upper == "NICK") {
        nick_execute(params, client_fd, this->clients, this->channels);
    } else if (command_upper == "USER") {
        user_execute(params, client_fd, this->clients);
    } else if (command_upper == "CAP") {
        std::cout << "CAP command" << std::endl;
    } else if (clients[client_fd].get_is_registered() == false) {
        const char *response = ":monserver 451 * :You have not registered\r\n";
        std::cout << "command not registered : " << command_upper << std::endl;
        send(client_fd, response, strlen(response), 0);
    }
    else if (command_upper == "JOIN") {
        if (params.size() != 1) {
            // Not enough parameters
            const char *response = ":monserver 461 * JOIN :Not enough parameters\r\n";
            send(client_fd, response, strlen(response), 0);
            return;
        }
        std::map<std::string, std::string> channels_to_join;
        size_t pos = 0;
        std::string token;
        std::string delimiter = ",";
        while ((pos = params[0].find(delimiter)) != std::string::npos) {
            token = params[0].substr(0, pos);
            std::string password;
            if (params.size() == 2) {
                password = params[1].substr(0, params[1].find(delimiter));
                if (params[1].find(delimiter) != std::string::npos)
                    params[1].erase(0, params[1].find(delimiter) + delimiter.length());
                else
                    params.erase(params.begin() + 1);
            }
            else {
                password = "";
            }
            channels_to_join[token] = password;
            params[0].erase(0, pos + delimiter.length());
        }
        channels_to_join[params[0]] = params.size() == 2 ? params[1] : "";
        for (std::map<std::string, std::string>::iterator it = channels_to_join.begin(); it != channels_to_join.end(); ++it) {
            std::vector<std::string> true_params;
            true_params.push_back(it->first);
            if (it->second != "") {
                true_params.push_back(it->second);
            }
            join_execute(client_fd, true_params, this->channels, this->clients);
            true_params.clear();
        }

    } else if (command_upper == "PING") {
        ping_execute(params, client_fd, message);
    } else if (command_upper == "QUIT") {
        cleanup_user(client_fd);
        close(client_fd);
        clients.erase(client_fd);
    } else if (command_upper == "PRIVMSG") {
        handle_privmsg(client_fd, command_upper, params, this->clients, this->channels);
    } else if (command_upper == "PART") {
        part_execute(params, client_fd, this->clients, this->channels);
    } else if (command_upper == "KICK") {
        execute_kick(params, client_fd, this->clients, this->channels);
    } else if (command_upper == "INVITE") {
        invite_execute(params, client_fd, this->clients, this->channels);
    } else if (command_upper == "TOPIC") {
        execute_topic(client_fd, params, this->clients, this->channels);
    } else if (command_upper == "MODE") {
        handle_mode_command(client_fd, params, this->channels, this->clients);
    }
}

