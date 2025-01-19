//THIS IS THE *LITE* VERSION
// Protected by Creative Commons License: Attribution-NonCommercial 4.0 International -- you may redistribute and modify this software as long as you provide attribution and do not use it for commercial purposes. Derivative works are allowed, such as porting. 

// These three lines must accompany any redistributions or modifications of this code: 
// Copyright Trey Carpenter, aka Treyus30, all rights reserved
// https://github.com/treyus30/HT-101CBR/

// Note this uses libraries subject to their own licenses. Please read and understand them as well. 

// Changelog:
// v0.17: updated hg/PSI conversion to be at 0 PSI when switchover to positive pressure. Changed HTserialOut enable to a debug mode for maximum reporting speed; added ability to toggle from serial. 
// v0.18: removed MCP2515 timer/delay, changed display update timer to 20Hz to promote more CANbus polling before introducing a delay from display updating. Added Serial check timer (every 2secs).

#include <EEPROM.h>
//CAN Bridge Stuff
  #include <SPI.h>
  #include <mcp2515.h>
  
  struct can_frame canMsgRx;
  struct can_frame canMsgTx;
  MCP2515 mcp2515_HT(10, 8000000);    //initialize CAN Controller ChipSelect (CS) on pin 10, clock the same as SSD1306Acsii

//SPI Display
  #include "SSD1306Ascii.h"
  #include "SSD1306AsciiSpi.h"
  
  #define RST_PIN         2
  #define DC_PIN          3
  #define OLED_CS_0       5
  
  //"soft" Chip Selects:
  const unsigned int maxDisplays = 8;
  //static const unsigned int OLED_CS[maxDisplays] = {9, 8, 7, 6, 5, 4, A0, A1};
  static const unsigned int OLED_CS[maxDisplays] = {A0, A1, A2, A3, A4, A5, A6, A7};    
            //I don't understand why, but using digital pin 9 (and sometimes 8) will cause it to pull low with main SPI (despite being told otherwise), causing it to update every time. 
            //Analog pins seem to be relatively on their own from the pinout, so we will use those. 
  
  SSD1306AsciiSpi oled;
//FUNDAMENTAL ARRAY SIZE DELCARATIONS
  const unsigned int maxUnits = 10;       //do not include the "extras" in this count
  //unsigned int newUnitIdx = 0;
  const unsigned int maxSensors = 32;

//reusuable strings:
  //size declarations
    const unsigned int unitsSize = 6;       //max size of units string (must be two larger than actual due to terminating char and added "| ")
    const unsigned int titleSize = 16;       //max size of data tile string (must be one larger than actual due to terminating char) -- somehow 16 size is able to pull 18 chars??
    char unitsBuf[unitsSize];               //buffer to pull the data out of PROGMEM
    char titleBuf[titleSize];               //buffer to pull the data out of PROGMEM
    unsigned int titleIdx;
    unsigned int unitIdx;
//units
    const char PROGMEM  perc[] = "%";
    const char PROGMEM  kpa[] = "kpa";      //convertable: PSI, PSI & inHg, bar
    const char PROGMEM  rpm[] = "RPM";
    const char PROGMEM  lph[] = "l/hr";     //convertable: gph, 
    const char PROGMEM  deg[] = "*";  //const char PROGMEM  deg[] = "°";
    const char PROGMEM  lambda[] = "L";     //convertible: AFR (petrol-E05), ?
    const char PROGMEM  kmh[] = "km/h";     //convertible: mph, ft/s
    const char PROGMEM  kel[] = "K";        //convertible: C, F
    const char PROGMEM  volts[] = "V";
    const char PROGMEM  unitless[] = " ";
    //extras (conversions)
    const char PROGMEM  psi[] = "PSI";
    const char PROGMEM  inhgPSI[] = "\"Hg+";
    const char PROGMEM  bar[] = "bar";
    const char PROGMEM  gph[] = "gph";
    const char PROGMEM  AFR_p[] = "pAFR";
    const char PROGMEM  mph[] = "mph";
    const char PROGMEM  fps[] = "ft/s";
    const char PROGMEM  C[] = "*C";
    const char PROGMEM  F[] = "*F";
    const char *const unit[] PROGMEM = {perc, kpa, rpm, lph, deg, lambda, kmh, kel, volts, unitless,   //forgetting the *const will cause device crash
                                        psi, inhgPSI, bar, gph, AFR_p, mph, fps, C, F};    
//                                        0      1   2    3    4     5      6    7     8       9

  //titles (add 1 to index for EEPROM address eg: 11 = 12EEPROM)        --Single words now require a space at the end to display properly. Could fix in the title display logic if that little bit of memory is vital 
    const char PROGMEM EngineSpeed[]        = "ENGINE SPEED";
    const char PROGMEM ManifoldPressure[]   = "Manifold Pressure";
    const char PROGMEM ThrottlePosition[]   = "Throttle Pos";
    const char PROGMEM FuelPressure[]       = "Fuel Pressure";
    const char PROGMEM OilPressure[]        = "Oil Pressure";
    const char PROGMEM PriInjDC[]           = "Injector DC1";
    const char PROGMEM SecInjDC[]           = "Injector DC2";
    const char PROGMEM LeadIgnAng[]         = "Leading Angle";
    const char PROGMEM TrailIgnAng[]        = "Trailing Angle";
    const char PROGMEM Lambda1[]            = "Lambda (Primary)";
    const char PROGMEM Lambda2[]            = "Lambda (Secondary)";
    const char PROGMEM MissCounter[]        = "Miss Count";
    const char PROGMEM TriggerCounter[]     = "Trigger Count";
    const char PROGMEM HomeCounter[]        = "Home Count";
    const char PROGMEM TrigSinceLast[]      = "Trigger Sync";
    const char PROGMEM WheelSpeed[]         = "Wheel Speed";
    const char PROGMEM Gear[]               = "GEAR ";
    const char PROGMEM IntakeCamAng1[]      = "Intake Cam 1 Ang";
    const char PROGMEM IntakeCamAng2[]      = "Intake Cam 2 Ang";
    const char PROGMEM TargetBoost[]        = "Target Boost";
    const char PROGMEM Barometer[]          = "Barometer";
  	const char PROGMEM CoolantTemp[]        = "Coolant Temp";
  	const char PROGMEM IntakeAirTemp[]      = "Intake Air";
  	const char PROGMEM FuelTemp[]           = "Fuel Temp";
  	const char PROGMEM OilTemp[]            = "Oil Temp";
  	const char PROGMEM FuelConsumption[]    = "Fuel Consump";
  	const char PROGMEM AvgFuelEcon[]        = "Avg Fuel Econ";
    const char PROGMEM BatteryVolt[]        = "Battery Volts";

    const char *const title[] PROGMEM = {EngineSpeed,   ManifoldPressure,   ThrottlePosition,   FuelPressure,   OilPressure,  PriInjDC,   
    SecInjDC,   LeadIgnAng,   TrailIgnAng,  Lambda1,  Lambda2,  MissCounter,  TriggerCounter,   HomeCounter,  TrigSinceLast,  WheelSpeed,   Gear,   IntakeCamAng1,  IntakeCamAng2,  
    TargetBoost,  Barometer,  CoolantTemp,  IntakeAirTemp,  FuelTemp,   OilTemp,  FuelConsumption,  AvgFuelEcon,  BatteryVolt}; 


unsigned int sensor[maxSensors][7];  //dictionary of numbers that lead to the correct array indicies, and/or can house data (limited to int)
float sensorData[maxSensors];   //store all last sensor data from CAN


// Timers
//const unsigned long limitHTFreq = 1000000;    //max CANbus (MCP2515) polling frequency - 1MHZ (crystal at 8MHZ defined above)
//float MCPwait = 1/limitHTFreq*1000;         //Hz to ms

//unsigned long lastReadMillis = 0;
//unsigned int TSLP = 0;     //time since last poll of MCP2515 (CANbus)


unsigned long lastRptMillis_S = 0;
unsigned long lastRptMillis_D = 0;
unsigned int TSLR_S = 0;     //time since last report - serial
unsigned int TSLR_D = 0;     //time since last report - display

unsigned long SerChkTimer = 2000;   //ms to wait between serial input checks
unsigned long lastSerChk = 0;       //time since last seral input check


// End of Timers

////////////////////////////////////////////////////////
bool serialOutHT = false;                   //output raw CAN from HT. Use for debugging. Will not do anything else like update displays.
int debugCID = -1;                          //CID selection for only outputting one ID from CAN

bool reportSerial = false;                  //feedback to Serial?
int  unsigned reportSerialWait = 1000;     //wait this many ms before reporting data updates via Serial when reportSerial = true

bool reportDisplay = true;                  //feedback to Display(s)?
int  unsigned reportDisplayWait = 100;       //wait this many ms before reporting data updates via Display(s) when reportDisplay = true (50ms = 20Hz refresh rate; faster may slow down other functions)

const byte dataCursorCol = 8;               //starting column of data on displays
const byte dataCursorRow = 4;               //starting row (x8 pixels) of data on displays (recommended 3 or 4)
const byte maxDigits = 5;                   //will vary with the font - how many digits of data can be displayed on the display
////////////////////////////////////////////////////////


String incomingWord[4];

//Set up OBD-II CAN responses
const word OBDCANID = 0x7E8;              //respond on this ID (other devices can be up to +7)
const byte OBDCANDLC = 8;                 //8 bytes of data
const unsigned long PID_SUPPORT[7] = {0xFE1FB810,0x0,0x0,0x0,0x0,0x0,0x0};      //encoded bit support of IDs (see wikipedia page for decoding)

struct OBDdata{
  byte PID;
  byte Bytes;
  byte Service;
  byte byte3;
}obd;


bool initialized[maxDisplays] = {0,0,0,0,0,0,0,0};  //hold intialized or not per display

float lastData[maxSensors];
//float currData[maxSensors];
char digit[13];          //needs to be as large as the largest floating number we have +1
char pastDigit[13];      //needs to be as large as the largest floating number we have +1
int col[5];
//byte number;
//int dec[maxSensors];

int charWidth;
int charHeight;

//identify the default display indicies for each sensor (end of program):
//You may have not exceed the declared maxDisplays for unique assignments and there can only be 1 display per sensor currently. Unused should be dS.
const unsigned int dS = 64;
byte sensorDisplay[maxSensors] =          { 0,  1,  2, dS, dS, dS, dS, dS, dS,  3,   //0-9
                                           dS, dS, dS, dS, dS, dS, dS, dS, dS, dS,   //10-19
                                           dS, dS,  4, dS, dS,  5,  6,  7, dS, dS,   //20-29
                                           dS, dS};                                   //30,31 --31 reserved for misc data 
unsigned int activeDisp = 0;
int tmp;
int tmp2;

int select;   //tracks serial-selected display Idx

// declare the variable for defaults & user setting changes of unit. References index of each unit index's list. Note that many have nothing to reference. 
unsigned int unitConversion[maxUnits];    //CHANGED: these will now POINT to the unit index we want. It should be a number greater than the normal units. 
unsigned int EEPoffset_unitConv = 50;   //set the offset in EEPROM for the unit conversion data. It MUST be higher than the sensor data (reserved up to addr 49)

unsigned int dispMiscID = 0;    //ID for unknown HT data to display to screen
unsigned int dispMiscWord = 0;  //Word number for display - must be index of first of 2-bit byte

int strln;

//SETUP
void setup() {
  
  pinMode(RST_PIN, OUTPUT);     //attempt to stabilize the reset pin immediately
  digitalWrite(RST_PIN, HIGH);
  
  delay(500);   //give time to stabilize initial voltages
  
  initSensorDict(); //initialize all sensor params (class at bottom of codeblock)

// Serial Initialize
  Serial.begin(19200);
  Serial.setTimeout(500);

// VARIABLES that will be stored in SRAM (non-volitile user settings)
  // save initial displays for each sensor:
     //see if user has made Display settings changes:
     if ((EEPROM.read(0) == 0) || (EEPROM.read(0) == 255)){    //no change - default - check/set that everything is default settings (do not re-write if it is - EEPROM has limited writes). 255 means not written.
      Serial.println(F("User tamper bit in-tact. Checking/resetting defaults..."));
      for (int i = 0; i < maxSensors; i++){
        tmp = sensorDisplay[i];        
        if (EEPROM.read(i+1) == tmp){    //starting address is 1, ending is maxSensors (currently 28)
          //do nothing if EEPROM address data matches current data --- NOTE: apparently EEPROM.update() checks for differences automatically so could switch over if willing to take out reporting
           Serial.print(F("EEPROM (")); Serial.print(i+1); Serial.print(F(") already= ")); Serial.println(tmp);
        }
        else {    //if address does not match, reset it to defaults set in the const variable
          EEPROM.update(i+1, sensorDisplay[i]);   //changed to update, revert to write if something breaks
          Serial.print(F("wrote default disp to EEPROM (")); Serial.print(i+1);  Serial.print(F("):")); Serial.println(tmp);
        }
      }
     }
     else if (EEPROM.read(0) == 1){   // data detected as changed - load rest from EEPROM
      Serial.println(F("User tamper bit enabled. Reading user settings..."));
      for (int i = 0; i < maxSensors; i++){
        sensorDisplay[i] = EEPROM.read(i+1);
        Serial.print(F("read user disp from EEPROM: ")); Serial.println(sensorDisplay[i]);
      }
     }
   //do the same for unit conversions:
      tmp = EEPROM.read(EEPoffset_unitConv);
      if ((tmp == 0) || (tmp == 255)){
        for (int i = 0; i < maxUnits; i++){
          tmp = EEPoffset_unitConv + i + 1;
          EEPROM.update(tmp, unitConversion[i]);  //if unit tamper bit in tact, make sure all EEPROM matches program defaults
        }
      }
      else {
        for (int i = 0; i < maxUnits; i++){
          tmp = EEPoffset_unitConv + i + 1;
          unitConversion[i] = EEPROM.read(tmp);   //if tamper bit was set, read all parameters that might have been changed from EEPROM to the appropriate variable
          Serial.print(F("read conv. idx from EEPROM: ")); Serial.println(unitConversion[i]);
        }
      }

//SPI display
    //digitalWrite(RST_PIN, LOW); //added these two at the recommendation of the library creator, but they didn't seem to do anything. Using the reset pin in the declaration below does this anyway..
    delay(10);
    
    oled.begin(&Adafruit128x64, OLED_CS_0, DC_PIN, RST_PIN);  //start primary instance
    oled.setFont(Adafruit5x7);
    
    //setup all possible Chip Selects 
    for (int n=0; n < maxDisplays; n++){
      pinMode(OLED_CS[n], OUTPUT);
      digitalWrite(OLED_CS[n], HIGH); 
      delay(1);   //seemed to stabilize display startup  
    }
         
    for (int n=0; n < maxDisplays-1; n++){
      digitalWrite(OLED_CS[n], LOW);
      delay(5);   //seemed to stabilize display startup @ >=1ms
      
      oled.clear();  
      oled.setContrast(64);   //64 = 25uA, 160 = 69uA, 255 = 100uA
            
      oled.setCursor(0,0);
      oled.set1X();
      oled.print(F("Screen ")); oled.print(n+1); oled.print(F(" (A")); oled.print(n); oled.println(F(")"));
      oled.println(F("Initializing..."));
      
      oled.setCursor(29,3);      
      oled.set2X(); 
      oled.print(F("Please"));
      oled.setCursor(40,5);
      oled.print(F("Wait"));
      
      digitalWrite(OLED_CS[n], HIGH);
      delay(5);
    }
    delay(250);   //brief pause after startups


      oled.setFont(lcdnums12x16); //set thick font for data updates
      charWidth = oled.charWidth('8');
      charHeight = oled.fontHeight();
      col[0] = dataCursorCol;
      col[1] = dataCursorCol + (charWidth);
      col[2] = dataCursorCol + (charWidth * 2);
      col[3] = dataCursorCol + (charWidth * 3);
      col[4] = dataCursorCol + (charWidth * 4);
      //Serial.println(charWidth);
      //Serial.println(charHeight);


// MCP2515 Initialize
  mcp2515_HT.reset();  
  mcp2515_HT.setBitrate(CAN_1000KBPS, MCP_8MHZ);  //set CAN bitrate and on-board oscillator frequency (8, 16, or 20MHz)  
  mcp2515_HT.setNormalMode();
  //mcp2515.setLoopbackMode();
  //mcp2515.setListenOnlyMode();

  Serial.println(); Serial.println(F("**Device Reset - Initialized**")); Serial.println();
//Report stock vehicle code supported PIDs (based on 1999 Tacoma)
  Serial.print(F("Stock Vehicle OBDII PID support from: 0x")); Serial.println(PID_SUPPORT[0], HEX);    //report the OBDII vehicle profile defined above
  for (int pid=0; pid < 6; pid++){                                                                    //report individual PIDs enabled for vehicle profile defined above
    for (int n=0; n < 32; n++){       //32 bits (4 bytes) in an 8 digit HEX
      if(PID_SUPPORT[pid] & (0x80000000 >> n)){  // 8 in front in hex = 1 in front in dec /4 places
        Serial.print(F("0x")); Serial.println(n+pid+1,HEX);  //each additional pid should also increment by 1 to account for its own PID
      }
    }
  }
  if (serialOutHT){
    Serial.println(F("--CAN Read--"));
    Serial.println(F("ID  DLC   DATA"));
  }
}


//MAIN LOOP
void loop() {

if (serialOutHT){   //enabling serial out will ONLY do serial out now. (for debugging)
  if (mcp2515_HT.readMessage(&canMsgRx) == MCP2515::ERROR_OK) {
    if ((debugCID > -1) && (canMsgRx.can_id == debugCID)){
      Serial.print(canMsgRx.can_id, HEX); // print ID
      Serial.print(F(" | ")); 
      Serial.print(canMsgRx.can_dlc, HEX); // print DLC
      Serial.print(F(" | "));
      
      for (int i = 0; i < canMsgRx.can_dlc; i++)  {  // print the data
        Serial.print(canMsgRx.data[i], HEX);
        Serial.print(F(" "));
      }
      Serial.println();
    }
  }
}
else {

//poll CANbus at defined interval
  //TSLP = (millis() - lastReadMillis);
 
  //if (TSLP >= MCPwait){  
    if (mcp2515_HT.readMessage(&canMsgRx) == MCP2515::ERROR_OK) {
      //Serial.print("TSLP: "); Serial.print(TSLP); Serial.println("ms");

      //lastReadMillis = millis();
  
//Known Haltech definitions:
//Save and convert each (to OBDII standard units*) found for this cycle:
      switch (canMsgRx.can_id) {
        //case 0x7DF:     //OBD-II REQUEST!!
          //obd.Bytes =   canMsgRx.data[0];   //# of bytes to follow (2-3)
          //obd.Service = canMsgRx.data[1];   //OBD-II Service being called            
          //obd.PID =     canMsgRx.data[2];   //SAE standard
          //obd.byte3 =   canMsgRx.data[3];   //vehicle-specific combined with PID byte
          
        break;
        case 0x360:   //50Hz
          goUpdateSensor(0, 0, 1);  //update sensor 0 with canMsgRx.data[0] and canMsgRx.data[1]
          goUpdateSensor(1, 2, 3);  //update sensor 1 with canMsgRx.data[2] and canMsgRx.data[3]
          goUpdateSensor(2, 4, 5);  //etc... 
        break;
        case 0x361:   //50Hz
          goUpdateSensor(3, 0, 1);
          goUpdateSensor(4, 2, 3);
        break;
        case 0x362:   //50Hz
          goUpdateSensor(5, 0, 1);
          goUpdateSensor(6, 2, 3);
          goUpdateSensor(7, 4, 5);
          goUpdateSensor(8, 6, 7);
        break;
        case 0x368:   //20Hz
          goUpdateSensor(9, 0, 1);    //lambda primary
          goUpdateSensor(10, 2, 3);          
        break;
        case 0x369:   //20Hz
          goUpdateSensor(11, 0, 1);
          goUpdateSensor(12, 2, 3);
          goUpdateSensor(13, 4, 5);
          goUpdateSensor(14, 6, 7);
        break;
        case 0x370:   //20Hz
          goUpdateSensor(15, 0, 1);
          goUpdateSensor(16, 2, 3);
          goUpdateSensor(17, 4, 5);
          goUpdateSensor(18, 6, 7);
        break;
        case 0x372:   //10Hz
          goUpdateSensor(27, 0, 1);
          goUpdateSensor(19, 2, 3);
          goUpdateSensor(20, 4, 5);
        break;
        case 0x373:   //10Hz
          //EGTs - skip
        break;
        case 0x374:   //10Hz
          //EGTs - skip
        break;
        case 0x375:   //10Hz
          //EGTs - skip
        break;
        case 0x3E0:   //5Hz
          goUpdateSensor(21, 0, 1);
          goUpdateSensor(22, 2, 3);
          goUpdateSensor(23, 4, 5);
          goUpdateSensor(24, 6, 7);
        break;
        case 0x3E2:   //5Hz
          goUpdateSensor(25, 0, 1);
          goUpdateSensor(26, 2, 3);
        break;

        default:      //for anything else, if it matches a target ID and word, save it for printing to a screen
          //if (dispMiscID == canMsgRx.can_id){
          //  goUpdateSensor(maxSensors-1, dispMiscWord, dispMiscWord+1);            
          //}          
        break;
      }
    //}
  }


//SERIAL STATUS REPORT
    
    //SERIAL STATUS REPORT
    if (reportSerial){
      TSLR_S = (millis() - lastRptMillis_S);
      if (TSLR_S >= reportSerialWait){
        //Serial.print("*TSLR: "); Serial.print(TSLR, DEC);  Serial.println("ms");

        Serial.println();
        for (int n=0; n < maxSensors; n++){
          if (sensor[n][3] == true){
              //Serial.print(sensor[n][0]);Serial.print(F(": ")); Serial.print(sensor[n].data, 1); Serial.println(sensor[n][2]);
          }      
        }
        lastRptMillis_S = millis();
      }
    }
    
//DISPLAY STATUS REPORT  
    if (reportDisplay){
      TSLR_D = (millis() - lastRptMillis_D);
      if (TSLR_D >= reportDisplayWait){
        
        for (int i = 0; i < maxSensors; i++){   
          activeDisp = sensorDisplay[i];  //mark this loop's display index for updating    
            
          if (activeDisp != dS){  //if current loop display index is not "disabled"...

            if ((sensor[i][6] == 1) && (initialized[activeDisp] == true)){  //waiting for initialized prevents math from triggering an initial "changed" state. Downside is low frequency updates could temporarily display the base unit on start. 
            // if (sensor[i][6] == 1){
               goFormatSensor(i);  //optimization: instead of converting every piece of received data as soon as we get it, only do it right before we're ready to update a display for it 
                            //honestly, to save ram we could reconfigure the lastData and SensorData in the next section to use unformatted data, but its minimal
            }           
                          
            if (lastData[i] != sensorData[i]){  //don't bother refreshing anything (flickering) if nothing changes)               
              dtostrf(sensorData[i], maxDigits, sensor[i][5], digit);     //BREAKDOWN NEXT NUMBER INTO DIGITS   
              dtostrf(lastData[i], maxDigits, sensor[i][5], pastDigit);   //BREAKDOWN LAST NUMBER INTO DIGITS TO DETERMINE UPDATES UPON COMPARE
    
    
           //INITIALIZE SCREEN WITH FIRST-TIME DATA 
              if (initialized[activeDisp] == false){
                //pull title & units for current loop display data out of storage
                titleIdx = sensor[i][0];  
                strcpy_P(titleBuf, (char *)pgm_read_word(&(title[titleIdx])));  //copied from Arduino reference. Not very insightful, but as long as it works.
                
                digitalWrite(OLED_CS[activeDisp], LOW);
                oled.clear();

                //set initial *TITLE*
                oled.setFont(Adafruit5x7); oled.set2X();
                oled.setCursor(0,0);
                oled.set1X();

                //discover where to split long TITLES by looking for first space
                tmp = getStringSpace(titleBuf);
                for (int j=0; j < tmp; j++){
                  oled.print(titleBuf[j]);
                }
                oled.setCursor(0,1);                
                for (int j=tmp+1; j < strln; j++){
                  oled.print(titleBuf[j]);
                }
                Serial.println(titleBuf);

                //set initial *UNITS*
                strcpy(unitsBuf, "| ");
                unitIdx = sensor[i][2];                
                oled.set2X();                
                if (unitIdx != 9){    // if not unitless..                                    
                  if (unitConversion[unitIdx] > maxUnits){ //handle unit change if not default
                    tmp = unitConversion[unitIdx];
                                      
                  }
                  else{ //else if it is default...
                    tmp = unitIdx;
                  }
                  //Serial.println(tmp);  
                  strcat_P(unitsBuf, (char *)pgm_read_word(&(unit[tmp])));  //copied from Arduino reference. Not very insightful, but as long as it works.
                  oled.setCursor(128-oled.fieldWidth(strlen(unitsBuf)),0); 
                  oled.print(unitsBuf);
                }                
        
                //font data for all post-initialize updates
                oled.setFont(lcdnums12x16); //set thick font for data updates    
                for (int dig = 0; dig < maxDigits ; dig++){
                      oled.clearField(col[dig],dataCursorRow,1);    // col #, row # in 8-pixel rows, number of chars to clear
                      //delay(500);
                      oled.setCursor(col[dig], dataCursorRow);                    // col # in pixels, row # in 8-pixel rows
                      oled.print(digit[dig]);  
                  }              
                initialized[activeDisp] = true;
                digitalWrite(OLED_CS[activeDisp], HIGH);
              }

              
           // SCREEN ALREADY INITIALIZED -- UPDATE OUTPUT   
              else {
                digitalWrite(OLED_CS[activeDisp], LOW);
                for (int dig = 0; dig < maxDigits ; dig++){
                  //Serial.print(digit[dig]); Serial.print("vs");     Serial.println(pastDigit[dig]);
                    if (digit[dig] != pastDigit[dig]){
                      oled.clearField(col[dig], dataCursorRow, 1);    // col #, row # in 8-pixel rows, number of chars to clear
                      //delay(250);
                      oled.setCursor(col[dig], dataCursorRow);                    // col # in pixels, row # in 8-pixel rows
                      oled.print(digit[dig]); 
                      //Serial.println(digit[dig]);
                    }                      
                }    
                //Serial.println(F("Display Updated"));                  
                digitalWrite(OLED_CS[activeDisp], HIGH);
              }
            }
              lastData[i] = sensorData[i];
              lastRptMillis_D = millis();
         }
        }
      }
    }
  }
  
  if ((millis() - lastSerChk) >= SerChkTimer){
    checkSerial();
    lastSerChk = millis();
  }

}

//CUSTOM FUNCTIONS::

void checkSerial(){
  //check to see if user has entered anything (up to 64 bytes)
  if (Serial.available() > 0){
    incomingWord[0] = Serial.readStringUntil(' ');
    incomingWord[1] = Serial.readStringUntil(' ');
    incomingWord[2] = Serial.readStringUntil(' ');
    incomingWord[3] = Serial.readString();

    Serial.println();
    Serial.print(F("received: ")); Serial.print(incomingWord[0]); Serial.print(">"); Serial.print(incomingWord[1]); Serial.print(">"); Serial.print(incomingWord[2]); Serial.print(">"); Serial.println(incomingWord[3]);
    //Serial.println(incomingWord[0]);
    //Serial.println(incomingWord[1]);
    //Serial.println(incomingWord[2]);

    //cmdAck = false;
 
 //USER-ADJUSTABLE SERIAL COMMANDS::
    
    // "Program/Backend"
    if (incomingWord[0] == F("prog")) {
      if (incomingWord[1] == F("rpt")) {        
          if (incomingWord[2] == F("debug")){   //new: v0.17
            serialOutHT = true;
            if (incomingWord[3] != ""){
              debugCID = incomingWord[3].toInt();
            }
          }
          else{
            reportSerialWait = incomingWord[2].toInt();   //set poll time of serial print
          }
        }
      else if(incomingWord[1] == F("unit")){                //"prog unit"
        //tmp = incomingWord[2].toInt();                      //sensor index (0-27)        
        //tmp2 = sensor[tmp][2];                //get sensor's unit idx
        tmp2 = incomingWord[2].toInt();
        
        unitConversion[tmp2] = incomingWord[3].toInt();     //alternate unit index
        EEPROM.update(EEPoffset_unitConv + tmp2 + 1, unitConversion[tmp2]);          //update the unit conversion array index for conversion
        EEPROM.update(EEPoffset_unitConv, 1);               //update tamper bit
        Serial.println(F("Updated units"));

        //loop through any active sensors that have the changed unit and re-initialize (so we can refresh the units mainly)
        for (int zz = 0; zz < maxSensors; zz++){ 
          if ((sensor[zz][2] == tmp2) && (sensorDisplay[tmp] <= maxDisplays)){            
            initialized[sensorDisplay[tmp]] = false;
          }
        }

      }
      else if(incomingWord[1] == F("miscan")){
        tmp = incomingWord[2].toInt();
        tmp2 = incomingWord[3].toInt();

        dispMiscID = tmp;
        dispMiscWord = tmp2;   
      }
    }    
    
    // "Display" : disp [#dispidx] data [#sensoridx]
    else if (incomingWord[0] == F("disp")){
      select = incomingWord[1].toInt();     //display index (0-7)
      if (select < maxDisplays){
        if (incomingWord[2] == F("data")){          
          
          unsigned int newDisplayIdx = incomingWord[1].toInt();
          unsigned int sensorIdx = incomingWord[3].toInt();
          //Serial.print(F("received: ")); Serial.print(incomingWord[0]); Serial.print(">"); Serial.print(incomingWord[1]); Serial.print(">"); Serial.print(incomingWord[2]); Serial.print(">"); Serial.println(incomingWord[3]);
          
          listEEPROM(8);  //list first 8 addresses of EEPROM
          
          //if sensor's display index is the same as the new display index, clear it (no duplicates)
          for (int zz = 0; zz < maxSensors; zz++){ 
            int oldSensorDisplay = sensorDisplay[zz];            
            
            if (((oldSensorDisplay != newDisplayIdx) && (zz == sensorIdx)) || (oldSensorDisplay == newDisplayIdx)){   //may be some optimizations to be had here but I'm so burnt out from this section.
              digitalWrite(OLED_CS[oldSensorDisplay], LOW);
              oled.clear();              // reset the display that had a duplicate variable
              //oled.println();
              digitalWrite(OLED_CS[oldSensorDisplay], HIGH);
              Serial.print(F("cleared disp ")); Serial.println(oldSensorDisplay);    
                     
              sensorDisplay[zz] = dS;
              EEPROM.update(zz+1, dS);    //update sensor's display reference ID in EEPROM
              
              //Serial.print(F("updated array: ")); Serial.print(zz); Serial.print("-"); Serial.println(sensorDisplay[zz]);
              //Serial.print(F("updated eeprom: ")); Serial.print(zz+1); Serial.print("-"); Serial.println(EEPROM.read(zz+1));   
            }
          }
          
          //then, write sensor's new display index and mark initialize flag for the new display (idx)         
          //could add an "if = 64 else the rest below if we wanted more feeback, but its pretty good as is
          for (int zz = 0; zz < maxSensors; zz++){   //for all sensors, set new display index if that sensor matches the target index 
            if (zz == sensorIdx){
              sensorDisplay[zz] = newDisplayIdx;
              initialized[newDisplayIdx] = false;  //flag display for re-initialization with new sensor data

              Serial.print(F("set disp ")); Serial.println(newDisplayIdx);  
              EEPROM.update(0, 1);                  //user has made a change, so flip settings bit
              EEPROM.update(zz+1, newDisplayIdx);   //update the screen setting in EEPROM
              
              //Serial.print(F("updated array: ")); Serial.print(zz); Serial.print("-"); Serial.println(sensorDisplay[zz]);
              //Serial.print(F("updated eeprom: ")); Serial.print(zz+1); Serial.print("-"); Serial.println(EEPROM.read(zz+1));   
            }
          }
          listEEPROM(8);
          //cmdAck = true;
        }
      }
    }
    //else if (incomingWord[1] == F("reset")){   //note this is not like pushing the RST button - it's more of an application reset than a hardware one
      //Serial.println(F("user soft reset"));
      //resetFunc();
    //}
  
  // "Print Variable Lists for User"
    else if (incomingWord[0] == F("eeprom")){
      if (incomingWord[1] == F("list")){
        listEEPROM(64);
      }
      else if (incomingWord[1] == F("clear")){
        for (int i = 0; i < 255; i++){
          EEPROM.update(i,255);              
        }
        Serial.println(F("EEPROM reset. Please restart."));
      }
      
    }
  }
}

void listEEPROM(byte endaddr){
  for (int zzz = 0; zzz < endaddr; zzz++){
      Serial.print(F("EEPROM addr ")); Serial.print(zzz); Serial.print("="); Serial.print(EEPROM.read(zzz));
      if (zzz == 0){
        Serial.print(F("(User tamper bit)"));
      }
      Serial.println();
    }
}

int getStringSpace(char str[]){  
   strln = strlen(str);   
   for (int zzz = 0; zzz < strln; zzz++){
    if (str[zzz] == ' '){   //omfg you need single quotes here. Double will break it. 
      return zzz; 
      Serial.println(strln);
      break;
    }
   }
  
}

void goUpdateSensor(int sensorIdx, int firstByte, int secondByte){
  sensorData[sensorIdx] = word(canMsgRx.data[firstByte], canMsgRx.data[secondByte]);
  sensor[sensorIdx][6] = 1;  //set flag to "unformatted"/new data 
  //Serial.print("new data - sensor ID "); Serial.print(sensorIdx); Serial.print(": "); Serial.println(sensorData[sensorIdx]);
}

void goFormatSensor(int sensorIdx){
  //sensorData[sensorIdx] = word(canMsgRx.data[firstByte], canMsgRx.data[secondByte]);     //set the base data for the sensor  
  //Serial.println();Serial.print(F("Sensor: ")); Serial.print(sensorIdx); Serial.print(F(": Step 1: ")); Serial.println(sensorData[sensorIdx]);
  sensorData[sensorIdx] =  sensorData[sensorIdx] / sensor[sensorIdx][1];                //convert the base data to a normal magnitude for that sensor
  //Serial.print(F("Step 2: ")); Serial.println(sensorData[sensorIdx]);
                  
  //after we receive new data, immediately convert it to another unit, if that unit is set to be converted      
  tmp = sensor[sensorIdx][2];         //sensor's unit index
  tmp2 = unitConversion[tmp];         //conversion index to target for given unit's data      
  if (tmp2 > 0){                      //skip 0 index (do not attempt to convert if set to 0)
    sensorData[sensorIdx] = goConvert(sensorData[sensorIdx], tmp, tmp2);    //get converted data via sending original, unit ref, conversion profile ref    
    //Serial.print(F("Step 3: ")); Serial.println(sensorData[sensorIdx]);      
  }
  sensor[sensorIdx][6] = 0;  //set flag to "formatted"/old data 
}

float goConvert(float data, int unitidx, int convidx){  
  unitConversion[unitidx] = convidx;
  switch (unitidx){
    case 0: //%
      switch (convidx){
        case 0:

        break;
      }
    break;

    case 1: //kpa
      switch (convidx){
        case 1:   //no change

        break;
        case 10:   //to PSI
          data = data * 0.1450377;
          //newUnitIdx = 11;
        break;
        case 11:   //to inHg/PSI
          if (data < 101.325){    //inHg
            //newUnitIdx = 12;
            data = (data * 0.295301)- 29.92137;   //29.9... is 101.325kpa in inHg. We want negative (vacuum) reading so subtract it from actual reading            
          }
          else{                   //PSI (relative to kpa at sea level)
            data = (data * 0.145037)- 14.69595;
            //newUnitIdx = 12;
          }
          
        break;
        case 12:   //to bar
          data = data * 0.01;
          //newUnitIdx = 13;
        break;
      }
    break;
    case 2: //RPM
      switch (convidx){
        case 2:

        break;
      }
    break;
    case 3: //l/hr
      switch (convidx){
        case 3:   //no change

        break;
        case 13:   //to gallon/hr
          data = data * 0.264172;
          //newUnitIdx = 14;
        break;
      }
    break;
    case 4: //deg
      switch (convidx){
        case 4:

        break;
      }
    break;
    case 5: //lambda
      switch (convidx){
        case 5:
  
        break;
        case 14:   //to AFR (petrol straight)
          data = data * 14.7; //if we can get the ethanol content DLC, we can calulate accurate afr
          //newUnitIdx = 15;
        break;
      }
    break;
    case 6: //km/h
      switch (convidx){
        case 6:
  
        break;
        case 15:   //to mph
          data = data * 0.6213712;
          //newUnitIdx = 16;
        break;
        case 16:   //to ft/s
          data = data * 0.9113444;
          //newUnitIdx = 17;
        break;
      }
    break;
    case 7: //K(elvin)
      switch (convidx){
        case 7:
  
        break;
        case 17:   //to C
          data = data - 272.15;
          //newUnitIdx = 18;
        break;
        case 18:   //to F
          data = ((data - 272.15)*1.8) + 32;
          //newUnitIdx = 19;
        break;

      }
    break;
    case 8: //V(olts)
      switch (convidx){
        case 8:

        break;
      }
    break;
    case 9: //unitless
      switch (convidx){
        case 9:

        break;        
      }
    break;
    default:
    
    break;
    
  }
  //Serial.print("Data now: "); Serial.println(data);
  return data;
}


void initSensorDict(){
//Engine Speed (RPM)
  sensor[0][0] = 0;       //title index
  sensor[0][1] = 1.0;       //unit multiple (scalar) (generally used to convert to standard units) 
  sensor[0][2] = 2;       //unit index
  sensor[0][3] = true;    //to show or not  -- not used either
  sensor[0][4] = true;    //odbII support or not
  sensor[0][5] = 0;       //default decimal points
  sensor[0][6] = 1;       //repurposed: now to set if data has been received and formatted (0), or is new and needs to be (1)
//MAP (kpa)
  sensor[1][0] = 1;
  sensor[1][1] = 10.0;
  sensor[1][2] = 1;
  sensor[1][3] = true;
  sensor[1][4] = true;
  sensor[1][5] = 1;      
  sensor[1][6] = 1;
//TPS (%)
  sensor[2][0] = 2;
  sensor[2][1] = 10.0;
  sensor[2][2] = 0;
  sensor[2][3] = true;
  sensor[2][4] = true;
  sensor[2][5] = 1;      
  sensor[2][6] = 1;
//Fuel Press (kpa)
  sensor[3][0] = 3;
  sensor[3][1] = 10.0;
  sensor[3][2] = 1;
  sensor[3][3] = false;
  sensor[3][4] = true;
  sensor[3][5] = 1;      
  sensor[3][6] = 1;
//Oil Press (kpa)
  sensor[4][0] = 4;
  sensor[4][1] = 10.0;
  sensor[4][2] = 1;
  sensor[4][3] = false;
  sensor[4][4] = true;
  sensor[4][5] = 1;      
  sensor[4][6] = 1;
//Inj 1 DC (%)
  sensor[5][0] = 5;
  sensor[5][1] = 10.0;
  sensor[5][2] = 0;
  sensor[5][3] = true;
  sensor[5][4] = true;
  sensor[5][5] = 1;      
  sensor[5][6] = 1;
//Inj 2 DC (%)
  sensor[6][0] = 6;
  sensor[6][1] = 10.0;
  sensor[6][2] = 0;
  sensor[6][3] = false;
  sensor[6][4] = true;
  sensor[6][5] = 1;      
  sensor[6][6] = 1;
//Lead Ign Angle
  sensor[7][0] = 7;
  sensor[7][1] = 10.0;
  sensor[7][2] = 4;
  sensor[7][3] = true;
  sensor[7][4] = true;
  sensor[7][5] = 1;      
  sensor[7][6] = 1;
//Trail Ign Angle
  sensor[8][0] = 8;
  sensor[8][1] = 10.0;
  sensor[8][2] = 4;
  sensor[8][3] = true;
  sensor[8][4] = true;
  sensor[8][5] = 1;      
  sensor[8][6] = 1;
//AFR (lambda) 1
  sensor[9][0] = 9;
  sensor[9][1] = 1000.0;
  sensor[9][2] = 5;
  sensor[9][3] = true;
  sensor[9][4] = true;
  sensor[9][5] = 2;      
  sensor[9][6] = 1;
//AFR (lambda) 2
  sensor[10][0] = 10;
  sensor[10][1] = 1000.0;
  sensor[10][2] = 5;
  sensor[10][3] = true;
  sensor[10][4] = true;
  sensor[10][5] = 2;      
  sensor[10][6] = 1;
//Miss Counter
  sensor[11][0] = 11;
  sensor[11][1] = 1;
  sensor[11][2]=  9;
  sensor[11][3] = false;
  sensor[11][4] = false;
  sensor[11][5] = 0;      
  sensor[11][6] = 1;
//Trigger Counter
  sensor[12][0] = 12;
  sensor[12][1] = 1;
  sensor[12][2] = 9;
  sensor[12][3] = false;
  sensor[12][4] = false;
  sensor[12][5] = 0;      
  sensor[12][6] = 1;
//Home Counter
  sensor[13][0] = 13;
  sensor[13][1] = 1;
  sensor[13][2] = 9;
  sensor[13][3] = false;
  sensor[13][4] = false;
  sensor[13][5] = 0;      
  sensor[13][6] = 1;
//Trigger since last miss
  sensor[14][0] = 14;
  sensor[14][1] = 1;
  sensor[14][2] = 9;
  sensor[14][3] = false;
  sensor[14][4] = false;
  sensor[14][5] = 0;      
  sensor[14][6] = 1;
// Wheel Speed
  sensor[15][0] = 15;
  sensor[15][1] = 10.0;
  sensor[15][2] = 6;
  sensor[15][3] = true;
  sensor[15][4] = true;
  sensor[15][5] = 0;      
  sensor[15][6] = 1;
// Gear #
  sensor[16][0] = 16;
  sensor[16][1] = 1;
  sensor[16][2] = 9;
  sensor[16][3] = true;
  sensor[16][4] = false;
  sensor[16][5] = 0;      
  sensor[16][6] = 1;
// Intake Cam Angle 1
  sensor[17][0] = 17;
  sensor[17][1] = 10.0;
  sensor[17][2] = 4;
  sensor[17][2] = 4;
  sensor[17][3] = false;
  sensor[17][4] = false;
  sensor[17][5] = 1;      
  sensor[17][6] = 1;
// Intake Cam Angle 2
  sensor[18][0] = 18;
  sensor[18][1] = 10.0;
  sensor[18][2] = 4;
  sensor[18][2] = 4;
  sensor[18][3] = false;
  sensor[18][4] = false;
  sensor[18][5] = 1;      
  sensor[18][6] = 1;
// Target Boost
  sensor[19][0] = 19;
  sensor[19][1] = 10.0;
  sensor[19][2] = 1;
  sensor[19][3] = true;
  sensor[19][4] = false;
  sensor[19][5] = 1;      
  sensor[19][6] = 1;
// Barometer
  sensor[20][0] = 20;
  sensor[20][1] = 10.0;
  sensor[20][2] = 1;
  sensor[20][3] = false;
  sensor[20][4] = false;
  sensor[20][5] = 1;      
  sensor[20][6] = 1;
// Cooltant Temp
  sensor[21][0] = 21;
  sensor[21][1] = 10.0;
  sensor[21][2] = 7;
  sensor[21][3] = true;
  sensor[21][4] = false;
  sensor[21][5] = 0;      
  sensor[21][6] = 1;
// Intake Air Temp (IAT)
  sensor[22][0] = 22;
  sensor[22][1] = 10.0;
  sensor[22][2] = 7;
  sensor[22][3] = true;
  sensor[22][4] = false;
  sensor[22][5] = 0;      
  sensor[22][6] = 1;
// Fuel Temp (requires Flex Fuel Sensor)
  sensor[23][0] = 23;
  sensor[23][1] = 10.0;
  sensor[23][2] = 7;
  sensor[23][3] = true;
  sensor[23][4] = false;
  sensor[23][5] = 0;      
  sensor[23][6] = 1;
// Oil Temp (requires external sensor)
  sensor[24][0] = 24;
  sensor[24][1] = 10.0;
  sensor[24][2] = 7;
  sensor[24][3] = true;
  sensor[24][4] = false;
  sensor[24][5] = 0;      
  sensor[24][6] = 1;
// Fuel Consumption
  sensor[25][0] = 25;
  sensor[25][1] = 100.0;
  sensor[25][2] = 3;
  sensor[25][3] = true;
  sensor[25][4] = false;
  sensor[25][5] = 1;      
  sensor[25][6] = 1;
// Avg Fuel Economy
  sensor[26][0] = 26;
  sensor[26][1] = 10.0;
  sensor[26][2] = 3;
  sensor[26][3] = true;
  sensor[26][4] = false;
  sensor[26][5] = 1;      
  sensor[26][6] = 1;
// Battery Voltage
  sensor[27][0] = 27;
  sensor[27][1] = 10.0;
  sensor[27][2] = 8;
  sensor[27][3] = true;
  sensor[27][4] = false;
  sensor[27][5] = 2;      
  sensor[27][6] = 1;
}
