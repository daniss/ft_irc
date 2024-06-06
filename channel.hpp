#ifndef CHANNEL_HPP
#define CHANNEL_HPP


#include <iostream>
//#include "db.hpp"
#include <sys/socket.h> // Include the header file that declares the 'send' function
#include <vector>
#include <map>

class Channel {
public:
    Channel(std::string &name, std::string &key, std::string &topic);
    Channel();
    int new_message(std::string username, std::string message);
    std::vector<std::string> operators;
    std::string chan_name;
    std::string key;
    std::string topic;
    std::vector<std::string> users;
    std::map<std::string, bool> user_modes;
    size_t user_limit;
};

#endif