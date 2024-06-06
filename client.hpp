#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <deque>

class Client {
private:
    std::string username;
    bool is_authenticated;
    std::deque<std::string> invited_channels;
    std::deque<std::string> channels;
    bool is_registered;
    std::string realname;
    int client_fd;
    std::string recv_buffer;

public:
    Client();
    ~Client();

    // Getters
    std::string get_username() const;
    bool get_is_authenticated() const;
    std::deque<std::string> get_invited_channels() const;
    std::deque<std::string> get_channels() const;
    bool get_is_registered() const;
    std::string get_realname() const;
    int get_client_fd() const;
    std::string get_recv_buffer() const;

    // Setters
    void set_username(const std::string &username);
    void set_is_authenticated(bool is_authenticated);
    void add_invited_channel(const std::string &channel);
    void add_channel(const std::string &channel);
    void set_is_registered(bool is_registered);
    void set_realname(const std::string &realname);
    void set_client_fd(int client_fd);
    void set_recv_buffer(const std::string &recv_buffer);
    void erase_recv_buffer(size_t pos);

    void erase_channel(const std::string &channel);
    void join_channel(const std::string &channel);
};

#endif