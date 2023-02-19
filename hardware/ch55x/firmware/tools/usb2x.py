#!/usr/bin/env python3

import usb.core
import fire

# USB device settings
VENDOR_ID = 0x1a86                    # VID (idVendor)
PRODUCT_ID = 0x4000                    # PID (idProduct)
# (bEndpointAddress) for bulk writing to device
BULK_EP_OUT = 0x01
# (bEndpointAddress) for bulk reading from device
BULK_EP_IN = 0x80 | BULK_EP_OUT

# USB vendor class control requests (bRequest)
VEN_REQ_BOOTLOADER = 0xF0
VEN_REQ_INT_GET = 0x10
VEN_REQ_SEEK = 0x20
VEN_REQ_WRITE_top = 0x30
VEN_REQ_READ_top = 0x40

VEN_REQ_WRITE = 0x40      # (bRequestType): vendor host to device
VEN_REQ_READ = 0xC0      # (bRequestType): vendor device to host

READ_BUF_MAX = 0xFF


class CH55xDevice():
    def __init__(self):
        self.dev = usb.core.find(idVendor=VENDOR_ID, idProduct=PRODUCT_ID)
        if self.dev is None:
            raise Exception('Device not found')
        try:
            self.dev.reset()
            self.dev.set_configuration()
        except:
            raise Exception('Could not access USB device')

    def sendcontrol(self, ctrl):
        return self.dev.ctrl_transfer(VEN_REQ_WRITE, ctrl, 0, 0)

    def check_interrupt(self):
        ret = self.dev.ctrl_transfer(
            VEN_REQ_READ, VEN_REQ_INT_GET, 0, 0, READ_BUF_MAX)
        interrupt = ret[0] << 8 | ret[1]
        return interrupt

    # define USB2X_ADDRTYPE_code  0
    # define USB2X_ADDRTYPE_data  1
    # define USB2X_ADDRTYPE_idata 2
    # define USB2X_ADDRTYPE_xdata 3
    # define USB2X_ADDRTYPE_sfr   4
    # index=address, value=data to write
    def read_byte(self, addr: int, length=1, typeofaddr=0):
        if typeofaddr == 4:
            length = 2
        ret = self.dev.ctrl_transfer(
            VEN_REQ_READ, VEN_REQ_READ_top + typeofaddr, 0, addr, length)
        return ','.join({hex(v) for v in ret})

    def write_byte(self, addr: int, data: int, typeofaddr=0):
        self.dev.ctrl_transfer(
            VEN_REQ_WRITE, VEN_REQ_WRITE_top + typeofaddr, data, addr)

    def test(self):
        while True:
            self.write_byte(0x90, 0xef, typeofaddr=4)
            self.write_byte(0x90, 0xff, typeofaddr=4)

    def reboot_loader(self):
        self.dev.ctrl_transfer(VEN_REQ_WRITE, VEN_REQ_BOOTLOADER, 0, 0)

    def seek(self, addr=0):
        addrH = addr >> 16
        addrL = addr & 0xffff
        self.dev.ctrl_transfer(VEN_REQ_WRITE, VEN_REQ_SEEK, addrL, addrH)

    def read(self, length=1):
        self.dev.read(BULK_EP_IN, length)

    def write(self, data):
        self.dev.write(BULK_EP_OUT, data)


if __name__ == "__main__":
    fire.Fire(CH55xDevice)
