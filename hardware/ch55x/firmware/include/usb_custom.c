
#include "usb_custom.h"
#include "ch554.h"
#include "usb.h"
#include "usb_descr.h"
#include "usb_handler.h"
#include "usb_vendor.h"

static struct USB2X_SEEK_ADDRESS {
	uint16_t type;
	uint16_t address;
} usb2x_seek_addr;

static uint16_t usb2x_irq = 0;

static uint8_t VEN_PROCESS_NAME(VEN_REQ_BOOTLOADER)(uint8_t *ptr)
{
	VEN_BOOT_flag = 1;
	ARG_UNUSED(ptr);

	return 0;
}

static uint8_t VEN_PROCESS_NAME(VEN_REQ_INT_GET)(uint8_t *ptr)
{
	EA = 0;
	ptr[0] = usb2x_irq >> 8;
	ptr[1] = usb2x_irq & 0xff;
	usb2x_irq = 0;
	EA = 1;

	return 2;
}

static uint8_t VEN_PROCESS_NAME(VEN_REQ_SEEK)(uint8_t *ptr)
{
	ARG_UNUSED(ptr);

	usb2x_seek_addr.type = USB_setupBuf->wIndexH << 8 | USB_setupBuf->wIndexL;
	usb2x_seek_addr.address = USB_setupBuf->wValueH << 8 | USB_setupBuf->wValueL;

	return 0;
}

static uint8_t VEN_PROCESS_NAME(VEN_REQ_BYTE_WRITE)(uint8_t *ptr)
{
	struct USB2X_SEEK_ADDRESS addr;
	ARG_UNUSED(ptr);

	addr.type = USB_setupBuf->wIndexH << 8 | USB_setupBuf->wIndexL;
	addr.address = USB_setupBuf->wValueH << 8 | USB_setupBuf->wValueL;

	switch (addr.type >> 8) {
		// USB2X_ADDRTYPE_WRITE(code); // can't write flash
		USB2X_ADDRTYPE_WRITE(idata);
		USB2X_ADDRTYPE_WRITE(xdata);
		USB2X_ADDRTYPE_WRITE(data);

	case USB2X_ADDRTYPE_sfr:
		// *(unsigned __sfr *)addr.address = addr.type & 0xff;
		break;

	default:
		break;
	}

	return 0;
}

#define SFRDefine(addr, ...) __sfr __at(addr) UTIL_CAT(_sfr_, addr)
SFR_LISTIFY(SFRDefine, (;));

static uint8_t SFRread(uint16_t sfr)
{
	switch (sfr) {
#define SFRreadCase(addr, ...)                                                                     \
	case addr:                                                                                 \
		return UTIL_CAT(_sfr_, addr)

		SFR_LISTIFY(SFRreadCase, (;));

	default:
		break;
	}
	return 0;
}

static uint8_t VEN_PROCESS_NAME(VEN_REQ_BYTE_READ)(uint8_t *ptr)
{
	struct USB2X_SEEK_ADDRESS addr;
	uint16_t length = USB_setupBuf->wLengthH << 8 | USB_setupBuf->wLengthL;
	if (length > EP0_BUF_SIZE)
		length = EP0_BUF_SIZE;

	addr.type = USB_setupBuf->wIndexH << 8 | USB_setupBuf->wIndexL;
	addr.address = USB_setupBuf->wValueH << 8 | USB_setupBuf->wValueL;

	switch (addr.type >> 8) {
	case USB2X_ADDRTYPE_code:
		for (uint8_t i = 0; i < length; i++) {
			ptr[i] = *((unsigned char __code *)addr.address + i);
		}

		break;
	case USB2X_ADDRTYPE_idata:
		ptr[0] = *(unsigned char __idata *)addr.address;
		break;
	case USB2X_ADDRTYPE_xdata:
		ptr[0] = *(unsigned char __xdata *)addr.address;
		break;
	case USB2X_ADDRTYPE_data:
		ptr[0] = *((unsigned char __data *)(addr.address));
		break;

	case USB2X_ADDRTYPE_sfr:
		ptr[0] = SFRread(addr.address);
		break;

	default:
		return 0;
	}

	return length;
}

uint8_t usb_custom(uint8_t *ptr)
{
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

void usb2x_isr(uint8_t irq_num)
{
	EA = 0;
	usb2x_irq |= BIT(irq_num);
	EA = 1;
}
