#include "config.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

static const char* BASE64_CHARS = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static std::string base64_encode(const std::vector<uint8_t>& data) {
    std::string result;
    result.reserve((data.size() + 2) / 3 * 4);
    
    for (size_t i = 0; i < data.size(); i += 3) {
        uint32_t n = static_cast<uint32_t>(data[i]) << 16;
        if (i + 1 < data.size()) n |= static_cast<uint32_t>(data[i + 1]) << 8;
        if (i + 2 < data.size()) n |= static_cast<uint32_t>(data[i + 2]);
        
        result += BASE64_CHARS[(n >> 18) & 0x3F];
        result += BASE64_CHARS[(n >> 12) & 0x3F];
        result += (i + 1 < data.size()) ? BASE64_CHARS[(n >> 6) & 0x3F] : '=';
        result += (i + 2 < data.size()) ? BASE64_CHARS[n & 0x3F] : '=';
    }
    
    return result;
}

static std::string load_favicon(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
    
    if (data.size() < 8 || data[0] != 0x89 || data[1] != 'P' || 
        data[2] != 'N' || data[3] != 'G') {
        std::cerr << "Warning: Favicon is not a valid PNG file\n";
        return "";
    }
    
    return "data:image/png;base64," + base64_encode(data);
}

static std::string parse_string_value(const std::string& json, const std::string& key) {
    std::string search = "\"" + key + "\"";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return "";
    
    pos = json.find(':', pos);
    if (pos == std::string::npos) return "";
    
    pos = json.find('"', pos + 1);
    if (pos == std::string::npos) return "";
    
    size_t end = pos + 1;
    while (end < json.size()) {
        if (json[end] == '"' && json[end - 1] != '\\') {
            break;
        }
        end++;
    }
    
    return json.substr(pos + 1, end - pos - 1);
}

static int parse_int_value(const std::string& json, const std::string& key, int default_val) {
    std::string search = "\"" + key + "\"";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return default_val;
    
    pos = json.find(':', pos);
    if (pos == std::string::npos) return default_val;
    
    pos++;
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) {
        pos++;
    }
    
    std::string num;
    while (pos < json.size() && (isdigit(json[pos]) || json[pos] == '-')) {
        num += json[pos++];
    }
    
    if (num.empty()) return default_val;
    return std::stoi(num);
}

bool Config::load(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file: " << path << "\n";
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string json = buffer.str();
    
    std::string host_val = parse_string_value(json, "host");
    if (!host_val.empty()) host = host_val;
    
    port = static_cast<uint16_t>(parse_int_value(json, "port", 25565));
    
    motd = parse_string_value(json, "motd");
    motd_outdated = parse_string_value(json, "motd_outdated");
    min_protocol = parse_int_value(json, "min_protocol", 0);
    
    kick_message = parse_string_value(json, "kick_message");
    
    std::string version_val = parse_string_value(json, "version_name");
    if (!version_val.empty()) version_name = version_val;
    
    max_players = parse_int_value(json, "max_players", 0);
    online_players = parse_int_value(json, "online_players", 0);
    
    std::string favicon_path = parse_string_value(json, "favicon");
    if (!favicon_path.empty()) {
        favicon = load_favicon(favicon_path);
        if (favicon.empty()) {
            std::cerr << "Warning: Could not load favicon from: " << favicon_path << "\n";
        }
    }
    
    if (motd.empty()) {
        std::cerr << "Config missing 'motd'\n";
        return false;
    }
    
    if (kick_message.empty()) {
        std::cerr << "Config missing 'kick_message'\n";
        return false;
    }
    
    std::cout << "Config OK:\n"
              << "  Host: " << host << "\n"
              << "  Port: " << port << "\n"
              << "  MOTD: " << motd << "\n";
    
    if (min_protocol > 0 && !motd_outdated.empty()) {
        std::cout << "  MOTD (outdated): " << motd_outdated << "\n"
                  << "  Min Protocol: " << min_protocol << "\n";
    }
    
    std::cout << "  Kick: " << kick_message << "\n"
              << "  Version: " << version_name << "\n";
    
    if (!favicon.empty()) {
        std::cout << "  Favicon: loaded (" << favicon.size() << " bytes)\n";
    }
    
    return true;
}
