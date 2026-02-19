from enum import Enum

class CommandStatus(Enum):
    FINISHED = "finished"
    ERROR = "error"
    CONTINUE = "continue"
    PROGRESS = "progress"

class CommandProcessor:
    _instance = None

    def __new__(cls):
        if cls._instance is None:
            cls._instance = super(CommandProcessor, cls).__new__(cls)
            cls._instance.reset()
        return cls._instance

    def reset(self):
        self.commands = [DummyCommand()]
        self.current_command_index = 0
        self.current_string = ""
        self.callback = lambda *args: None
        self.progress_callback = lambda *args: None
        self.txQueue = None
        self.result = []

    def append_character(self, char):
        if char == '\n':
            self.current_string = self.current_string.rstrip('\r\n')
            self.process_line(self.current_string)
            self.current_string = ""
        else:
            self.current_string += char

            if self.current_command_index < len(self.commands):
                event = self.commands[self.current_command_index]
                status, result = event.check_progress(self.current_string)
                if status == CommandStatus.PROGRESS:
                    self.progress_callback(self.current_command_index, len(self.commands), result)

    def remove_last_character(self):
        self.current_string = self.current_string[:-1]

    def execute_commands(self, commands, txQueue, callback = lambda *args: None, progress_callback = lambda *args: None):
        self.commands = commands
        self.current_command_index = 0
        self.callback = callback
        self.progress_callback = progress_callback
        self.txQueue = txQueue
        txQueue.put(self.commands[self.current_command_index].get_command() + "\n")

    def process_line(self, input_string):
        event = self.commands[self.current_command_index]
        status, result = event.process(input_string)

        match status:
            case CommandStatus.FINISHED:
                self.result.append(result)
                self.current_command_index += 1
                if self.current_command_index >= len(self.commands):
                    l_result = self.result
                    l_callback = self.callback
                    self.reset()
                    l_callback(l_result, status)
                else:
                    self.txQueue.put(self.commands[self.current_command_index].get_command() + "\n")
            case CommandStatus.ERROR:
                self.result.append(result)
                self.callback(self.result, status)
                self.reset()
            case CommandStatus.CONTINUE:
                pass

class Command:
    def process(self, input_string):
        pass

    def check_progress(self, current_string):
        return CommandStatus.CONTINUE, None

    def command(self):
        pass

class DummyCommand(Command):
    def process(self, input_string):
        return CommandStatus.CONTINUE, None

class ReadDevice(Command):
    def process(self, input_string):
        if input_string == self.get_command():
            return CommandStatus.CONTINUE, None
        if input_string.startswith("No device selected"):
            return CommandStatus.ERROR, "No device selected"
        if input_string == "OK!":
            return CommandStatus.FINISHED, None
    
    def get_command(self):
        return "R"


class WriteDevicePromptCommand(Command):
    def process(self, input_string):
        if input_string == self.get_command():
            return CommandStatus.CONTINUE, None
        if input_string.startswith("No device selected"):
            return CommandStatus.ERROR, "No device selected"
        return CommandStatus.FINISHED, input_string.split('[')[0].strip()

    def get_command(self):
        return "W"

class WriteDevice(Command):
    def __init__(self):
        self.total_steps = 64

    def process(self, input_string):
        if input_string == self.get_command():
            return CommandStatus.CONTINUE, None
        if bool(input_string) and all(ch == "." for ch in input_string):
            return CommandStatus.CONTINUE, None
        if input_string == "OK!":
            return CommandStatus.FINISHED, input_string
        else:
            return CommandStatus.ERROR, input_string

    def check_progress(self, current_string):
        if bool(current_string) and all(ch == "." for ch in current_string):
            return CommandStatus.PROGRESS, (len(current_string), self.total_steps)
        return CommandStatus.CONTINUE, None

    def get_command(self):
        return "Y"

class DownloadData(Command):
    def __init__(self):
        self.data = ""

    def process(self, input_string):
        if input_string == self.get_command() and len(self.data) == 0:
            return CommandStatus.CONTINUE, None

        self.data += input_string + "\n"

        if input_string.startswith("S9"):
            return CommandStatus.FINISHED, self.data
        else:
            return CommandStatus.CONTINUE, None

    def get_command(self):
        return "D"

class ListDevices(Command):
    def __init__(self):
        self.device_list = []

    def process(self, input_string):
        if input_string == self.get_command() and len(self.device_list) == 0:
            return CommandStatus.CONTINUE, None
        if input_string == "OK!":
            return CommandStatus.FINISHED, self.device_list
        else:
            id, device = input_string.split(')', 1)
            manufacturer, name = device.split('-', 1)
            self.device_list += [{'id': id.strip(), 'manufacturer': manufacturer.strip(), 'name': name.strip()}]
            return CommandStatus.CONTINUE, None

    def get_command(self):
        return "L"

class BlankCheck(Command):
    def __init__(self):
        self.result = ""

    def process(self, input_string):
        if input_string == self.get_command() and len(self.result) == 0:
            return CommandStatus.CONTINUE, None
        if input_string.startswith("No device selected"):
            return CommandStatus.ERROR, "No device selected"
        if input_string == "OK!":
            return CommandStatus.FINISHED, input_string
        if len(self.result) == 0:
            self.result += input_string + "\n"
            return CommandStatus.CONTINUE, None
        else:
            self.result += input_string
            return CommandStatus.ERROR, self.result

    def get_command(self):
        return "B"

class Verify(Command):
    def __init__(self):
        self.result = ""

    def process(self, input_string):
        if input_string == self.get_command() and len(self.result) == 0:
            return CommandStatus.CONTINUE, None
        if input_string.startswith("No device selected"):
            return CommandStatus.ERROR, "No device selected"
        if input_string == "OK!":
            return CommandStatus.FINISHED, input_string
        if len(self.result) == 0:
            self.result += input_string + "\n"
            return CommandStatus.CONTINUE, None
        else:
            self.result += input_string
            return CommandStatus.ERROR, self.result

    def get_command(self):
        return "C"

class Info(Command):
    def _parse_kv_string(self, input_string):
        result = {}
        for item in [it.strip() for it in input_string.split(",") if it.strip()]:
            if ":" in item:
                key, val = item.split(":", 1)
                result[key.strip()] = val.strip()
            else:
                result[item] = ""
        return result

    def process(self, input_string):
        if input_string == self.get_command():
            return CommandStatus.CONTINUE, None
        if input_string.startswith("No device selected"):
            return CommandStatus.ERROR, "No device selected"
        if input_string.startswith("Currently selected device is:"):
            self.manufacturer_name = input_string.split(":")[1].split('(')[0].strip()
            return CommandStatus.CONTINUE, None
        else:
            extra_info = self._parse_kv_string(input_string)
            return CommandStatus.FINISHED, self.manufacturer_name

    def get_command(self):
        return "I"
