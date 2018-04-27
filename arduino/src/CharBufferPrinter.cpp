#include "CharBufferPrinter.h"

CharBufferPrinter::CharBufferPrinter (char* outBuffer, int inSize) :
    mWritePos(outBuffer),
    mEndPos(outBuffer + inSize - 1)
{
}

void CharBufferPrinter::write (uint8_t inByte) {
    *mWritePos = static_cast <char> (inByte);
    ++mWritePos;
}
