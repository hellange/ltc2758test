/*!
Linear Technology DC1684AA Demonstration Board.
LTC2758: Dual Serial 18-Bit SoftSpan IOUT DAC

Modified by Helge Langehaug, 2019:
 - Independent on DC1684 and Linduino. Tested with Teensy with a LTC2756
 - Added pulse menu 

@verbatim
NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

   An external +/- 15V power supply is required to power the circuit.

  Explanation of Commands:
   1- Select DAC
   Select between DAC A, DAC B, or both.

   2- Change Span of Selected DAC
   |   Command   | Range Selected |
   | C3 C2 C1 C0 |                |
   |-------------|----------------|
   | 1  0  0  0  |    0V - 5V     |
   | 1  0  0  1  |    0V - 10V    |
   | 1  0  1  0  |   -5V - +5V    |
   | 1  0  1  1  |  -10V - +10V   |
   | 1  1  0  0  | -2.5V - +2.5V  |
   | 1  1  0  1  | -2.5V - +7V    |

   3- Voltage Output
   Displays the calculated voltage depending on the code input from user and
   voltage range selected.

   4- Square wave output
   Generates a square wave on the output pin. This function helps to measure
   settling time and glitch impulse.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2758

http://www.linear.com/product/LTC2758#demoboards

REVISION HISTORY
$Revision: 3489 $
$Date: 2015-05-26 11:02:02 -0700 (Tue, 26 May 2015) $

Copyright (c) 2013, Linear Technology Corp.(LTC)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of Linear Technology Corp.

The Linear Technology Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.
*/

/*! @file
    @ingroup LTC2758
*/

// Headerfiles
#include <SPI.h>
#include "LTC2758.h"
#include "UserInterface.h"
// Global variables
 
float DACA_RANGE_LOW = 0;
float DACA_RANGE_HIGH = 5;

float DACB_RANGE_LOW = 0;
float DACB_RANGE_HIGH = 5;

uint8_t DAC_SELECTED = ADDRESS_DACA;

// Function Declarations
void print_title();
void print_prompt();
uint8_t menu1_select_dac();
void menu2_change_range();
uint8_t menu3_voltage_output();
uint8_t menu4_square_wave_output();
uint8_t menu5_pulse_output();
uint8_t menu6_sequence_output();

void setup()
{

  Serial.begin(115200);             // Initialize the serial port to the PC
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(12,INPUT);
  pinMode(13,OUTPUT);
  SPI.begin();
  SPI.beginTransaction (SPISettings (2000000, MSBFIRST, SPI_MODE0));
  
  print_title();

  LTC2758_write(LTC2758_CS, LTC2758_WRITE_SPAN_DAC, ADDRESS_DAC_ALL, 0);  // initialising all channels to 0V - 5V range
  float v = voltage_to_code_adj(0.56789, DACA_RANGE_LOW, DACA_RANGE_HIGH, true);
  LTC2758_write(LTC2758_CS, LTC2758_WRITE_CODE_UPDATE_DAC, DAC_SELECTED, v); // initialize with a value to see that output works

}

uint32_t voltage_to_code_adj(float dac_voltage, float min_output, float max_output, bool serialOut){
  dac_voltage = dac_voltage - 0.000930; // offset
  dac_voltage = dac_voltage * 5.0/4.99930; // gain
  return LTC2758_voltage_to_code(dac_voltage, min_output, max_output, serialOut);
}
void loop()
{
  int16_t user_command;
  if (Serial.available())               // Check for user input
  {
    user_command = read_int();          // Read the user command
    Serial.println(user_command);
    Serial.flush();
    switch (user_command)
    {
      case 1:
        menu1_select_dac();
        break;
      case 2:
        menu2_change_range();
        break;
      case 3:
        menu3_voltage_output();
        break;
      case 4:
        menu4_square_wave_output();
        break;
      case 5:
        menu5_pulse_output();
        break;      
      case 6:
        menu6_sequence_output();
      default:
        Serial.println(F("Incorrect Option"));
        break;
    }
    Serial.println(F("\n************************************************************"));
    print_prompt();
  }
}

//! Prints the title block when program first starts.
void print_title()
{
  Serial.println();
  Serial.println(F("*****************************************************************"));
  Serial.println(F("* Testing LTC275x DAC                                           *"));
  Serial.println(F("* Based on DC1684A-A Demonstration Program                      *"));  Serial.println(F("*                                                               *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* Set the baud rate to 115200 and select the newline terminator.*"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("*****************************************************************\n"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.println(F("\nCommand Summary:"));
  Serial.println(F("\n  1. Select DAC"));
  Serial.println(F("  2. Change Span of selected DAC"));
  Serial.println(F("  3. Voltage Output"));
  Serial.println(F("  4. Square wave output"));
  Serial.println(F("  5. Pulse output"));
  Serial.println(F("  6. Sequence output"));

  Serial.println(F("\nPresent Values:\n"));
  Serial.print(F("  DAC A Range: "));
  Serial.print(DACA_RANGE_LOW);
  Serial.print(F(" V to "));
  Serial.print(DACA_RANGE_HIGH);
  Serial.println(F(" V"));

  Serial.print(F("  DAC B Range: "));
  Serial.print(DACB_RANGE_LOW);
  Serial.print(F(" V to "));
  Serial.print(DACB_RANGE_HIGH);
  Serial.println(F(" V"));

  Serial.print(F("\n  Selected DAC: "));
  switch (DAC_SELECTED)
  {
    case ADDRESS_DACA:
      Serial.println(F("DAC A (choose this for LTC2756)"));
      break;
    case ADDRESS_DACB:
      Serial.println(F("DAC B (don't choose this for LTC2756)"));
      break;
    case ADDRESS_DAC_ALL:
      Serial.println(F("ALL DACs"));
      break;
  }

  Serial.print(F("\n\nEnter a command: "));
  Serial.flush();
}

//! Function to select DAC and set DAC address
uint8_t menu1_select_dac()
{
  uint8_t choice;
  Serial.println(F("\n1. DAC A (or use LTC2756)"));
  Serial.println(F("2. DAC B"));
  Serial.println(F("3. All DACs"));
  Serial.print(F("\nEnter a choice: "));
  choice = read_int();          // Read the user command

  switch (choice)
  {
    case 1:
      DAC_SELECTED = ADDRESS_DACA;
      Serial.println("DAC A");
      break;
    case 2:
      DAC_SELECTED = ADDRESS_DACB;
      Serial.println("DAC B");
      break;
    default:
      DAC_SELECTED = ADDRESS_DAC_ALL;
      Serial.println("ALL DACs");
      break;
  }
  Serial.flush();
  return 0;
}

//! Function to choose the range of voltages to be used
void menu2_change_range()
{
  uint8_t choice;
  uint32_t span;
  Serial.println("\n| Choice | Range         |");
  Serial.println("|--------|---------------|");
  Serial.println("|    0   |    0 - 5 V    |");
  Serial.println("|    1   |    0 - 10 V    |");
  Serial.println("|    2   |   -5 - +5 V   |");
  Serial.println("|    3   |  -10 - +10 V  |");
  Serial.println("|    4   | -2.5 - +2.5 V |");
  Serial.println("|    5   | -2.5 - +7.5 V |");

  Serial.print("\nEnter your choice: ");
  choice = read_int();
  Serial.println(choice);
  span = (uint32_t)(choice << 2);
  if (DAC_SELECTED == ADDRESS_DACA || DAC_SELECTED == ADDRESS_DAC_ALL)
  {
    switch (choice)
    {
      case 0:
        DACA_RANGE_LOW = 0;
        DACA_RANGE_HIGH = 5;
        break;

      case 1:
        DACA_RANGE_LOW = 0;
        DACA_RANGE_HIGH = 10;
        break;

      case 2:
        DACA_RANGE_LOW = -5;
        DACA_RANGE_HIGH = 5;
        break;

      case 3:
        DACA_RANGE_LOW = -10;
        DACA_RANGE_HIGH = 10;
        break;

      case 4:
        DACA_RANGE_LOW = -2.5;
        DACA_RANGE_HIGH = 2.5;
        break;

      case 5:
        DACA_RANGE_LOW = -2.5;
        DACA_RANGE_HIGH = 7.5;
        break;

      default:
        Serial.println("\nWrong choice!");
    }
    Serial.print(F("Span Changed!"));
  }
  if (DAC_SELECTED == ADDRESS_DACB || DAC_SELECTED == ADDRESS_DAC_ALL)
  {
    switch (choice)
    {
      case 0:
        DACB_RANGE_LOW = 0;
        DACB_RANGE_HIGH = 5;
        break;

      case 1:
        DACB_RANGE_LOW = 0;
        DACB_RANGE_HIGH = 10;
        break;

      case 2:
        DACB_RANGE_LOW = -5;
        DACB_RANGE_HIGH = 5;
        break;

      case 3:
        DACB_RANGE_LOW = -10;
        DACB_RANGE_HIGH = 10;
        break;

      case 4:
        DACB_RANGE_LOW = -2.5;
        DACB_RANGE_HIGH = 2.5;
        break;

      case 5:
        DACB_RANGE_LOW = -2.5;
        DACB_RANGE_HIGH = 7.5;
        break;

      default:
        Serial.println("\nWrong choice!");
    }
    Serial.print(F("Span Changed!"));
  }
  LTC2758_write(LTC2758_CS, LTC2758_WRITE_SPAN_DAC, DAC_SELECTED, span);
}

//! Function to enter a digital value and get the analog output
uint8_t menu3_voltage_output()
{
  uint8_t choice;
  uint32_t data;
  float voltage;

  Serial.println(F("\n1. Enter Voltage"));
  Serial.println(F("2. Enter Code"));
  Serial.print(F("\nEnter a choice: "));
  choice = read_int();          // Read the user command
  Serial.print(choice);

  if (choice == 2)
  {
    Serial.print("\nEnter the 18-bit data as decimal or hex: ");
    data = read_int();
    Serial.print("0x");
    Serial.println(data, HEX);

    if (DAC_SELECTED == ADDRESS_DACA || DAC_SELECTED == ADDRESS_DAC_ALL)
    {
      voltage = LTC2758_code_to_voltage(data, DACA_RANGE_LOW, DACA_RANGE_HIGH);
      Serial.print("\nDACA Output voltage = ");
      Serial.print(voltage);
      Serial.println(" V");
    }

    if (DAC_SELECTED == ADDRESS_DACB || DAC_SELECTED == ADDRESS_DAC_ALL)
    {
      voltage = LTC2758_code_to_voltage(data, DACB_RANGE_LOW, DACB_RANGE_HIGH);
      Serial.print("\nDACB Output voltage = ");
      Serial.print(voltage);
      Serial.println(" V");
    }
    LTC2758_write(LTC2758_CS, LTC2758_WRITE_CODE_UPDATE_DAC, DAC_SELECTED, data);
  }
  else if (choice == 1)
  {
    Serial.print("\nEnter voltage: ");
    while (!Serial.available());
    voltage = read_float();
    Serial.print(voltage);
    Serial.println(" V");

    if (DAC_SELECTED == ADDRESS_DACA || DAC_SELECTED == ADDRESS_DAC_ALL)
    {
      data = voltage_to_code_adj(voltage, DACA_RANGE_LOW, DACA_RANGE_HIGH, true);
      LTC2758_write(LTC2758_CS, LTC2758_WRITE_CODE_UPDATE_DAC, DAC_SELECTED, data);
      Serial.print("\nDACA Output voltage = ");
      Serial.print(voltage,6);
      Serial.println(" V");
      Serial.print("\nDAC CODE: 0x");
      Serial.println(data, HEX);
    }

    if (DAC_SELECTED == ADDRESS_DACB || DAC_SELECTED == ADDRESS_DAC_ALL)
    {
      data = voltage_to_code_adj(voltage, DACB_RANGE_LOW, DACB_RANGE_HIGH, true);
      LTC2758_write(LTC2758_CS, LTC2758_WRITE_CODE_UPDATE_DAC, DAC_SELECTED, data);
      Serial.println(DACB_RANGE_HIGH);
      Serial.print("\nDACB Output voltage = ");
      Serial.print(voltage,6);
      Serial.println(" V");
      Serial.print("\nDAC CODE: 0x");
      Serial.println(data, HEX);
    }
  }
  return 0;
}

//! Function to generate a square wave of desired frequency and voltage ranges
uint8_t menu4_square_wave_output()
{
  uint16_t freq;
  float time;
  float voltage_high, voltage_low;
  uint32_t code_high, code_low;
  uint8_t receive_enter;  // To receive enter key pressed

  Serial.print("\nEnter voltage_high: ");
  while (!Serial.available());
  voltage_high = read_float();
  Serial.print(voltage_high);
  Serial.println(" V");

  Serial.print("\nEnter voltage_low: ");
  while (!Serial.available());
  voltage_low = read_float();
  Serial.print(voltage_low);
  Serial.println(" V");

  Serial.print("\nEnter the required frequency in Hz: ");
  freq = read_int();
  Serial.print(freq);
  Serial.println(" Hz");

  time = (float)1000/freq;
  Serial.print("\nT = ");
  Serial.print(time);
  Serial.println(" ms");

  //! Converting voltage into data
  if (DAC_SELECTED == ADDRESS_DACA || DAC_SELECTED == ADDRESS_DAC_ALL)
  {
    code_high = voltage_to_code_adj(voltage_high, DACA_RANGE_LOW, DACA_RANGE_HIGH, true);
    code_low = voltage_to_code_adj(voltage_low, DACA_RANGE_LOW, DACA_RANGE_HIGH, true);
  }
  if (DAC_SELECTED == ADDRESS_DACB || DAC_SELECTED == ADDRESS_DAC_ALL)
  {
    code_high = voltage_to_code_adj(voltage_high, DACB_RANGE_LOW, DACB_RANGE_HIGH, true);
    code_low = voltage_to_code_adj(voltage_low, DACB_RANGE_LOW, DACB_RANGE_HIGH, true);
  }

  while (!Serial.available()) //! Generate square wave until a key is pressed
  {
          digitalWrite(9,HIGH);

    LTC2758_write(LTC2758_CS, LTC2758_WRITE_CODE_UPDATE_DAC, DAC_SELECTED, code_high);
    delayMicroseconds(time * 500);
              digitalWrite(9,LOW);

    LTC2758_write(LTC2758_CS, LTC2758_WRITE_CODE_UPDATE_DAC, DAC_SELECTED, code_low);
    delayMicroseconds(time * 500);
  }
  receive_enter = read_int();
  return 0;
}

uint8_t menu5_pulse_output()
{
  uint16_t freq;
  uint16_t pw;
  float time;
  float voltage_high, voltage_low;
  uint32_t code_high, code_low;
  uint8_t receive_enter;  // To receive enter key pressed

  Serial.print("\nEnter voltage_high: ");
  while (!Serial.available());
  voltage_high = read_float();
  Serial.print(voltage_high);
  Serial.println(" V");

  Serial.print("\nEnter voltage_low: ");
  while (!Serial.available());
  voltage_low = read_float();
  Serial.print(voltage_low);
  Serial.println(" V");

  Serial.print("\nEnter the required frequency in Hz: ");
  freq = read_int();
  Serial.print(freq);
  Serial.println(" Hz");

  Serial.print("\nEnter the pulse width in uS: ");
  pw = read_int();
  Serial.print(pw);
  Serial.println(" uS");

  time = (float)1000/freq;
  Serial.print("\nT = ");
  Serial.print(time);
  Serial.println(" ms");

  //! Converting voltage into data
  if (DAC_SELECTED == ADDRESS_DACA || DAC_SELECTED == ADDRESS_DAC_ALL)
  {
    code_high = voltage_to_code_adj(voltage_high, DACA_RANGE_LOW, DACA_RANGE_HIGH, true);
    code_low = voltage_to_code_adj(voltage_low, DACA_RANGE_LOW, DACA_RANGE_HIGH, true);
  }
  if (DAC_SELECTED == ADDRESS_DACB || DAC_SELECTED == ADDRESS_DAC_ALL)
  {
    code_high = voltage_to_code_adj(voltage_high, DACB_RANGE_LOW, DACB_RANGE_HIGH, true);
    code_low = voltage_to_code_adj(voltage_low, DACB_RANGE_LOW, DACB_RANGE_HIGH, true);
  }

  while (!Serial.available()) //! Generate square wave until a key is pressed
  {

    LTC2758_write(LTC2758_CS, LTC2758_WRITE_CODE_UPDATE_DAC, DAC_SELECTED, code_high);
    delayMicroseconds(pw);

    LTC2758_write(LTC2758_CS, LTC2758_WRITE_CODE_UPDATE_DAC, DAC_SELECTED, code_low);
    delayMicroseconds(time*1000 - pw);

  }
  receive_enter = read_int();
  return 0;
}

uint8_t menu6_sequence_output()
{
  uint32_t sw;
  int16_t dir = 1;
  float time;
  float voltage_high, voltage_low, voltage, step;
  uint32_t code;
  uint8_t receive_enter;  // To receive enter key pressed

  Serial.print("\nEnter voltage_high: ");
  while (!Serial.available());
  voltage_high = read_float();
  Serial.print(voltage_high);
  Serial.println(" V");

  Serial.print("\nEnter voltage_low: ");
  while (!Serial.available());
  voltage_low = read_float();
  Serial.print(voltage_low);
  Serial.println(" V");

  Serial.print("\nEnter voltage step: ");
  step = read_float();
  Serial.print(step);
  Serial.println(" V");

  Serial.print("\nEnter the step width in uS: ");
  sw = read_int();
  Serial.print(sw);
  Serial.println(" uS");
  voltage = voltage_low;
  while (!Serial.available()) //! Generate square wave until a key is pressed
  {
    //! Converting voltage into data
    if (DAC_SELECTED == ADDRESS_DACA || DAC_SELECTED == ADDRESS_DAC_ALL)
    {
      code = voltage_to_code_adj(voltage, DACA_RANGE_LOW, DACA_RANGE_HIGH, false);
    }
    if (DAC_SELECTED == ADDRESS_DACB || DAC_SELECTED == ADDRESS_DAC_ALL)
    {
      code = voltage_to_code_adj(voltage, DACB_RANGE_LOW, DACB_RANGE_HIGH, false);
    }
  
    LTC2758_write(LTC2758_CS, LTC2758_WRITE_CODE_UPDATE_DAC, DAC_SELECTED, code);
    delayMicroseconds(sw);
    voltage = voltage + step * dir;
    if (voltage >= voltage_high) {
      dir = -1;
    }
    if (voltage <= voltage_low) {
      dir = 1;
    }

  }
  receive_enter = read_int();
  return 0;
}
