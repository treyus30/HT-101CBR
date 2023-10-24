# HT-101CBR
**NOTE: This project is currently early stages and meant for a small selection of members in external contact with me. If you'd like to help with testing/early access hardware packages, DM me on TacomaWorld @treyus30. **
# Announcement
Haltech has recently made _all_ CAN IDs and datatypes Public! This has increased the available "sensor" data from around 30 types, to over 250, thus prompting a complete re-write of this program! 
I have decided to take a very different approach and use a shift-register basis of updating displays, which means that display quantities will now be loosely capped at 32 instead of 8, and Analog Pins 0-7 will be reserved for future implementation of external sensor inputs, increasing the effective pins of your Haltech ECU!

Work has progressed slowly, as this is one of many facets of my life, but I have recently finished prototyping a 2x2 OLED plug-and-play PCB with the mentioned shift register logic. Parts are currently processing overseas and should be ready before 2024 for testing. I plan on releasing DIY and pre-assembled kits for purchase in conjunction with the new free-to-share binaries. Up to v0.16 will use the old Analog Chip Select method, and going forward will require at least one SN74HC595N IC (~$1.50) in addition to the previous hardware requirements. I think this will be a highly beneficial trade-off in the future. 

## Overview
**What is this, exactly, and why?**
The Haltech 101 CAN Bridge is a DIY solution to circumvent the expense of gauge and dash displays offered by Haltech themselves (and some others) which merely display data that the vehicle's Haltech ECU already stores. Haltech uses proprietary CAN protocols for its family of devices, but some of these sensors' IDs and data/units have been cracked, and we can read them. Haltech does support OBD-II based CAN, but because only their higher-end ECUs like the Elite 2500 have 2 ports, this leaves lesser models like the 750s and similar stranded if they need their single port for something like a wideband O2 sensor module. Furthermore, OBD-II is limited in it's status reporting and Haltech's list of supported sensors is fairly small at the moment, though it does provide a majority of the important ones. 

This project seeks to enable reading of this proprietary set of standards, and allow the end-user to display this data very inexpensively in a highly readable, rapid manner. Further development will seek to expand the CANbus to the OBD-II port and support industry standard reporting so that devices like the ELM327 (common bluetooth/wifi OBDII readers) can read statuses just like any other vehicle.

The program is built on ATmega328 platform, mainly the Arduino Nano, though it should work without any port work with the Uno as well. The code is optimized for extremely fast display updating over hardware SPI, utilizing a codeblock to breakout numbers into indidivual digits, then compare these digits to the last digits recorded for a given CAN reading, to only update characters that change from one loop to the next. 

Up to 8 SSD1306 128x64pixel OLED displays are supported (Nano; 6 for Uno), with their Chip Select (CS) pins being the 8 analog pins, A0 - A7. 

## Uploading Binaries (.hex)
I'm keeping the source code private for now for various reasons, so you will be flashing the releases as a pre-compiled binary directly onto the device. The easiest way to do this is Xloader. (https://github.com/binaryupdates/xLoader)

![image](https://github.com/treyus30/HT-101CBR/assets/136277393/461253f2-5f9f-4130-843d-aaec86dda0bb)

Save the zip file somewhere accessible, run Xloader.exe, and select the correct device and Hex file. 

![image](https://github.com/treyus30/HT-101CBR/assets/136277393/adf06e8c-4121-4211-802e-b4623bbf302f)

Make sure your Arduino is plugged in, and COM port is known and set. Then click Upload. 

## Parts & Pre-reqs

**Required:**
- Arduino Nano or Uno (authentic or clone).
- 1-8x SSD1306 monochrome **SPI** 128x64 OLED displays. **Note that the Uno is missing Pins A6 and A7, so is limited to 6 displays. **These must be 7-pin SPI displays. Do NOT get the 4-pin I2C version.**
- 1x MCP2515 CAN Transceiver Module (make sure this is the full board, as pictured below).
- Some method of wiring.
- Some method of splitting connections. I recommend covered, low-amp screw terminal blocks.
- Laptop with USB port & **Mini** (for Nano) or **2.0 Type B** (for Uno) USB cable. 
  
**Optional:**
- DTM06-4S (female) connector set (for connecting directly into Haltech CAN port). You can find knockoff kits for reasonable prices on Amazon.
- 16PIN J1962F OBD-II Female pigtail (for gaining access to the new CANbus over OBDII). This will become more relevant in the future. ~$12 on Amazon. 

![image](https://github.com/treyus30/HT-101CBR/assets/136277393/64dd7140-89de-436d-ac0f-f3dddb5633d8) ![Capture](https://github.com/treyus30/HT-101CBR/assets/136277393/be5876b0-9810-407e-9b40-d968fcf0984e)



## Wiring
![image](https://github.com/treyus30/HT-101CBR/assets/136277393/f193cb99-d602-4b6f-9f1d-fdcd29ef7cde)

![image](https://github.com/treyus30/HT-101CBR/assets/136277393/7e3e6e5f-cf13-4c77-99e8-f6e3c9581b88)

*Word of caution: SPI is an unbalanced protocol (not differential pair). Keep SPI CLK and DATA wires, especially, away from stray capacitance and other wires. 

## Supported Sensors
The following is what has been uncovered by FTY Racing and currently implemented as options. If more are decoded separately, I will asterisk them. If you can add to this list, please create a discussion and include the CAN ID, byte(s), and name. Note that EGTs 1-12 have been deliberately excluded. 
- Engine RPM
- MAP
- TPS
- Fuel Pressure
- Oil Pressure
- Injector Duty Cycles
- Ignition Angles
- Lambdas
- Miss, Trigger, and Home Counter
- Vehicle Speed
- Gear
- Intake Cam Angles
- Battery Voltage
- Target Boost
- Barometer
- CTS
- IAT
- Fuel Temp
- Oil Temp
- Fuel Consumption (/hr)
- Avg Fuel Economy (/hr)

# Example
https://www.youtube.com/watch?v=qoIh3fBZvfI

**See "MANUAL....xlsx" file in repository for user commands. You will need the Arduino IDE, or some other way of communicating via the hardware Serial to set up your initial sensors and preferences.**

---

## Donate
This has taken a lot of my time and I will receive minimal reward. If you think it's worth it, consider sparing some change :) <br>
<a href ="https://tcarpenterprises.square.site/product/help-support-my-work-tip-me/1?cs=true&cst=popular">Square</a><br>
<a href ="https://paypal.me/TCarpEnterprises?country.x=US&locale.x=en_US">PayPal</a>

---

## License
This work is licensed under a `Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International License`.
This work is free to redistribute for personal, non-commercial use. Any modifications performed are not licensed for redistribution. 
