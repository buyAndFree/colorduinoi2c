/*
  Arduino I2C to Colorduino Demo

  Upload this sketch to the Colorduino (use USB to Serial module)
  
  Patched so modifying the Wire.h library does not need to be modified.
  Compiles properly now on latest Arduino IDE (using new Wire library
   
  based on 
  -arduino firmware by michael vogt <michu@neophob.com>
  -blinkm firmware by thingM
  -"daft punk" firmware by Scott C / ThreeFN 

  This DEMO is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This DEMO is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <Wire.h>
#include <Colorduino.h>          // colorduino library

#define I2C_DEVICE_ADDRESS 0x05  // I2C address for this device 
#define START_OF_DATA 0x10       // data markers
#define END_OF_DATA 0x20         // data markers

void setup() {
  Colorduino.Init();
  // compensate for relative intensity differences in R/G/B brightness
  // array of 6-bit base values for RGB (0~63)
  // whiteBalVal[0]=red
  // whiteBalVal[1]=green
  // whiteBalVal[2]=blue
  unsigned char whiteBalVal[3] = {33,63,63}; // for LEDSEE 6x6cm round matrix
  Colorduino.SetWhiteBal(whiteBalVal);
  
  Wire.begin(I2C_DEVICE_ADDRESS);  // join i2c bus with address #8
  Wire.onReceive(receiveEvent);    // register event
  //Serial.begin(9600);              // start serial for output
}

void loop() {
  delay(1000);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int num) {
  while (Wire.available()) { // loop through all
    if (Wire.read() != START_OF_DATA) {
      //else handle error by reading remaining data until end of data marker (if available)
      while (Wire.available()>0 && Wire.read()!=END_OF_DATA) {}      
      return;
    }
    byte c = Wire.read(); // read our color byte so we know if these are the R, G or B pixels. (0 = r, 1 = g, 2 = b)
    byte i = Wire.read(); // iteration (due to max buffer size of 32bytes, need multiple transmissions for 192 values (8x8xNumber of Colours)
    if (c == 0){
      for (byte y = 0; y < 2; y++) { 
        for (byte x = 0; x < 8; x++) {
          PixelRGB *p = Colorduino.GetPixel(x,2*i+y);
          p->r = Wire.read();
        }
      }
    }
    if (c == 1){
      for (byte y = 0; y < 2; y++) { 
        for (byte x = 0; x < 8; x++) {
          PixelRGB *p = Colorduino.GetPixel(x,2*i+y);
          p->g = Wire.read();
        }
      }
    }
    if (c == 2){
      for (byte y = 0; y < 2; y++) { 
        for (byte x = 0; x < 8; x++) {
          PixelRGB *p = Colorduino.GetPixel(x,2*i+y);
          p->b = Wire.read();
        }
      }
    }
    if (Wire.read()==END_OF_DATA) {
      //if colour is blue and it's on the last iteration, then update display
      if (c == 2 && i == 3) {
        Colorduino.FlipPage();
      }
    }   
  }
}
