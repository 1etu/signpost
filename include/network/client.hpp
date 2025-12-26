#pragma once

#include "protocol/packet.hpp"
#include <functional>
#include <vector>
#include <atomic>

namespace network {

class Client {
public:
    using PacketCallback = std::function<void(Client&, protocol::Packet&)>;
    
    explicit Client(int fd);
    ~Client();
    
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;
    
    bool process(const PacketCallback& on_packet);
    
    void send(const protocol::Buffer& packet);
    
    void close();
    bool is_closed() const { return closed_; }

    protocol::State state() const { return state_; }
    void set_state(protocol::State s) { state_ = s; }
    
    int32_t protocol_version() const { return protocol_version_; }
    void set_protocol_version(int32_t v) { protocol_version_ = v; }

private:
    int fd_;
    bool closed_ = false;
    protocol::State state_ = protocol::State::Handshaking;
    int32_t protocol_version_ = 0;
    std::vector<uint8_t> recv_buffer_;
};

}
