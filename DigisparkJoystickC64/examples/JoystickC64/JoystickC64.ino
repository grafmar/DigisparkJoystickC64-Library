//DigiJoystickC64 test and usage documentation

#include "DigiJoystickC64.h"

void setup() {
  // Do nothing? It seems as if the USB hardware is ready to go on reset
}

void loop() {
  // If not using plentiful DigiJoystick.delay() calls, make sure to
  //DigiJoystick.update(); // call this at least every 50ms
  // calling more often than that is fine
  // this will actually only send the data every once in a while unless the data is different
  
  // you can set the values from a raw byte array with:
  // char myBuf[8] = {
  //   x, y, y, y, y,
  //   buttons<<4 | 0x0F, buttons >> 4, 0x40
  // };
  // DigiJoystick.setValues(myBuf);
  
  // Or we can also set values like this:
  DigiJoystick.setX((byte) 0x7F);
  DigiJoystick.setY((byte) 0x7F);

  DigiJoystick.delay(3000); // wait 3s
  // X-Axis 3x left-center-right-center
  for(int i=0;i<3;i++){
    DigiJoystick.setX((byte) 0x00); // left
    DigiJoystick.delay(100);
    DigiJoystick.setX((byte) 0x7F); // center
    DigiJoystick.delay(200);
    DigiJoystick.setX((byte) 0xFF); // right
    DigiJoystick.delay(100);
    DigiJoystick.setX((byte) 0x7F); // center
    DigiJoystick.delay(200);
  }
  
  DigiJoystick.delay(1000); // wait 1s

  // >-Axis 3x up-center-down-center
  for(int i=0;i<3;i++){
    DigiJoystick.setY((byte) 0x00); // up
    DigiJoystick.delay(100);
    DigiJoystick.setY((byte) 0x7F); // center
    DigiJoystick.delay(200);
    DigiJoystick.setY((byte) 0xFF); // down
    DigiJoystick.delay(100);
    DigiJoystick.setY((byte) 0x7F); // center
    DigiJoystick.delay(200);
  }
  
  // button 1 to 8 each 1s
  for(int i=0;i<8;i++){
    DigiJoystick.setButtons((byte) (0x01 << i));
    DigiJoystick.delay(1000); // wait 1s
  }
  DigiJoystick.setButtons((byte) 0x00);

  // it's best to use DigiJoystick.delay() because it knows how to talk to
  // the connected computer - otherwise the USB link can crash with the 
  // regular arduino delay() function
  // DigiJoystick.delay(50); // wait 50 milliseconds
  
  // we can also set buttons like this (lowByte, highByte)
  //DigiJoystick.setButtons(0x00, 0x00);
}
