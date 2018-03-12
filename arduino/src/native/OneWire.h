#if __AVR__
static_assert (false, "Now why would you want to build this file for AVR?");
#endif

#ifndef NATIVE_ONEWIRE_H__
#define NATIVE_ONEWIRE_H__
#include <map>
#include "constants.h"

/**
Minimal OneWire emulation for DS18S20
*/
class OneWire {
public:
    OneWire (uint8_t inPin);
    uint8_t reset ();

    void reset_search ();

    uint8_t search(uint8_t *newAddr, bool search_mode /*= true*/);

    void select (const uint8_t rom[8]);

    uint8_t read ();

    void read_bytes (uint8_t *buf, uint16_t count);

    void write(uint8_t v, uint8_t power/* = 0*/);

    static uint8_t crc8 (const uint8_t *addr, uint8_t len);
private:
    uint8_t mAddress [kAddressSize];
    uint8_t mPin;
};

#endif /* end of include guard: NATIVE_ONEWIRE_H__ */