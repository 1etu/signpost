#include "protocol/buffer.hpp"

namespace protocol {

int32_t Buffer::read_varint() {
    int32_t result = 0;
    int shift = 0;
    uint8_t byte;
    
    do {
        if (pos_ >= data_.size()) {
            throw std::runtime_error("Buffer underflow reading VarInt");
        }
        byte = data_[pos_++];
        result |= (byte & 0x7F) << shift;
        shift += 7;
        
        if (shift > 35) {
            throw std::runtime_error("VarInt too large");
        }
    } while (byte & 0x80);
    
    return result;
}

std::string Buffer::read_string() {
    int32_t length = read_varint();
    
    if (length < 0 || static_cast<size_t>(length) > remaining()) {
        throw std::runtime_error("buff string length");
    }
    
    std::string result(reinterpret_cast<const char*>(&data_[pos_]), length);
    pos_ += length;
    return result;
}

uint16_t Buffer::read_ushort() {
    if (remaining() < 2) {
        throw std::runtime_error("buff underflow reading ushort");
    }
    uint16_t result = (data_[pos_] << 8) | data_[pos_ + 1];
    pos_ += 2;
    return result;
}

int64_t Buffer::read_long() {
    if (remaining() < 8) {
        throw std::runtime_error("buff underflow reading long");
    }
    int64_t result = 0;
    for (int i = 0; i < 8; i++) {
        result = (result << 8) | data_[pos_++];
    }
    return result;
}

uint8_t Buffer::read_byte() {
    if (pos_ >= data_.size()) {
        throw std::runtime_error("buff underflow reading byte");
    }
    return data_[pos_++];
}

void Buffer::write_varint(int32_t value) {
    uint32_t uvalue = static_cast<uint32_t>(value);
    do {
        uint8_t byte = uvalue & 0x7F;
        uvalue >>= 7;
        if (uvalue != 0) {
            byte |= 0x80;
        }
        data_.push_back(byte);
    } while (uvalue != 0);
}

void Buffer::write_string(const std::string& value) {
    write_varint(static_cast<int32_t>(value.size()));
    data_.insert(data_.end(), value.begin(), value.end());
}

void Buffer::write_ushort(uint16_t value) {
    data_.push_back((value >> 8) & 0xFF);
    data_.push_back(value & 0xFF);
}

void Buffer::write_long(int64_t value) {
    for (int i = 7; i >= 0; i--) {
        data_.push_back((value >> (i * 8)) & 0xFF);
    }
}

void Buffer::write_byte(uint8_t value) {
    data_.push_back(value);
}

void Buffer::write_bytes(const uint8_t* bytes, size_t len) {
    data_.insert(data_.end(), bytes, bytes + len);
}

std::vector<uint8_t> Buffer::build_packet() const {
    std::vector<uint8_t> result;
    
    int32_t len = static_cast<int32_t>(data_.size());
    uint32_t ulen = static_cast<uint32_t>(len);
    do {
        uint8_t byte = ulen & 0x7F;
        ulen >>= 7;
        if (ulen != 0) {
            byte |= 0x80;
        }
        result.push_back(byte);
    } while (ulen != 0);
    
    result.insert(result.end(), data_.begin(), data_.end());
    return result;
}

}
