#include "CharBufferPrinter.h"

CharBufferPrinter::CharBufferPrinter (char* outBuffer, int inSize) :
    mWritePos(outBuffer),
    mEndPos(outBuffer + inSize - 1)
{
    *mWritePos = '\0';
    mWritePos [inSize - 1] = '\0';
}

void CharBufferPrinter::write (uint8_t inByte) {
    if (mWritePos >= mEndPos) {
        return;
    }

    *mWritePos = static_cast <char> (inByte);
    ++mWritePos;
    *mWritePos = '\0';
}
