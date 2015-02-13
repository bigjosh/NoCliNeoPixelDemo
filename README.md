NoCliNeoPixelDemo
==================

A demonstration showing how to drive WS2812 NeoPixels without ever turning off interrupts. 

These files are written to run on an Arduino Uno, but can be easily changed to work on any AVR.

Note that this code uses Timer2 which is also used by the Arduino `tone()` function. If you need to use `tone()`
(or another library that uses Timer2), then this code can be adapted to use an avaiable Timer. 

More info at:
http://wp.josh.com/2014/05/13/ws2812-neopixels-are-not-so-finicky-once-you-get-to-know-them/
