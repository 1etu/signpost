#include "handler/login.hpp"
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

void handle_login_start(network::Client& client, protocol::Packet& /*packet*/, const std::string& kick_message) {
    std::ostringstream json;
    json << "{\"text\":\"" << escape_json(kick_message) << "\"}";
    
    std::string json_str = json.str();
    
    protocol::Buffer response;
    response.write_varint(protocol::PacketId::Disconnect);
    response.write_string(json_str);
    
    client.send(response);
    client.close();
}

}
