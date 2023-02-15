// ===================================================================================
// USB VENDOR-Specific Control and Data Transfer Functions for CH551, CH552 and CH554
// ===================================================================================

#include "ch554.h"
#include "usb.h"
#include "usb_vendor.h"
#include "usb_descr.h"
#include "usb_handler.h"

// ===================================================================================
// Variables and Defines
// ===================================================================================

volatile __xdata uint8_t VEN_EP1_readByteCount = 0; // number of data bytes in IN buffer
volatile __xdata uint8_t VEN_EP1_readPointer   = 0; // data pointer for fetching
volatile __bit VEN_EP1_writeBusyFlag = 0;           // flag of whether upload pointer is busy
__xdata uint8_t VEN_EP1_writePointer = 0;           // data pointer for writing

volatile __bit VEN_BOOT_flag    = 0;                // bootloader flag
volatile __bit VEN_I2C_flag     = 0;                // I2C active flag
volatile __bit VEN_BUZZER_flag  = 0;                // buzzer state flag

// ===================================================================================
// Setup Function
// ===================================================================================

// Setup USB VENDOR
void VEN_init(void) {
  USB_init();
  UEP1_T_LEN  = 0;
}

// ===================================================================================
// Bulk Data Transfer Functions
// ===================================================================================

// Flush the OUT buffer
void VEN_flush(void) {
  if(!VEN_EP1_writeBusyFlag && VEN_EP1_writePointer > 0) {  // not busy and buffer not empty?
    UEP1_T_LEN = VEN_EP1_writePointer;                      // number of bytes in OUT buffer
    UEP1_CTRL  = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK; // respond ACK
    VEN_EP1_writeBusyFlag = 1;                              // busy for now
    VEN_EP1_writePointer  = 0;                              // reset write pointer
  }
}

// Write single byte to OUT buffer
void VEN_write(uint8_t b) {
  while(!VEN_ready());                                      // wait for ready to write
  EP1_buffer[MAX_PACKET_SIZE + VEN_EP1_writePointer++] = b; // write data byte
  if(VEN_EP1_writePointer == EP1_SIZE) VEN_flush();         // flush if buffer full
}

// Read single byte from IN buffer
uint8_t VEN_read(void) {
  uint8_t b;
  while(!VEN_available());                                  // wait for data
  b = EP1_buffer[VEN_EP1_readPointer++];                    // get data byte
  if(--VEN_EP1_readByteCount == 0)                          // dec number of bytes in buffer
    UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_R_RES | UEP_R_RES_ACK;// request new data if empty
  return b;
}

// ===================================================================================
// VENDOR-Specific USB Handler Functions
// ===================================================================================
struct usb_vendor_webusb
{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bScheme;
  uint8_t URL[sizeof(WEBUSB_URL)];
} usb_vendor_webusb = {
  .bLength = sizeof(usb_vendor_webusb),
  .bDescriptorType = 3,
  .bScheme = 1,
  .URL = WEBUSB_URL,
};

// Handle vendor-specific non-standard control requests
uint8_t VEN_control(void) {
  uint8_t i;
  uint8_t* ptr;
  uint8_t len = 0xFF;
  uint8_t* usb_vendor_webusb_ptr = (uint8_t*)&usb_vendor_webusb;

  if((USB_setupBuf->bRequestType & USB_REQ_TYP_MASK) == USB_REQ_TYP_VENDOR) {
    switch(USB_setupBuf->bRequest) {
      case VEN_REQ_WEBUSB_URL:
        ptr = (uint8_t*)&usb_vendor_webusb;
        len = sizeof(usb_vendor_webusb);
        break;

#ifdef WCID_VENDOR_CODE
      case WCID_VENDOR_CODE:
        if (USB_setupBuf->wIndexL == 0x04) {
          len = WCID_FEATURE_DESCR[0];
          ptr = WCID_FEATURE_DESCR;
        }
        break;
#endif // WCID_VENDOR_CODE

      default:
        break;                        // command not supported
    }
  }

  if (len != 0xFF) {
      for (i = 0; i < len; i++)
          EP0_buffer[i] = ptr[i];
  }
}

// Setup endpoints
void VEN_setup(void) {
  UEP1_DMA    = EP1_ADDR;                   // EP1 data transfer address
  UEP1_CTRL   = bUEP_AUTO_TOG               // EP1 Auto flip sync flag
              | UEP_T_RES_NAK               // EP1 IN transaction returns NAK
              | UEP_R_RES_ACK;              // EP1 OUT transaction returns ACK
  UEP4_1_MOD  = bUEP1_RX_EN | bUEP1_TX_EN;  // EP1 double buffer
}

// Reset parameters
void VEN_reset(void) {
  UEP1_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK | UEP_R_RES_ACK;
  VEN_EP1_readByteCount = 0;                // reset received bytes counter
  VEN_EP1_writeBusyFlag = 0;                // reset write busy flag
}

// Endpoint 1 IN handler (bulk data transfer to host)
void VEN_EP1_IN(void) {
  UEP1_T_LEN = 0;                                           // no data to send anymore
  UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;  // default NAK
  VEN_EP1_writeBusyFlag = 0;                                // clear busy flag
}

// Endpoint 1 OUT handler (bulk data transfer from host)
void VEN_EP1_OUT(void) {
  if(U_TOG_OK) {                            // discard unsynchronized packets
    VEN_EP1_readByteCount = USB_RX_LEN;     // set number of received data bytes
    VEN_EP1_readPointer = 0;                // reset read pointer for fetching
    if(VEN_EP1_readByteCount) 
      UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_R_RES | UEP_R_RES_NAK; // respond NAK after a packet. Let main code change response after handling.
  }
}
