#!/usr/bin/python

import sys
import os
sys.path.append(os.path.join(os.path.dirname(os.path.realpath(sys.argv[0])), 'appdeps'))
import EasyMCP2221
from EasyMCP2221.Constants import *
import argparse
import time
import serial.tools.list_ports;
import subprocess

MANUFACTURER = "TheWaffleGuy"
PRODUCT = "EPROM-programmer"
FQBN = "MightyCore:avr:1284:clock=20MHz_external,variant=modelNonP"


class NoDeviceFound(Exception):
    pass

class MultipleDevicesFound(Exception):
    pass

class MissingProgram(FileNotFoundError):
    pass

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

def find_programmer():
    devlist = filter(lambda device: device.read_flash_info()["USB_VENDOR"] == MANUFACTURER and device.read_flash_info()["USB_PRODUCT"] == PRODUCT , devices())
    device = None
    try:
        device = next(devlist)
    except StopIteration:
        raise NoDeviceFound("No programmers found") from None
    sentinel = object()
    if next(devlist, sentinel) != sentinel:
        raise MultipleDevicesFound("Multiple programmers found")
    return device

def find_first_mcp2221():
    devlist = devices()
    device = None
    try:
        device = next(devlist)
    except StopIteration:
        raise NoDeviceFound("No MCP2221 found") from None
    sentinel = object()
    if next(devlist, sentinel) != sentinel:
        raise MultipleDevicesFound("Multiple MCP2221 connected. When configuring, make sure that only one device is connected")
    return device

def configure(device=None):
    if device == None:
        device = find_first_mcp2221()
    device.set_pin_function(gp3 = "GPIO_IN")
    configure_data_string(device, FLASH_DATA_USB_MANUFACTURER, MANUFACTURER)
    configure_data_string(device, FLASH_DATA_USB_PRODUCT, PRODUCT)
    device.enable_cdc_serial(True)
    device.save_config()
    device.reset()

def reset(device=None):
    if device == None:
        device = find_programmer()
    device.set_pin_function(gp3 = "GPIO_OUT", out3 = False)
    time.sleep(100 / 1000_000.0)
    device.set_pin_function(gp3 = "GPIO_IN")

def get_com_port(device=None):
    if device == None:
        device = find_programmer()
    comports = [com for com in serial.tools.list_ports.comports() if com.serial_number == device.read_flash_info()["USB_SERIAL"]]
    return comports[0].device

def update(file, device=None):
    if device == None:
        device = find_programmer()
    com_port = get_com_port(device)
    reset(device)
    try:
        subprocess.run(["arduino-cli", "upload", "-b", FQBN, "-i", file, "-p", com_port])
    except FileNotFoundError:
        raise MissingProgram("arduino-cli required, but not found. Either it isn't installed or it it isn't in PATH")

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(dest="operation", required=True)
    configure_parser = subparsers.add_parser('configure', help="configure MCP2221 for reset handling")
    com_port_parser = subparsers.add_parser('get_com_port', help="get com-port of connected programmer")
    reset_parser = subparsers.add_parser('reset', help="reset programmer")
    update_parser = subparsers.add_parser('update', help="update firmware of programmer")
    update_parser.add_argument('-f', required=True, help="firmware file to flash")
    args = parser.parse_args()
    match args.operation:
        case 'configure':
            configure()
        case 'get_com_port':
            print(get_com_port())
        case 'reset':
            reset()
        case 'update':
            update(args.f)
