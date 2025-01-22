#!/usr/bin/python

import sys
import os
sys.path.append(os.path.join(os.path.dirname(os.path.realpath(sys.argv[0])), 'appdeps'))
import EasyMCP2221
from EasyMCP2221.Constants import *
import argparse
import time

MANUFACTURER = "TheWaffleGuy"
PRODUCT = "EPROM-programmer"

def devices():
    devnum = 0
    try:
        mcp = EasyMCP2221.Device(devnum = devnum)
        while mcp:
            yield mcp
            devnum+=1
            mcp = EasyMCP2221.Device(devnum = devnum)
    except RuntimeError:
        pass

def configure_data_string(device, descriptor_type, data_str):
    data = list(bytearray(data_str.encode('utf-16-le')))
    data_length = len(data) + 2
    cmd = [CMD_WRITE_FLASH_DATA, descriptor_type, data_length, 0x03] + data
    device.send_cmd(cmd)

def configure(device):
    device.set_pin_function(gp3 = "GPIO_IN")
    configure_data_string(device, FLASH_DATA_USB_MANUFACTURER, MANUFACTURER)
    configure_data_string(device, FLASH_DATA_USB_PRODUCT, PRODUCT)
    device.enable_cdc_serial(True)
    device.save_config()
    device.reset()

def reset(device):
    device.set_pin_function(gp3 = "GPIO_OUT", out3 = False)
    time.sleep(100 / 1000_000.0)
    device.set_pin_function(gp3 = "GPIO_IN")
 

def main(configure_device):
    if configure_device:
        devlist = devices()
        mcp = None
        try:
            mcp = next(devlist)
        except StopIteration:
            print("Error: no MCP2221 found", file=sys.stderr)
            sys.exit(1)
        sentinel = object()
        if next(devlist, sentinel) != sentinel:
            print("Error: multiple MCP2221 connected. When configuring, make sure that only one device is connected", file=sys.stderr)
            sys.exit(1)
        configure(mcp)
    else:
        devlist = filter(lambda device: device.read_flash_info()["USB_VENDOR"] == MANUFACTURER and device.read_flash_info()["USB_PRODUCT"] == PRODUCT , devices())
        mcp = None
        try:
            mcp = next(devlist)
        except StopIteration:
            print("Error: no programmers found", file=sys.stderr)
            sys.exit(1)
        sentinel = object()
        if next(devlist, sentinel) != sentinel:
            print("Error: multiple programmers found", file=sys.stderr)
            sys.exit(1)
        reset(mcp)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("-c", "--configure", help="configure MCP2221 for reset handling", action="store_true")
    args = parser.parse_args()
    main(args.configure)
