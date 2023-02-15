// ===================================================================================
// USB Descriptors
// ===================================================================================

#include "usb_descr.h"

// ===================================================================================
// Device Descriptor
// ===================================================================================
__code USB_DEV_DESCR DevDescr = {
  .bLength            = sizeof(DevDescr),       // size of the descriptor in bytes: 18
  .bDescriptorType    = USB_DESCR_TYP_DEVICE,   // device descriptor: 0x01
  .bcdUSB             = 0x0250,                 // USB specification: USB 2.0
  .bDeviceClass       = 0,                      // interface will define class
  .bDeviceSubClass    = 0,                      // unused
  .bDeviceProtocol    = 0,                      // unused
  .bMaxPacketSize0    = EP0_SIZE,               // maximum packet size for Endpoint 0
  .idVendor           = USB_VENDOR_ID,          // VID
  .idProduct          = USB_PRODUCT_ID,         // PID
  .bcdDevice          = USB_DEVICE_VERSION,     // device version
  .iManufacturer      = 1,                      // index of Manufacturer String Descr
  .iProduct           = 2,                      // index of Product String Descriptor
  .iSerialNumber      = 3,                      // index of Serial Number String Descr
  .bNumConfigurations = 1                       // number of possible configurations
};

// ===================================================================================
// Configuration Descriptor
// ===================================================================================
__code USB_CFG_DESCR_HID CfgDescr = {

  // Configuration Descriptor
  .config = {
    .bLength            = sizeof(USB_CFG_DESCR),  // size of the descriptor in bytes
    .bDescriptorType    = USB_DESCR_TYP_CONFIG,   // configuration descriptor: 0x02
    .wTotalLength       = sizeof(CfgDescr),       // total length in bytes
    .bNumInterfaces     = 1,                      // number of interfaces: 1
    .bConfigurationValue= 1,                      // value to select this configuration
    .iConfiguration     = 0,                      // no configuration string descriptor
    .bmAttributes       = 0x80,                   // attributes = bus powered, no wakeup
    .MaxPower           = USB_MAX_POWER_mA / 2    // in 2mA units
  },

  // Interface Descriptor
  .interface0 = {
    .bLength            = sizeof(USB_ITF_DESCR),  // size of the descriptor in bytes: 9
    .bDescriptorType    = USB_DESCR_TYP_INTERF,   // interface descriptor: 0x04
    .bInterfaceNumber   = 0,                      // number of this interface: 0
    .bAlternateSetting  = 0,                      // value used to select alternative setting
    .bNumEndpoints      = 2,                      // number of endpoints used: 2
    .bInterfaceClass    = USB_DEV_CLASS_VEN_SPEC, // vendor specific class: 0xFF
    .bInterfaceSubClass = 0,                      // no subclass
    .bInterfaceProtocol = 0,                      // no protocoll
    .iInterface         = 4                       // interface string descriptor
  },

  // Endpoint Descriptor: Endpoint 1 (OUT, Bulk)
  .ep1OUT = {
    .bLength            = sizeof(USB_ENDP_DESCR), // size of the descriptor in bytes: 7
    .bDescriptorType    = USB_DESCR_TYP_ENDP,     // endpoint descriptor: 0x05
    .bEndpointAddress   = USB_ENDP_ADDR_EP1_OUT,  // endpoint: 1, direction: OUT (0x01)
    .bmAttributes       = USB_ENDP_TYPE_BULK,     // transfer type: bulk (0x02)
    .wMaxPacketSize     = EP1_SIZE,               // max packet size
    .bInterval          = 0                       // polling intervall (ignored for bulk)
  },

  // Endpoint Descriptor: Endpoint 1 (IN, Bulk)
  .ep1IN = {
    .bLength            = sizeof(USB_ENDP_DESCR), // size of the descriptor in bytes: 7
    .bDescriptorType    = USB_DESCR_TYP_ENDP,     // endpoint descriptor: 0x05
    .bEndpointAddress   = USB_ENDP_ADDR_EP1_IN,   // endpoint: 1, direction: OUT (0x81)
    .bmAttributes       = USB_ENDP_TYPE_BULK,     // transfer type: bulk (0x02)
    .wMaxPacketSize     = EP1_SIZE,               // max packet size
    .bInterval          = 0                       // polling intervall (ignored for bulk)
  }
};

__code struct usb_vendor_webusb usb_vendor_webusb = {
  .bLength = sizeof(usb_vendor_webusb),
  .bDescriptorType = 3,
  .bScheme = 1,
  .URL = WEBUSB_URL,
};

__code uint8_t msos2_descriptor[] = {
	/* MS OS 2.0 set header descriptor   */
	0x0A, 0x00,             /* Descriptor size (10 bytes)                 */
	0x00, 0x00,             /* MS_OS_20_SET_HEADER_DESCRIPTOR             */
	0x00, 0x00, 0x03, 0x06, /* Windows version (8.1) (0x06030000)         */
	(0x0A + 0x14 + 0x08), 0x00, /* Length of the MS OS 2.0 descriptor set */

	/* MS OS 2.0 function subset ID descriptor
	 * This means that the descriptors below will only apply to one
	 * set of interfaces
	 */
	0x08, 0x00, /* Descriptor size (8 bytes) */
	0x02, 0x00, /* MS_OS_20_SUBSET_HEADER_FUNCTION */
	0x02,       /* Index of first interface this subset applies to. */
	0x00,       /* reserved */
	(0x08 + 0x14), 0x00, /* Length of the MS OS 2.0 descriptor subset */

	/* MS OS 2.0 compatible ID descriptor */
	0x14, 0x00, /* Descriptor size                */
	0x03, 0x00, /* MS_OS_20_FEATURE_COMPATIBLE_ID */
	/* 8-byte compatible ID string, then 8-byte sub-compatible ID string */
	'W',  'I',  'N',  'U',  'S',  'B',  0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
__code uint8_t msos2_descriptor_size = sizeof(msos2_descriptor);

__code uint8_t msos1_compatid_descriptor[] = {
	/* See https://github.com/pbatard/libwdi/wiki/WCID-Devices */
	/* MS OS 1.0 header section */
	0x28, 0x00, 0x00, 0x00, /* Descriptor size (40 bytes)          */
	0x00, 0x01,             /* Version 1.00                        */
	0x04, 0x00,             /* Type: Extended compat ID descriptor */
	0x01,                   /* Number of function sections         */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,       /* reserved    */

	/* MS OS 1.0 function section */
	0x02,     /* Index of interface this section applies to. */
	0x01,     /* reserved */
	/* 8-byte compatible ID string, then 8-byte sub-compatible ID string */
	'W',  'I',  'N',  'U',  'S',  'B',  0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00 /* reserved */
};
__code uint8_t msos1_compatid_descriptor_size = sizeof(msos1_compatid_descriptor);

__code USB_BDO BdoDescr = {
    .bdo = {
      .bLength = sizeof(USB_BinaryDeviceObject_DESCR),
      .bDescriptorType = USB_DESCR_TYP_BOSD,
      .wTotalLength = sizeof(USB_BDO),
      .bNumDeviceCaps = 1,
    },
    .webusb = {
      .bLength = sizeof(USB_DeviceCapability_DESCR),
      .bDescriptorType = USB_DESCR_TYP_DCD,
      .bDevCapabilityType = 0x05,
      .bReserved = 0,
      .PlatformCapablityUUID = {0x38, 0xB6, 0x08, 0x34, 0xA9, 0x09, 0xA0, 0x47, 0x8B, 0xFD, 0xA0, 0x76, 0x88, 0x15, 0xB6, 0x65},
      .bcdVersion = 0x0100,
      .bVendorCode = 1,
      .iLandingPage = 1,
    },
    .msos2 = {
      .bLength = sizeof(USB_MSOS2_DESCR),
      .bDevCapabilityType = 0x05,
      .bReserved = 0,
      .PlatformCapablityUUID = {
        /**
         * MS OS 2.0 Platform Capability ID
         * D8DD60DF-4589-4CC7-9CD2-659D9E648A9F
         */
        0xDF, 0x60, 0xDD, 0xD8,
        0x89, 0x45,
        0xC7, 0x4C,
        0x9C, 0xD2,
        0x65, 0x9D, 0x9E, 0x64, 0x8A, 0x9F,
      },
      .dwWindowsVersion = 0x06030000,
      .wMSOSDescriptorSetTotalLength = sizeof(msos2_descriptor),
      .bMS_VendorCode = 0x02,
      .bAltEnumCode = 0x00
    },
};

// ===================================================================================
// String Descriptors
// ===================================================================================

// Language Descriptor (Index 0)
__code uint16_t LangDescr[] = {
  ((uint16_t)USB_DESCR_TYP_STRING << 8) | sizeof(LangDescr), 0x0409 };  // US English

// Manufacturer String Descriptor (Index 1)
__code uint16_t ManufDescr[] = {
  ((uint16_t)USB_DESCR_TYP_STRING << 8) | sizeof(ManufDescr), MANUFACTURER_STR };

// Product String Descriptor (Index 2)
__code uint16_t ProdDescr[] = {
  ((uint16_t)USB_DESCR_TYP_STRING << 8) | sizeof(ProdDescr), PRODUCT_STR };

// Serial String Descriptor (Index 3)
__code uint16_t SerDescr[] = {
  ((uint16_t)USB_DESCR_TYP_STRING << 8) | sizeof(SerDescr), SERIAL_STR };

// Interface String Descriptor (Index 4)
__code uint16_t InterfDescr[] = {
  ((uint16_t)USB_DESCR_TYP_STRING << 8) | sizeof(InterfDescr), INTERFACE_STR };

// ===================================================================================
// Windows Compatible ID (WCID) descriptors for automated driver installation
// ===================================================================================

#ifdef WCID_VENDOR_CODE
// Microsoft WCID feature descriptor (index 0x0004)
__code uint8_t WCID_FEATURE_DESCR[] = {
    0x28, 0x00, 0x00, 0x00,                         // length = 40 bytes
    0x00, 0x01,                                     // version 1.0 (in BCD)
    0x04, 0x00,                                     // compatibility descriptor index 0x0004
    0x01,                                           // number of sections
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,       // reserved (7 bytes)
    0x00,                                           // interface number 0
    0x01,                                           // reserved
    'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,       // Compatible ID "WINUSB\0\0"
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Subcompatible ID (unused)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00              // reserved 6 bytes
};

// Microsoft OS string descriptor for WCID driver (index 0xEE)
__code uint16_t MicrosoftDescr[] = {
  ((uint16_t)USB_DESCR_TYP_STRING << 8) | sizeof(MicrosoftDescr),
  'M','S','F','T','1','0','0', WCID_VENDOR_CODE, 0x00};
#endif
