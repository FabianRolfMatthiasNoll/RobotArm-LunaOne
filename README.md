# RobotArm-LunaOne
The App and other interesting files to build your own LunaOne Robot Arm

![IMG_20211221_070444](https://user-images.githubusercontent.com/43731826/147088112-ccfa90cb-4773-4f00-a7b4-66fb38f28338.jpg)

## Future Plans

- Removing the "Stand Up" Button from the App and exchange it for a configurable Preset Menu

## Basic Build

#### What you need:
- Power Jack
- 12V to 5V Converter
- Arduino Uno
- Bluetooth Module
- Adafruit PCA9685 PWM Controller
- 5x MG996R Servos
- 3D Printed Parts

![Basic Build](https://user-images.githubusercontent.com/43731826/147089719-1434631e-51ea-4a56-9b6e-ee174c6e3278.PNG)

## 3D Printing

All necessary Files are attached to this subreddit.

For the "Greifer 1" and "Greifer 2" please print it so that the teeth show up.
You need to set "Supports everywhere" for this to achieve.

All Parts can be printed up to 0.2mm. More than that is not tested, especially the
"Greifer" and "Zahnrad" parts.

## How to use the App

![Screenshot_2021-12-22-13-22-37-934_appinventor ai_fabian_noll RoboticArmV2](https://user-images.githubusercontent.com/43731826/147093059-2c50c1b2-923c-422b-a833-d5b779db89d9.jpg) 

After installing the App you first have to pair your phone with the bluetooth module.
You can do this over your phones bluetooth settings. 

> If you are using a new HC-05 Module it will be named that. If you used a configured one the name could differ
> The pairing Password will be 1234 if it wasnt changed

After that you can go into the App and Connect to the Bluetooth Module of the Arm via the [Bluetooth Connect] Button.

Anything else is self explanatory i hope, if not testing and finding out should be easy enough ^^  
A english Version will hopefully soon follow as well as a function for the Stand Up Button.

## The PCBs

### The Bluetooth and PWM-Controller Shield

The PCB is designed to directly house the Bluetooth Module. The Conection to the PWM Controller is via Wires.  
The Order of Connections is: Vcc, SDA, SCL, GND

Schematic             |  PCB
:-------------------------:|:-------------------------:
![Schematic for Shield Bluetooth + Controller](https://user-images.githubusercontent.com/43731826/147091757-9f74ec83-147e-4b3c-9c98-bad9bcac3641.PNG) | ![PCB for Shield Bluetooth + Controller](https://user-images.githubusercontent.com/43731826/147091798-8b20c37f-475e-4171-8d4c-28c87f2b7405.PNG)

### The Hardware Controller

Schematic Controller            |  Schematic Arduino
:-------------------------:|:-------------------------:
![Schematic for Button-Panel](https://user-images.githubusercontent.com/43731826/147092584-5aab7098-7d51-4c06-94dc-6ddfa4add549.PNG) | ![Schematic for Shield Button-Panel](https://user-images.githubusercontent.com/43731826/147092683-b656c1a8-348d-4997-9243-76e09a7911d5.PNG)


