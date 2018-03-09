#ifndef ONEWIREWRAPPER_H__
#define ONEWIREWRAPPER_H__
#include "IOneWire.h"
#include "OneWire.h"

class OneWireWrapper : public IOneWire {
public:
    OneWireWrapper (int inPin) :
        mOneWire (inPin),
        mPin (inPin)
    {
    }

    uint8_t GetPin () const override {
        return mPin;
    }    

    uint8_t reset () {
        return mOneWire.reset ();
    }

    void reset_search () {
        mOneWire.reset_search ();
    }

    uint8_t search(uint8_t *newAddr, bool search_mode /*= true*/) {
        return mOneWire.search (newAddr, search_mode);
    }

    void select (const uint8_t rom[8]) {
        mOneWire.select (rom);
    };

    uint8_t read () {
        return mOneWire.read ();
    }

    void read_bytes (uint8_t *buf, uint16_t count) {
        mOneWire.read_bytes (buf, count);
    }

    void write(uint8_t v, uint8_t power/* = 0*/) {
        mOneWire.write (v, power);
    }

    uint8_t crc8 (const uint8_t *addr, uint8_t len) override {
        return OneWire::crc8 (addr, len);
    }
private:
    OneWire mOneWire;
    const uint8_t mPin;
};

#endif //ONEWIREWRAPPER_H__