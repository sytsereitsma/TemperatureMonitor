#ifndef UTILS_H__
#define UTILS_H__
#include <cstdint>

template <typename taType>
Stream& operator<<(Stream& outStream, taType value) {
    outStream.print(value);
    return outStream;
}

struct Hex {
    uint8_t value;
};
Stream& operator<<(Stream& outStream, Hex value);

struct OneWireAddress {
    const uint8_t* address;
};
Stream& operator<<(Stream& outStream, OneWireAddress address);

#endif /* end of include guard: UTILS_H__ */