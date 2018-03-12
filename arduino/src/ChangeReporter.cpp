#include "Print.h"
#include "ChangeReporter.h"
#include "utils.h"
#include <cmath>
#include <limits>

namespace {
    /**
    Just chose a communication standard I knew already.
    The checksum for NMEA messages is way too simple, but since the XBee does error
    checking and retransmits for you in the background the risk of receiving erroneous
    data is minimal. The main reason for the NMEA protocol is:
        * It is a standardized protocol
        * It provides a start ($) and end (\n) character for deserializing the stream.
    */
    class NMEADecoratedStream : public Print {
    public:
        NMEADecoratedStream (Print& inStream) :
            mStream (inStream),
            mChecksum (0)
        {
            mStream.write('$');
        }

        ~NMEADecoratedStream () {
            mStream << '*' << Hex {mChecksum} << '\n';
        }

        void write(uint8_t inValue) override {
            mStream.write(inValue);
            mChecksum ^= inValue;
        }
    private:
        Print& mStream;
        uint8_t mChecksum;
    };
}

ChangeReporter::ChangeReporter (Print& inReportStream, const uint8_t* inAddress, uint8_t inPin) :
    mReportStream (inReportStream),
    mAddress (inAddress),
    mPin (inPin),
    mPrevious (std::numeric_limits <float>::max ()),
    mMinimumChange (0)
{
}

void ChangeReporter::SetMinimumChange (float inChange) {
    mMinimumChange = inChange;
}

void ChangeReporter::Update (float inValue) {
    if (fabs (mPrevious - inValue) >= mMinimumChange) {
        NMEADecoratedStream str (mReportStream);
        str << "TMP," << int {mPin} << '-' << OneWireAddress {mAddress} << ',' << inValue;
        mPrevious = inValue;
    }
}
