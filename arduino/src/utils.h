#ifndef UTILS_H__
#define UTILS_H__
#include <stdint.h>
#include "Print.h"

template <typename taType>
Print& operator<<(Print& outStream, taType value) {
    outStream.print(value);
    return outStream;
}

struct Hex {
    uint8_t value;
};
Print& operator<<(Print& outStream, Hex value);

struct OneWireAddress {
    const uint8_t* address;
};
Print& operator<<(Print& outStream, OneWireAddress address);

uint8_t NMEAChecksum (const uint8_t* inBegin, const uint8_t* inEnd);

#endif /* end of include guard: UTILS_H__ */