neopixels_light_scanner
=============================
<p align="center">
    <img src="images/demoNLS480.gif">
</p>


Circuit and its programming to make the effect of lights moving from one side to the other on a bar of 8 neopixels in the style of the car (KITT) of the Knight Rider series.
Depending on the operating mode, which can be configured via the source code before compiling, it can be used: with the neopixel bar (8 by default) alone or,
with the neopixel bar plus a button that serves to pause/resume the movement of the lights with a short press or turn them on/off by doing a
fade-in or fade-out, this with a long press. Or with the neopixel bar, plus the button and 3 potentiometers that control the speed of the movement of the lights, the
speed of the movement of the lights, the hue and the overall brightness of the lights within limits. And the last mode of
operation, which is using the neopixel bar together with these 3 potentiometers only. Below is a video explaining how it works.

## Assembly diagram (hardware)
Here you can see the components used for the project:
![MONTAJE](images/assembly.png)

A Pro Micro board is used (Atmega32U4 microcontroller at 5V and 16MHz) which is a Chinese clone of the Sparkfun Pro Micro, but it should work with anyone, other Arduino for example, that is compatible with the libraries used, that works at 5V and have enough memory. By making adaptations to the hardware, it could be used with boards that are powered at 3.3V. The pushbutton (digital) and potentiometer (analog) pins can be changed in the source code.

## Source code (software)
Select the corresponding version according to the programming environment to be used.
It is necessary to install the following libraries in the programming environment that you are going to use before compiling the source code. If you use the Arduino IDE, they are installed by searching for them in the *Library Manager* and clicking the *INSTALL* button. In Visual Studio Code with PlatformIO they are installed automatically
when doing the *Build*:

* [FastLED by Daniel Garcia](https://github.com/FastLED/FastLED)
* [FireTimer by PowerBroker2](https://github.com/PowerBroker2/FireTimer)

### Arduino IDE version:
Select the *Arduino Leonardo* in the *Tools menu > Board*, if a Pro Micro is programmed:

[Source code for Arduino IDE](ArduinoIDE/)

### PlatformIO version:
To program the Pro Micro board:

[Source code for PlatformIO](PlatformIO/)

## YouTube video explaining how it works

<div align="center">
      <a href="https://www.youtube.com/watch?v=k_aON715wnU">
         <img src="https://img.youtube.com/vi/k_aON715wnU/0.jpg" style="width:75%;">
      </a>
</div>

## Licence
Read the archive: [LICENSE.md](./LICENSE.md)

## Disclaimer
I, electrozeustic (@ElectroZeusTIC), am not responsible for the use that may be made of the software and hardware used in this project by other people, companies, institutions, etc., in any way.
Offered as is, for experimentation and testing. It may have errors and therefore Do not use in critical environments and situations, as certain dangers may occur with serious consequences. The persons, companies, entities, etc. that use it are solely responsible for the consequences that may occur.
