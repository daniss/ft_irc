#include "channel.hpp"

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
    std::vector<std::string>::iterator it = operators.begin();
    while (it != operators.end()) {
        if (*it == op) {
            it = operators.erase(it);  // Met à jour l'itérateur après l'effacement
        } else {
            ++it;  // Passe à l'élément suivant
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

void Channel::remove_client(const std::string& name) {
    for (std::vector<std::string>::iterator it = users.begin(); it != users.end(); ++it) {
        if (*it == name) {
            users.erase(it);
            break;
        }
    }
}

void Channel::changeOperator(const std::string& old_op, const std::string& new_op) {
    for (std::vector<std::string>::iterator it = operators.begin(); it != operators.end(); ++it) {
        if (*it == old_op) {
            *it = new_op;
            break;
        }
    }
}

void Channel::changeUser(const std::string& old_user, const std::string& new_user) {
    for (std::vector<std::string>::iterator it = users.begin(); it != users.end(); ++it) {
        if (*it == old_user) {
            *it = new_user;
            break;
        }
    }
}