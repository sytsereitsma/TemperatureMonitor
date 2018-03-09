# TemperatureMonitor
Simple wireless temperature monitor using a DS18S20, an Arduino Fio and an XBee module

Measure up to to 4 DS18B20 devices and transmit the data to a XBee Coordinator for storage/processing.

# Prerequisites

## SparkFun DEV-10116

See [here](https://www.sparkfun.com/products/10116)

## SparkFun SEN-11050

See [here](https://www.sparkfun.com/products/11050) for product details.
See [here](https://datasheets.maximintegrated.com/en/ds/DS18S20.pdf) for the DS18S20 datasheet.

|Wire|Signal     |
|-----|----------|
|Red  |VCC (3-5V)|
|Black|GND       |
|White|Signal    |

or (Parsite power mode)

|Wire|Signal     |
|-----|----------|
|Red  |GND|
|Black|GND       |
|White|Signal    |

Note that a 4k7 pullup is required between VCC and Signal (one per bus).

## Arduino 

## Arduino libraries

* [OneWire](https://www.pjrc.com/teensy/td_libs_OneWire.html)
  Also available from Arduino IDE directly.
