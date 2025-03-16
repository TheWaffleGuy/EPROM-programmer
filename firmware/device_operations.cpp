#include <Arduino.h>
#include <util/atomic.h>
#include <SPI.h>

#include "device_operations.h"

#include "device.h"
#include "firmware.h"

#define VCC_EN_PORT PORTD
#define VCC_EN_PIN 6

#define VPP_EN_PORT PORTD
#define VPP_P19_EN_PIN 5
#define VPP_P20_EN_PIN 4
#define VPP_P21_EN_PIN 3

#define DAC_SS_PORT PORTD
#define DAC_SS_PIN 2

#define DEVICE_ENABLE_PORT PORTD
#define DEVICE_ENABLE_PIN 7

uint8_t port_a;
uint8_t port_b;

uint16_t g_current_address = 0xFFFF;


extern voltage_offsets v_offset;
extern uint8_t buffer[];
extern IC selected_ic;
extern uint8_t selected_ic_size;
extern uint8_t is_2364_mode;


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


static void write_byte(uint8_t byte) {
  union { uint8_t val; struct { uint8_t low_nibble: 4; uint8_t high_nibble: 4; } val_split; } data;
  data.val = byte;
  Serial.print(HEX_DIGIT(data.val_split.high_nibble));
  Serial.print(HEX_DIGIT(data.val_split.low_nibble));
}

static void __attribute__((always_inline)) inline write_2byte(uint16_t byte2) {
  union { uint16_t val; uint8_t val_split[2]; } data;
  data.val = byte2;
  write_byte(data.val_split[1]);
  write_byte(data.val_split[0]);
}

void resetVCCandVPP() {
  setVCC(VOLT(5, 0), 1);
  setVPP(VOLT(1, 250), 0);
}

void device_operations_init() {
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
}

static void set_address(uint16_t address) {
    if (address == g_current_address) {
      return;
    }
    g_current_address = address;
    port_a = 0;
    port_b = 0;

    for(uint8_t i = 0; i < selected_ic_size; i++) {
        uint8_t adr_pin = selected_ic.device_definition.adr_pins[i];
        pin p = pins[adr_pin - 1];
        *(p.port) |= ( address & 1 ) << p.index;
        address = address >> 1;
    }

    for(uint8_t i = 0; i < sizeof(selected_ic.device_definition.ctrl_pins_read_h); i++) {
        uint8_t ctrl_pin = selected_ic.device_definition.ctrl_pins_read_h[i];
        if (ctrl_pin == 0) break;
        pin p = pins[ctrl_pin - 1];
        *(p.port) |= 1 << p.index;
    }

    portWrite(0, port_a);

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      PORTB = ( PORTB & 0b11100000 ) | port_b;
    }
}

static void disable_device_output() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { DEVICE_ENABLE_PORT |= 1 << DEVICE_ENABLE_PIN; }
}

static void enable_device_output() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { DEVICE_ENABLE_PORT &= ~(1 << DEVICE_ENABLE_PIN); }
}

static void turn_vpp_on(uint8_t pin_id) {
  if(pin_id == PGM_VPP_PIN_P19) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { VPP_EN_PORT |= 1 << VPP_P19_EN_PIN; }
  } else if(pin_id == PGM_VPP_PIN_P20) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { VPP_EN_PORT |= 1 << VPP_P20_EN_PIN; }
  } else if (pin_id == PGM_VPP_PIN_P21) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { VPP_EN_PORT |= 1 << VPP_P21_EN_PIN; }
  }
}

static void turn_vpp_off() {
  uint8_t vpp_off_mask = ~(1 << VPP_P19_EN_PIN | 1 << VPP_P20_EN_PIN | 1 << VPP_P21_EN_PIN);
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { VPP_EN_PORT &= vpp_off_mask; }
}

static void turn_device_on() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { VCC_EN_PORT |= 1 << VCC_EN_PIN; }
  delay(100);
  //Some devices requires one address transition after initial power-up to reset the outputs.
  set_address(1);
  delayMicroseconds(3);
  set_address(0);
}

static void turn_device_off() {
  //Turn all address-pins off
  portWrite(0, 0);
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    PORTB &= 0b11100000; //Lower 5 bits used for address
  }
  g_current_address = 0xFFFF;

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

  for(uint16_t current_address = 0; current_address < 1U << selected_ic_size; current_address++) {
    set_address(current_address);
    __asm__ __volatile__ ("rjmp .+0" "\n\t");
    __asm__ __volatile__ ("rjmp .+0" "\n\t");
    buffer[current_address] = portRead(2); // Port C
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

  if (!selected_ic.device_definition.f_can_blank_check) {
    Serial.print("Device ");
    Serial.print(selected_ic.name);
    Serial.println(" cannot be blank-checked.");
    Serial.println("Initially, and after erasure, all bits are in an undefined state.");
    return;
  }

  if (selected_ic.device_definition.f_blank_check_value == 0) {
    blank_value = 0;
  } else {
    blank_value = 0xFF;
  }

  turn_device_on();

  portMode(2, INPUT); // Port C

  for(uint16_t current_address = 0; current_address < 1U << selected_ic_size; current_address++) {
    set_address(current_address);
    __asm__ __volatile__ ("rjmp .+0" "\n\t");
    __asm__ __volatile__ ("rjmp .+0" "\n\t");
    data = portRead(2); // Port C
    if(data != blank_value) {
      turn_device_off();
      Serial.println("Device not blank!");
      write_byte(data);
      Serial.print(" read at address: ");
      write_2byte(current_address);
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

  for(uint16_t current_address = 0; current_address < 1U << selected_ic_size; current_address++) {
    set_address(current_address);
    __asm__ __volatile__ ("rjmp .+0" "\n\t");
    __asm__ __volatile__ ("rjmp .+0" "\n\t");
    data = portRead(2); // Port C
    if(data != buffer[current_address]) {
      turn_device_off();
      Serial.println("Device data does not match current buffer!");
      Serial.print("Device data: ");
      write_byte(data);
      Serial.print(", buffer data: ");
      write_byte(buffer[current_address]);
      Serial.print(" read at address: ");
      write_2byte(current_address);
      Serial.println();
      return;
    }
  }

  turn_device_off();

  Serial.println("OK!");
}


#define SETUP_HOLD_TIME_US 10

static void pgm_variant_vpp_pulsed_positive(uint8_t data, uint16_t address, uint16_t pw) {
  uint8_t tens_of_ms = 0;
  pw -= 3;
  if(pw > 10000) {
    tens_of_ms = pw / 10000;
    pw %= 10000;
  }
  disable_device_output();
  delayMicroseconds(SETUP_HOLD_TIME_US);
  set_address(address);
  portMode(2, OUTPUT); // Port C
  portWrite(2, data); // Port C
  delayMicroseconds(SETUP_HOLD_TIME_US);
  turn_vpp_on(selected_ic.device_definition.pgm_vpp_pin);
  delayMicroseconds(pw);
  if(tens_of_ms) {
    delay(tens_of_ms * 10);
  }
  turn_vpp_off();
  delayMicroseconds(SETUP_HOLD_TIME_US);
  portMode(2, INPUT); // Port C
  enable_device_output();
  delayMicroseconds(3);
}

static void pgm_variant_p20_pulsed_negative(uint8_t data, uint16_t address, uint16_t pw) {
  uint8_t tens_of_ms = 0;
  pw -= 3;
  if(pw > 10000) {
    tens_of_ms = pw / 10000;
    pw %= 10000;
  }
  disable_device_output();
  delayMicroseconds(SETUP_HOLD_TIME_US);
  set_address(address);
  portMode(2, OUTPUT); // Port C
  portWrite(2, data); // Port C
  turn_vpp_on(selected_ic.device_definition.pgm_vpp_pin);
  delayMicroseconds(SETUP_HOLD_TIME_US);
  enable_device_output();
  delayMicroseconds(pw);
  if(tens_of_ms) {
    delay(tens_of_ms * 10);
  }
  disable_device_output();
  turn_vpp_off();
  delayMicroseconds(SETUP_HOLD_TIME_US);
  portMode(2, INPUT); // Port C
  enable_device_output();
  delayMicroseconds(3);
}

static void pgm_variant_p18_pulsed_negative_p19_verify(uint8_t data, uint16_t address, uint16_t pw) {
  uint8_t tens_of_ms = 0;
  pw -= 3;
  if(pw > 10000) {
    tens_of_ms = pw / 10000;
    pw %= 10000;
  }
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { PORTB |= 1 << 4; }; //19 high
  set_address(address);
  portMode(2, OUTPUT); // Port C
  portWrite(2, data); // Port C
  delayMicroseconds(SETUP_HOLD_TIME_US);
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { PORTB &= ~(1 << 2); }; //18 low
  delayMicroseconds(pw);
  if(tens_of_ms) {
    delay(tens_of_ms * 10);
  }
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { PORTB |= 1 << 2; }; //18 high
  delayMicroseconds(SETUP_HOLD_TIME_US);
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { PORTB &= ~(1 << 4); }; //19 low
  portMode(2, INPUT); // Port C
  delayMicroseconds(3);
}

static void pgm_variant_p18_pulsed_negative(uint8_t data, uint16_t address, uint16_t pw) {
  uint8_t tens_of_ms = 0;
  pw -= 3;
  if(pw > 10000) {
    tens_of_ms = pw / 10000;
    pw %= 10000;
  }
  disable_device_output();
  delayMicroseconds(SETUP_HOLD_TIME_US);
  set_address(address);
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { PORTB |= 1 << 2; }; //18 high
  portMode(2, OUTPUT); // Port C
  portWrite(2, data); // Port C
  turn_vpp_on(selected_ic.device_definition.pgm_vpp_pin);
  delayMicroseconds(SETUP_HOLD_TIME_US);
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { PORTB &= ~(1 << 2); }; //18 low
  delayMicroseconds(pw);
  if(tens_of_ms) {
    delay(tens_of_ms * 10);
  }
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { PORTB |= 1 << 2; }; //18 high
  delayMicroseconds(SETUP_HOLD_TIME_US);
  turn_vpp_off();
  delayMicroseconds(SETUP_HOLD_TIME_US);
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { PORTB &= ~(1 << 2); }; //18 low
  portMode(2, INPUT); // Port C
  enable_device_output();
  delayMicroseconds(3);
}

static void pgm_variant_p18_pulsed_positive(uint8_t data, uint16_t address, uint16_t pw) {
  uint8_t tens_of_ms = 0;
  pw -= 3;
  if(pw > 10000) {
    tens_of_ms = pw / 10000;
    pw %= 10000;
  }
  disable_device_output();
  delayMicroseconds(SETUP_HOLD_TIME_US);
  set_address(address);
  portMode(2, OUTPUT); // Port C
  portWrite(2, data); // Port C
  delayMicroseconds(SETUP_HOLD_TIME_US);
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { PORTB |= 1 << 2; }; //18 high
  delayMicroseconds(pw);
  if(tens_of_ms) {
    delay(tens_of_ms * 10);
  }
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { PORTB &= ~(1 << 2); }; //18 low
  delayMicroseconds(SETUP_HOLD_TIME_US);
  portMode(2, INPUT); // Port C
  enable_device_output();
  delayMicroseconds(3);
}

static uint8_t reverse_bits(uint8_t num)
{
    uint8_t count = 7;
    uint8_t reverse_num = num;

    num >>= 1;
    while (num) {
        reverse_num <<= 1;
        reverse_num |= num & 1;
        num >>= 1;
        count--;
    }
    reverse_num <<= count;
    return reverse_num;
}

static void pgm_variant_cypress(uint8_t data, uint16_t address, uint16_t pw) {
  static uint8_t latched_adr = 0xFF;
  union { uint16_t val; uint8_t val_split[2]; } adr;
  adr.val = address;
  uint8_t tens_of_ms = 0;
  pw -= 3;
  if(pw > 10000) {
    tens_of_ms = pw / 10000;
    pw %= 10000;
  }
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { PORTB |= 1 << 0; }; //23 high
  delayMicroseconds(3);
  if (adr.val_split[1] != latched_adr) {
    latched_adr = adr.val_split[1];
    if (is_2364_mode && selected_ic.device_definition.f_2364_compat_pinout) {
      uint8_t a10 = ( adr.val_split[1] & 0b00000100 ) > 1;
      uint8_t a11 = ( adr.val_split[1] & 0b00001000 ) > 1;
      uint8_t a12 = ( adr.val_split[1] & 0b00010000 ) > 1;
      adr.val_split[1] = ( adr.val_split[1] & 0b11100011 ) | a10 << 3 | a11 << 4 | a12 << 2;
    }
    portWrite(0, reverse_bits(adr.val_split[1])); //Port A
    delayMicroseconds(3);
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { PORTB |= 1 << 3; }; //21 high
    delayMicroseconds(3);
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { PORTB &= ~(1 << 3); }; //21 low
  }
  portMode(2, OUTPUT); // Port C
  portWrite(2, data); // Port C
  delayMicroseconds(3);
  portWrite(0, reverse_bits(adr.val_split[0]));
  delayMicroseconds(3);
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { PORTB &= ~(1 << 1); }; //22 low
  delayMicroseconds(pw);
  if(tens_of_ms) {
    delay(tens_of_ms * 10);
  }
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { PORTB |= 1 << 1; }; //22 high
  delayMicroseconds(3);
  portMode(2, INPUT); // Port C
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { PORTB &= ~(1 << 0); }; //23 low
  delayMicroseconds(3);
}

void write_data() {
  uint8_t pulse_number;
  uint16_t address_start = 0;
  uint16_t address_end = ( 1U << selected_ic_size ) - 1;
  uint16_t address;
  uint8_t write_data;
  uint8_t verified = 1;
  void (*pgm_variant)(uint8_t data, uint16_t address, uint16_t pw);

  //Set up voltages
  if(selected_ic.device_definition.pgm_vcc_extra != 0) {
    setVCC(VOLT(5, 0) + selected_ic.device_definition.pgm_vcc_extra, 1);
  }
  setVPP(selected_ic.device_definition.vpp, 1);
  turn_device_on();

  switch(selected_ic.device_definition.pgm_variant) {
    case PGM_VARIANT_VPP_PULSED_POSITIVE:
      pgm_variant = &pgm_variant_vpp_pulsed_positive;
      break;
    case PGM_VARIANT_P20_PULSED_NEGATIVE:
      pgm_variant = &pgm_variant_p20_pulsed_negative;
      break;
    case PGM_VARIANT_P18_PULSED_NEGATIVE_P19_VERIFY:
      pgm_variant = &pgm_variant_p18_pulsed_negative_p19_verify;
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        PORTB |= (1 << 2) | (1 << 4); //18 high, 19 high
      }
      break;
    case PGM_VARIANT_P18_PULSED_NEGATIVE:
      pgm_variant = &pgm_variant_p18_pulsed_negative;
      break;
    case PGM_VARIANT_P18_PULSED_POSITIVE:
      pgm_variant = &pgm_variant_p18_pulsed_positive;
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { PORTB &= ~(1 << 2); }; //18 low
      break;
    case PGM_VARIANT_CYPRESS:
      pgm_variant = &pgm_variant_cypress;
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        PORTB &= ~(1 << 3); //21 low
        PORTB |= (1 << 1) | (1 << 0); //22, 23 high
      };
      break;
    default:
      Serial.println("This functionality is not yet implemented");
      verified = 0;
  }

  if(verified && selected_ic.device_definition.pgm_vpp_always_on) {
      delayMicroseconds(SETUP_HOLD_TIME_US);
      turn_vpp_on(selected_ic.device_definition.pgm_vpp_pin);
      delay(20);
  }

  for (address = address_start; verified && address <= address_end; address++) {
    write_data = buffer[address];
    pulse_number = 0;
    //Iterative programming
    do {
      pulse_number++;
      pgm_variant(write_data, address, selected_ic.device_definition.pgm_pw_us);
      verified = write_data == portRead(2); // Port C
    } while (!verified && pulse_number < selected_ic.device_definition.pgm_pulses);
    //Overprogram section if it should be done in the main programming loop
    if (selected_ic.device_definition.pgm_overprogram_pw > 0 && (verified || selected_ic.device_definition.pgm_overprogram_ignore_verify) && !selected_ic.device_definition.pgm_overprogram_after) {
      uint16_t pw = ( selected_ic.device_definition.pgm_overprogram_pw * selected_ic.device_definition.pgm_pw_us ) / 2; //pgm_overprogram_pw is in half-units
      if(selected_ic.device_definition.pgm_overprogram_multiply_n) {
        pw *= pulse_number;
      }
      if(selected_ic.device_definition.pgm_overprogram_5v_vcc) {
        setVCC(VOLT(5, 0), 1);
        delayMicroseconds(1000);
      }
      pgm_variant(write_data, address, pw);
      if(selected_ic.device_definition.pgm_overprogram_5v_vcc) {
        setVCC(VOLT(5, 0) + selected_ic.device_definition.pgm_vcc_extra, 1);
        delayMicroseconds(20);
      }
      if (!verified) {
        verified = write_data == portRead(2); // Port C
      }
    }
  }
  //Overprogram section if it should be done after the main programming loop
  if (verified && selected_ic.device_definition.pgm_overprogram_after) {
    uint16_t pw = ( selected_ic.device_definition.pgm_overprogram_pw * selected_ic.device_definition.pgm_pw_us ) / 2; //pgm_overprogram_pw is in half-units
    if(selected_ic.device_definition.pgm_overprogram_5v_vcc) {
      setVCC(VOLT(5, 0), 1);
      delayMicroseconds(1000);
    }
    for (address = address_start; address <= address_end; address++) {
      write_data = buffer[address];
      pgm_variant(write_data, address, pw);
    }
  }

  turn_vpp_off();
  delay(20);
  turn_device_off();
  resetVCCandVPP();

  if (!verified) {
    Serial.print("Error: write failed at address: ");
    write_2byte(address - 1);
    Serial.println();
    return;
  }

  Serial.println("OK!");
}

void setVCC(uint8_t volt, uint8_t calibrated) {
  int16_t offset = 0;
  if(calibrated) {
    offset = ( ( VCC_CAL_HIGH - volt ) * v_offset.vcc_low + ( volt - VCC_CAL_LOW ) * v_offset.vcc_high ) / ( VCC_CAL_HIGH - VCC_CAL_LOW );
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
    offset = ( ( VPP_CAL_HIGH - volt ) * v_offset.vpp_low + ( volt - VPP_CAL_LOW ) * v_offset.vpp_high ) / ( VPP_CAL_HIGH - VPP_CAL_LOW );
  }
  volt -= VOLT(1, 250);
  uint16_t data = 0b0011000000000000 | ( (volt << 4) + offset );

  SPI.beginTransaction(SPISettings(F_CPU / 2, MSBFIRST, SPI_MODE0));
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { DAC_SS_PORT &= ~(1 << DAC_SS_PIN); };
  SPI.transfer16(data);
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { DAC_SS_PORT |= 1 << DAC_SS_PIN; };
  SPI.endTransaction();
}