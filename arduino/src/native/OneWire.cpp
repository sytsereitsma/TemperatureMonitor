#include "OneWire.h"
#include <chrono>
#include <cmath>

namespace {
    const uint8_t kFixedCRC (0xCB); 
}

OneWire::OneWire (uint8_t inPin) :
    mPin (inPin)
{
    mAddress [0] = 0x28;
    mAddress [1] = inPin;
    mAddress [2] = inPin;
    mAddress [3] = inPin;
    mAddress [4] = inPin;
    mAddress [5] = inPin;
    mAddress [6] = inPin;
    mAddress [7] = crc8 (mAddress, 7);
}

uint8_t OneWire::reset () {
    return 1;
}

void OneWire::reset_search () {
}

uint8_t OneWire::search(uint8_t *newAddr, bool search_mode /*= true*/) {
    memcpy (newAddr, mAddress, kAddressSize);
    return 1;
}

void OneWire::select (const uint8_t [8]) {
}

uint8_t OneWire::read () {
    return 0;
}

void OneWire::read_bytes (uint8_t *buf, uint16_t count) {
    constexpr uint8_t kScratchPadSize = { 9u };
    if (count!= kScratchPadSize) {
        return;
    }

    const auto now (std::chrono::system_clock::now ());
    const auto epoch (now.time_since_epoch ());
    const double phase (static_cast <double> (epoch.count () * std::chrono::system_clock::period::num) / std::chrono::system_clock::period::den);

    //Cycle 5 degrees around 21 degree approx every 30 secs
    float temperature = 20.0f + mPin + 5.0f * static_cast <float> (sin (phase / 5));
    auto temp16 = static_cast <int16_t> (temperature * 16 + 0.5f);
    buf [0] = temp16 & 0xFF;
    buf [1] = (temp16 >> 8) & 0xFF;
    buf [kScratchPadSize - 1] = crc8 (buf, kScratchPadSize - 1);
}

void OneWire::write (uint8_t v, uint8_t power/* = 0*/) {
}

uint8_t OneWire::crc8 (const uint8_t *addr, uint8_t len) {
    uint8_t sum (0);

    auto p (addr);
    auto end (addr + len);
    while (p != end) {
        sum += *p;
        ++p;
    }

    return sum;
}
