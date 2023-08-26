# Arduino-MBUS-Meter

This is an example for Arduino IDE how to create an own MBUS consumption meter (MBUS Slave) for Arduino MKR boards and Feather Boards (including ESP32, RP2040, nRF52840, Cortex M0 ....).


![Data output of the simulated meter in Lorusfree](https://user-images.githubusercontent.com/3049858/263453247-d6c13182-0374-48ee-8505-16257cd2addd.jpg)



## Hardware 

I've used our MBUS Slave wing for feather borads and/or MBUS slave shield for Arduino MKR boards

![MBUS Slave Wing](https://user-images.githubusercontent.com/3049858/263453697-6a00bde5-259d-4733-a12a-3dff900e32d1.jpg)  ![MBUS Slave shield for Arduino MKR](https://user-images.githubusercontent.com/3049858/263453696-eaf3f158-7afa-4ac2-a786-6002ce8581bb.jpg)

You can find all information about the hardware here:
https://www.hwhardsoft.de/english/projects/

The code was tested with the following boards:

* Arduino MKR Wifi 1010
* Adafruit Feather Huzzah32
* Adafruit Feather nRF52840
* Adafruit Feather M0 Express
* Sparkfun ESP32 Thing Plus
* Sparkfun Thing Plus RP2040

Furthermore, an MBUS master is required to supply the MBUS and to be able to exchange data with the slave. For example, you can use a [Raspberry Pi with libmbus and our MBUS Master HAT](https://www.hwhardsoft.de/english/projects/m-bus-rpi-hat/)....


## Remarks

The default baud rate, default bus address and id of the device can be adjusted in mbusslave.h.

Consumption values ​​can be added or edited in the Encode_Payload() function. Be careful with MBUS codes of the "Device" domain. This code does not work or is already contained in the header of the protocol.

You can view the structure of the header in mbusslave.cpp and change parameters, e.g. the manufacturer ID, if necessary.


## Libraries

Install the following librarie through Arduino Library Manager

MBUSPayload Library [MBUSPayload Library](https://github.com/AllWize/mbus-payload)


## Credits 

Based on the awesome work of OpenEnergyMonitor code by Trystan Lea, Glyn Hudson, and others

https://github.com/openenergymonitor/HeatpumpMonitor

and Bryan McLellan  <btm@loftninjas.org>

https://github.com/btm/emonMbus


# License

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
