#ifndef STRINGSTREAM_H__
#define STRINGSTREAM_H__
#include "Print.h"

struct StringStream : public Print {
    void write (uint8_t inValue) override {
        mString += static_cast <char> (inValue);
    }

    std::string GetAndReset () {
        return std::move (mString);
    }
    
    std::string mString;
};

#endif /* end of include guard: STRINGSTREAM_H__ */
