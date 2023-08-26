# Arduino-MBUS-Meter

This is an example for Arduino IDE how to create an own MBUS consumption meter (MBUS Slave) for Arduino MKR boards and Feather Boards (including ESP32, RP2040, nRF52840, Cortex M0 ....).


![Data output of the simulated meter in Lorusfree](https://user-images.githubusercontent.com/3049858/263453247-d6c13182-0374-48ee-8505-16257cd2addd.jpg)



## Hardware 

I've used our MBUS Slave wing for feather borads and/or MBUS slave shield for Arduino MKR boards

![MBUS Slave Wing](https://user-images.githubusercontent.com/3049858/263453240-bcba954c-a1f5-4691-90cb-c857e5c9d9bf.jpg)

![MBUS Slave shield for Arduino MKR](https://cdn.shopify.com/s/files/1/1509/1638/products/2.Produkt_500x.jpg?v=1604667072)

You can find all information about the hardware here:
https://www.hwhardsoft.de/english/projects/

The code was tested with the following boards:

* Arduino MKR Wifi 1010
* Adafruit Feather Huzzah32
* Adafruit Feather nRF52840
* Adafruit Feather M0 Express
* Sparkfun ESP32 Thing Plus
* Sparkfun Thing Plus RP2040

Furthermore, an MBUS master is required to supply the MBUS and to be able to exchange data with the slave. For example, you can use a Raspberry Pi with libmbus and our MBUS Master HAT....


## Camera

This example was written for the ESP32-CAM camera. 
![My image](https://user-images.githubusercontent.com/3049858/105034491-d8c04300-5a59-11eb-9577-a203aa188705.jpg)

Please install the CameraWebServer example on the ESP32-cam which comes with the Arduino IDE.
![My image](https://user-images.githubusercontent.com/3049858/105033964-1c667d00-5a59-11eb-9af5-de14a0812b04.jpg)

Please choose "ESP Wrover Module" as board and "Huge APP ..." as partition scheme:
![My image](https://user-images.githubusercontent.com/3049858/105033944-17093280-5a59-11eb-8acb-c223c089b0b0.jpg)


The software can be used with other IP cameras too if this cameras can provide a jpg picture via http in QVGA (320 x 240) or VGA (640 x 480) size. Probably some small changes especially in the host address configuration are needed in this case.


## Libraries

Install the following librarie through Arduino Library Manager

MBUSPayload Library [MBUSPayload Library](https://github.com/AllWize/mbus-payload)



## Credits 

This example based on the awesome work of:

* Stefan Fambach    www.fambach.net
* Github user 0015  https://github.com/0015/IdeasNProjects




# License

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
