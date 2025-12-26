#pragma once

#include "protocol/buffer.hpp"
#include <memory>

namespace protocol {

enum class State {
    Handshaking = 0,
    Status = 1,
    Login = 2
};

namespace PacketId {
    constexpr int32_t Handshake = 0x00;
    constexpr int32_t StatusRequest = 0x00;
    constexpr int32_t Ping = 0x01;
    constexpr int32_t StatusResponse = 0x00;
    constexpr int32_t Pong = 0x01;
    constexpr int32_t LoginStart = 0x00;
    constexpr int32_t Disconnect = 0x00;
}

struct Handshake {
    int32_t protocol_version;
    std::string server_address;
    uint16_t server_port;
    State next_state;
};

struct Packet {
    int32_t id;
    Buffer payload;
    
    Packet() : id(0) {}
    Packet(int32_t id, Buffer buf) : id(id), payload(std::move(buf)) {}
};

std::unique_ptr<Packet> read_packet(const uint8_t* data, size_t len, size_t& consumed);

}
