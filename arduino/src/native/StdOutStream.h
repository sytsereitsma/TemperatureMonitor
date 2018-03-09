#ifndef NATIVE_STDOUTSTREAM_H__
#define NATIVE_STDOUTSTREAM_H__
#include <iostream>
#include "Stream.h"

struct StdOutStream : public Stream {
    void println() override {
        std::cout << std::endl;
    }

    void println(char inValue) override {
        std::cout << inValue << std::endl;
    }

    void println(int32_t inValue) override {
        std::cout << inValue << std::endl;
    }

    void println(uint32_t inValue) override {
        std::cout << inValue << std::endl;
    }

    void println(int64_t inValue) override {
        std::cout << inValue << std::endl;
    }

    void println(float inValue) override {
        std::cout << inValue << std::endl;
    }

    void println(const __FlashStringHelper* inValue) override {
        std::cout << inValue << std::endl;
    }

    void println(const char* inValue) override {
        std::cout << inValue << std::endl;
    }
    
    void print(char inValue) override {
        std::cout << inValue;
    }

    void print(int32_t inValue) override {
        std::cout << inValue;
    }

    void print(uint32_t inValue) override {
        std::cout << inValue;
    }

    void print(int64_t inValue) override {
        std::cout << inValue;
    }

    void print(float inValue) override {
        std::cout << inValue;
    }

    void print(const __FlashStringHelper* inValue) override {
        std::cout << inValue;
    }

    void print(const char* inValue) override {
        std::cout << inValue;
    }
};

#endif /* end of include guard: NATIVE_STDOUTSTREAM_H__ */