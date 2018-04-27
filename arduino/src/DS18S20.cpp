#include "DS18S20.h"
#include "IOneWire.h"
#include "utils.h"
#include <Stream.h>
#include "CharBufferPrinter.h"
#include <string.h>


DS18S20::DS18S20 (IOneWire& inOneWire, Print& inLogStream) :
    mDS (inOneWire),
    mLogStream (inLogStream),
    mParasitePowered (false)
{
    ResetAddress ();
}


bool DS18S20::Detect () {

    bool found (false);

    uint8_t addr [kAddressSize];

    if (!mDS.search (addr, true)) {
        //no more sensors on chain, reset search
        mDS.reset_search ();
        mLogStream << "No devices found at pin " << mDS.GetPin () << ".\n";
    }
    else {
        constexpr uint8_t kCRCIndex {7};
        const auto expectedCRC (mDS.crc8 (addr, kCRCIndex));
        if (expectedCRC == addr[kCRCIndex]) {
            mLogStream << "Device found at pin " << mDS.GetPin () << ": " << OneWireAddress {addr} << "\n";

            constexpr uint8_t kDS18S20FamilyCode = {0x28};
            if (addr[0] != kDS18S20FamilyCode) {
                mLogStream << "Unrecognized/unsupported 1-Wire family code at pin" << mDS.GetPin () << "\n";
            }
            else {
                found = true;
                memcpy (mAddress, addr, sizeof (mAddress));

                char nameBuffer [32];
                CharBufferPrinter printer (nameBuffer, 32);
                printer << "TEMP-" << int {mDS.GetPin ()} << '-' << OneWireAddress {mAddress};
                SetName(nameBuffer);
            }
        }        
        else {
              mLogStream << "CRC is not valid for device at pin " << mDS.GetPin ()
                         << " (address " << OneWireAddress {addr}
                         << ", expected CRC 0x" << Hex { expectedCRC }
                         << ", got 0x" << Hex {addr[kCRCIndex]} << ")."
                         << "\n";
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
        mDS.write(inCommand, inPower); // start conversion, with parasite power on at the end
        ok = true;
    }

    return ok;
}


bool DS18S20::ReadScratchPad (uint8_t* outScratchPad, uint8_t inSize) {
    constexpr uint8_t kReadScratchPadCommand = {0xBE};
    if (SendCommand (kReadScratchPadCommand, false)) {
        mDS.read_bytes (outScratchPad, inSize);
        constexpr uint8_t kCRCIndex {8};
        const auto expectedCRC (mDS.crc8 (outScratchPad, kCRCIndex));
        if (expectedCRC == outScratchPad[kCRCIndex]) {
            return true;
        }
        else {
            mLogStream << "Scratchpad CRC is not valid for device " << OneWireAddress {mAddress} << " at pin " << mDS.GetPin ()
                         << " (expected CRC 0x" << Hex { expectedCRC } << ", got 0x" << Hex {outScratchPad[kCRCIndex]} << ").\n";
        }
    }        

    return false;
}

void DS18S20::ReadPowerSupplyType () {
    constexpr uint8_t kReadPowerSupplyCommand = {0xB4};
    if (SendCommand (kReadPowerSupplyCommand, false)) {
        mParasitePowered = (mDS.read () != 0xFF);
        mLogStream << "Device " << OneWireAddress {mAddress} << " at pin " << mDS.GetPin ()
                   << " is " << (mParasitePowered ? "" : "not ") << "parasite powered.\n";
    }
}

void DS18S20::ResetAddress () {
    memset (mAddress, 0, kAddressSize);
}
