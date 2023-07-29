#define HEX_DIGIT(n) ((char)((n) + (((n) < 10) ? '0' : ('A' - 10))))

uint8_t port_a;
uint8_t port_b;
uint8_t port_c;
uint8_t port_d;

typedef struct pin {
  uint8_t *port;
  uint8_t index;
} pin;

pin pins[] = {
  { .port = &port_a, 0 },
  { .port = &port_a, 1 },
  { .port = &port_a, 2 },
  { .port = &port_a, 3 },
  { .port = &port_a, 4 },
  { .port = &port_a, 5 },
  { .port = &port_a, 6 },
  { .port = &port_a, 7 },
  { .port = &port_c, 0 }, //Data 0
  { .port = &port_c, 1 }, //Data 1
  { .port = &port_c, 2 }, //Data 2
  { .port = NULL, 0 },    //GND
  { .port = &port_c, 3 }, //Data 3
  { .port = &port_c, 4 }, //Data 4
  { .port = &port_c, 5 }, //Data 5
  { .port = &port_c, 6 }, //Data 6
  { .port = &port_c, 7 }, //Data 7
  { .port = &port_d, 7 },
  { .port = &port_d, 6 },
  { .port = NULL, 0 },    //#OE
  { .port = &port_d, 5 },
  { .port = &port_d, 4 },
  { .port = &port_d, 3 },
  { .port = NULL, 0 },    //VCC
};

typedef struct IC {
  char name[9];
  uint8_t adr_pins[13];
  uint8_t ctrl_pins_read_h[2];
} IC;

IC ics[] = {
  {
    .name = {'W','S','5','7','C','4','9','C','\0'},
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 }
  },
  {
    .name = {'M','C','M','6','8','7','6','4','\0'},
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 18, 21 }
  },
  {
    .name = {'2','5','3','2','\0'},
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 18 },
    .ctrl_pins_read_h = { 21 }
  },
  {
    .name = {'5','7','C','4','3','\0'},
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19 },
    .ctrl_pins_read_h = { 18 }
  },
  {
    .name = {'2','7','3','2','\0'},
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 21 }
  },
  {
    .name = {'2','7','1','6','\0'},
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19 },
    .ctrl_pins_read_h = { 21 }
  }
};


uint8_t num_ics = sizeof(ics) / sizeof(ics[0]);

uint8_t buffer[8192];

String str;
IC *selected_ic = NULL;
uint8_t selected_ic_size = 0;


void setup() {
  Serial.begin(9600);

  portMode(0, OUTPUT); // Port A

  //Upper 5 pins outputs Port D
  uint8_t oldSREG = SREG;
  cli();
  DDRD |= 0b11111000;
  SREG = oldSREG;
}

bool is_numeric(String string) {
  for(auto character : string) {
    if(!isDigit(character)) {
      return false;
    }
  }

  return true;
}

void print_help() {
  Serial.println("Eprom programmer help");
  Serial.println("?: This help menu");
  Serial.println("l: List devices");
  Serial.println("u: Upload data to buffer");
  Serial.println("d: Download data from buffer");
  Serial.println("w: Write buffer to device");
  Serial.println("r: Read device to buffer");
  Serial.println("c: Compare buffer with device");
  Serial.println("b: Blank-check the device");
  Serial.println("t [device number]: Select type of device");
  Serial.println("i: Info about currently selected device");
}

void list_devices() {
  for (uint8_t i = 0; i < num_ics; i++) {
    Serial.printf("%u) %s\n", i, ics[i].name);
  }
}

void write_byte(uint8_t byte) {
  uint8_t high_nibble = byte >> 4;
  uint8_t low_nibble = byte & 0x0FU;
  Serial.print(HEX_DIGIT(high_nibble));
  Serial.print(HEX_DIGIT(low_nibble));
}

void print_record(char *type, uint8_t *data, uint8_t data_length, uint16_t adress) {
  uint8_t record_length = data_length + 3; // 2 bytes for adress, 1 for checksum
  uint8_t chksum = record_length + (uint8_t) adress + (uint8_t) ( adress >> 8 );
  Serial.printf("%s%02hX%04hX", type, record_length, adress);
  for (uint8_t i = 0; i < data_length; i++) {
    write_byte(data[i]);
    chksum += data[i];
  }
  write_byte(~chksum);
  Serial.print('\n');
}

void print_buffer() {
  uint16_t data_byte_count = pow(2, selected_ic_size);
  uint8_t byte_per_record = 32;
  char header[] = "HDR";
  print_record("S0", (uint8_t*) header, sizeof(header) - 1, 0);

  for (uint16_t i = 0; i < data_byte_count; i+=byte_per_record) {
    print_record("S1", buffer + i, byte_per_record, i);
  }

  print_record("S5", NULL, 0, data_byte_count / byte_per_record);
  print_record("S9", NULL, 0, 0);
}

void select_device() {
  String arg;
  uint16_t arg_num;

  arg = str.substring(1);
  arg.trim();
  if (arg.length() == 0) {
    Serial.println("\"t\" requires a numeric argument");
    return;
  } else if(is_numeric(arg)) {
    arg_num = arg.toInt();
    if(arg_num < num_ics) {
      selected_ic = ics + arg_num;
      for(selected_ic_size = 0; selected_ic_size < sizeof(selected_ic->adr_pins); selected_ic_size++) {
        uint8_t adr_pin = selected_ic->adr_pins[selected_ic_size];
        if (adr_pin == 0) break;
      }
    } else {
      Serial.printf("No device with id \"%hu\" exists\n", arg_num);
      Serial.println("Use l to list available devices");
      return;
    }
  } else {
    Serial.println("\"t\" requires a numeric argument");
    return;
  }

  Serial.println("OK!");
}

void print_device_info() {
  if(selected_ic == NULL) {
    Serial.println("No device selected. Select a device with \"t\"");
    return;
  }

  Serial.printf("Currently selected device is: %s\n", selected_ic->name);
}

void set_adress(uint16_t adress) {
    port_a = 0;
    port_d = 0;

    for(uint8_t i = 0; i < selected_ic_size; i++) {
        uint8_t adr_pin = selected_ic->adr_pins[i];
        pin p = pins[adr_pin - 1];
        *(p.port) |= ( adress & 1 ) << p.index;
        adress = adress >> 1;
    }

    for(uint8_t i = 0; i < sizeof(selected_ic->ctrl_pins_read_h); i++) {
        uint8_t ctrl_pin = selected_ic->ctrl_pins_read_h[i];
        if (ctrl_pin == 0) break;
        pin p = pins[ctrl_pin - 1];
        *(p.port) |= 1 << p.index;
    }

    portWrite(0, port_a);

    uint8_t oldSREG = SREG;
    cli();
    PORTD &= 0b00000111; //Upper 5 bits used for adress
    PORTD |= port_d;
    SREG = oldSREG;
}

void read_device() {
  if(selected_ic == NULL) {
    Serial.println("No device selected. Select a device with \"t\"");
    return;
  }

  portMode(2, INPUT); // Port C

  for(uint16_t current_adress = 0; current_adress < pow(2, selected_ic_size); current_adress++) {
    set_adress(current_adress);
    buffer[current_adress] = portRead(2); // Port C
  }

  Serial.println("OK!");
}

void blank_check() {
  uint8_t data;

  if(selected_ic == NULL) {
    Serial.println("No device selected. Select a device with \"t\"");
    return;
  }

  portMode(2, INPUT); // Port C

  for(uint16_t current_adress = 0; current_adress < pow(2, selected_ic_size); current_adress++) {
    set_adress(current_adress);
    data = portRead(2); // Port C
    if(data != 0xFF) {
      Serial.println("Device not blank!");
      Serial.printf("%02hX read at adress: %04hX\n", data, current_adress);
      return;
    }
  }
  Serial.println("OK!");
}

void compare_data() {
  uint8_t data;

  if(selected_ic == NULL) {
    Serial.println("No device selected. Select a device with \"t\"");
    return;
  }

  portMode(2, INPUT); // Port C

  for(uint16_t current_adress = 0; current_adress < pow(2, selected_ic_size); current_adress++) {
    set_adress(current_adress);
    data = portRead(2); // Port C
    if(data != buffer[current_adress]) {
      Serial.println("Device data does not match current buffer!");
      Serial.printf("Device data: %02hX, buffer data: %02hX read at adress: %04hX\n", data, buffer[current_adress], current_adress);
      return;
    }
  }
  Serial.println("OK!");
}

void not_implemented() {
  Serial.println("This functionality is not yet implemented");
}

void loop() {
  if(Serial.available() > 0)
  {
    str = Serial.readStringUntil('\n');
    str.trim();
    if (str.length() > 0) {
      switch(str[0]) {
        case '?':
          print_help();
          break;
        case 'l':
          list_devices();
          break;
        case 'u':
          not_implemented();
          break;
        case 'd':
          print_buffer();
          break;
        case 'w':
          not_implemented();
          break;
        case 'r':
          read_device();
          break;
        case 'c':
          compare_data();
          break;
        case 'b':
          blank_check();
          break;
        case 't':
          select_device();
          break;
        case 'i':
          print_device_info();
          break;
        default:
          Serial.printf("Unknown command: %c\n", str[0]);
          print_help();
          break;
      }
    }
  }
}
