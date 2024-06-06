#include "client.hpp"

Client::Client() : is_authenticated(false), is_registered(false)
{
}

Client::~Client()
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

std::string Client::get_username() const
{
    return username;
}

bool Client::get_is_authenticated() const
{
    return is_authenticated;
}

std::deque<std::string> Client::get_invited_channels() const
{
    return invited_channels;
}

std::deque<std::string> Client::get_channels() const
{
    return channels;
}

bool Client::get_is_registered() const
{
    return is_registered;
}

std::string Client::get_realname() const
{
    return realname;
}

int Client::get_client_fd() const
{
    return client_fd;
}

std::string Client::get_recv_buffer() const
{
    return recv_buffer;
}

void Client::set_is_authenticated(bool is_authenticated)
{
    this->is_authenticated = is_authenticated;
}

void Client::add_invited_channel(const std::string &channel)
{
    invited_channels.push_back(channel);
}

void Client::add_channel(const std::string &channel)
{
    channels.push_back(channel);
}

void Client::set_is_registered(bool is_registered)
{
    this->is_registered = is_registered;
}

void Client::set_client_fd(int client_fd)
{
    this->client_fd = client_fd;
}

void Client::set_recv_buffer(const std::string &recv_buffer)
{
    this->recv_buffer = recv_buffer;
}

void Client::erase_recv_buffer(size_t pos)
{
    recv_buffer.erase(0, pos);
}

void Client::erase_channel(const std::string &channel)
{
    for (std::deque<std::string>::iterator it = channels.begin(); it != channels.end(); ++it)
    {
        if (it->compare(channel) == 0)
        {
            channels.erase(it);
            break;
        }
    }
}