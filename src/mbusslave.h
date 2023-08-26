#ifndef _mbusslave_
#define _mbusslave_

// device declarations
#define MBUS_DEFAULT_BAUD_RATE 300
#define MBUS_DEFAULT_ADDRESS 5
#define MBUS_ID_NUMBER 0x12345678
#define MBUS_TIMEOUT 1000 // milliseconds
#define MBUS_DATA_SIZE 255
#define MBUS_GOOD_FRAME true
#define MBUS_BAD_FRAME false
#define DEBUG true



// -------------------------------------------------------------------------------------
// C_field: Control Field, Function Field
// FCB: Frame count bit
// FCV: Frame count bit valid
// ADC: Access demand (slave wants to transmit class 1 data)
// DFC: Data flow control = 1 (slave cannot accept further data)

// SND_NKE Initialization of Slave:   BIN:0100 0000 HEX:40           (SHORT FRAME)
// SND_UD  Send User Data to Slave:   BIN:01F1 0011 HEX:53/73        (LONG/CONTROL FRAME)
// REQ_UD2 Request for Class 2 Data:  BIN:01F1 1011 HEX:5B/7B        (SHORT FRAME)
// REQ_UD1 Request for Class 1 Data:  BIN:01F1 1010 HEX:5A/7A        (SHORT FRAME)
// RSP_UD  Data Transfer from Slave:  BIN:00AD 1000 HEX:08/18/28/38  (LONG/CONTROL FRAME)
// F: FCB-Bit, A: ACD-Bit, D: DFC-bit
// -------------------------------------------------------------------------------------

// Address 1-250, 254,255 broadcast, 0 unconfigured

// CI Field
// 51h data send
// 52h selection of slaves
// 50h appliction reset

// --------------------------------------------------------------
// MBUS CONTROL FRAME
//
// C_field: Control Field, Function Field
// --------------------------------------------------------------

#define MBUS_SND_NKE  0x40
#define MBUS_SND_UD   0x53
#define MBUS_REQ_UD2  0x5B
#define MBUS_REQ_UD1  0x5A
#define FCB_BIT_MASK  0xDF


#define MBUS_FRAME_SHORT_START          0x10
#define MBUS_FRAME_LONG_START           0x68
#define MBUS_FRAME_STOP                 0x16

#define MBUS_CONTROL_MASK_SND_NKE       0x40
#define MBUS_CONTROL_MASK_DIR_M2S       0x40
#define MBUS_ADDRESS_NETWORK_LAYER      0xFE


#define MBUS_ACK                        0xE5 


//function prototypes
void mbus_send_ack();
void mbus_data_slave(byte address, byte C_field);
int mbus_get_response(byte *pdata, unsigned char len_pdata);
int transmit_delay_time();

#endif
