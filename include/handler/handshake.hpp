#pragma once

#include "network/client.hpp"
#include "protocol/packet.hpp"

namespace handler { void handle_handshake(network::Client& client, protocol::Packet& packet); }
