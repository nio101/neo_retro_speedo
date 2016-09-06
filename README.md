# neo_retro_speedo
This is the source code for our **open source neoretro GPS-driven speedometer driver** board, using a Microchip 16F1823 to drive a DC motor through a MOSFET.

## compilation requirements
The 16F1823 source code was compiled with the following tools:
- **MPLAB X v3.40+**
- **XC8 v1.38+**
- MPLAB Code Configurator plugin v3.16+ (*not required for compilation, only to to properly edit/generate uC config and peripherals libs source code*)

## dev notes

## todo list
GPS todo list beforehand:
connect the GPS proto to inboard 12V & use the bus pirate to read the NMEA sentences.
Check the speed, evaluate what happens when the GPS looses the fix => speed==0?

1. wire a 16F1823 with regulators/power supply + pickit3 headers + reset button + status LED output
2. wire the RX/TX & use the UART to a) receive NMEA sentences & b) send GPS commands
3. write a state program that a) checks the GPS status/presence b) filters the NEMA sentences needed (only VTS & fix status) c) pushes the updates to 10Hz for VTS/speed sentences & then only check the fix, and if fix OK, read the speed.
4. wire the MOSFET, motor & send a PWM signal to make it run.
5. link the speedo to the motor & calibrate manually, test drive in board
6. establish a calibration process using pushbutton(s) & status_LED, to select between MPH & KMH & pushing button every 10/20/30 KMH/MPH read on speedo.
*The calibration process can also be used as a factory reset.*
7. write/read those calibration values to/from EEPROM
