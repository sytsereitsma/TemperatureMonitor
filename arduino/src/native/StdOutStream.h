#ifndef NATIVE_STDOUTSTREAM_H__
#define NATIVE_STDOUTSTREAM_H__
#include <iostream>
#include "Print.h"

struct StdOutStream : public Print {
    void write (uint8_t inValue) override {
        std::cout << static_cast <char> (inValue);
    }
};

#endif /* end of include guard: NATIVE_STDOUTSTREAM_H__ */