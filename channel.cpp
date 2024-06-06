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

int Channel::new_message(std::string username, std::string message)
{
    //db.messages.push_back(username + ": " + message + "\n");
    std::cout << "Message " << message << " added to channel " << chan_name << std::endl;
    return 0;
}

