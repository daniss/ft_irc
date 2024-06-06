#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <deque>

class Client {
public:
    Client();
    std::string username;
    bool is_authenticated;
    std::deque<std::string> invited_channels;
    std::deque<std::string> channels;
    bool is_registered;
    void set_username(const std::string &username);
    std::string realname;
    void set_realname(const std::string &realname);
    int client_fd;
    void join_channel(const std::string &channel); 
    std::string recv_buffer;
};

#endif