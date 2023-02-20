#!/usr/bin/env python3

import fire
import logging

from usb2x import CH55xDevice

log: logging.Logger = logging.getLogger(name='usb2x')


class USB2XRegisters(dict):
    def __init__(self, dev, typeofaddr, regs):
        self.typeofaddr = typeofaddr
        self.dev = dev
        self.regs = regs

    def __getitem__(self, key):
        return self.dev.read_byte(self.regs[key], length=1, typeofaddr=self.typeofaddr)

    def __setitem__(self, key, value):
        self.dev.write_byte(self.regs[key], value,
                            typeofaddr=self.typeofaddr)


class USB2XPin():
    # pin = port.pin, example: port2 - pin3 = 2.3
    def __init__(self, reg: dict, pin: float) -> None:
        self.pin = pin
        self.reg = reg
        pass

    def __eq__(self, other):
        match other:
            case int() | float():
                return self.pin == other

            case USB2XPin():
                return self.pin == other.pin

            case _:
                log.critical(f'can not eq with {type(other)}')

    def __le__(self, other):
        match other:
            case 0:
                self.reg['sfr']['P1'] = 0xef
            case 1:
                self.reg['sfr']['P1'] = 0xff
            # case 'z':
            case _:
                log.critical(f'can not set value {type(other)} to pin {self}')

    def __repr__(self) -> str:
        return str(self.pin)


class USB2XPinList(dict):
    def __init__(self, reg: dict, pinList: list):
        self.pinlist = pinList
        self.reg = reg

    def __getitem__(self, key: float):
        return USB2XPin(self.reg, key)

    def items(self):
        return {USB2XPin(self.reg, p) for p in self.pinlist}

    def __repr__(self) -> str:
        return ','.join({str(p) for p in self.pinlist})


class CH552:
    def __init__(self):
        self.dev = CH55xDevice()
        self.regs = {
            "sfr": USB2XRegisters(self.dev, CH55xDevice.USB2X_ADDRTYPE_sfr, {
                "P1": 0x90,
                "P1_MOD_OC": 0x92,
                "P1_DIR_PU": 0x93,
            }),
        }

        # if ch552e
        self.pins = USB2XPinList(self.regs, [
            # port.pin
            1.4,
            1.5,
            1.6,
            1.7,

            # usb port can't use
        ])

    def port(self, method: str, pin):
        if pin not in self.pins:
            log.critical(f'Port "{ pin }" Not Found, We only have {self.pins}')

        match method:
            case 'sethigh' | 'high' | 'h':
                self.pins[pin] <= 1
            case 'setlow' | 'low' | 'l':
                self.pins[pin] <= 0
            case 'setod' | 'setopendrain' | 'od':
                self.pins[pin] <= 'z'
            case 'get' | 'read':
                return self.pins[pin]
            case _:
                log.critical(f'Method "{method}" Not Support')


if __name__ == "__main__":
    fire.Fire(CH552)
