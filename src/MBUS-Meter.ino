/*
 * External library "MBUSPayload" needed: https://github.com/AllWize/mbus-payload
 * 
 * 
 * Based on OpenEnergyMonitor code by Trystan Lea, Glyn Hudson, and others
 * https://github.com/openenergymonitor/HeatpumpMonitor
 * Copyright 2018, Bryan McLellan <btm@loftninjas.org>
 * License: GPLv3 https://www.gnu.org/licenses/gpl.txt
 */


#include <MBUSPayload.h>
#include "mbusslave.h"

// enable the following line for Adafruit feather boards with TinyUSB support like nRF52840 
// #include <Adafruit_TinyUSB.h> // for Serial


MBUSPayload payload(200);

// converted ID number
byte ID_0, ID_1, ID_2, ID_3; 
bool device_selected = false;
extern int MBUS_BAUD_RATE;
extern byte MBUS_ADDRESS;


// Serial interface used for mbus to allow use of 8E1 encoding
#include <HardwareSerial.h>
HardwareSerial *MBusSerial;
HardwareSerial *DebugSerial;

void setup() {
  #ifdef ESP8266    
    // ESP8266 or other boards with only one UART
    DebugSerial = NULL;
    MBusSerial = &Serial;  

  #else
    // all other boards with 2 or more UARTS
    DebugSerial = &Serial;
    DebugSerial->begin(115200); //init debug interface with 115200 Baud
    MBusSerial = &Serial1;  
  #endif

  // init MBUS UART
  MBusSerial->begin(MBUS_BAUD_RATE, SERIAL_8E1); // mbus uses 8E1 encoding
  delay(1000); // let the serial initialize, or we get a bad first frame  

  // convert ID number for later use
  ID_0 = MBUS_ID_NUMBER >> 24;
  ID_1 = (MBUS_ID_NUMBER & 0x00FF0000) >> 16;
  ID_2 = (MBUS_ID_NUMBER & 0x0000FF00) >> 8;
  ID_3 = (MBUS_ID_NUMBER & 0x000000FF);
  
  // start mesage 
  DebugSerial->println(F("Simple MBUS Slave/Counter Demo startup"));
  DebugSerial->print(F("mbus slave address:"));
  DebugSerial->println(MBUS_ADDRESS);
  DebugSerial->print(F("mbus baud rate: "));
  DebugSerial->println(MBUS_BAUD_RATE);
  DebugSerial->print(F("mbus ID number: "));
  DebugSerial->println(MBUS_ID_NUMBER, HEX);   
}

void loop() {
  byte mbus_data[MBUS_DATA_SIZE] = { 0 };
  int packet_size = mbus_get_response(mbus_data, sizeof(mbus_data));
  if (packet_size > 0) {
      if (DEBUG) DebugSerial->println(F("mbus: good frame: "));
      if (DEBUG) print_bytes(mbus_data, packet_size);
      
      // Address equal and short protocol
      if ((packet_size == 5)  && (mbus_data[2] == MBUS_ADDRESS)) {        

     // Init slaves
        if (mbus_data[1]== MBUS_SND_NKE) {
            if (DEBUG) DebugSerial->println(F("init slave request"));
            device_selected = false;
            mbus_send_ack(); //Send ACK
        }
                       
        // class 2 data request
        if ((mbus_data[1] & FCB_BIT_MASK)== MBUS_REQ_UD2) {
            if (DEBUG) DebugSerial->println(F("class 2 data request"));
            Encode_Payload(); // encode payload buffer
            mbus_data_slave(5,0x08); //Send Counter data to mbus master
        }             
      }


      // Broadcast and short protocol
      if ((packet_size == 5)  && ((mbus_data[2] == 0xFD) || (mbus_data[2] == 0xFE))) {        
        
        // Init slaves
        if (mbus_data[1]== MBUS_SND_NKE) {
            if (DEBUG) DebugSerial->println(F("init slaves request"));
            device_selected = false;
            mbus_send_ack(); //Send ACK
        }
        
        // class 2 data request 
        if (((mbus_data[1] & FCB_BIT_MASK)== MBUS_REQ_UD2) && (device_selected)) {
            if (DEBUG) DebugSerial->println(F("class 2 data request"));
            Encode_Payload(); // encode payload buffer
            mbus_data_slave(5,0x08); //Send Counter data to mbus master
        }
        
      }  


      // secondary select via ID number
      if ((packet_size == 17)  && ((mbus_data[4] & FCB_BIT_MASK)== MBUS_SND_UD) && (mbus_data[5] == 0xFD) && (mbus_data[6] == 0x52)) {
        if (DEBUG) DebugSerial->println(F("mbus secondary select!"));
        // check ID number
        if ((mbus_data[7] == ID_3) && (mbus_data[8] == ID_2) && (mbus_data[9] == ID_1) && (mbus_data[10] == ID_0)) {
          if (DEBUG) DebugSerial->println(F("ID number equal - Device selected!"));
          // ID number is equal
          device_selected = true;
          mbus_send_ack(); //Send ACK
        } else {
          // not my ID number
          device_selected = false;          
        }
        
      }


      // change address request
      if ((packet_size == 12)  && ((mbus_data[4] & FCB_BIT_MASK)== MBUS_SND_UD) && (mbus_data[5] == MBUS_ADDRESS) &&
          (mbus_data[6] == 0x51) && (mbus_data[7] == 0x1) && (mbus_data[8] == 0x7A)) {
        if (DEBUG) DebugSerial->println(F("mbus change address request!"));
        if (DEBUG) DebugSerial->print(F("new device address is "));
        if (DEBUG) DebugSerial->println(mbus_data[9]);
        MBUS_ADDRESS = mbus_data[9];  
        mbus_send_ack(); //Send ACK             
      }
 

      // change baud rate request
      if ((packet_size == 9)  && ((mbus_data[4] & FCB_BIT_MASK)== MBUS_SND_UD) && (mbus_data[5] == MBUS_ADDRESS)) {
        if (DEBUG) DebugSerial->println(F("mbus change baudrate request!"));
        if (DEBUG) DebugSerial->print(F("new device baudrate is "));
        switch (mbus_data[6]) {
          case 0xB8: MBUS_BAUD_RATE = 300;
                     break; 
          case 0xB9: MBUS_BAUD_RATE = 600;
                     break; 
          case 0xBA: MBUS_BAUD_RATE = 1200;
                     break; 
          case 0xBB: MBUS_BAUD_RATE = 2400;
                     break; 
          case 0xBC: MBUS_BAUD_RATE = 4800;
                     break; 
          case 0xBD: MBUS_BAUD_RATE = 9600;
                     break;                                
        }
        if (DEBUG) DebugSerial->println(MBUS_BAUD_RATE);                
        mbus_send_ack(); //Send ACK  
        delay(100);  
        MBusSerial->begin(MBUS_BAUD_RATE, SERIAL_8E1); // mbus uses 8E1 encoding                   
      }

      
      
  } else {
    if (DEBUG) DebugSerial->println(F("mbus: bad frame!"));  
    if (DEBUG) print_bytes(mbus_data, sizeof(mbus_data)); 
  } 
}

/**
 * encode data fields for meter
 * please see for detailed information about data fields:  
 * https://github.com/AllWize/mbus-payload#method-addfield
 * 
 * Note: ID number, Manufacturer code. Medium etc. can be adjusted in
 * mbusslave.h. Please dont use payload.addfield for these parameters!
 * 
 **/
void Encode_Payload(){
  payload.reset();
  payload.addField(MBUS_CODE::BAUDRATE_BPS, 2400);  
  payload.addField(MBUS_CODE::FIRMWARE_VERSION, 25);  
  payload.addField(MBUS_CODE::VOLTS, 10.3);  
  payload.addField(MBUS_CODE::AMPERES, 4.8); 
  payload.addField(MBUS_CODE::POWER_W, 49.44); 
  payload.addField(MBUS_CODE::DIGITAL_INPUT, 5);
  payload.addField(MBUS_CODE::DIGITAL_OUTPUT, 2);
}
  

// Prints a whole response as a string for debugging
void print_bytes(byte *bytes, unsigned char len_bytes) {
  for (int i = 0; i < len_bytes; i++) {
    DebugSerial->print(String(bytes[i], HEX));
    DebugSerial->print(F(" "));
  } 
  DebugSerial->println();
}
