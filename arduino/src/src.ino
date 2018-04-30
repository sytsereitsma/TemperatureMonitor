#include "utils.h" 
#include "DS18S20.h" 
#include "OneWireWrapper.h" 
#if !__AVR__
#include "StdOutStream.h" 
#endif


namespace {
#if __AVR__
    Print& sLogStream = Serial;
#else
    StdOutStream sStreamInstance;
    Print& sLogStream = sStreamInstance;
#endif
}

void setup() {
    Serial.begin(9600);
}

int pin (2);
void loop() {
    for (int pin=2; pin <= 5; ++pin) {
        OneWireWrapper wrapper (pin);
        DS18S20 sensor (wrapper, sLogStream);
        float temperature;
        if (sensor.GetMeasurement (temperature)) {
            sLogStream << OneWireAddress {sensor.GetAddress ()} << ':' << temperature << "\n";
        }
    }

    delay(1000);
}
