
#include "usb_custom.h"

// Prototypes for used interrupts
void USB_interrupt(void);
void USB_ISR(void) __interrupt(INT_NO_USB) { USB_interrupt(); }

USB2X_ISR(INT_NO_INT0)
USB2X_ISR(INT_NO_TMR0)
USB2X_ISR(INT_NO_INT1)
USB2X_ISR(INT_NO_TMR1)
USB2X_ISR(INT_NO_UART0)
USB2X_ISR(INT_NO_TMR2)
USB2X_ISR(INT_NO_SPI0)
USB2X_ISR(INT_NO_TKEY)
// USB2X_ISR(INT_NO_USB)
USB2X_ISR(INT_NO_ADC)
USB2X_ISR(INT_NO_UART1)
USB2X_ISR(INT_NO_PWMX)
USB2X_ISR(INT_NO_GPIO)
USB2X_ISR(INT_NO_WDOG)
