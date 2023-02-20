
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
static uint8_t usb2x_iic = 0;

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

static void SFRwrite(uint16_t sfr, uint8_t data)
{
	switch (sfr) {
#define SFRwriteCase(addr, ...)                                                                    \
	case addr:                                                                                 \
		UTIL_CAT(_sfr_, addr) = data;                                                      \
		break

		SFR_LISTIFY(SFRwriteCase, (;));

	default:
		break;
	}
}

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
		// SFRwrite(address, value);
		SFRwrite(address, USB_setupBuf->wValueL);
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

	ARG_UNUSED(par);

	address = USB_setupBuf->wIndexH << 8 | USB_setupBuf->wIndexL;

	switch (par - VEN_REQ_READ_code) {
		USB2X_ADDRTYPE_READ(code);
		USB2X_ADDRTYPE_READ(data);
		USB2X_ADDRTYPE_READ(idata);
		USB2X_ADDRTYPE_READ(xdata);

	case USB2X_ADDRTYPE_sfr:
		ptr[0] = SFRread(address);
		length = 1;
		break;

	default:
		return 0;
	}

	return length;
}

#define USB_CUSTOM_IIC_FLAG_COMMAND 1
#define USB_CUSTOM_IIC_FLAG_DATA    0

// | cmd | parameter |
// | ... |   .....   |
#define USB_CUSTOM_IIC_COMMAND_RESET (0x0 << 5)
#define USB_CUSTOM_IIC_COMMAND_DELAY (0x1 << 5) // par: time_100us x n
#define USB_CUSTOM_IIC_COMMAND_START (0x2 << 5)
#define USB_CUSTOM_IIC_COMMAND_STOP  (0x3 << 5)
#define USB_CUSTOM_IIC_COMMAND_READ  (0x4 << 5)
#define USB_CUSTOM_IIC_COMMAND_WRITE (0x5 << 5)
#define USB_CUSTOM_IIC_COMMAND_Trig  (0x6 << 5)
#define USB_CUSTOM_IIC_COMMAND_CHECK (0x7 << 5)

struct usb_custom_iic {
	uint8_t point;
	uint8_t data[32];
	uint32_t flag; // 0 = data, 1 = flags
};

static void usb_custom_iic_set(struct usb_custom_iic *iic, uint8_t flag, uint8_t val)
{
	if (flag)
		iic->flag |= 1 << iic->point;
	else
		iic->flag &= ~(1 << iic->point);

	iic->data[iic->point] = val;
	iic->point++; // fixme: overflow check
}

static uint8_t VEN_PROCESS_NAME(VEN_REQ_IIC)(uint8_t *ptr, uint8_t par)
{
	uint16_t command;
	uint8_t i;
	__xdata static struct usb_custom_iic iic;
	uint8_t *iic_ptr;
	ARG_UNUSED(par);

	command = USB_setupBuf->wIndexH << 8 | USB_setupBuf->wIndexL;

	switch (command) {
	case USB_CUSTOM_IIC_COMMAND_RESET:
		iic.point = 0; // clear buf
		break;
	case USB_CUSTOM_IIC_COMMAND_START:
	case USB_CUSTOM_IIC_COMMAND_STOP:
	case USB_CUSTOM_IIC_COMMAND_READ:
	case USB_CUSTOM_IIC_COMMAND_DELAY:
		usb_custom_iic_set(&iic, USB_CUSTOM_IIC_FLAG_COMMAND,
				   command | (USB_setupBuf->wValueL & 0x3f));
		break;

	case USB_CUSTOM_IIC_COMMAND_WRITE:
		usb_custom_iic_set(&iic, USB_CUSTOM_IIC_FLAG_DATA, USB_setupBuf->wValueL);
		break;

	case USB_CUSTOM_IIC_COMMAND_CHECK:
		iic_ptr = (uint8_t *)(&iic);
		for (i = 0; i < sizeof(iic); i++) {
			ptr[i] = iic_ptr[i];
		}

		return sizeof(iic);
	case USB_CUSTOM_IIC_COMMAND_Trig:
		usb2x_iic = 1;
		break;
	default:
		break;
	}

	return 0;
}

void usb_custom_iic_polling()
{
	if (!usb2x_iic) {
		return;
	}

	usb2x_iic = 0;
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

		VEN_PROCESS(VEN_REQ_IIC, VEN_REQ_IIC);

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
