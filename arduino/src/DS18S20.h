#ifndef _H_DS18B20
#define _H_DS18B20
#include "constants.h"
#include "Sensor.h"

class IOneWire;
class Print;

class DS18S20 : public Sensor {
public:
    DS18S20 (IOneWire& inOneWire, Print& inLogStream);

    bool Detect ();

    bool StartConversion ();
    bool GetTemperature (float& outTemp);
    bool IsParasitePowered () const;

    const uint8_t* GetAddress () const;
private:
    bool SendCommand (uint8_t inCommand, bool inPower);
    bool ReadScratchPad (uint8_t* outScratchPad, uint8_t inSize);
    void ReadPowerSupplyType ();
    void ResetAddress ();
private:
    uint8_t mAddress [kAddressSize];
    IOneWire& mDS;
    Print& mLogStream;
    bool mParasitePowered;
};

#endif  //_H_DS18B20
