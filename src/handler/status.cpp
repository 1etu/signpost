#include "handler/status.hpp"
#include <sstream>

namespace handler {

static std::string escape_json(const std::string& s) {
    std::ostringstream o;
    for (char c : s) {
        switch (c) {
            case '"':  o << "\\\""; break;
            case '\\': o << "\\\\"; break;
            case '\b': o << "\\b";  break;
            case '\f': o << "\\f";  break;
            case '\n': o << "\\n";  break;
            case '\r': o << "\\r";  break;
            case '\t': o << "\\t";  break;
            default:   o << c;      break;
        }
    }
    return o.str();
}

void handle_status_request(network::Client& client, const Config& config) {
    bool outdated = config.min_protocol > 0 && 
                    client.protocol_version() < config.min_protocol &&
                    !config.motd_outdated.empty();
    
    std::ostringstream desc;
    if (outdated) {
        desc << "{\"text\":\"" << escape_json(config.motd) << "\\n" 
             << escape_json(config.motd_outdated) << "\"}";
    } else {
        desc << "{\"text\":\"" << escape_json(config.motd) << "\"}";
    }
    
    std::ostringstream json;
    json << "{"
         << "\"version\":{\"name\":\"" << escape_json(config.version_name) << "\",\"protocol\":-1},"
         << "\"players\":{\"max\":" << config.max_players << ",\"online\":" << config.online_players << "},"
         << "\"description\":" << desc.str();
    
    if (!config.favicon.empty()) {
        json << ",\"favicon\":\"" << config.favicon << "\"";
    }
    
    json << "}";
    
    std::string json_str = json.str();
    
    protocol::Buffer response;
    response.write_varint(protocol::PacketId::StatusResponse);
    response.write_string(json_str);
    
    client.send(response);
}

void handle_ping(network::Client& client, protocol::Packet& packet) {
    int64_t payload = packet.payload.read_long();
    
    protocol::Buffer response;
    response.write_varint(protocol::PacketId::Pong);
    response.write_long(payload);
    
    client.send(response);
}

}
