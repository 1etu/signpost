#pragma once

#include <string>
#include <cstdint>

struct Config {
    std::string host = "0.0.0.0";
    uint16_t port = 25565;
    
    std::string motd;
    std::string motd_outdated;  
    int32_t min_protocol = 0;
    
    std::string kick_message;
    
    std::string version_name = "Placeholder";
    int max_players = 0;
    int online_players = 0;
    
    std::string favicon;
    
    bool load(const std::string& path);
};
