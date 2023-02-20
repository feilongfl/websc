#!/usr/bin/env python3

import fire
import logging

from usb2x import CH55xDevice

log: logging.Logger = logging.getLogger(name='usb2x')
log.setLevel(logging.NOTSET)


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
    def __init__(self, reg: dict, pinid: float) -> None:
        self.pinid = pinid
        self.reg = reg

        self.port, self.pin = str(self.pinid).split('.')
        self.pin = int(self.pin)
        pass

    def __eq__(self, other):
        match other:
            case int() | float():
                return self.pinid == other

            case USB2XPin():
                return self.pinid == other.pinid

            case _:
                log.critical(f'can not eq with {type(other)}')

    def __le__(self, other):
        val = self.reg['sfr'][f'P{self.port}'][0]
        bit = (1 << self.pin)
        match other:
            case 0:
                self.reg['sfr'][f'P{self.port}'] = val & ~bit
            case 1:
                self.reg['sfr'][f'P{self.port}'] = val | bit
            # open drain
            case 'z':
                self.reg['sfr'][f'P{self.port}'] = val | bit
                self.reg['sfr'][f'P{self.port}_MOD_OC'] = val | bit
            # no pull-up
            case 'x':
                self.reg['sfr'][f'P{self.port}_DIR_PU'] = val & ~bit
            # pull up
            case 'u':
                self.reg['sfr'][f'P{self.port}_DIR_PU'] = val | bit
            # push-pull
            case 'p':
                self.reg['sfr'][f'P{self.port}_MOD_OC'] = val & ~ bit
            case _:
                log.critical(f'can not set value {type(other)} to pin {self}')

    def __int__(self):
        return self.reg['sfr'][f'P{self.port}']

    def __repr__(self) -> str:
        return str(self.pinid)


class USB2XPinList(dict):
    def __init__(self, reg: dict, pinList: list):
        self.reg = reg
        self.pinmap = {}

        for p in pinList:
            self.pinmap[p] = USB2XPin(self.reg, p)

    def __getitem__(self, key):
        return self.pinmap[key]

    def __contains__(self, item):
        return item in self.pinmap.keys()

    def __repr__(self) -> str:
        # return ','.join({str(p) for p in self.pinlist})
        return ','.join({str(v) for p, v in self.pinmap.items()})


class CH55x:
    def __init__(self):
        self.dev = CH55xDevice()
        self.regs = {
            "sfr": USB2XRegisters(self.dev, CH55xDevice.USB2X_ADDRTYPE_sfr, {
                "P1": 0x90,
                "P1_MOD_OC": 0x92,
                "P1_DIR_PU": 0x93,
            }),
        }

    def port(self, method: str, pin):
        if pin not in self.pins:
            log.critical(f'Port "{ pin }" Not Found, We only have {self.pins}')

        match method:
            case 'sethigh' | 'high' | 'h':
                self.pins[pin] <= 1
            case 'setlow' | 'low' | 'l':
                self.pins[pin] <= 0
            case 'setopendrain' | 'setod' | 'od' | 'z':
                self.pins[pin] <= 'z'
            case 'pushpull':
                self.pins[pin] <= 'p'
            case 'nopullup' | 'npu' | 'x':
                self.pins[pin] <= 'x'
            case 'pullup' | 'pu' | 'u':
                self.pins[pin] <= 'u'
            case 'get' | 'read':
                return int(self.pins[pin])
            case _:
                log.critical(f'Method "{method}" Not Support')


class CH552E(CH55x):
    def __init__(self):
        super().__init__()

        self.pins = USB2XPinList(self.regs, [
            # port.pin
            1.4,
            1.5,
            1.6,
            1.7,

            # usb port can't use
        ])


if __name__ == "__main__":
    fire.Fire(CH552E)
