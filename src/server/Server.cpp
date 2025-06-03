// src/Server.cpp
#include "Server.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

Server::Server(const std::string& configPath) {
    (void)configPath; // We’ll read real configs later
    initSocket(8080);
}

Server::~Server() {
    for (size_t i = 0; i < _poll_fds.size(); ++i)
        close(_poll_fds[i].fd);
}

void Server::initSocket(int port) {
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd < 0)
        throw std::runtime_error("socket() failed");

    fcntl(_server_fd, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

	int opt = 1;
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    throw std::runtime_error("setsockopt() failed");
    if (bind(_server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("bind() failed");

    if (listen(_server_fd, 10) < 0)
        throw std::runtime_error("listen() failed");

    struct pollfd server_poll;
    server_poll.fd = _server_fd;
    server_poll.events = POLLIN;
    _poll_fds.push_back(server_poll);

    std::cout << "Listening on port " << port << std::endl;
}

void Server::run() {
    while (true) {
        int ready = poll(&_poll_fds[0], _poll_fds.size(), -1);
        if (ready < 0)
            throw std::runtime_error("poll() failed");

        for (size_t i = 0; i < _poll_fds.size(); ++i) {
            if (_poll_fds[i].revents & POLLIN) {
                if (_poll_fds[i].fd == _server_fd) {
                    handleNewConnection();
                } else {
                    handleClientRequest(_poll_fds[i].fd);
                }
            }
        }
    }
}

void Server::handleNewConnection() {
    int client_fd = accept(_server_fd, NULL, NULL);
    if (client_fd < 0)
        return;

    fcntl(client_fd, F_SETFL, O_NONBLOCK);

    struct pollfd pfd;
    pfd.fd = client_fd;
    pfd.events = POLLIN;
    _poll_fds.push_back(pfd);

    std::cout << "New connection: " << client_fd << std::endl;
}
struct PollfdMatcher {
    int fd;
    PollfdMatcher(int target_fd) : fd(target_fd) {}
    bool operator()(const struct pollfd& p) const {
        return p.fd == fd;
    }
};

void Server::handleClientRequest(int fd) {
    char buffer[1024];
    ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes <= 0) {
        std::cout << "Client " << fd << " disconnected." << std::endl;
        close(fd);
        _poll_fds.erase(
            std::remove_if(_poll_fds.begin(), _poll_fds.end(), PollfdMatcher(fd)),
            _poll_fds.end()
        );
        return;
    }

    buffer[bytes] = '\0';
    std::string body = "Hello world";
std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " +
                       std::to_string(body.size()) + "\r\nConnection: close\r\n\r\n" + body;

    send(fd, response.c_str(), response.size(), 0);

    // Fecha o socket para forçar desconexão após responder
    close(fd);
    _poll_fds.erase(
        std::remove_if(_poll_fds.begin(), _poll_fds.end(), PollfdMatcher(fd)),
        _poll_fds.end()
    );

    std::cout << "Client " << fd << " disconnected after response." << std::endl;
}


