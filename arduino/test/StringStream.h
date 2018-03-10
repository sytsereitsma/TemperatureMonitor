#ifndef STRINGSTREAM_H__
#define STRINGSTREAM_H__
#include <sstream>
#include "Stream.h"

struct StringStream : public Stream {
    void println() override {
        mStream << std::endl;
    }

    void println(char inValue) override {
        mStream << inValue << std::endl;
    }

    void println(int32_t inValue) override {
        mStream << inValue << std::endl;
    }

    void println(uint32_t inValue) override {
        mStream << inValue << std::endl;
    }

    void println(int64_t inValue) override {
        mStream << inValue << std::endl;
    }

    void println(float inValue) override {
        mStream << inValue << std::endl;
    }

    void println(const __FlashStringHelper* inValue) override {
        mStream << inValue << std::endl;
    }

    void println(const char* inValue) override {
        mStream << inValue << std::endl;
    }
    
    void print(char inValue) override {
        mStream << inValue;
    }

    void print(int32_t inValue) override {
        mStream << inValue;
    }

    void print(uint32_t inValue) override {
        mStream << inValue;
    }

    void print(int64_t inValue) override {
        mStream << inValue;
    }

    void print(float inValue) override {
        mStream << inValue;
    }

    void print(const __FlashStringHelper* inValue) override {
        mStream << inValue;
    }

    void print(const char* inValue) override {
        mStream << inValue;
    }

    std::ostringstream mStream;
};

#endif /* end of include guard: STRINGSTREAM_H__ */
