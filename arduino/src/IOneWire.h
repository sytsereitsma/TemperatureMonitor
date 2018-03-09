#ifndef IONEWIRE_H__
#define IONEWIRE_H__
#include <cstdint>

class IOneWire {
public:
    virtual ~IOneWire () = default;

    virtual uint8_t GetPin () const = 0;    

    virtual uint8_t reset () = 0;
    virtual void reset_search () = 0;
    virtual uint8_t search(uint8_t *newAddr, bool search_mode) = 0;
    virtual void select(const uint8_t rom[8]) = 0;

    virtual uint8_t read () = 0;
    virtual void read_bytes (uint8_t *buf, uint16_t count) = 0;

    virtual void write(uint8_t v, uint8_t power = 0) = 0;

    virtual uint8_t crc8 (const uint8_t *addr, uint8_t len) = 0;
};

#endif //IONEWIRE_H__
