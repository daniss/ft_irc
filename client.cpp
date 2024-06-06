#include "client.hpp"

Client::Client() : is_authenticated(false), is_registered(false)
{
}

void Client::join_channel(const std::string &channel)
{
    channels.push_back(channel);
    // erase invitation after joining
    if (!invited_channels.empty())
        invited_channels.pop_back();
}

void Client::set_username(const std::string &username)
{
    this->username = username;
}

void Client::set_realname(const std::string &realname)
{
    this->realname = realname;
}