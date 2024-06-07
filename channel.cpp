#include "channel.hpp"

Channel::Channel(std::string &name, std::string &key, std::string &topic) : chan_name(name), key(key), topic(topic)
{
}


Channel::Channel()
{
    user_modes["topic_lock"] = false;
    user_modes["invite_only"] = false;
    user_limit = 0;
}

Channel::~Channel()
{
}

// Getters
std::vector<std::string> Channel::getOperators() const {
    return operators;
}

std::string Channel::getChannelName() const {
    return chan_name;
}

std::string Channel::getKey() const {
    return key;
}

std::string Channel::getTopic() const {
    return topic;
}

std::vector<std::string> Channel::getUsers() const {
    return users;
}

std::map<std::string, bool> Channel::getUserModes() const {
    return user_modes;
}

size_t Channel::getUserLimit() const {
    return user_limit;
}

// Setters
void Channel::addOperator(const std::string& op) {
    operators.push_back(op);
}

void Channel::eraseOperator(const std::string& op) {
    for (std::vector<std::string>::iterator it = operators.begin(); it != operators.end(); ++it) {
        if (*it == op) {
            operators.erase(it);
            break;
        }
    }
}

void Channel::setChannelName(const std::string& name) {
    chan_name = name;
}

void Channel::setKey(const std::string& key) {
    this->key = key;
}

void Channel::setTopic(const std::string& topic) {
    this->topic = topic;
}

void Channel::addUser(const std::string& name)
{
    users.push_back(name);
}

void Channel::addUsermode(const std::string& mode, bool value) {
    user_modes[mode] = value;
}

void Channel::setUserLimit(size_t limit) {
    user_limit = limit;
}