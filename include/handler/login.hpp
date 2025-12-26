#pragma once

#include "network/client.hpp"
#include "protocol/packet.hpp"
#include <string>

namespace handler { void handle_login_start(network::Client& client, protocol::Packet& packet, const std::string& kick_message); }
