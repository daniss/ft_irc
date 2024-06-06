#ifndef DB_HPP
#define DB_HPP

#include <iostream>
#include <map>
#include "client.hpp" // Include the header file for the Client class

class DB {
public:
    std::deque<std::string> messages;
};

#endif