#pragma once
#include <stdint.h>
#include <util_macro.h>

// #define BIT(n) (1 << (n))
#define ARG_UNUSED(x) (void)(x)

#define USB2X_ADDRTYPE_code  BIT(0)
#define USB2X_ADDRTYPE_idata BIT(1)
#define USB2X_ADDRTYPE_xdata BIT(2)
#define USB2X_ADDRTYPE_data  BIT(3)
#define USB2X_ADDRTYPE_sfr   BIT(4)

#define _VEN_PROCESS_NAME(name) ven_process_##name
#define VEN_PROCESS_NAME(name)	_VEN_PROCESS_NAME(name)
#define VEN_PROCESS(name)                                                                          \
	case name:                                                                                 \
		len = VEN_PROCESS_NAME(name)(ptr);                                                 \
		break // no ;

#define USB2X_ADDRTYPE_WRITE(area)                                                                 \
	case USB2X_ADDRTYPE_##area:                                                                \
		*(unsigned char __##area *)addr.address = addr.type & 0xff;                        \
		break // no;

#define USB2X_ISR(irq)                                                                             \
	void ISR_##irq(void) __interrupt(irq)                                                      \
	{                                                                                          \
		usb2x_isr(irq);                                                                    \
	}

#undef Z_UTIL_LISTIFY_128
#undef Z_UTIL_LISTIFY_129
#define Z_UTIL_LISTIFY_128(F, sep, ...)
#define Z_UTIL_LISTIFY_129(F, sep, ...) F(128, __VA_ARGS__)

#define SFR_LISTIFY(F, sep, ...) LISTIFY(256, F, sep, __VA_ARGS__...)

uint8_t usb_custom(uint8_t *ptr);
void usb2x_isr(uint8_t irq_num);
