#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <sys/socket.h>
#include <vector>
#include <map>

class Channel {
private:
    std::vector<std::string> operators;
    std::string chan_name;
    std::string key;
    std::string topic;
    std::vector<std::string> users;
    std::map<std::string, bool> user_modes;
    size_t user_limit;

public:
    Channel();
    ~Channel();

    // Getters
    std::vector<std::string> getOperators() const;
    std::string getChannelName() const;
    std::string getKey() const;
    std::string getTopic() const;
    std::vector<std::string> getUsers() const;
    std::map<std::string, bool> getUserModes() const;
    size_t getUserLimit() const;

    // Setters
    void addOperator(const std::string& op);
    void eraseOperator(const std::string& op);
    void changeOperator(const std::string& old_op, const std::string& new_op);
    void changeUser(const std::string& old_user, const std::string& new_user);
    
    void setChannelName(const std::string& name);
    void setKey(const std::string& key);
    void setTopic(const std::string& topic);
    void addUser(const std::string& name);
    void addUsermode(const std::string& mode, bool value);
    void setUserLimit(size_t limit);
    void remove_client(const std::string& name);

};

    
#endif