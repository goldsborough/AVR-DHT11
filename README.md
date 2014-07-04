AVR-DHT11
=========

AVR code to interface with the DHT11 temperature and relative humidity sensors. 

This code is written for Attiny85 @8Mhz, but it shouldn't take more than a 
few seconds to make it work with any other Atmel AVR chip. Just change the clock speed
in the header file and possibly the prescalers to match the speeds described.

As of now, the code fetches the data from the sensor and stores the 4 bytes in an array.
According to the DHT11 protocol, the zeroth byte [0] is the integral part of the humidity,
the first byte [1] is the decimal part of the humidity, the second byte [2] is the integral
part of the temperature and the last byte [3] is the decimal part of the temperature. Note
however that the datasheets for the DHT11 sensor also state that the resolution of the sensor
is 1, so I'm not sure whether the decimal bytes are actually ever used. 


Blog post: http://thecodeinn.blogspot.co.at/2014/07/reading-data-from-dht11-sensor-with-avr.html
