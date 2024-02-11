#!/usr/bin/python

import EasyMCP2221
import argparse
import time

parser = argparse.ArgumentParser()
parser.add_argument("-c", "--configure", help="configure MCP2221 for reset handling", action="store_true")
args = parser.parse_args()

mcp = EasyMCP2221.Device()

if args.configure:
    mcp.set_pin_function(gp3 = "GPIO_IN")
    mcp.GPIO_write(gp3 = False)
    mcp.save_config()
else:
    mcp.set_pin_function(gp3 = "GPIO_OUT")
    time.sleep(100 / 1000_000.0)
    mcp.set_pin_function(gp3 = "GPIO_IN")
