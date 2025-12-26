#include "config.hpp"
#include "network/server.hpp"
#include "handler/handshake.hpp"
#include "handler/status.hpp"
#include "handler/login.hpp"

#include <iostream>
#include <csignal>

static network::Server* g_server = nullptr;

void signal_handler(int /*sig*/) {
    std::cout << "\nShutting down...\n";
    if (g_server) {
        g_server->stop();
    }
}

int main(int argc, char* argv[]) {
    std::cout << "signpost:";
    
    std::string config_path = "config.json";
    if (argc > 1) {
        config_path = argv[1];
    }
    
    Config config;
    if (!config.load(config_path)) {
        return 1;
    }
    
    std::cout << "\n";
    
    network::Server server(config.host, config.port);
    g_server = &server;
    
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    if (!server.start()) {
        return 1;
    }
    
    auto packet_handler = [&config](network::Client& client, protocol::Packet& packet) {
        switch (client.state()) {
            case protocol::State::Handshaking:
                if (packet.id == protocol::PacketId::Handshake) {
                    handler::handle_handshake(client, packet);
                }
                break;
                
            case protocol::State::Status:
                if (packet.id == protocol::PacketId::StatusRequest) {
                    handler::handle_status_request(client, config);
                } else if (packet.id == protocol::PacketId::Ping) {
                    handler::handle_ping(client, packet);
                }
                break;
                
            case protocol::State::Login:
                if (packet.id == protocol::PacketId::LoginStart) {
                    handler::handle_login_start(client, packet, config.kick_message);
                }
                break;
        }
    };
    
    server.run(packet_handler);
    
    std::cout << "server killed.\n";
    return 0;
}
