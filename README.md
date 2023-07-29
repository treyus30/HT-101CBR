# HT-101CBR
## Uploading Binaries (.hex)
This program is built on ATmega328 platform, mainly the Arduino Nano, though it should work without any port work with the Uno as well. 
I'm keeping the source code private for now for various reasons, so you will be flashing the releases as a pre-compiled binary directly onto the device. The easiest way to do this is Xloader. (https://github.com/binaryupdates/xLoader)

![image](https://github.com/treyus30/HT-101CBR/assets/136277393/8c89f1f1-d301-4bb2-b1d2-b6094aa9fa6a)

Save the zip file somewhere accessible, run Xloader.exe, and select the correct device and Hex file. 

![image](https://github.com/treyus30/HT-101CBR/assets/136277393/679bda4d-403e-430c-8365-6190dd08b5be)

Make sure your Arduino is plugged in, and COM port is known and set. Then click Upload. 

## Parts & Pre-reqs
The code is optimized for extremely fast display updating over hardware SPI, utilizing a codeblock to breakout numbers into indidivual digits, then compare these digits to the last digits recorded for a given CAN reading. 
Up to 8 SSD1306 128x64pixel OLED displays are supported, with their Chip Select (CS) pins being the 8 analog pins, A0 - A7. 

**Required:**
- Arduino Nano or Uno (authentic or clone).
- 1-8x SSD1306 monochrome 128x64 OLED displays.
- 1x MCP2515 CAN Transceiver Module (make sure this is the full board, as pictured below).
- Some method of wiring.
- Some method of splitting connections. I recommend covered, low-amp screw terminal blocks. 
  
**Optional:**
- DTM06-4S (female) connector set (for connecting directly into Haltech CAN port). You can find knockoff kits for reasonable prices on Amazon.

![image](https://github.com/treyus30/HT-101CBR/assets/136277393/eabb3ddf-1e83-4671-af30-2ccc8925f393)


## Wiring
![image](https://github.com/treyus30/HT-101CBR/assets/136277393/8763e77f-8e93-4969-9150-53df4368d10b)

![image](https://github.com/treyus30/HT-101CBR/assets/136277393/28058247-4e99-4d89-bd72-d304ec4466d6)

# Example
https://www.youtube.com/watch?v=qoIh3fBZvfI

---

## License
This work is licensed under a `Creative Commons Attribution-NonCommercial-NoDerivatives 4.0 International License`.
This work is free to redistribute for personal, non-commercial use. Any modifications performed are not licensed for redistribution. 
