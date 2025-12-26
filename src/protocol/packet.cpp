#include "protocol/packet.hpp"

namespace protocol {

std::unique_ptr<Packet> read_packet(const uint8_t* data, size_t len, size_t& consumed) {
    if (len == 0) return nullptr;
    
    size_t pos = 0;
    int32_t packet_len = 0;
    int shift = 0;
    
    while (pos < len) {
        uint8_t byte = data[pos++];
        packet_len |= (byte & 0x7F) << shift;
        shift += 7;
        
        if (!(byte & 0x80)) {
            break;
        }
        
        if (shift > 21) {
            throw std::runtime_error("pkt length varint too large");
        }
    }
    
    if (len - pos < static_cast<size_t>(packet_len)) {
        return nullptr;
    }
    
    Buffer buf(std::vector<uint8_t>(data + pos, data + pos + packet_len));
    int32_t packet_id = buf.read_varint();
    
    consumed = pos + packet_len;
    
    std::vector<uint8_t> payload_data(
        data + pos + varint_size(packet_id),
        data + pos + packet_len
    );
    
    auto pkt = std::make_unique<Packet>();
    pkt->id = packet_id;
    pkt->payload = Buffer(payload_data);
    
    return pkt;
}

}
