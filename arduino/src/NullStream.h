#ifndef NULLSTREAM_H__
#define NULLSTREAM_H__
#include "Print.h"

class NullStream : public Print {
public:
    void write (uint8_t) override {}
};

#endif /* end of include guard: NULLSTREAM_H__ */