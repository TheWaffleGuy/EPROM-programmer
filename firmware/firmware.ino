#ifdef ARDUINO_AVR_ATmega1284
#include <Arduino.h>
#include <util/atomic.h>
#include <SPI.h>
#include <EEPROM.h>
#endif

#define restrict __restrict__
extern "C" {
  #include "kk_srec.h"
}
#include "device_definitions.h"

#define VCC_EN_PORT PORTD
#define VCC_EN_PIN 6

#define VPP_EN_PORT PORTD
#define VPP_P19_EN_PIN 5
#define VPP_P20_EN_PIN 4
#define VPP_P21_EN_PIN 3

#define DAC_SS_PORT PORTD
#define DAC_SS_PIN 2

#define MAX_LINE_LENGTH 80
#define DATA_BUFFER_SIZE 8192
#define SERIAL_SPEED 38400

#define STATE_NORMAL 0
#define STATE_SREC   1
#define STATE_VCAL   2

#define HEX_DIGIT(n) ((char)((n) + (((n) < 10) ? '0' : ('A' - 10))))

#define VCC_CAL_LOW VOLT(4, 500)
#define VCC_CAL_HIGH VOLT(6, 500)

#define VPP_CAL_LOW VOLT(12, 500)
#define VPP_CAL_HIGH VOLT(25, 0)

#define EPROM_ADR_VCC_LOW_OFFSET ( EEPROM.length() - 10 )
#define EPROM_ADR_VCC_HIGH_OFFSET ( EEPROM.length() - 8 )
#define EPROM_ADR_VPP_LOW_OFFSET ( EEPROM.length() - 6 )
#define EPROM_ADR_VPP_HIGH_OFFSET ( EEPROM.length() - 4 )
#define EPROM_ADR_IS_CALIBRATED ( EEPROM.length() - 2 )
#define EPROM_ADR_EPROM_VER ( EEPROM.length() - 1 )
#define EPROM_VER 0xFF

int16_t vcc_low_offset = 0;
int16_t vcc_high_offset = 0;

int16_t vpp_low_offset = 0;
int16_t vpp_high_offset = 0;

uint8_t port_a;
uint8_t port_b;

char line[MAX_LINE_LENGTH + 1];
uint8_t line_length = 0;

uint8_t state = STATE_NORMAL;

typedef struct pin {
  uint8_t *port;
  uint8_t index;
} pin;

pin pins[] = {
  { .port = &port_a, 0 }, //ZIF 1
  { .port = &port_a, 1 }, //ZIF 2
  { .port = &port_a, 2 }, //ZIF 3
  { .port = &port_a, 3 }, //ZIF 4
  { .port = &port_a, 4 }, //ZIF 5
  { .port = &port_a, 5 }, //ZIF 6
  { .port = &port_a, 6 }, //ZIF 7
  { .port = &port_a, 7 }, //ZIF 8
  { .port = NULL,    0 }, //ZIF 9  (Data 0)
  { .port = NULL,    1 }, //ZIF 10 (Data 1)
  { .port = NULL,    2 }, //ZIF 11 (Data 2)
  { .port = NULL,    0 }, //ZIF 12 (GND)
  { .port = NULL,    3 }, //ZIF 13 (Data 3)
  { .port = NULL,    4 }, //ZIF 14 (Data 4)
  { .port = NULL,    5 }, //ZIF 15 (Data 5)
  { .port = NULL,    6 }, //ZIF 16 (Data 6)
  { .port = NULL,    7 }, //ZIF 17 (Data 7)
  { .port = &port_b, 2 }, //ZIF 18
  { .port = &port_b, 4 }, //ZIF 19
  { .port = NULL,    7 }, //ZIF 20 (#OE)
  { .port = &port_b, 3 }, //ZIF 21
  { .port = &port_b, 1 }, //ZIF 22
  { .port = &port_b, 0 }, //ZIF 23
  { .port = NULL, 0 },    //ZIF 24 (VCC)
};


uint8_t buffer[DATA_BUFFER_SIZE];

IC selected_ic = {0};
uint8_t selected_ic_size = 0;

void setVCC(uint8_t volt, uint8_t calibrated);

void setVPP(uint8_t volt, uint8_t calibrated);

void resetVCCandVPP() {
  setVCC(VOLT(5, 0), 1);
  setVPP(VOLT(1, 250), 0);
}

void readCal() {
  if (EEPROM.read(EPROM_ADR_IS_CALIBRATED) == 0xFF) {
    return;
  }

  EEPROM.get(EPROM_ADR_VCC_LOW_OFFSET, vcc_low_offset);
  EEPROM.get(EPROM_ADR_VCC_HIGH_OFFSET, vcc_high_offset);
  EEPROM.get(EPROM_ADR_VPP_LOW_OFFSET, vpp_low_offset);
  EEPROM.get(EPROM_ADR_VPP_HIGH_OFFSET, vpp_high_offset);
}

void setup() {
  Serial.begin(SERIAL_SPEED);

  portMode(0, OUTPUT); // Port A

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    //Lower 5 pins outputs Port B
    DDRB |= 0b00011111;

    DAC_SS_PORT |= 1 << DAC_SS_PIN;
    VCC_EN_PORT &= ~(1 << VCC_EN_PIN);
    VPP_EN_PORT &= ~(1 << VPP_P19_EN_PIN);
    VPP_EN_PORT &= ~(1 << VPP_P20_EN_PIN);
    VPP_EN_PORT &= ~(1 << VPP_P21_EN_PIN);

    //Upper 6 pins outputs Port D
    DDRD |= 0b11111100;
  };

  SPI.begin();

  resetVCCandVPP();

  if (EEPROM.read(EPROM_ADR_EPROM_VER) == EPROM_VER) {
    readCal();
  }
}

bool is_numeric(char *string) {
  while(*string && isdigit(*string)) string++;
  while(*string && isspace(*string)) string++;

  return *string == '\0';
}

void print_help() {
  Serial.println("Eprom programmer help");
  Serial.println("H: This help menu");
  Serial.println("L: List devices");
  Serial.println("S[srec data]: Upload SREC to buffer");
  Serial.println("D: Download data from buffer");
  Serial.println("W: Write buffer to device");
  Serial.println("R: Read device to buffer");
  Serial.println("C: Compare buffer with device");
  Serial.println("B: Blank-check the device");
  Serial.println("T [device number]: Select type of device");
  Serial.println("I: Info about currently selected device");
  Serial.println("V: Calibrate VCC and VPP voltages");
}

void list_devices() {
  for (uint8_t i = 0; i < num_ics; i++) {
    if (i < 10) Serial.print(" ");
    Serial.print(i, DEC);
    Serial.print(") ");
    Serial.print(reinterpret_cast<const __FlashStringHelper *>(ics[i].manufacturer));
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

void print_record(const char *type, uint8_t *data, uint8_t data_length, uint16_t adress) {
  uint8_t record_length = data_length + 3; // 2 bytes for adress, 1 for checksum
  uint8_t chksum = record_length + (uint8_t) adress + (uint8_t) ( adress >> 8 );
  Serial.print(type);
  write_byte(record_length);
  write_2byte(adress);
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

void select_device() {
  char *arg;
  uint16_t arg_num;

  arg = line + 1;
  while(*arg && isspace(*arg)) arg++;

  if (! *arg || !is_numeric(arg)) {
    Serial.println("\"t\" requires a numeric argument");
    return;
  }

  arg_num = atol(arg);
  if(arg_num < num_ics) {
    memcpy_P(&selected_ic, &(ics[arg_num]), sizeof(ics[0]));
    for(selected_ic_size = 0; selected_ic_size < sizeof(selected_ic.adr_pins); selected_ic_size++) {
      uint8_t adr_pin = selected_ic.adr_pins[selected_ic_size];
      if (adr_pin == 0) break;
    }
  } else {
    Serial.print("No device with id ");
    Serial.print(arg_num, DEC);
    Serial.println(" exists");
    Serial.println("Use l to list available devices");
    return;
  }

  Serial.print("Selected: ");
  Serial.print(selected_ic.manufacturer);
  Serial.print(" - ");
  Serial.println(selected_ic.name);
  Serial.println("OK!");
}

void print_device_info() {
  if(selected_ic.name[0] == '\0') {
    Serial.println("No device selected. Select a device with \"t\"");
    return;
  }

  Serial.print("Currently selected device is: ");
  Serial.print(selected_ic.manufacturer);
  Serial.print(" - ");
  Serial.println(selected_ic.name);
  Serial.println("OK!");
}

void set_adress(uint16_t adress) {
    port_a = 0;
    port_b = 0;

    for(uint8_t i = 0; i < selected_ic_size; i++) {
        uint8_t adr_pin = selected_ic.adr_pins[i];
        pin p = pins[adr_pin - 1];
        *(p.port) |= ( adress & 1 ) << p.index;
        adress = adress >> 1;
    }

    for(uint8_t i = 0; i < sizeof(selected_ic.ctrl_pins_read_h); i++) {
        uint8_t ctrl_pin = selected_ic.ctrl_pins_read_h[i];
        if (ctrl_pin == 0) break;
        pin p = pins[ctrl_pin - 1];
        *(p.port) |= 1 << p.index;
    }

    portWrite(0, port_a);

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      PORTB &= 0b11100000; //Lower 5 bits used for adress
      PORTB |= port_b;
    }
}

void turn_vpp_on(uint8_t pin_number) {
  if(pin_number == 19) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { VPP_EN_PORT |= 1 << VPP_P19_EN_PIN; }
  } else if(pin_number == 20) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { VPP_EN_PORT |= 1 << VPP_P20_EN_PIN; }
  } else if (pin_number == 21) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { VPP_EN_PORT |= 1 << VPP_P21_EN_PIN; }
  }
}

void turn_vpp_off() {
  uint8_t vpp_off_mask = ~(1 << VPP_P19_EN_PIN | 1 << VPP_P20_EN_PIN | 1 << VPP_P21_EN_PIN);
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { VPP_EN_PORT &= vpp_off_mask; }
}

void turn_device_on() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { VCC_EN_PORT |= 1 << VCC_EN_PIN; }
  delay(100);
}

void turn_device_off() {
  set_adress(0);
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { VCC_EN_PORT &= ~(1 << VCC_EN_PIN); }
  delay(100);
}

void read_device() {
  if(selected_ic.name[0] == '\0') {
    Serial.println("No device selected. Select a device with \"t\"");
    return;
  }

  turn_device_on();

  portMode(2, INPUT); // Port C

  for(uint16_t current_adress = 0; current_adress < 1U << selected_ic_size; current_adress++) {
    set_adress(current_adress);
    buffer[current_adress] = portRead(2); // Port C
  }

  turn_device_off();

  Serial.println("OK!");
}

void blank_check() {
  uint8_t data;
  uint8_t blank_value;

  if(selected_ic.name[0] == '\0') {
    Serial.println("No device selected. Select a device with \"t\"");
    return;
  }

  if (!selected_ic.f_can_blank_check) {
    Serial.print("Device ");
    Serial.print(selected_ic.name);
    Serial.println(" cannot be blank-checked.");
    Serial.println("Initially, and after erasure, all bits are in an undefined state.");
    return;
  }

  if (selected_ic.f_blank_check_value == 0) {
    blank_value = 0;
  } else {
    blank_value = 0xFF;
  }

  turn_device_on();

  portMode(2, INPUT); // Port C

  for(uint16_t current_adress = 0; current_adress < 1U << selected_ic_size; current_adress++) {
    set_adress(current_adress);
    data = portRead(2); // Port C
    if(data != blank_value) {
      turn_device_off();
      Serial.println("Device not blank!");
      write_byte(data);
      Serial.print(" read at adress: ");
      write_2byte(current_adress);
      Serial.println();
      return;
    }
  }

  turn_device_off();

  Serial.println("OK!");
}

void compare_data() {
  uint8_t data;

  if(selected_ic.name[0] == '\0') {
    Serial.println("No device selected. Select a device with \"t\"");
    return;
  }

  turn_device_on();

  portMode(2, INPUT); // Port C

  for(uint16_t current_adress = 0; current_adress < 1U << selected_ic_size; current_adress++) {
    set_adress(current_adress);
    data = portRead(2); // Port C
    if(data != buffer[current_adress]) {
      turn_device_off();
      Serial.println("Device data does not match current buffer!");
      Serial.print("Device data: ");
      write_byte(data);
      Serial.print(", buffer data: ");
      write_byte(buffer[current_adress]);
      Serial.print(" read at adress: ");
      write_2byte(current_adress);
      Serial.println();
      return;
    }
  }

  turn_device_off();

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
        vcc_low_offset = target - measured;

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
        vcc_high_offset = target - measured;

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
        vpp_low_offset = target - measured;

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
        vpp_high_offset = target - measured;

        resetVCCandVPP();
        step = 0;
        state = STATE_NORMAL;

        EEPROM.update(EPROM_ADR_IS_CALIBRATED, 0xFF);
        EEPROM.put(EPROM_ADR_VCC_LOW_OFFSET, vcc_low_offset);
        EEPROM.put(EPROM_ADR_VCC_HIGH_OFFSET, vcc_high_offset);
        EEPROM.put(EPROM_ADR_VPP_LOW_OFFSET, vpp_low_offset);
        EEPROM.put(EPROM_ADR_VPP_HIGH_OFFSET, vpp_high_offset);
        EEPROM.update(EPROM_ADR_IS_CALIBRATED, 1);

        Serial.println("OK!");
      } else {
        Serial.println("Error: invalid input! Use format dd.ddd");
      }
      break;
  }
}

void not_implemented() {
  Serial.println("This functionality is not yet implemented");
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

void setVCC(uint8_t volt, uint8_t calibrated) {
  int16_t offset = 0;
  if(calibrated) {
    offset = ( ( VCC_CAL_HIGH - volt ) * vcc_low_offset + ( volt - VCC_CAL_LOW ) * vcc_high_offset ) / ( VCC_CAL_HIGH - VCC_CAL_LOW );
  }
  volt -= VOLT(1, 250);
  uint16_t data = 0b1011000000000000 | ( (volt << 4) + offset );

  SPI.beginTransaction(SPISettings(F_CPU / 2, MSBFIRST, SPI_MODE0));
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { DAC_SS_PORT &= ~(1 << DAC_SS_PIN); };
  SPI.transfer16(data);
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { DAC_SS_PORT |= 1 << DAC_SS_PIN; };
  SPI.endTransaction();
}

void setVPP(uint8_t volt, uint8_t calibrated) {
  int16_t offset = 0;
  if(calibrated) {
    offset = ( ( VPP_CAL_HIGH - volt ) * vpp_low_offset + ( volt - VPP_CAL_LOW ) * vpp_high_offset ) / ( VPP_CAL_HIGH - VPP_CAL_LOW );
  }
  volt -= VOLT(1, 250);
  uint16_t data = 0b0011000000000000 | ( (volt << 4) + offset );

  SPI.beginTransaction(SPISettings(F_CPU / 2, MSBFIRST, SPI_MODE0));
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { DAC_SS_PORT &= ~(1 << DAC_SS_PIN); };
  SPI.transfer16(data);
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { DAC_SS_PORT |= 1 << DAC_SS_PIN; };
  SPI.endTransaction();
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
          case 'v':
            voltage_calibration();
            state = STATE_VCAL;
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
    }

    line_length = 0;
  }
}
