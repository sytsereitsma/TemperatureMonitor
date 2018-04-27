#include "utils.h"
#include "constants.h"

namespace {
    const char kToHex [16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
}

Print& operator<<(Print& outStream, Hex value) {
    outStream << (kToHex [(value.value >> 4) & 0xF])
              << (kToHex [value.value & 0xF]);
    return outStream;
}

Print& operator<<(Print& outStream, OneWireAddress address) {
    outStream << Hex {address.address [0]} << '-';

    const uint8_t kCrcIndex (kAddressSize - 1);
    for (uint8_t t = 1; t < kCrcIndex; ++t) {
        outStream << Hex {address.address [t]};
    }

    outStream << '-' << Hex {address.address [kCrcIndex]};
    return outStream;
}

uint8_t NMEAChecksum (const uint8_t* inBegin, const uint8_t* inEnd) {
    auto c (inBegin);
    uint8_t checksum (0);
    while (c != inEnd) {
        checksum ^= *c;
        ++c;
    }

    return checksum;
}
