// src/Server.hpp
#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <poll.h>

class Server {
public:
    Server(const std::string& configPath);
    ~Server();

    void run();

private:
    int _server_fd;
    std::vector<struct pollfd> _poll_fds;
    std::map<int, std::string> _client_buffers;

    void initSocket(int port);
    void handleNewConnection();
    void handleClientRequest(int fd);
};

#endif
