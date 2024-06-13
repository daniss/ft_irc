#ifndef COMMAND_HPP
#define COMMAND_HPP

# include "../server.hpp"

void ping_execute(std::vector<std::string> &params, int client_fd, std::string &message);
void join_execute(int client_fd, std::vector<std::string> &params, std::map<std::string, Channel> &channels, std::map<int, Client> &clients);
void execute_topic(int client_fd, std::vector<std::string> &params, std::map<int, Client> &clients, std::map<std::string, Channel> &channels);
int is_in_channel(const std::string &channel, std::map<int, Client> &clients, int client_fd);
void broadcast_message_to_channel(const std::string &message, const std::string &channel, std::map<int, Client> &clients);
std::string whois_operator(std::map<int, Client> &clients, std::string &channel, int client_fd, std::map<std::string, Channel> &channels);
void pass_execute(std::vector<std::string> &params, int client_fd, std::map<int, Client> &clients, std::string &password);
void nick_execute(std::vector<std::string> &params, int client_fd, std::map<int, Client> &clients, std::map<std::string, Channel> &channels);
int check_if_already_exists(std::string &username, std::map<int, Client> &clients);
void user_execute(std::vector<std::string> &params, int client_fd, std::map<int, Client> &clients);
void handle_privmsg(int client_fd, std::vector<std::string> &params, std::map<int, Client> &clients, std::map<std::string, Channel> &channels);
void broadcast_privmsg_to_channel(const std::string &message, const std::string &channel, int client_fd, std::map<int, Client> &clients);
void part_execute(std::vector<std::string> &params, int client_fd, std::map<int, Client> &clients, std::map<std::string, Channel> &channels);
void execute_kick(std::vector<std::string> &params, int client_fd, std::map<int, Client> &clients, std::map<std::string, Channel> &channels);
void invite_execute(std::vector<std::string> &params, int fd, std::map<int, Client> &clients, std::map<std::string, Channel> &channels);
void handle_mode_command(int client_fd, const std::vector<std::string>& params, std::map<std::string, Channel>& channels, std::map<int, Client>& clients);
int find_fd_username(std::string &username, std::map<int, Client> &clients);



#endif