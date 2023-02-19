
#include "ch554.h"
#include "usb.h"
#include "usb_descr.h"
#include "usb_handler.h"
#include "usb_vendor.h"

static struct USB2X_POINTER {
	uint16_t type;
	uint16_t address;
};

static uint16_t usb2x_irq = 0;

static uint8_t VEN_PROCESS_NAME(VEN_REQ_BOOTLOADER)(uint8_t *ptr, uint8_t par)
{
	VEN_BOOT_flag = 1;
	ARG_UNUSED(ptr);
	ARG_UNUSED(par);

	return 0;
}

static uint8_t VEN_PROCESS_NAME(VEN_REQ_INT_GET)(uint8_t *ptr, uint8_t par)
{
	ARG_UNUSED(par);

	EA = 0;
	ptr[0] = usb2x_irq >> 8;
	ptr[1] = usb2x_irq & 0xff;
	usb2x_irq = 0;
	EA = 1;

	return 2;
}

// static uint8_t VEN_PROCESS_NAME(VEN_REQ_SEEK)(uint8_t *ptr, uint8_t par)
// {
// 	ARG_UNUSED(ptr);
// 	ARG_UNUSED(par);

// 	usb2x_seek_addr.type = USB_setupBuf->wIndexH << 8 | USB_setupBuf->wIndexL;
// 	usb2x_seek_addr.address = USB_setupBuf->wValueH << 8 | USB_setupBuf->wValueL;

// 	return 0;
// }

#define SFRDefine(addr, ...) __sfr __at(addr) UTIL_CAT(_sfr_, addr)
SFR_LISTIFY(SFRDefine, (;));

static void SFRwrite(uint16_t sfr, uint16_t data)
{
	switch (sfr) {
#define SFRwriteCase(addr, ...)                                                                    \
	case addr:                                                                                 \
		UTIL_CAT(_sfr_, addr) = data

		SFR_LISTIFY(SFRwriteCase, (;));

	default:
		break;
	}
}

static uint16_t SFRread(uint16_t sfr)
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

// index = address
// value = value to set
static uint8_t VEN_PROCESS_NAME(VEN_REQ_WRITE)(uint8_t *ptr, uint8_t par)
{
	uint16_t address;
	uint16_t value;

	ARG_UNUSED(ptr);

	address = USB_setupBuf->wIndexH << 8 | USB_setupBuf->wIndexL;
	value = USB_setupBuf->wValueH << 8 | USB_setupBuf->wValueL;

	switch (par - VEN_REQ_WRITE_code) {
		// USB2X_ADDRTYPE_WRITE(code); // can't write flash
		USB2X_ADDRTYPE_WRITE(data);
		USB2X_ADDRTYPE_WRITE(idata);
		USB2X_ADDRTYPE_WRITE(xdata);

	case USB2X_ADDRTYPE_sfr:
		// SFRwrite()
		break;

	default:
		break;
	}

	return 0;
}

// index = address
static uint8_t VEN_PROCESS_NAME(VEN_REQ_READ)(uint8_t *ptr, uint8_t par)
{
	uint16_t address;
	uint16_t length = USB_setupBuf->wLengthH << 8 | USB_setupBuf->wLengthL;
	if (length > EP0_BUF_SIZE)
		length = EP0_BUF_SIZE;
	uint16_t temp = 0;

	ARG_UNUSED(par);

	address = USB_setupBuf->wIndexH << 8 | USB_setupBuf->wIndexL;

	switch (par - VEN_REQ_READ_code) {
		USB2X_ADDRTYPE_READ(code);
		USB2X_ADDRTYPE_READ(data);
		USB2X_ADDRTYPE_READ(idata);
		USB2X_ADDRTYPE_READ(xdata);

	case USB2X_ADDRTYPE_sfr:
		temp = SFRread(address);
		ptr[0] = temp >> 8;
		ptr[1] = temp & 0xff;
		length = 2;
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
		VEN_PROCESS(VEN_REQ_INT_GET, VEN_REQ_INT_GET);

		VEN_PROCESS(VEN_REQ_BOOTLOADER, VEN_REQ_BOOTLOADER);

		// VEN_PROCESS(VEN_REQ_SEEK, VEN_REQ_SEEK);

		VEN_PROCESS(VEN_REQ_WRITE_code, VEN_REQ_WRITE);
		VEN_PROCESS(VEN_REQ_WRITE_data, VEN_REQ_WRITE);
		VEN_PROCESS(VEN_REQ_WRITE_idata, VEN_REQ_WRITE);
		VEN_PROCESS(VEN_REQ_WRITE_xdata, VEN_REQ_WRITE);
		VEN_PROCESS(VEN_REQ_WRITE_sfr, VEN_REQ_WRITE);
		VEN_PROCESS(VEN_REQ_READ_code, VEN_REQ_READ);
		VEN_PROCESS(VEN_REQ_READ_data, VEN_REQ_READ);
		VEN_PROCESS(VEN_REQ_READ_idata, VEN_REQ_READ);
		VEN_PROCESS(VEN_REQ_READ_xdata, VEN_REQ_READ);
		VEN_PROCESS(VEN_REQ_READ_sfr, VEN_REQ_READ);

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
