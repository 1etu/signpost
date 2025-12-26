#include "handler/handshake.hpp"

namespace handler {

void handle_handshake(network::Client& client, protocol::Packet& packet) {
    int32_t protocol_version = packet.payload.read_varint();
    [[maybe_unused]] std::string server_address = packet.payload.read_string();
    [[maybe_unused]] uint16_t server_port = packet.payload.read_ushort();
    int32_t next_state = packet.payload.read_varint();
    
    client.set_protocol_version(protocol_version);
    
    if (next_state == 1) {
        client.set_state(protocol::State::Status);
    } else if (next_state == 2) {
        client.set_state(protocol::State::Login);
    } else {
        client.close();
    }
}

}
