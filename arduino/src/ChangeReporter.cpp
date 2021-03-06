#include <math.h>
#include "Print.h"
#include "ChangeReporter.h"
#include "utils.h"
#include "Sensor.h"

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
        explicit NMEADecoratedStream (Print& inStream) :
            mStream (inStream),
            mChecksum (0)
        {
            mStream.write('$');
        }

        ~NMEADecoratedStream () {
            mStream << '*' << Hex {mChecksum} << '\n';
        }

        size_t write(uint8_t inValue) override {
            size_t count (mStream.write(inValue));
            mChecksum ^= inValue;
            return count;
        }
    private:
        Print& mStream;
        uint8_t mChecksum;
    };
}

ChangeReporter::ChangeReporter (Print& inReportStream, Sensor& ioSensor) :
    mReportStream (inReportStream),
    mSensor (ioSensor),
    mPrevious (9.0e20f),
    mMinimumChange (0)
{
}

void ChangeReporter::SetMinimumChange (float inChange) {
    mMinimumChange = inChange;
}

void ChangeReporter::Update () {
    float value;
    if (mSensor.GetMeasurement (value)) {
        if (fabs (mPrevious - value) >= mMinimumChange) {
            NMEADecoratedStream str (mReportStream);
            str << "MEAS," << mSensor.GetName () << ',' << value;
            mPrevious = value;
        }
    }
}
