# HT-101CBR (Lite Version)
For Full Version that supports all sensors through efficient memory management, click here (donationware): https://github.com/T-Carpenter-Enterpises/HT-101CBR_Full-Access. (coming soon)

A Deluxe Version is a long-term goal that will enable J1979 OBDII PID reporting, including emissions readiness emulation, and analog/digial _Inputs_ on the CAN rail back into the Haltech ECU. 

## Overview/Summary
**The Goal: Build the absolute lowest-cost Haltech Elite display possible with as many sensor options as possible.** 

Required parts can all be acquired for less than $10 USD!

The Haltech 101 CAN Bridge is a DIY solution to circumvent the expense of gauge and dash displays offered by Haltech (and some others) which merely display data that the vehicle's Haltech ECU already stores. Haltech uses proprietary CAN protocols for its family of devices, but some of these sensors' IDs and data/units have been cracked (now fully released by Haltech), and we can read them. 

Haltech does support OBD-II based CAN, but because only their higher-end ECUs like the Elite 2500 have 2 ports, this leaves lesser models like the 750s and similar stranded if they need their single port for something dedicated. Furthermore, the HT OBD-II is limited in its status reporting.

The program is built on ATmega328 platform, mainly the Arduino Nano, though it should work without any port work with the Uno as well. The code is optimized for extremely fast display updating over hardware SPI, utilizing a codeblock to breakout numbers into indidivual digits, then compare these digits to the last digits recorded for a given CAN reading, to only update characters that change from one loop to the next. 

Up to 8x SSD1306 or SSD1309 128x64pixel OLED displays are supported (6 for Uno), with their Chip Select (CS) pins being the 8 analog pins, A0 - A7. 

Example setup:

![PXL_20230817_182119066~3](https://github.com/treyus30/HT-101CBR/assets/136277393/47412e7d-a376-46a8-88b0-59214c891d1a)

Proof of concept: https://www.youtube.com/watch?v=qoIh3fBZvfI

## Donate
Donations greatly appreciated if my work saved you time or money :)  

<a href ="https://paypal.me/TCarpEnterprises?country.x=US&locale.x=en_US">![PayPal-Donate-Button-Download-PNG-1403448781](https://github.com/treyus30/HT-101CBR/assets/136277393/137c5a8d-a726-4e98-8de9-1123748c9d18)</a>


# Guide

## Parts & Pre-requisites

**Required:**
- 1x Arduino Nano or Uno (authentic or clone).
  - Note that the Uno is missing Pins A6 and A7, so is limited to 6 displays.
- 1 to 8x SSD1306 (0.96") or SSD1309 (2.42") monochrome **SPI** 128x64 OLED displays.
  - These must be 7-pin SPI displays, _not_ 4-pin I2C version.**
- 1x MCP2515 CAN Transceiver Module (make sure this is the full board, as pictured below).
- Soldering iron & solder
- Low gauge wires 
- PC & USB cable
  - Many clones will come with relevant cables. USB-C has also been seen popping up. As long as you can connect to a PC you're fine.  
  
**Recommended:**
- DTM06-4S (female) connector set (for connecting directly into Haltech CAN port which also supplies +12V). You can find knockoff kits for reasonable prices on Amazon.
- PCB/protoboard Jumper wires
- Arduino IDE (or some other way of transmitting commands to Arduino Serial)

![image](https://github.com/treyus30/HT-101CBR/assets/136277393/64dd7140-89de-436d-ac0f-f3dddb5633d8) 

## Uploading Binaries (.hex)
You will be flashing the releases as a pre-compiled binary directly onto the device. The easiest way to do this is <a href ="https://github.com/binaryupdates/xLoader"> Xloader</a>.

![image](https://github.com/treyus30/HT-101CBR/assets/136277393/461253f2-5f9f-4130-843d-aaec86dda0bb)

Save the zip file somewhere accessible, run Xloader.exe, and select the correct device and Hex file. 

![upload_2024-6-22_11-15-22](https://github.com/treyus30/HT-101CBR/assets/136277393/dca7e9a7-5549-4163-ad71-19c2e6b201c3)


Make sure your Arduino is plugged in, and COM port is known and set. Then click Upload. 

## Example Wiring (Free/Legacy Version)
![image](https://github.com/treyus30/HT-101CBR/assets/136277393/f193cb99-d602-4b6f-9f1d-fdcd29ef7cde)

![image](https://github.com/treyus30/HT-101CBR/assets/136277393/7e3e6e5f-cf13-4c77-99e8-f6e3c9581b88)

*Word of caution: SPI is an unbalanced protocol (not differential pair). Keep SPI CLK and DATA wires, especially, away from stray capacitance and other wires. 

## Supported Sensors
The following is what is currently available in the latest versions. If you wish to have access to ALL 250+ sensors that are Haltech-supported, please refer to "Announcement 3-18-2024". 
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


**See "MANUAL....xlsx" file in repository for user commands, and to change default sensors and units.** 

# Announcement 3/18/2024
Primary codebase (Full Version) is rebuilt to support:
- Up to 32 OLED Displays (plans for 2x2 PCBs w/ 3D printed modular housings) with common shift registers toggling Chip Selects 
- All 272 addressable CAN sensors (a few are reserved by Haltech and do nothing as of now) assignable to any of the 32 displays.
- Standard HT sensor units & user-configurable conversions (eg: lambda to AFR, or kpa to inHG/PSI)

Status: Still need to hardware test & spend a lot of time debugging the small things. Expecting to ask for $5 per binary once stable. This will be more than enough for almost everyone. 

Future plans (Extended Tiers):
- Output/redirection to OBDII port for reading with off-the-shelf ELM327 devices (mobile device screen - OLEDs not required), including emissions readiness emulation [Deluxe]
  - Will be able to accept HEX addressing of default supported sensors on your factory ECU
  - 16PIN J1962F OBD-II Female pigtail will be required. ~$12 on Amazon.
  - Will likely require 1 additional MCP2515
- 8 analog inputs for feed via CAN to the Haltech ECU [Elite]
  - Theoretically can emulate a Haltech expansion module (normally $$$)
  - Not sure how auto-negotiation works between multiple controllers; may take a while to debug. 

Status: Not really started other than reserving the OBDII CANbus PID and Arduino analog pins. Expecting to ask for $10-50 per binary depending on Tier once stable. 

With Github sponsorships, I'm considering allowing different tiers of reoccurring subscriptions access to the various codeblocks (private repos) for supporters. 
Free Version code will be cleaned up and released once Full Version is completed. Hope this sounds reasonable to everyone :)

# Announcement 12/2023
Haltech has recently made _all_ CAN IDs and datatypes Public! This has increased the available "sensor" data from around 30 types, to over 250, thus prompting a complete re-write of this program! 
I have decided to take a very different approach and use a shift-register basis of updating displays, which means that display quantities will now be loosely capped at 32 instead of 8, and Analog Pins 0-7 will be reserved for future implementation of external sensor inputs, increasing the effective pins of your Haltech ECU!

Work has progressed slowly, as this is one of many facets of my life, but I have recently finished prototyping a 2x2 OLED plug-and-play PCB using analog out pins (not shift register). Parts are currently processing overseas and should be ready before 2024 for testing. I plan on releasing DIY and pre-assembled kits for purchase in conjunction with the new free-to-share binaries. Up to v0.16 will use the old Analog Chip Select method, and going forward will require at least one SN74HC595N IC (~$1.50) in addition to the previous hardware requirements. I think this will be a highly beneficial trade-off in the future. 


---

## License
See License file.
