#include "utils.h"
#include "constants.h"

namespace {
    const char kToHex [16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
}

void PrintAddress(Stream& outStream, const uint8_t* inAddress) {
    outStream << Hex {inAddress [0]} << '-';

    const uint8_t kCrcIndex (kAddressSize - 1);
    for (uint8_t t = 1; t < kCrcIndex; ++t) {
        outStream << Hex {inAddress [t]};
    }

    outStream << '-' << Hex {inAddress [kCrcIndex]};
}

Stream& operator<<(Stream& outStream, Hex value) {
    outStream << (kToHex [(value.value >> 4) & 0xF])
              << (kToHex [value.value & 0xF]);
    return outStream;
}