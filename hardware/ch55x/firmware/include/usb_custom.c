
#include "ch554.h"
#include "usb.h"
#include "usb_descr.h"
#include "usb_handler.h"
#include "usb_vendor.h"

#define _VEN_PROCESS_NAME(name) ven_process_##name
#define VEN_PROCESS_NAME(name) _VEN_PROCESS_NAME(name)
#define VEN_PROCESS(name)                                                      \
  case name:                                                                   \
    len = VEN_PROCESS_NAME(name)(ptr);                                         \
    break // no ;

static uint8_t VEN_PROCESS_NAME(VEN_REQ_BOOTLOADER)(uint8_t *ptr) {
  VEN_BOOT_flag = 1;

  return 0;
}

static uint8_t VEN_PROCESS_NAME(VEN_REQ_INT_GET)(uint8_t *ptr) { return 0; }

static uint8_t VEN_PROCESS_NAME(VEN_REQ_SEEK)(uint8_t *ptr) { return 0; }
static uint8_t VEN_PROCESS_NAME(VEN_REQ_BYTE_WRITE)(uint8_t *ptr) { return 0; }
static uint8_t VEN_PROCESS_NAME(VEN_REQ_BYTE_READ)(uint8_t *ptr) { return 0; }

uint8_t usb_custom(uint8_t *ptr) {
  uint8_t len = 0xFF;

  switch (USB_setupBuf->bRequest) {
    VEN_PROCESS(VEN_REQ_BOOTLOADER);

    VEN_PROCESS(VEN_REQ_INT_GET);

    VEN_PROCESS(VEN_REQ_SEEK);
    VEN_PROCESS(VEN_REQ_BYTE_WRITE);
    VEN_PROCESS(VEN_REQ_BYTE_READ);

  default:
    break; // command not supported
  }

  return len;
}
