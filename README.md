# I-Ching
An Arduino driven I-Ching device, that allows one to cast the hexagrams associated with their consultation.
Currently this project is in development. The PCB is now at version 6.

The latest major update was to drive the LEDs and Buzzer through transistors.
The Arduino was burning up under the previous PCBs, and presumably was passing too much current.
The LEDs run at 12.5 mA each (6x) and the buzzer passes about 32 mA.
This adds up to the current passed to the MAX display device, so the voltage regulator on the Nano (from 9V out of the battery
into 5V on the board) was getting way too hot. The v4 PCB also has a mistake where a 5V part was connected to a 9V voltage(!).

Version 6 introduces a Kyrio MP1584EN Buck DC/DC converter to get the 9V battery voltage down to a 5V/3A power supply
for the entire device. The Arduino Nano, the LEDs, Buzzer and MAX will each independently draw their power from the Kyrio board,
so the Nano voltage regulator is bypassed entirely.

Also, the v6 PCB is using the JLC SMT assembly service, which means that the basic components (resistors, caps and
transistors) are placed by the board manufacturer. This will simplify the assembly of the device further.
The facilitate this, the BOM and Pick&Place files have been included in the design.

Parts:
 - Arduino NANO v3
 - Kyrio MT1584EN Buck DC/DC converter
 - .ino Sketch file to load into the Arduino
 - PCB design (PDF, Gerber, BOM and Pick&Place) for a board that connects all components
 - 12x LED
 - 1x push button
 - 1x MAX7219 8x8 LED matrix module with onboard driver

Ferrie J Bank,
Amsterdam 26 June 2020 - 24 October 2020

https://github.com/oudesnepelaar/i-ching
