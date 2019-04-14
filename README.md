# ltc2758test
This code is used for initial testing of LTC2756 DAC.
It's a slightly modified version of on Analog Device's official code for LTC2758 (DC1684x board) and Linduino/QuickEval.

Removed some code as indicated in "Going Generic" article:
https://www.analog.com/en/technical-articles/going-generic.html

Code tested on a Teensy 3.2 with SPI connected to LTC2756.
The LTC2756 was connected to a dual opamp for reference/output buffering as indicated in the datasheet.
Reference (preliminary) connected directly to 5V power to reduce component count. 
