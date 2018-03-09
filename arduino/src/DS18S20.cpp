#include "DS18S20.h"
#include "IOneWire.h"
#include "utils.h"
#include <Stream.h>


DS18S20::DS18S20 (IOneWire& inOneWire, Stream& inLogStream) :
    mDS (inOneWire),
    mLogStream (inLogStream)
{
    ResetAddress ();
    Detect ();
}


bool DS18S20::Detect () {
    constexpr uint8_t kCRCIndex {7};

    bool found (false);

    uint8_t addr [kAddressSize];

    if (!mDS.search (addr, true)) {
        //no more sensors on chain, reset search
        mDS.reset_search ();
        mLogStream << "No temperature sensor found at pin " << mDS.GetPin () << "\n";
    }
    else {
        const auto expectedCRC (mDS.crc8 (addr, kCRCIndex));
        if (expectedCRC == addr[kCRCIndex]) {
            mLogStream << "Device found at pin " << mDS.GetPin () << ": ";
            PrintAddress(mLogStream, addr);
            mLogStream.println();

            constexpr uint8_t kDS18S20FamilyCode = {0x28};
            if (addr[0] != kDS18S20FamilyCode && addr[0] != kDS18S20FamilyCode) {
                mLogStream << "Unrecognized/unsupported 1-Wire family code at pin" << mDS.GetPin () << "\n";
            }
            else {
                found = true;
                memcpy (mAddress, addr, sizeof (mAddress));
            }
        }        
        else {
              mLogStream << "CRC is not valid for device at pin " << mDS.GetPin () << " (address ";
              PrintAddress(mLogStream, addr);
              mLogStream << ", expected CRC " << Hex { expectedCRC } << ")." << "\n";
        }
    }

    if (found) {
        ReadPowerSupplyType ();
    }
    else {
        ResetAddress ();
    }

    return found;
}

bool DS18S20::StartConversion () {
    constexpr uint8_t kConvertTempCommand {0x44};
    return SendCommand (kConvertTempCommand, true);
}

bool DS18S20::GetTemperature (float& outTemp) {
    if (mAddress [0] == 0) {
        return false;
    }

    bool ok (false);
    if (StartConversion ()) {
        constexpr uint8_t kScratchPadSize = { 9u };
        uint8_t data [kScratchPadSize];
        if (ReadScratchPad (data, kScratchPadSize)) {
            uint8_t lsb = data [0];
            uint8_t msb = data [1];

            float tempRead = ((msb << 8) | lsb); //using two's compliment
            float TemperatureSum = tempRead / 16;

            outTemp = TemperatureSum;//Fahrenheit: (TemperatureSum * 18 + 5)/10 + 32;
            ok = true;
        }
    }

    return ok;
}

bool DS18S20::IsParasitePowered () const {
    return mParasitePowered;
}

const uint8_t* DS18S20::GetAddress () const {
    return mAddress;
}

bool DS18S20::SendCommand (uint8_t inCommand, bool inPower) {
    bool ok (false);
    if (mDS.reset()) {
        mDS.select(mAddress);
        mDS.write(inCommand, inPower ? 1 : 0); // start conversion, with parasite power on at the end
        ok = true;
    }

    return ok;
}


bool DS18S20::ReadScratchPad (uint8_t* outScratchPad, uint8_t inSize) {
    constexpr uint8_t kReadScratchPadCommand = {0xBE};
    if (SendCommand (kReadScratchPadCommand, false)) {
        mDS.read_bytes (outScratchPad, inSize);
        constexpr uint8_t kCRCIndex {8};

        if (mDS.crc8 (outScratchPad, kCRCIndex) == outScratchPad[kCRCIndex]) {
            return true;
        }
        else {
            mLogStream << '[' << mDS.GetPin () << "] Data CRC error\n";
        }
    }        

    return false;
}

void DS18S20::ReadPowerSupplyType () {
    constexpr uint8_t kReadPowerSupplyCommand = {0xB4};
    if (SendCommand (kReadPowerSupplyCommand, false)) {
        mParasitePowered = (mDS.read () != 0xFF);

        PrintAddress(mLogStream, mAddress);
        mLogStream << " is " << (mParasitePowered ? "" : "not ") << "parasite powered\n";
    }
}

void DS18S20::ResetAddress () {
    memset (mAddress, 0, kAddressSize);
}
