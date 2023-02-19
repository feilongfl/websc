
import sys
import time
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
VEN_REQ_BYTE_WRITE = 0x21
VEN_REQ_BYTE_READ = 0x22

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

    # def seek(self, addr):
    #     addrH = addr >> 16
    #     addrL = addr & 0xffff
    #     return self.dev.ctrl_transfer(VEN_REQ_READ, VEN_REQ_DMA_READ, addrL, addrH, READ_BUF_MAX)

    def reboot_loader(self):
        self.dev.ctrl_transfer(VEN_REQ_WRITE, VEN_REQ_BOOTLOADER, 0, 0)

    def seek(self, addr=0):
        addrH = addr >> 16
        addrL = addr & 0xffff
        self.dev.ctrl_transfer(VEN_REQ_WRITE, VEN_REQ_SEEK, addrL, addrH)

    def read(self, length=1):
        self.dev.read(BULK_EP_OUT, READ_BUF_MAX)


if __name__ == "__main__":
    # _main()
    fire.Fire(CH55xDevice)