#pragma once

#include "network/client.hpp"
#include "protocol/packet.hpp"
#include "config.hpp"

namespace handler {
void handle_status_request(network::Client& client, const Config& config);
void handle_ping(network::Client& client, protocol::Packet& packet);
}
