// Licence: GPL v3
#include <HardwareSerial.h>
#include <Arduino.h>
#include <MBUSPayload.h>
#include "mbusslave.h"

extern HardwareSerial *MBusSerial;
extern HardwareSerial *DebugSerial;
extern MBUSPayload payload;

byte transmission_counter = 0;
extern byte ID_0, ID_1, ID_2, ID_3; 
int MBUS_BAUD_RATE = MBUS_DEFAULT_BAUD_RATE;
byte MBUS_ADDRESS = MBUS_DEFAULT_ADDRESS;


// ---------------------------------------------------------------

void mbus_send_ack()
{  
  delay(random(1,10)); // random delay to avoid bus collission
  MBusSerial->write(MBUS_ACK);
}


void mbus_data_slave(byte address, byte C_field) {
 
  byte data[266];
  int i=0;
  
  data[i++] = MBUS_FRAME_LONG_START;
  data[i++] = payload.getSize() + 15;
  data[i++] = payload.getSize() + 15;
  data[i++] = MBUS_FRAME_LONG_START;
  
  data[i++] = C_field;
  data[i++] = address;
  data[i++] = 0x72;

  // identification number (secondary address) 
  data[i++] = ID_3;
  data[i++] = ID_2;
  data[i++] = ID_1;
  data[i++] = ID_0;

  // manufacturer ID - AEG 0x04A7 
  // see https://www.m-bus.de/man.html for more information
  data[i++] = 0xA7;
  data[i++] = 0x04;

  data[i++] = 0x01;  // hardware version
  
  // Medium (kind of device) - Electricity
  // see https://www.m-bus.de/medium.html for more information 
  data[i++] = 0x02;   

  
  data[i++] = transmission_counter++; // transmission counter
  data[i++] = 0x00; // status: no alarm, no error
  
  // signature - 0x0000 not in use
  data[i++] = 0x00;
  data[i++] = 0x00;

  i += payload.copy(&data[i]);
   
  unsigned char checksum = 0;
  for (byte c=4; c<i; c++) checksum += data[c];
  data[i++] = (byte) checksum; 
  
  data[i++] = 0x16;
  
  //customSerial->write((char*)data);
  for(int i2 = 0; i2 < i; i2++) {
    MBusSerial->write(data[i2]);
    delay(transmit_delay_time()); 
  }
  
}


int transmit_delay_time() {
  switch(MBUS_BAUD_RATE) {
    case 300: return(80);
    case 600: return(50);
    case 1200: return(20);
    case 2400: return(10);
    case 4800: return(5);
    case 9600: return(1);     
  }
  
}



int mbus_get_response(byte *pdata, unsigned char len_pdata) {
  byte bid = 0;             // current byte of response frame
  byte bid_end = 255;       // last byte of frame calculated from length byte sent
  byte bid_checksum = 255;  // checksum byte of frame (next to last)
  byte len = 0;
  byte checksum = 0;
  bool long_frame_found = false;
  bool short_frame_found = false;
  bool complete_frame  = false;
  bool frame_error = false;
 
  //unsigned long timer_start = millis();
  //while (!frame_error && !complete_frame && (millis()-timer_start) < MBUS_TIMEOUT) {
  
  unsigned int timer_start = 0;
  while (!frame_error && !complete_frame) {
    delay(1);
    while (MBusSerial->available()) {
      byte received_byte = (byte) MBusSerial->read();
      timer_start = 0;
       
      // Try to skip noise
      if (bid == 0 && received_byte != 0xE5 && received_byte != 0x68 && received_byte != 0x10) {
        if (DEBUG) DebugSerial->print(F(">"));
        continue;
      }
      
      if (bid > len_pdata) {
        if (DEBUG) DebugSerial->print("mbus: error: frame length exceeded variable size of ");
        if (DEBUG) DebugSerial->println(len_pdata);
        return -1;
      }
      pdata[bid] = received_byte;

      // Single Character (ACK)
      if (bid == 0 && received_byte == 0xE5) {
        if (DEBUG) DebugSerial->println(F("mbus: single character (ack)"));
        return 1;
      }

      // Short frame start
      if (bid == 0 && received_byte == 0x10) {
        if (DEBUG) DebugSerial->println(F("mbus: start short frame"));
        short_frame_found = true;
        bid_end = 4;
        bid_checksum = bid_end-1;
      }

      if (short_frame_found) {
        if (bid && bid < bid_checksum) checksum += received_byte;    // Increment checksum during data portion of frame
        
        if (bid == bid_checksum && received_byte != checksum) {          // Validate checksum
          if (DEBUG) DebugSerial->println(F("mbus: frame failed checksum"));
          frame_error = true;
        }
        if (bid == bid_end && received_byte == 0x16) {                   // Parse frame if still valid
          complete_frame  = true;          
        }
      }
        
      
      // Long frame start
      if (bid == 0 && received_byte == 0x68) {
        if (DEBUG) DebugSerial->println(F("mbus: start long frame"));
        long_frame_found = true;
      }

      if (long_frame_found) {
        // 2nd byte is the frame length
        if (bid == 1) {
          len = received_byte;
          bid_end = len+4+2-1;
          bid_checksum = bid_end-1;
        }
            
        if (bid == 2 && received_byte != len) {                          // 3rd byte is also length, check that its the same as 2nd byte
          if (DEBUG) DebugSerial->println(F("mbus: frame length byte mismatch"));
          frame_error = true;
        }
        if (bid == 3 && received_byte != 0x68) {        ;                // 4th byte is the start byte again
          if (DEBUG) DebugSerial->println(F("mbus: missing second start byte in long frame"));
          frame_error = true;
        }
        if (bid > 3 && bid < bid_checksum) checksum += received_byte;    // Increment checksum during data portion of frame
        
        if (bid == bid_checksum && received_byte != checksum) {          // Validate checksum
          if (DEBUG) DebugSerial->println(F("mbus: frame failed checksum"));
          frame_error = true;
        }
        if (bid == bid_end && received_byte == 0x16) {                   // Parse frame if still valid
          complete_frame  = true;
        }
      }
      bid++;
    }
  }
  if ((DEBUG) && (timer_start >= MBUS_TIMEOUT)) DebugSerial->print(F("Timeout"));
  
  if (complete_frame && !frame_error) {
    // good frame
    return bid;
  } else {
    // bad frame
    return -1;
  }  
}  
