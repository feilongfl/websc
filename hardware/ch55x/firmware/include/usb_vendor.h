// ===================================================================================
// USB VENDOR-Specific Control and Data Transfer Functions for CH551, CH552 and CH554
// ===================================================================================

#pragma once
#include <stdint.h>
#include "usb_custom.h"

// Setup function
void VEN_init(void); // setup USB vendor-specific device

// Vendor class control requests
#define VEN_REQ_WEBUSB_URL 1 // webusb url req
#define VEN_REQ_MS2_DESC   2 // Microsoft OS 2.0 platform capability descriptor
#define VEN_REQ_MS1_DESC   3 // Microsoft OS 1.0 platform capability descriptor

#define VEN_REQ_BOOTLOADER 0xF0

#define VEN_REQ_INT_GET 0x10

#define VEN_REQ_SEEK 0x20

#define VEN_REQ_WRITE_top   0x30
#define VEN_REQ_READ_top    0x40
#define VEN_REQ_WRITE_code  (VEN_REQ_WRITE_top | USB2X_ADDRTYPE_code)
#define VEN_REQ_WRITE_data  (VEN_REQ_WRITE_top | USB2X_ADDRTYPE_data)
#define VEN_REQ_WRITE_idata (VEN_REQ_WRITE_top | USB2X_ADDRTYPE_idata)
#define VEN_REQ_WRITE_xdata (VEN_REQ_WRITE_top | USB2X_ADDRTYPE_xdata)
#define VEN_REQ_WRITE_sfr   (VEN_REQ_WRITE_top | USB2X_ADDRTYPE_sfr)
#define VEN_REQ_READ_code   (VEN_REQ_READ_top | USB2X_ADDRTYPE_code)
#define VEN_REQ_READ_data   (VEN_REQ_READ_top | USB2X_ADDRTYPE_data)
#define VEN_REQ_READ_idata  (VEN_REQ_READ_top | USB2X_ADDRTYPE_idata)
#define VEN_REQ_READ_xdata  (VEN_REQ_READ_top | USB2X_ADDRTYPE_xdata)
#define VEN_REQ_READ_sfr    (VEN_REQ_READ_top | USB2X_ADDRTYPE_sfr)

#define VEN_REQ_IIC 0x50

// Bulk data transfer functions
#define VEN_available() (VEN_EP1_readByteCount)	 // number of received bytes
#define VEN_ready()	(!VEN_EP1_writeBusyFlag) // check if ready to write
uint8_t VEN_read(void);				 // read byte from BULK IN buffer
void VEN_write(uint8_t b);			 // write byte to BULK OUT buffer
void VEN_flush(void);				 // flush BULK OUT buffer

// Variables
extern volatile __bit VEN_BOOT_flag;   // bootloader flag
extern volatile __bit VEN_I2C_flag;    // I2C active flag
extern volatile __bit VEN_BUZZER_flag; // buzzer state flag

extern volatile __xdata uint8_t VEN_EP1_readByteCount;
extern volatile __bit VEN_EP1_writeBusyFlag;
