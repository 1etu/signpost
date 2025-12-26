#include "network/client.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <cerrno>
#include <cstring>

namespace network {

Client::Client(int fd) : fd_(fd) {
    recv_buffer_.reserve(4096);
}

Client::~Client() {
    close();
}

bool Client::process(const PacketCallback& on_packet) {
    if (closed_) return false;
    
    uint8_t temp[4096];
    ssize_t n = recv(fd_, temp, sizeof(temp), 0);
    
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return true;
        }
        return false;
    }
    
    if (n == 0) {
        return false;
    }
    
    recv_buffer_.insert(recv_buffer_.end(), temp, temp + n);
    
    while (!recv_buffer_.empty()) {
        size_t consumed = 0;
        
        try {
            auto packet = protocol::read_packet(recv_buffer_.data(), recv_buffer_.size(), consumed);
            
            if (!packet) {
                break;
            }
            
            recv_buffer_.erase(recv_buffer_.begin(), recv_buffer_.begin() + consumed);
            
            on_packet(*this, *packet);
            
            if (closed_) return false;
            
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    return true;
}

void Client::send(const protocol::Buffer& packet) {
    if (closed_) return;
    
    auto data = packet.build_packet();
    ::send(fd_, data.data(), data.size(), 0);
}

void Client::close() {
    if (!closed_) {
        closed_ = true;
        ::close(fd_);
    }
}

}
