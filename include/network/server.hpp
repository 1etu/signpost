#pragma once

#include "network/client.hpp"
#include <string>
#include <memory>
#include <vector>
#include <thread>
#include <atomic>

namespace network {

class Server {
public:
    Server(const std::string& host, uint16_t port);
    ~Server();
    
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;
    
    bool start();
    void run(const Client::PacketCallback& on_packet);
    void stop();

private:
    std::string host_;
    uint16_t port_;
    int listen_fd_ = -1;
    std::atomic<bool> running_{false};
};

}
