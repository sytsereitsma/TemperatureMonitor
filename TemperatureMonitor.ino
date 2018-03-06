#include <OneWire.h> 
#include <PrintEx.h> 

namespace {
    constexpr uint8_t kAddressSize {8};
    constexpr uint8_t kDS18S20FamilyCode {0x10};
    constexpr uint8_t kDS18B20FamilyCode {0x28};

    void PrintAddress(Stream& outStream, const uint8_t* inAddress) {
        const char* kToHex {"0123456789ABCDEF"};

        for (uint8_t t = 0; t < kAddressSize; ++t) {
            const uint8_t part (inAddress [t]);
            outStream.write (kToHex [(part >> 4) & 0xF]);
            outStream.write (kToHex [part & 0xF]);
        }
    }
}

class DS18S20 {
public:
    DS18S20 (int pin) :
        mPin (pin),
        mDS (pin)
    {
        mAddress [0] = 0;
        Detect ();
    }

    bool GetTemp (float& outTemp) {
        if (mAddress [0] == 0) {
            return false;
        }

        bool ok (false);
        if (ConvertTemperature ()) {
            constexpr uint8_t kScratchPadSize { 9u };
            uint8_t data [kScratchPadSize];
            if (ReadScratchPad (data, kScratchPadSize)) {
                uint8_t LSB = data [0];
                uint8_t MSB = data [1];

                float tempRead = ((MSB << 8) | LSB); //using two's compliment
                float TemperatureSum = tempRead / 16;

                outTemp = TemperatureSum;//Fahrenheit: (TemperatureSum * 18 + 5)/10 + 32;
                ok = true;
            }
        }

        return ok;
    }

    const uint8_t* GetAddress () const {
        return mAddress;
    }
private:
    bool SendCommand (uint8_t inCommand, bool inPower) {
        bool ok (false);
        if (mDS.reset()) {
            mDS.select(mAddress);
            mDS.write(inCommand, inPower ? 1 : 0); // start conversion, with parasite power on at the end
            ok = true;
        }

        return ok;
    }

    bool ConvertTemperature () {
        constexpr uint8_t kConvertTempCommand {0x44};
        return SendCommand (kConvertTempCommand, true);
    }

    bool ReadScratchPad (uint8_t* outScratchPad, uint8_t inSize) {
        constexpr uint8_t kReadScratchPadCommand {0xBE};
        if (SendCommand (kReadScratchPadCommand, false)) {
            mDS.read_bytes (outScratchPad, inSize);
            constexpr uint8_t kCRCIndex {8};

            if (OneWire::crc8 (outScratchPad, kCRCIndex) == outScratchPad[kCRCIndex]) {
                return true;
            }
            else {
                Serial << '[' << int { mPin } << "] Data CRC error\n";
            }
        }        

        return false;
    }

    bool Detect () {
        constexpr uint8_t kCRCIndex {7};

        bool found (false);

        uint8_t addr [kAddressSize];

        if (!mDS.search (addr)) {
            //no more sensors on chain, reset search
            mDS.reset_search ();
            Serial << "No temperature sensor found at pin " << mPin << "\n";

        }
        else if (OneWire::crc8 (addr, kCRCIndex) == addr[kCRCIndex]) {
            Serial << "Device found at pin " << mPin << ": ";
            PrintAddress(Serial, mAddress);
            Serial.println();

            if (addr[0] != kDS18S20FamilyCode && addr[0] != kDS18B20FamilyCode) {
                Serial << "Unrecognized/unsupported 1-Wire family code at pin" << mPin << "\n";
            }
            else {
                found = true;
                memcpy (mAddress, addr, sizeof (mAddress));
            }
        }        
        else {
              Serial << "CRC is not valid for device at pin " << mPin << " (address ";
              PrintAddress(Serial, addr);
              Serial << ", expected CRC " << ios::hex << OneWire::crc8 (addr, kCRCIndex) << ")." << "\n";
        }

        if (found) {
            ReadPowerSupplyType ();
        }

        return found;
    }

    void ReadPowerSupplyType () {
        constexpr uint8_t kReadPowerSupplyCommand {0xB4};
        if (SendCommand (kReadPowerSupplyCommand, false)) {
            mParasitePowered = (mDS.read () != 0xFF);

            PrintAddress(Serial, mAddress);
            Serial << " is " << (mParasitePowered ? "" : "not ") << "parasite powered\n";
        }
    }
private:
    const int mPin;
    uint8_t mAddress [kAddressSize];
    OneWire mDS;
    bool mParasitePowered;

    enum State {
        kReadTemperature,
        kWaitingForConversion,
        kReadResult,
    }
};


void setup() {
    Serial.begin(9600);
}

int pin (2);
void loop() {
    for (int pin=2; pin <= 5; ++pin) {
        DS18S20 sensor (pin);
        float temperature;
        if (sensor.GetTemp (temperature)) {
            PrintAddress(Serial, sensor.GetAddress ());
            Serial << ':';
            Serial.println(temperature);
        }
    }

    delay(1000);
}
