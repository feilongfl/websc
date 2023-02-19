// ===================================================================================
// User configurations
// ===================================================================================

#pragma once

// Pin definitions
#define PIN_BUZZER          P15       // buzzer pin
#define PIN_SDA             P16       // I2C SDA
#define PIN_SCL             P17       // I2C SCL

// USB device descriptor
#define USB_VENDOR_ID       0x1a86    // QinHeng Electronics
#define USB_PRODUCT_ID      0x4000    // USB2X
#define USB_DEVICE_VERSION  0x0100    // v1.0 (BCD-format)

// USB configuration descriptor
#define USB_MAX_POWER_mA    100       // max power in mA 

// USB string descriptors
#define MANUFACTURER_STR    'F','e','i','L','o','n','g'
#define PRODUCT_STR         'U','S','B','2','X'
#define SERIAL_STR          'C','H','5','5','2'
#define INTERFACE_STR       'u','s','b','2','x','|','w','c','h',',','w','c','h','5','5','2','e','_','d','e','m','o'
#define WEBUSB_URL          "github.com/feilongfl"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! EXPERIMENTAL !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Windows Compatible ID (WCID) code for automated driver installation.
// Theoretically, no manual installation of a driver for Windows OS is necessary.
// However, since I (un)fortunately do not have a Windows system, this function 
// is untested. The vendor code should not be identical to other vendor codes used,
// the device must be declared as USB 2.0 and the maximum packet size of the EP0 
// must be 64 bytes. Uncomment this define to enable experimental WCID feature.
#define WCID_VENDOR_CODE    0x37
