# DigisparkJoystickC64-Library
Library based on DigisparkJoystick adapted to match USB identification of a ["The C64 Joystick"](https://retrogames.biz/thec64-joystick)

## Installation
* [Arduino IDE](https://www.arduino.cc/en/main/software)
* Install [Digistump connection for Arduino IDE](https://digistump.com/wiki/digispark/tutorials/connecting)
* Copy directory to (in Win10): `%UserProfile%\AppData\Local\Arduino15\packages\digistump\hardware\avr\1.6.7\libraries`
* Use example project for first start or implement something like in the project [Digispark C64 Joystick Adapter](https://github.com/grafmar/Digispark_C64_Joystick_Adapter)

## Hardware
The hardware you need is any kind of [digispark](http://digistump.com/products/1) which can also be orded from [Aliexpress](https://www.aliexpress.com/wholesale?SearchText=digispark) or you can even build your [diy (do-it-yourself) version](https://www.instructables.com/Digispark-DIY-The-smallest-USB-Arduino/).

To be able to use 4 IOs (2 IOs are used by USB D+ and D-), you have to [set the fuses to disable the reset](http://thetoivonen.blogspot.com/2015/12/fixing-pin-p5-or-6-on-digispark-clones.html). Doing so you can't programm the ATtiny anymore using a low-voltage-programmer. But you have the bootloader, wich should be enough to program it. If you need to program the bootloader, you can use a high-voltage-programmer.

## Author
[Marco Graf](https://github.com/grafmar)
