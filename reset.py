#!/usr/bin/python

import sys
import os
sys.path.append(os.path.join(os.path.dirname(os.path.realpath(sys.argv[0])), 'appdeps'))
import EasyMCP2221
import argparse
import time

parser = argparse.ArgumentParser()
parser.add_argument("-c", "--configure", help="configure MCP2221 for reset handling", action="store_true")
args = parser.parse_args()

mcp = EasyMCP2221.Device()

if args.configure:
    mcp.set_pin_function(gp3 = "GPIO_IN")
    mcp.save_config()
else:
    mcp.set_pin_function(gp3 = "GPIO_OUT", out3 = False)
    time.sleep(100 / 1000_000.0)
    mcp.set_pin_function(gp3 = "GPIO_IN")
