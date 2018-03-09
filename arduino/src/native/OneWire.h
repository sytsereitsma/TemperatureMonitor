#ifndef NATIVE_ONEWIRE_H__
#define NATIVE_ONEWIRE_H__

#if __AVR__ != 1

class OneWire {
public:
    OneWire (int /*inPin*/) {
    }

    uint8_t reset () {
        return 0;
    }

    void reset_search () {
    }

    uint8_t search(uint8_t *newAddr, bool search_mode /*= true*/) {
        return 0;
    }

    void select (const uint8_t rom[8]) {
    };

    uint8_t read () {
        return 0;
    }

    void read_bytes (uint8_t *buf, uint16_t count) {
    }

    void write(uint8_t v, uint8_t power/* = 0*/) {
    }

    static uint8_t crc8 (const uint8_t *addr, uint8_t len) {
        return 0;
    }
};

#endif

#endif /* end of include guard: NATIVE_ONEWIRE_H__ */