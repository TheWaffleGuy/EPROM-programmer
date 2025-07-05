#!/usr/bin/python

from tool import find_programmer, get_com_port, reset, DeviceError
import time

class ATMega1284Programmer:
    # ATMega1284 specific constants
    MCU_CONSTANTS = {
        'FLASH_SIZE': 128 * 1024,  # 128KB flash
        'PAGE_SIZE': 256,          # bytes per page
        'EEPROM_SIZE': 4 * 1024,   # 4KB EEPROM
        'MCU_ID': 0x8C
    }

    URBOOT_CONSTANTS = {
        'DEFAULT_BAUD': 115200,
        'MAX_CPUS': 2040,
        'CMD_ENTER_PROG': 0x50,
        'CMD_LEAVE_PROG': 0x51,
        'CMD_WRITE_FLASH': 0x02,
        'CMD_READ_FLASH': 0x03,
        'MAX_SYNC_ATTEMPTS': 16,
        'INSYNC_BYTE_FIRST': 0x30,
        'INSYNC_BYTE_RETRY': 0x20,
        'OK_BYTE': 0x20,
        'FEATURE_READ_FLASH': 0x4,
        'CAP_AUTOBAUD': 128,
        'CAP_EEPROM': 64,
        'CAP_URPROTOCOL': 32, #v7.6 and v7.7 only
        'CAP_UPDATE_FL': 32, #from v8.0
        'CAP_DUAL': 16,
        'CAP_VBLMASK': 12, #up to v7.7 only
        'CAP_PROTECTME': 2,
        'CAP_HAS_CE': 1,
        'RET_OPCODE': 0x9508
    }

    def __init__(self, port, baud=None, debug=0):
        """
        Initialize the programmer
        
        Args:
            port (str): Serial port name
            baud (int): Baud rate (defaults to DEFAULT_BAUD if None)
            debug (bool): Enable debug output
        """
        import serial
        self.debug = debug
        self.baud = baud or self.URBOOT_CONSTANTS['DEFAULT_BAUD']
        self.serial = serial.Serial(
            port=port,
            baudrate=self.baud,
            timeout=0.1
        )
        self.sync_bytes = None  # Will store the two sync bytes after successful sync
        self.ur_features = 0
        self._log("Initialized programmer on port {} at {} baud".format(port, self.baud))

    def _log(self, message):
        """Print debug messages if debug is enabled"""
        if self.debug:
            print(f"DEBUG: {message}")
    
    def _serial_write(self, bytes):
        self.serial.write(bytes)
        if self.debug > 1:
            for byte in bytes:
                print(f"< 0x{byte:02x}")

    def _serial_read(self, size):
        resp = self.serial.read(size)
        if self.debug > 1:
            for byte in resp:
                print(f">       0x{byte:02x}")
        return resp

    def _sync_with_bootloader(self):
        """
        Synchronize with the bootloader. Sync is achieved when we get the same
        response bytes two times in a row, and the bytes aren't identical.
        
        Returns:
            tuple: (insync_byte, ok_byte) for verifying future responses
        
        Raises:
            Exception: If sync fails after maximum attempts
        """
        last_response = None
        
        for attempt in range(self.URBOOT_CONSTANTS['MAX_SYNC_ATTEMPTS']):
            self._log(f"Sync attempt {attempt + 1}")
            
            if attempt == 0:
                sync_bytes = bytes([self.URBOOT_CONSTANTS['INSYNC_BYTE_FIRST'], 
                                  self.URBOOT_CONSTANTS['OK_BYTE']])
            else:
                sync_bytes = bytes([self.URBOOT_CONSTANTS['INSYNC_BYTE_RETRY'], 
                                  self.URBOOT_CONSTANTS['OK_BYTE']])
                
            self._serial_write(sync_bytes)
            
            response = self._serial_read(2)
            
            if len(response) != 2:
                self._log("Timeout waiting for sync response")
                last_response = None
                continue
                
            response_bytes = tuple(response)
            self._log(f"Received sync bytes: 0x{response_bytes[0]:02X} 0x{response_bytes[1]:02X}")
            
            if response_bytes[0] == response_bytes[1]:
                self._log("Invalid sync response - bytes are identical")
                last_response = None
                continue
            
            if response_bytes == last_response:
                self._log("Sync successful")

                response16 = response_bytes[0]*255 + response_bytes[1]
                mcu_id = response16 % self.URBOOT_CONSTANTS['MAX_CPUS']
                self.ur_features = response16 // self.URBOOT_CONSTANTS['MAX_CPUS']
                if mcu_id == self.MCU_CONSTANTS['MCU_ID']:
                    self._log("Signature verified successfully")
                else:
                    raise Exception(f"Unexpected MCU ID. Got: 0x{mcu_id:02X}, Expected: 0x{self.MCU_CONSTANTS['MCU_ID']:02X}")

                return response_bytes
                
            last_response = response_bytes
            
        raise Exception("Failed to sync with bootloader after maximum attempts")

    def enter_programming_mode(self):
        """Enter programming mode"""
        self._log("Entering programming mode")
        
        self.sync_bytes = self._sync_with_bootloader()
        
        self._serial_write([self.URBOOT_CONSTANTS['CMD_ENTER_PROG'], self.URBOOT_CONSTANTS['OK_BYTE']])
        
        response = self._serial_read(2)
        
        if not self._verify_response(response):
            raise Exception("Invalid response format")
        
        return True
    
    def leave_programming_mode(self):
        """Leaving programming mode"""
        self._log("Leaving programming mode")

        self._serial_write([self.URBOOT_CONSTANTS['CMD_LEAVE_PROG'], self.URBOOT_CONSTANTS['OK_BYTE']])
        response = self._serial_read(2)
        if not self._verify_response(response):
            raise Exception("Invalid response format")

    def _verify_response(self, response):
        """
        Verify that response contains the sync bytes in the correct positions
        
        Args:
            response (bytes): Response to verify
        
        Returns:
            bool: True if response is valid
        """
        if not self.sync_bytes:
            raise Exception("Sync bytes not initialized - must sync first")
            
        if len(response) < 2:
            return False
            
        # First and last bytes should match sync bytes
        return response[0] == self.sync_bytes[0] and response[-1] == self.sync_bytes[1]

    def read_flash(self, address, length):
        """
        Read flash memory
        
        Args:
            address (int): flash address
            length (int): number of bytes to read
        Returns:
            bytes: Page data
        """

        if not self.ur_features & self.URBOOT_CONSTANTS['FEATURE_READ_FLASH']:
            raise Exception("Read flash must be enabled in bootloader")

        if length > 256:
            raise Exception(f"Invalid read length: {length}. Max length: 256")
            
        self._log(f"Reading {length} bytes at address 0x{address:04X}")

        read_cmd_bytes = [self.URBOOT_CONSTANTS['CMD_READ_FLASH'], address & 0xff, (address >> 8) & 0xff]
        if self.MCU_CONSTANTS['FLASH_SIZE'] > 0x10000:
            read_cmd_bytes.append((address >> 16) & 0xff)
        read_cmd_bytes.extend([0 if length == 256 else length, self.URBOOT_CONSTANTS['OK_BYTE']])
        
        self._serial_write(read_cmd_bytes)
        
        response = self._serial_read(length + 2)

        if len(response) < length + 2:
            raise Exception("Invalid response format")
        
        if not self._verify_response(response):
            raise Exception("Invalid response format")
            
        return response[1:-1]  # Return data between sync bytes

    def get_bootloader_info(self):
        bl_info = p.read_flash(p.MCU_CONSTANTS['FLASH_SIZE'] - 6, 6)

        bl_version = bl_info[5]
        bl_minor = bl_version & 0x07
        bl_major = bl_version >> 3
        bl_pages = bl_info[0] & 0x7f
        bl_size = bl_pages*self.MCU_CONSTANTS['PAGE_SIZE']
        bl_application_vector = bl_info[1] & 0x7f
        bl_capabilities = bl_info[4]
        bl_rjmpwp = bl_info[2] | bl_info[3]<<8

        if bl_version < 0x3f:
            raise Exception(f"Unsupported Bootloader version: {bl_minor}.{bl_major}. Update to at least 7.7")

        cap_str = ""
        cap_str += 'w' if bl_rjmpwp != self.URBOOT_CONSTANTS["RET_OPCODE"] else '-'
        cap_str += 'e' if self.URBOOT_CONSTANTS["CAP_EEPROM"] else '-'
        if bl_version > 0x3f:
            cap_str += 'U' if bl_capabilities & self.URBOOT_CONSTANTS["CAP_UPDATE_FL"] else '-'
        else:
            cap_str += 'u' if bl_capabilities & self.URBOOT_CONSTANTS["CAP_URPROTOCOL"] else 's'
        cap_str += 'd' if bl_capabilities & self.URBOOT_CONSTANTS["CAP_DUAL"] else '-'
        flags = (bl_capabilities//(self.URBOOT_CONSTANTS["CAP_VBLMASK"] & -self.URBOOT_CONSTANTS["CAP_VBLMASK"])) & (1 if bl_version > 0x3f else 3)
        cap_str += 'V' if flags == 3 else 'v' if flags == 2 else 'j' if flags else 'h'
        cap_str += 'P' if bl_capabilities & self.URBOOT_CONSTANTS["CAP_PROTECTME"] else 'p'
        cap_str += 'r'
        cap_str += 'a' if bl_capabilities & self.URBOOT_CONSTANTS["CAP_AUTOBAUD"] else '-'
        cap_str += 'c' if bl_capabilities & self.URBOOT_CONSTANTS["CAP_HAS_CE"] else '-'

        self._log(f"Bootloader size: {bl_size}")
        self._log(f"Bootloader version: u{bl_minor}.{bl_major} {cap_str}")
        self._log(f"Application vector: {bl_application_vector}")

    def __del__(self):
        """Cleanup when object is destroyed"""
        if hasattr(self, 'serial') and self.serial.is_open:
            self.serial.close()


if __name__ == '__main__':
    programmer = find_programmer()
    com_port = get_com_port(programmer)
    p = ATMega1284Programmer(com_port, debug = 2)
    reset(programmer)
    time.sleep(0.2)
    p.enter_programming_mode()
    p.get_bootloader_info()
    p.leave_programming_mode()