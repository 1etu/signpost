#include "network/server.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <cstring>
#include <iostream>

namespace network {

Server::Server(const std::string& host, uint16_t port)
    : host_(host), port_(port) {}

Server::~Server() {
    stop();
}

bool Server::start() {
    listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd_ < 0) {
        std::cerr << "err: create socket\n";
        return false;
    }
    
    int opt = 1;
    setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    
    if (host_ == "0.0.0.0") {
        addr.sin_addr.s_addr = INADDR_ANY;
    } else {
        inet_pton(AF_INET, host_.c_str(), &addr.sin_addr);
    }
    
    if (bind(listen_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "Failed to bind to " << host_ << ":" << port_ << "\n";
        close(listen_fd_);
        return false;
    }
    
    if (listen(listen_fd_, 128) < 0) {
        std::cerr << "Failed to listen\n";
        close(listen_fd_);
        return false;
    }
    
    running_ = true;
    std::cout << "Listening on " << host_ << ":" << port_ << "\n";
    return true;
}

void Server::run(const Client::PacketCallback& on_packet) {
    std::vector<std::unique_ptr<Client>> clients;
    std::vector<pollfd> fds;
    
    while (running_) {
        fds.clear();
        
        fds.push_back({listen_fd_, POLLIN, 0});
        
        for (size_t i = 0; i < clients.size(); i++) {
            if (!clients[i]->is_closed()) {
            }
        }
        
        pollfd listen_pfd = {listen_fd_, POLLIN, 0};
        int ret = poll(&listen_pfd, 1, 10);
        
        if (ret > 0 && (listen_pfd.revents & POLLIN)) {
            sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);
            int client_fd = accept(listen_fd_, reinterpret_cast<sockaddr*>(&client_addr), &client_len);
            
            if (client_fd >= 0) {
                int flags = fcntl(client_fd, F_GETFL, 0);
                fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
                
                clients.push_back(std::make_unique<Client>(client_fd));
            }
        }
        
        for (auto it = clients.begin(); it != clients.end();) {
            if (!(*it)->process(on_packet) || (*it)->is_closed()) {
                it = clients.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void Server::stop() {
    running_ = false;
    if (listen_fd_ >= 0) {
        close(listen_fd_);
        listen_fd_ = -1;
    }
}

}
