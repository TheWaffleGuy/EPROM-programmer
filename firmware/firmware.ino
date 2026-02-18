#ifdef ARDUINO_AVR_ATmega1284
#include <Arduino.h>
#include <util/atomic.h>
#include <EEPROM.h>
#include <assert.h>
#endif

#include "firmware.h"
#define restrict __restrict__
extern "C" {
  #include "kk_srec.h"
}
#include "device_definitions.h"
#include "device_operations.h"

#define MAX_LINE_LENGTH 80
#define DATA_BUFFER_SIZE 8192
#define SERIAL_SPEED 38400

#define STATE_NORMAL        0
#define STATE_SREC          1
#define STATE_VCAL          2
#define STATE_CONFIRM_WRITE 3

#define EPROM_ADR_2364_MODE ( EEPROM.length() - 11 )
#define EPROM_ADR_VCC_OFFSETS ( EEPROM.length() - 10 )
#define EPROM_ADR_IS_CALIBRATED ( EEPROM.length() - 2 )
#define EPROM_ADR_EPROM_VER ( EEPROM.length() - 1 )
#define EPROM_VER 0xFF

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
voltage_offsets v_offset { 0 };
#pragma GCC diagnostic pop

char line[MAX_LINE_LENGTH + 1];
uint8_t line_length = 0;

uint8_t state = STATE_NORMAL;

uint8_t buffer[DATA_BUFFER_SIZE];
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
IC selected_ic = {0};
#pragma GCC diagnostic pop
uint8_t selected_ic_size = 0;
uint8_t is_2364_mode;
uint8_t adr_pins_original_2364_mode[13];

void readCal() {
  if (EEPROM.read(EPROM_ADR_IS_CALIBRATED) == 0xFF) {
    return;
  }

  EEPROM.get(EPROM_ADR_VCC_OFFSETS, v_offset);
}

void setup() {
  Serial.begin(SERIAL_SPEED);

  if (EEPROM.read(EPROM_ADR_EPROM_VER) == EPROM_VER) {
    readCal();
    is_2364_mode = EEPROM.read(EPROM_ADR_2364_MODE) != 0xFF;
  }

  device_operations_init();
}

bool is_hex(char *string) {
  while(*string && (isdigit(*string) || ((*string | 0b00100000) >= 'a' && (*string | 0b00100000) <= 'f'))) string++;
  while(*string && isspace(*string)) string++;

  return *string == '\0';
}

void print_help() {
  Serial.println("Eprom programmer help");
  Serial.println("H: This help menu");
  Serial.println("L [search string]: List devices");
  Serial.println("S[srec data]: Upload SREC to buffer");
  Serial.println("D: Download data from buffer");
  Serial.println("W: Write buffer to device");
  Serial.println("R: Read device to buffer");
  Serial.println("C: Compare buffer with device");
  Serial.println("B: Blank-check the device");
  Serial.println("T [device number]: Select type of device");
  Serial.println("I: Info about currently selected device");
  Serial.println("V: Calibrate VCC and VPP voltages");
  Serial.println("X [0/1]: Toggle \"2364 mode\"");
}

PGM_P strnstr_P_ram(PGM_P pgm_str, const char* ram_str, size_t n) {
  if (n == 0) {
    return pgm_str;
  }

  char first_ram_char = *ram_str;
  char pgm_char;

  while ((pgm_char = pgm_read_byte(pgm_str))) {
    if (pgm_char == first_ram_char && strncmp_P(ram_str, pgm_str, n) == 0) {
      return pgm_str;
    }
    pgm_str++;
  }

  return NULL;
}

void list_devices() {
  char *arg;
  uint8_t has_search_arg;

  arg = line + 1;
  while(*arg && isspace(*arg)) arg++;

  has_search_arg = *arg != '\0';

  char *chr = arg;
  while(*chr) {
      *chr=toupper(*chr);
      chr++;
  }

  for (uint8_t i = 0; i < num_ics; i++) {
    if (has_search_arg) {
      uint8_t match = 1;
      const char* p = arg;
      const char* token_start = p;

      while (*p != '\0' && match) {
        while (*p != ' ' && *p != '\0') {
          p++;
        }

        size_t token_len = p - token_start;

        int is_dash = token_len == 1 && *token_start == '-';

        match = is_dash ||
                strnstr_P_ram(ics[i].manufacturer, token_start, token_len) ||
                strnstr_P_ram(ics[i].name,         token_start, token_len);

        while (*p == ' ') {
          p++;
        }
        token_start = p;
      }

      if (!match) {
        continue;
      }
    }

    write_byte(i);
    Serial.print(") ");
    Serial.print(reinterpret_cast<const __FlashStringHelper *>(ics[i].manufacturer));
    for(uint8_t j = 0; j < sizeof(ics[i].manufacturer) - 1 - strlen_P(ics[i].manufacturer); j++) {
      Serial.print(' ');
    }
    Serial.print(" - ");
    Serial.println(reinterpret_cast<const __FlashStringHelper *>(ics[i].name));
  }
  Serial.println("OK!");
}

void write_byte(uint8_t byte) {
  union { uint8_t val; struct { uint8_t low_nibble: 4; uint8_t high_nibble: 4; } val_split; } data;
  data.val = byte;
  Serial.print(HEX_DIGIT(data.val_split.high_nibble));
  Serial.print(HEX_DIGIT(data.val_split.low_nibble));
}

void __attribute__((always_inline)) inline write_2byte(uint16_t byte2) {
  union { uint16_t val; uint8_t val_split[2]; } data;
  data.val = byte2;
  write_byte(data.val_split[1]);
  write_byte(data.val_split[0]);
}

void print_record(const char *type, uint8_t *data, uint8_t data_length, uint16_t address) {
  uint8_t record_length = data_length + 3; // 2 bytes for address, 1 for checksum
  uint8_t chksum = record_length + (uint8_t) address + (uint8_t) ( address >> 8 );
  Serial.print(type);
  write_byte(record_length);
  write_2byte(address);
  for (uint8_t i = 0; i < data_length; i++) {
    write_byte(data[i]);
    chksum += data[i];
  }
  write_byte(~chksum);
  Serial.println();
}

void print_buffer() {
  uint16_t data_byte_count = 1U << selected_ic_size;
  uint8_t byte_per_record = 32;
  char header[] = { 'H', 'D', 'R' };
  print_record("S0", (uint8_t*) header, sizeof(header), 0);

  for (uint16_t i = 0; i < data_byte_count; i+=byte_per_record) {
    print_record("S1", buffer + i, byte_per_record, i);
  }

  print_record("S5", NULL, 0, data_byte_count / byte_per_record);
  print_record("S9", NULL, 0, 0);
}

void print_volt(int volt) {
    int whole = volt / 8;
    int rem8 = volt % 8;
    int frac = (rem8 * 1000) / 8;
    Serial.print(whole, DEC);

    if(frac == 0) {
      return;
    }

    Serial.print(".");

    while(frac % 10 == 0) {
      frac /= 10;
    }

    Serial.print(frac, DEC);
}

void print_dev() {
  Serial.print(selected_ic.manufacturer);
  Serial.print(" - ");
  Serial.print(selected_ic.name);
  if(selected_ic.device_definition.f_2364_compat_pinout && is_2364_mode) {
    Serial.print(" (2364 mode)");
  }
  Serial.println();
  Serial.print("Size: ");
  Serial.print(0x01 << selected_ic_size, DEC);
  Serial.print("x8, VPP: ");
  print_volt(selected_ic.device_definition.vpp);
  Serial.println("V");
}

void select_device() {
  char *arg;
  char *arg_tmp;
  uint8_t arg_num;

  arg = line + 1;
  while(*arg && isspace(*arg)) arg++;

  if (arg[0] == '0' && (arg[1] | 0b00100000) == 'x') arg+=2;

  if (! *arg || !is_hex(arg)) {
    Serial.println("Error: \"t\" requires a hex argument");
    return;
  }

  arg_tmp = arg;
  arg_num = *arg_tmp <= '9' ? *arg_tmp - '0' : ( *arg_tmp | 0b00100000 ) - 'a' + 10;
  arg_tmp++;
  if ( *arg_tmp ) {
    arg_num <<= 4;
    arg_num |= *arg_tmp <= '9' ? *arg_tmp - '0' : ( *arg_tmp | 0b00100000 ) - 'a' + 10;
    arg_tmp++;
  }

  if ( ( *arg_tmp && !isspace(arg[2]) ) || arg_num >= num_ics ) {
    Serial.print("No device with id ");
    Serial.print(arg);
    Serial.println(" exists");
    Serial.println("Use l to list available devices");
    return;
  }

  memcpy_P(&selected_ic, &(ics[arg_num]), sizeof(ics[0]));
  for(selected_ic_size = 0; selected_ic_size < sizeof(selected_ic.device_definition.adr_pins); selected_ic_size++) {
    uint8_t adr_pin = selected_ic.device_definition.adr_pins[selected_ic_size];
    if (adr_pin == 0) break;
  }

  if(selected_ic.device_definition.f_2364_compat_pinout && is_2364_mode) {
    memcpy(adr_pins_original_2364_mode, selected_ic.device_definition.adr_pins, sizeof(adr_pins_original_2364_mode));
    memcpy_P(selected_ic.device_definition.adr_pins, adr_pins_2364, sizeof(selected_ic.device_definition.adr_pins));
  }

  Serial.print("Selected: ");
  print_dev();
  Serial.println("OK!");
}

void print_device_info() {
  if(selected_ic.name[0] == '\0') {
    Serial.println("No device selected. Select a device with \"t\"");
    return;
  }

  Serial.print("Currently selected device is: ");
  print_dev();
  Serial.println("OK!");
}

uint8_t parse_decimal(char *string, uint8_t* integer_part, uint16_t* thousandths_part)  {
  uint8_t digits = 0;
  char thousandths[3 + 1] = "000";
  char *start = string;

  while(*string && isspace(*string)) string++;

  while(*string && isdigit(*string)) {
    string++;
    digits++;
    if(digits > 2) {
      return 0;
    }
  }

  if(*string && *string != ',' && *string != '.') {
    return 0;
  }
  string++;

  digits = 0;
  while(*string && isdigit(*string)) {
    if(digits < sizeof(thousandths) - 1) {
      thousandths[digits] = *string;
    }
    string++;
    digits++;
  }

  if(*string == '\0') {
    *integer_part = atol(start);
    *thousandths_part = atol(thousandths);
    return 1;
  } else {
    return 0;
  }
}

void voltage_calibration() {
  static uint8_t step = 0;
  
  uint8_t integer_part;
  uint16_t thousandths_part;

  uint16_t target;
  uint16_t measured;
  
  switch(step) {
    case 0:
      Serial.println("Warning! Remove any ICs from socket before continuing!");
      Serial.println("Continue? (y/n)");
      step++;
      break;
    case 1:
      switch(line[0] | 0b00100000) { //Force lower-case 
        case 'y':
          setVCC(VCC_CAL_LOW, 0);
          Serial.println("Enter voltage measured on VCC (1/2):");
          step++;
          break;
        case 'n':
          step = 0;
          state = STATE_NORMAL;
          Serial.println("Cancelled!");
          break;
        default:
          step = 0;
          state = STATE_NORMAL;
          Serial.println("Unknown option selected, cancelled!");
          break;
      }
      break;
    case 2:
      if (parse_decimal(line, &integer_part, &thousandths_part)) {
        target = ( VCC_CAL_LOW << 4 ) + 13;
        measured = ( ( integer_part * 8 ) << 4 ) + ( (uint32_t) thousandths_part * 128 ) / 1000;
        v_offset.vcc_low = target - measured;

        setVCC(VCC_CAL_HIGH, 0);
        Serial.println("Enter voltage measured on VCC (2/2):");
        step++;
      } else {
        Serial.println("Error: invalid input! Use format dd.ddd");
      }
      break;
    case 3:
      if (parse_decimal(line, &integer_part, &thousandths_part)) {
        target = ( VCC_CAL_HIGH << 4 ) + 13;
        measured = ( ( integer_part * 8 ) << 4 ) + ( (uint32_t) thousandths_part * 128 ) / 1000;
        v_offset.vcc_high = target - measured;

        setVPP(VPP_CAL_LOW, 0);
        Serial.println("Enter voltage measured on VPP (1/2):");
        step++;
      } else {
        Serial.println("Error: invalid input! Use format dd.ddd");
      }
      break;
    case 4:
      if (parse_decimal(line, &integer_part, &thousandths_part)) {
        target = ( VPP_CAL_LOW << 4 ) + 13;
        measured = ( integer_part * 8 << 4 ) + ( (uint32_t) thousandths_part * 128 ) / 1000;
        v_offset.vpp_low = target - measured;

        setVPP(VPP_CAL_HIGH, 0);
        Serial.println("Enter voltage measured on VPP (2/2):");
        step++;
      } else {
        Serial.println("Error: invalid input! Use format dd.ddd");
      }
      break;
    case 5:
      if (parse_decimal(line, &integer_part, &thousandths_part)) {
        target = ( VPP_CAL_HIGH << 4 ) + 13;
        measured = ( integer_part * 8 << 4 ) + ( (uint32_t) thousandths_part * 128 ) / 1000;
        v_offset.vpp_high = target - measured;

        resetVCCandVPP();
        step = 0;
        state = STATE_NORMAL;

        EEPROM.update(EPROM_ADR_IS_CALIBRATED, 0xFF);
        EEPROM.put(EPROM_ADR_VCC_OFFSETS, v_offset);
        EEPROM.update(EPROM_ADR_IS_CALIBRATED, 1);

        Serial.println("OK!");
      } else {
        Serial.println("Error: invalid input! Use format dd.ddd");
      }
      break;
  }
}

void confirm_write_data() {
  if(selected_ic.name[0] == '\0') {
    Serial.println("No device selected. Select a device with \"t\"");
    return;
  }

  state = STATE_CONFIRM_WRITE;

  Serial.print("Do you want to continuing programming ");
  Serial.print(selected_ic.manufacturer);
  Serial.print(" - ");
  Serial.print(selected_ic.name);
  Serial.println("? [y/n]");
}

void toggle_2364_mode() {
  char *arg;
  uint8_t new_2364_mode;

  arg = line + 1;
  while(*arg && isspace(*arg)) arg++;

  if (! *arg) {
    Serial.println("Error: \"x\" requires an argument");
    return;
  }

  if (*(arg + 1)) {
    Serial.print("Error: invalid argument for x: ");
    Serial.print(arg);
    Serial.println();
    return;
  }

  switch(*arg) {
    case '0':
      EEPROM.update(EPROM_ADR_2364_MODE, 0xFF);
      new_2364_mode = 0;
      break;
    case '1':
      EEPROM.update(EPROM_ADR_2364_MODE, 1);
      new_2364_mode = 1;
      break;
    default:
      Serial.print("Error: invalid argument for x: ");
      Serial.print(arg);
      Serial.println();
      return;
  }

  if(new_2364_mode != is_2364_mode && selected_ic.name[0] != '\0' && selected_ic.device_definition.f_2364_compat_pinout) {
    if(new_2364_mode) {
      memcpy(adr_pins_original_2364_mode, selected_ic.device_definition.adr_pins, sizeof(adr_pins_original_2364_mode));
      memcpy_P(selected_ic.device_definition.adr_pins, adr_pins_2364, sizeof(selected_ic.device_definition.adr_pins));
    } else {
      memcpy(selected_ic.device_definition.adr_pins, adr_pins_original_2364_mode, sizeof(selected_ic.device_definition.adr_pins));
    }
  }
  is_2364_mode = new_2364_mode;

  Serial.println("OK!");
}

struct srec_state srec;

void srec_data_read (struct srec_state *srec,
                    srec_record_number_t record_type,
                    srec_address_t address,
                    uint8_t *data, srec_count_t length,
                    srec_bool_t checksum_error) {
  static uint16_t records_read;
  records_read++;

  if (srec->length != srec->byte_count) {
    if (srec->byte_count > SREC_LINE_MAX_BYTE_COUNT) {
      Serial.print("Error: Byte count too high on record: ");
      Serial.println(records_read, DEC);
    } else {
      Serial.print("Error: Byte count mismatch on record: ");
      Serial.println(records_read, DEC);
    }
    return;
  }

  if (checksum_error) {
    Serial.print("Error: Checksum mismatch on record: ");
    Serial.println(records_read, DEC);
    return;
  }

  if (SREC_IS_DATA(record_type)) {
    if(address + length > sizeof(buffer)) {
      Serial.print("Error: Buffer overrun on record: ");
      Serial.println(records_read, DEC);
      return;
    }
    memcpy(buffer + address, data, length);
  } else if (SREC_IS_TERMINATION(record_type)) {
    state = STATE_NORMAL;
    records_read = 0;
  }
}

uint8_t tryReadLine() {
  int c;
  char c_char;
  while((c = Serial.read()) > 0) {
    c_char = (char)c;
    Serial.print(c_char);
    if (c_char == '\n') {
      if(line_length > 0) {
        if(line_length < sizeof(line)) {
          line[line_length] = '\0';
          return 1;
        } else {
          Serial.print("Error: Line-length to long: ");
          Serial.print(line_length, DEC);
          Serial.print(", max length is: ");
          Serial.println((unsigned long) sizeof(line) - 1, DEC);
          line_length = 0;
          return 0;
        }
      }
    } else if (c_char == 8) {
      if(line_length > 0) {
        line_length--;
      }
    } else if (c_char != '\r') {
      if(line_length < sizeof(line) - 1) {
        line[line_length] = c_char;
      }
      line_length++;
    }
  }

  return 0;
}

void loop() {
  if(tryReadLine()) {
    switch(state) {
      case STATE_NORMAL:
        switch(line[0] | 0b00100000) { //Force lower-case 
          case 'l':
            list_devices();
            break;
          case 's':
            srec_begin_read(&srec);
            srec_read_bytes(&srec, line, line_length);
            state = STATE_SREC;
            break;
          case 'd':
            print_buffer();
            break;
          case 'w':
            confirm_write_data();
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
          case 'v':
            voltage_calibration();
            state = STATE_VCAL;
            break;
          case 'x':
            toggle_2364_mode();
            break;
          default:
            Serial.print("Unknown command: ");
            Serial.println(line[0]);
            /* FALLTHRU */
          case 'h':
            print_help();
            break;
        }
        break;
      case STATE_SREC:
        srec_read_bytes(&srec, line, line_length);
        break;
      case STATE_VCAL:
        voltage_calibration();
        break;
      case STATE_CONFIRM_WRITE:
        switch(line[0] | 0b00100000) { //Force lower-case 
          case 'y':
            state = STATE_NORMAL;
            write_data();
            break;
          case 'n':
            state = STATE_NORMAL;
            break;
          default:
            Serial.print("Unknown option: ");
            Serial.println(line[0]);
            break;
        }
        break;
    }

    line_length = 0;
  }
}
