#ifndef UTILS_H__
#define UTILS_H__
#include <cstdint>

template <typename taType>
Stream& operator<<(Stream& outStream, taType value) {
    outStream.print(value);
    return outStream;
}

extern void PrintAddress(Stream& outStream, const uint8_t* inAddress);

struct Hex {
    uint8_t value;
};
Stream& operator<<(Stream& outStream, Hex value);


#endif /* end of include guard: UTILS_H__ */