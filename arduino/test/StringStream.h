#ifndef STRINGSTREAM_H__
#define STRINGSTREAM_H__
#include "Stream.h"

struct OStringStream : public Print {
    size_t write (uint8_t inValue) override {
        mString += static_cast <char> (inValue);
        return 1u;
    }

    std::string GetAndReset () {
        return std::move (mString);
    }
    
    std::string mString;
};

#endif /* end of include guard: STRINGSTREAM_H__ */
