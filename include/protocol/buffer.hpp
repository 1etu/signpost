#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>

namespace protocol {
class Buffer {
public:
    Buffer() = default;
    explicit Buffer(const std::vector<uint8_t>& data) : data_(data), pos_(0) {}
    int32_t read_varint();
    std::string read_string();
    uint16_t read_ushort();
    int64_t read_long();
    uint8_t read_byte();
    void write_varint(int32_t value);
    void write_string(const std::string& value);
    void write_ushort(uint16_t value);
    void write_long(int64_t value);
    void write_byte(uint8_t value);
    void write_bytes(const uint8_t* data, size_t len);
    size_t remaining() const { return data_.size() - pos_; }
    size_t size() const { return data_.size(); }
    const uint8_t* data() const { return data_.data(); }
    void reset() { pos_ = 0; }
    void clear() { data_.clear(); pos_ = 0; }
    std::vector<uint8_t> build_packet() const;

private:
    std::vector<uint8_t> data_;
    size_t pos_ = 0;
};

inline int varint_size(int32_t value) {
    int size = 0;
    do {
        size++;
        value = static_cast<uint32_t>(value) >> 7;
    } while (value != 0);
    return size;
}
}
