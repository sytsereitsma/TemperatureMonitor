#include "utils.h" 
#include "DS18S20.h" 
#include "OneWireWrapper.h" 


void setup() {
    Serial.begin(9600);
}

int pin (2);
void loop() {
    for (int pin=2; pin <= 5; ++pin) {
        OneWireWrapper wrapper (pin);
        DS18S20 sensor (wrapper, Serial);
        float temperature;
        if (sensor.GetTemperature (temperature)) {
            PrintAddress(Serial, sensor.GetAddress ());
            Serial << ':';
            Serial.println(temperature);
        }
    }

    delay(1000);
}
