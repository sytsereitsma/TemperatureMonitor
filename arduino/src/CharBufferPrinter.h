#ifndef CHARBUFFERPRINTER_H__
#define CHARBUFFERPRINTER_H__
#include "Print.h"

class CharBufferPrinter : public Print {
public:
    CharBufferPrinter (char* outBuffer, int inSize);
    size_t write (uint8_t) override;
private:
    char* mWritePos;
    const char* mEndPos;
};


#endif /* end of include guard: CHARBUFFERPRINTER_H__ */