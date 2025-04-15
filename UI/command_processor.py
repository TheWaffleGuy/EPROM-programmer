from enum import Enum

class CommandStatus(Enum):
    FINISHED = "finished"
    ERROR = "error"
    CONTINUE = "continue"

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
        self.txQueue = None
        self.result = []

    def append_character(self, char):
        if char == '\n':
            self.current_string = self.current_string.rstrip('\r\n')
            self.process_line(self.current_string)
            self.current_string = ""
        else:
            self.current_string += char

    def remove_last_character(self):
        self.current_string = self.current_string[:-1]

    def execute_commands(self, commands, txQueue, callback = lambda *args: None):
        self.commands = commands
        self.current_command_index = 0
        self.callback = callback
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
                    self.callback(self.result, status)
                    self.reset()
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
            self.device_list += [input_string]
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

    def process(self, input_string):
        if input_string == self.get_command():
            return CommandStatus.CONTINUE, None
        if input_string.startswith("No device selected"):
            return CommandStatus.ERROR, "No device selected"
        if input_string.startswith("Currently selected device is:"):
            return CommandStatus.FINISHED, input_string.split(":")[1].split('(')[0].strip()

    def get_command(self):
        return "I"
