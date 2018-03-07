#include <OneWire.h> 
#include <PrintEx.h> 

namespace {
    void PrintAddress(Stream& outStream, const uint8_t* inAddress) {
        const char* kToHex {"0123456789ABCDEF"};

        for (uint8_t t = 0; t < kAddressSize; ++t) {
            const uint8_t part (inAddress [t]);
            outStream.write (kToHex [(part >> 4) & 0xF]);
            outStream.write (kToHex [part & 0xF]);
        }
    }
}



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
