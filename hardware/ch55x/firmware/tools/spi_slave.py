#!/usr/bin/env python3

import fire

from usb2x import CH55xDevice


# class CH552Registers:
#     SFR = {
#         "P0": 0x90,

#         "SPI0": {
#             "SETUP": 0xFC,
#             "S_PRE": 0xFB,
#             "CK_SE": 0xFB,
#             "CTRL": 0xFA,
#             "DATA": 0xF9,
#             "STAT": 0xF8,
#         },
#     }


# class CH552(CH552Registers):
#     def __init__(self):
#         self.dev = CH55xDevice()

#     def start(self):
#         while True:
#             self.dev.write_byte(
#                 self.SFR['P0'], 0xef, typeofaddr=CH55xDevice.USB2X_ADDRTYPE_sfr)
#             self.dev.write_byte(
#                 self.SFR['P0'], 0xff, typeofaddr=CH55xDevice.USB2X_ADDRTYPE_sfr)

#     def spi_init(self, dev="SPI0", slave=True):
#         self.dev.write_byte(
#             self.SFR[dev]["SETUP"], 0x80, typeofaddr=CH55xDevice.USB2X_ADDRTYPE_sfr)
#         pass

class USB2XRegisters(dict):
    def __init__(self, dev, typeofaddr):
        self.typeofaddr = typeofaddr
        self.dev = dev
        self.regs = {}

    def __getitem__(self, key):
        return self.dev.read_byte(self.regs[key], length=1, typeofaddr=self.typeofaddr)

    def __setitem__(self, key, value):
        self.dev.write_byte(self.regs[key], value,
                            typeofaddr=self.typeofaddr)


class CH552:
    def __init__(self):
        self.dev = CH55xDevice()
        self.regs = {
            "sfr": USB2XRegisters(self.dev, CH55xDevice.USB2X_ADDRTYPE_sfr),
        }


if __name__ == "__main__":
    fire.Fire(CH552)
