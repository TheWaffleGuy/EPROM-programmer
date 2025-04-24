#include "device.h"

#if __GNUC__ > 11
#define MANUFACTURER(n) .manufacturer = n
#define NAME(n) .name = n
#else
#define NAME(n) { .name = n }
#define MANUFACTURER(n) { .manufacturer = n }
#endif

const uint8_t adr_pins_2364[13] PROGMEM = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 18, 21 };

const device_def WS57C49_WS57C49B PROGMEM = {
  .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
  .ctrl_pins_read_h = {},
  .vpp = VOLT(13, 500),
  .pgm_pw_us = 1000,
  .pgm_vcc_extra = VOLT(0, 500),
  .pgm_intial_single_pass = 0,
  .pgm_overprogram_5v_vcc = 0,
  .pgm_overprogram_ignore_verify = 0,
  .pgm_overprogram_multiply_n = 0,
  .pgm_overprogram_after = 0,
  .pgm_overprogram_pw = OVERPROGRAM_PW(1, 0),
  .pgm_vpp_pin = PGM_VPP_PIN_P20,
  .pgm_pulses = 5,
  .pgm_vpp_always_on = 0,
  .f_can_blank_check = 1,
  .f_blank_check_value = 1,
  .f_2364_compat_pinout = 1,
  .pgm_variant = PGM_VARIANT_VPP_PULSED_POSITIVE
};

const device_def MCM6876X PROGMEM = {
  .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 18, 21 },
  .ctrl_pins_read_h = {},
  .vpp = VOLT(25, 0),
  .pgm_pw_us = 1000,
  .pgm_vcc_extra = VOLT(1, 0),
  .pgm_intial_single_pass = 0,
  .pgm_overprogram_5v_vcc = 0,
  .pgm_overprogram_ignore_verify = 0,
  .pgm_overprogram_multiply_n = 0,
  .pgm_overprogram_after = 0,
  .pgm_overprogram_pw = 0,
  .pgm_vpp_pin = PGM_VPP_PIN_P20,
  .pgm_pulses = 20, //No max-value specified in datasheet
  .pgm_vpp_always_on = 0,
  .f_can_blank_check = 1,
  .f_blank_check_value = 1,
  .f_2364_compat_pinout = 0,
  .pgm_variant = PGM_VARIANT_VPP_PULSED_POSITIVE
};

const device_def GENERIC_2532 PROGMEM = {
  .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 18 },
  .ctrl_pins_read_h = { 21 },
  .vpp = VOLT(25, 0),
  .pgm_pw_us = 50000,
  .pgm_vcc_extra = 0,
  .pgm_intial_single_pass = 0,
  .pgm_overprogram_5v_vcc = 0,
  .pgm_overprogram_ignore_verify = 0,
  .pgm_overprogram_multiply_n = 0,
  .pgm_overprogram_after = 0,
  .pgm_overprogram_pw = 0,
  .pgm_vpp_pin = PGM_VPP_PIN_P21,
  .pgm_pulses = 1,
  .pgm_vpp_always_on = 0,
  .f_can_blank_check = 1,
  .f_blank_check_value = 1,
  .f_2364_compat_pinout = 0,
  .pgm_variant = PGM_VARIANT_P20_PULSED_NEGATIVE
};

const device_def WS57C43_WS57C43B PROGMEM = {
  .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19 },
  .ctrl_pins_read_h = { 18 },
  .vpp = VOLT(13, 500),
  .pgm_pw_us = 1000,
  .pgm_vcc_extra = VOLT(0, 500),
  .pgm_intial_single_pass = 0,
  .pgm_overprogram_5v_vcc = 0,
  .pgm_overprogram_ignore_verify = 0,
  .pgm_overprogram_multiply_n = 0,
  .pgm_overprogram_after = 0,
  .pgm_overprogram_pw = OVERPROGRAM_PW(1, 0),
  .pgm_vpp_pin = PGM_VPP_PIN_P20,
  .pgm_pulses = 5,
  .pgm_vpp_always_on = 0,
  .f_can_blank_check = 1,
  .f_blank_check_value = 1,
  .f_2364_compat_pinout = 0,
  .pgm_variant = PGM_VARIANT_VPP_PULSED_POSITIVE
};

const device_def WS57C191_WS57C191B PROGMEM = {
  .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21 },
  .ctrl_pins_read_h = { 19, 18 },
  .vpp = VOLT(13, 500),
  .pgm_pw_us = 1000,
  .pgm_vcc_extra = VOLT(0, 500),
  .pgm_intial_single_pass = 0,
  .pgm_overprogram_5v_vcc = 0,
  .pgm_overprogram_ignore_verify = 0,
  .pgm_overprogram_multiply_n = 0,
  .pgm_overprogram_after = 0,
  .pgm_overprogram_pw = OVERPROGRAM_PW(1, 0),
  .pgm_vpp_pin = PGM_VPP_PIN_P20,
  .pgm_pulses = 5,
  .pgm_vpp_always_on = 0,
  .f_can_blank_check = 1,
  .f_blank_check_value = 1,
  .f_2364_compat_pinout = 0,
  .pgm_variant = PGM_VARIANT_VPP_PULSED_POSITIVE
};

const device_def GENERIC_2758_L PROGMEM = {
  .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22 },
  .ctrl_pins_read_h = { 21 },
  .vpp = VOLT(25, 0),
  .pgm_pw_us = 50000,
  .pgm_vcc_extra = 0,
  .pgm_intial_single_pass = 0,
  .pgm_overprogram_5v_vcc = 0,
  .pgm_overprogram_ignore_verify = 0,
  .pgm_overprogram_multiply_n = 0,
  .pgm_overprogram_after = 0,
  .pgm_overprogram_pw = 0,
  .pgm_vpp_pin = PGM_VPP_PIN_P21,
  .pgm_pulses = 1,
  .pgm_vpp_always_on = 1,
  .f_can_blank_check = 1,
  .f_blank_check_value = 1,
  .f_2364_compat_pinout = 0,
  .pgm_variant = PGM_VARIANT_P18_PULSED_POSITIVE
};

const device_def GENERIC_2758_H PROGMEM = {
  .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22 },
  .ctrl_pins_read_h = { 21, 19 },
  .vpp = VOLT(25, 0),
  .pgm_pw_us = 50000,
  .pgm_vcc_extra = 0,
  .pgm_intial_single_pass = 0,
  .pgm_overprogram_5v_vcc = 0,
  .pgm_overprogram_ignore_verify = 0,
  .pgm_overprogram_multiply_n = 0,
  .pgm_overprogram_after = 0,
  .pgm_overprogram_pw = 0,
  .pgm_vpp_pin = PGM_VPP_PIN_P21,
  .pgm_pulses = 1,
  .pgm_vpp_always_on = 1,
  .f_can_blank_check = 1,
  .f_blank_check_value = 1,
  .f_2364_compat_pinout = 0,
  .pgm_variant = PGM_VARIANT_P18_PULSED_POSITIVE
};

const device_def GENERIC_2716 PROGMEM = {
  .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19 },
  .ctrl_pins_read_h = { 21 },
  .vpp = VOLT(25, 0),
  .pgm_pw_us = 50000,
  .pgm_vcc_extra = 0,
  .pgm_intial_single_pass = 0,
  .pgm_overprogram_5v_vcc = 0,
  .pgm_overprogram_ignore_verify = 0,
  .pgm_overprogram_multiply_n = 0,
  .pgm_overprogram_after = 0,
  .pgm_overprogram_pw = 0,
  .pgm_vpp_pin = PGM_VPP_PIN_P21,
  .pgm_pulses = 1,
  .pgm_vpp_always_on = 1,
  .f_can_blank_check = 1,
  .f_blank_check_value = 1,
  .f_2364_compat_pinout = 0,
  .pgm_variant = PGM_VARIANT_P18_PULSED_POSITIVE
};

const device_def GENERIC_2732 PROGMEM = {
  .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 21 },
  .ctrl_pins_read_h = {},
  .vpp = VOLT(25, 0),
  .pgm_pw_us = 50000,
  .pgm_vcc_extra = 0,
  .pgm_intial_single_pass = 0,
  .pgm_overprogram_5v_vcc = 0,
  .pgm_overprogram_ignore_verify = 0,
  .pgm_overprogram_multiply_n = 0,
  .pgm_overprogram_after = 0,
  .pgm_overprogram_pw = 0,
  .pgm_vpp_pin = PGM_VPP_PIN_P20,
  .pgm_pulses = 1,
  .pgm_vpp_always_on = 0,
  .f_can_blank_check = 1,
  .f_blank_check_value = 1,
  .f_2364_compat_pinout = 0,
  .pgm_variant = PGM_VARIANT_P18_PULSED_NEGATIVE
};

const device_def GENERIC_2732A PROGMEM = {
  .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 21 },
  .ctrl_pins_read_h = {},
  .vpp = VOLT(21, 0),
  .pgm_pw_us = 50000,
  .pgm_vcc_extra = 0,
  .pgm_intial_single_pass = 0,
  .pgm_overprogram_5v_vcc = 0,
  .pgm_overprogram_ignore_verify = 0,
  .pgm_overprogram_multiply_n = 0,
  .pgm_overprogram_after = 0,
  .pgm_overprogram_pw = 0,
  .pgm_vpp_pin = PGM_VPP_PIN_P20,
  .pgm_pulses = 1,
  .pgm_vpp_always_on = 0,
  .f_can_blank_check = 1,
  .f_blank_check_value = 1,
  .f_2364_compat_pinout = 0,
  .pgm_variant = PGM_VARIANT_P18_PULSED_NEGATIVE
};

const IC ics[] PROGMEM = {
  {
    MANUFACTURER("CYPRESS"),
    NAME("CY7C264"),
    .device_definition = {
      .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
      .ctrl_pins_read_h = {},
      .vpp = VOLT(12, 500),
      .pgm_pw_us = 200,
      .pgm_vcc_extra = 0,
      .pgm_intial_single_pass = 0,
      .pgm_overprogram_5v_vcc = 0,
      .pgm_overprogram_ignore_verify = 1,
      .pgm_overprogram_multiply_n = 1,
      .pgm_overprogram_after = 0,
      .pgm_overprogram_pw = OVERPROGRAM_PW(4, 0),
      .pgm_vpp_pin = PGM_VPP_PIN_P19,
      .pgm_pulses = 10,
      .pgm_vpp_always_on = 1,
      .f_can_blank_check = 1,
      .f_blank_check_value = 0,
      .f_2364_compat_pinout = 1,
      .pgm_variant = PGM_VARIANT_CYPRESS
    }
  },
  {
    MANUFACTURER("AMD"),
    NAME("AM27C49"),
    .device_definition = {
      .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
      .ctrl_pins_read_h = {},
      .vpp = VOLT(13, 500),
      .pgm_pw_us = 1000,
      .pgm_vcc_extra = VOLT(1, 0),
      .pgm_intial_single_pass = 0,
      .pgm_overprogram_5v_vcc = 1,
      .pgm_overprogram_ignore_verify = 0,
      .pgm_overprogram_multiply_n = 1,
      .pgm_overprogram_after = 0,
      .pgm_overprogram_pw = OVERPROGRAM_PW(2, 0),
      .pgm_vpp_pin = PGM_VPP_PIN_P20,
      .pgm_pulses = 25,
      .pgm_vpp_always_on = 0,
      .f_can_blank_check = 1,
      .f_blank_check_value = 1,
      .f_2364_compat_pinout = 1,
      .pgm_variant = PGM_VARIANT_VPP_PULSED_POSITIVE
    }
  },
  { //Same algorithm as 27CX321
    MANUFACTURER("ICT"),
    NAME("27CX641"),
    .device_definition = {
      .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
      .ctrl_pins_read_h = {},
      .vpp = VOLT(12, 500),
      .pgm_pw_us = 100,
      .pgm_vcc_extra = VOLT(1, 0),
      .pgm_intial_single_pass = 0,
      .pgm_overprogram_5v_vcc = 0,
      .pgm_overprogram_ignore_verify = 0,
      .pgm_overprogram_multiply_n = 1,
      .pgm_overprogram_after = 0,
      .pgm_overprogram_pw = OVERPROGRAM_PW(5, 5),
      .pgm_vpp_pin = PGM_VPP_PIN_P20,
      .pgm_pulses = 30,
      .pgm_vpp_always_on = 0,
      .f_can_blank_check = 0,
      .f_blank_check_value = 0,
      .f_2364_compat_pinout = 1,
      .pgm_variant = PGM_VARIANT_VPP_PULSED_POSITIVE
    }
  },
  { //Express algorithm
    MANUFACTURER("MICROCHIP"),
    NAME("27HC641"),
    .device_definition = {
      .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
      .ctrl_pins_read_h = {},
      .vpp = VOLT(13, 0),
      .pgm_pw_us = 100,
      .pgm_vcc_extra = VOLT(1, 500),
      .pgm_intial_single_pass = 0,
      .pgm_overprogram_5v_vcc = 0,
      .pgm_overprogram_ignore_verify = 0,
      .pgm_overprogram_multiply_n = 0,
      .pgm_overprogram_after = 0,
      .pgm_overprogram_pw = 0,
      .pgm_vpp_pin = PGM_VPP_PIN_P20,
      .pgm_pulses = 10,
      .pgm_vpp_always_on = 0,
      .f_can_blank_check = 1,
      .f_blank_check_value = 1,
      .f_2364_compat_pinout = 1,
      .pgm_variant = PGM_VARIANT_VPP_PULSED_POSITIVE
    }
  },
  { //Algorithm observed on oscilloscope on ALL-03 programmer
    MANUFACTURER("SIGNETICS"),
    NAME("27HC641"),
    .device_definition = {
      .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
      .ctrl_pins_read_h = {},
      .vpp = VOLT(12, 500),
      .pgm_pw_us = 2250,
      .pgm_vcc_extra = VOLT(0, 500),
      .pgm_intial_single_pass = 0,
      .pgm_overprogram_5v_vcc = 0,
      .pgm_overprogram_ignore_verify = 0,
      .pgm_overprogram_multiply_n = 0,
      .pgm_overprogram_after = 0,
      .pgm_overprogram_pw = 0,
      .pgm_vpp_pin = PGM_VPP_PIN_P20,
      .pgm_pulses = 20,
      .pgm_vpp_always_on = 0,
      .f_can_blank_check = 0,
      .f_blank_check_value = 0,
      .f_2364_compat_pinout = 1,
      .pgm_variant = PGM_VARIANT_VPP_PULSED_POSITIVE
    }
  },
  {
    MANUFACTURER("ATMEL"),
    NAME("AT27HC641"),
    .device_definition = {
      .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
      .ctrl_pins_read_h = {},
      .vpp = VOLT(12, 500),
      .pgm_pw_us = 1000,
      .pgm_vcc_extra = VOLT(1, 0),
      .pgm_intial_single_pass = 0,
      .pgm_overprogram_5v_vcc = 0,
      .pgm_overprogram_ignore_verify = 0,
      .pgm_overprogram_multiply_n = 1,
      .pgm_overprogram_after = 0,
      .pgm_overprogram_pw = OVERPROGRAM_PW(3, 0),
      .pgm_vpp_pin = PGM_VPP_PIN_P20,
      .pgm_pulses = 25,
      .pgm_vpp_always_on = 0,
      .f_can_blank_check = 1,
      .f_blank_check_value = 1,
      .f_2364_compat_pinout = 1,
      .pgm_variant = PGM_VARIANT_VPP_PULSED_POSITIVE
    }
  },
  {
    MANUFACTURER("ATMEL"),
    NAME("AT27HC641R"),
    .device_definition = {
      .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
      .ctrl_pins_read_h = {},
      .vpp = VOLT(13, 0),
      .pgm_pw_us = 100,
      .pgm_vcc_extra = VOLT(1, 500),
      .pgm_intial_single_pass = 1, //Not implemented at the moment. Overprogram used instead
      .pgm_overprogram_5v_vcc = 0,
      .pgm_overprogram_ignore_verify = 0,
      .pgm_overprogram_multiply_n = 0,
      .pgm_overprogram_after = 0,
      .pgm_overprogram_pw = OVERPROGRAM_PW(1, 0),
      .pgm_vpp_pin = PGM_VPP_PIN_P20,
      .pgm_pulses = 10,
      .pgm_vpp_always_on = 0,
      .f_can_blank_check = 1,
      .f_blank_check_value = 1,
      .f_2364_compat_pinout = 1,
      .pgm_variant = PGM_VARIANT_VPP_PULSED_POSITIVE
    }
  },
  {
    MANUFACTURER("SHARP"),
    NAME("LH5749"),
    .device_definition = {
      .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
      .ctrl_pins_read_h = {},
      .vpp = VOLT(13, 0),
      .pgm_pw_us = 1000,
      .pgm_vcc_extra = VOLT(1, 0),
      .pgm_intial_single_pass = 0,
      .pgm_overprogram_5v_vcc = 0,
      .pgm_overprogram_ignore_verify = 0,
      .pgm_overprogram_multiply_n = 1,
      .pgm_overprogram_after = 0,
      .pgm_overprogram_pw = OVERPROGRAM_PW(3, 0),
      .pgm_vpp_pin = PGM_VPP_PIN_P20,
      .pgm_pulses = 25,
      .pgm_vpp_always_on = 0,
      .f_can_blank_check = 1,
      .f_blank_check_value = 1,
      .f_2364_compat_pinout = 1,
      .pgm_variant = PGM_VARIANT_VPP_PULSED_POSITIVE
    }
  },
  {
    MANUFACTURER("WSI"),
    NAME("WS57C49"),
    .device_definition = WS57C49_WS57C49B
  },
  {
    MANUFACTURER("WSI"),
    NAME("WS57C49B"),
    .device_definition = WS57C49_WS57C49B
  },
  {
    MANUFACTURER("WSI"),
    NAME("WS57C49C"),
    .device_definition = {
      .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
      .ctrl_pins_read_h = {},
      .vpp = VOLT(12, 750),
      .pgm_pw_us = 100,
      .pgm_vcc_extra = VOLT(1, 250),
      .pgm_intial_single_pass = 0,
      .pgm_overprogram_5v_vcc = 0,
      .pgm_overprogram_ignore_verify = 0,
      .pgm_overprogram_multiply_n = 0,
      .pgm_overprogram_after = 0,
      .pgm_overprogram_pw = OVERPROGRAM_PW(10, 0),
      .pgm_vpp_pin = PGM_VPP_PIN_P20,
      .pgm_pulses = 25,
      .pgm_vpp_always_on = 0,
      .f_can_blank_check = 1,
      .f_blank_check_value = 1,
      .f_2364_compat_pinout = 1,
      .pgm_variant = PGM_VARIANT_VPP_PULSED_POSITIVE
    }
  },
  {
    MANUFACTURER("MOTOROLA"),
    NAME("MCM68764"),
    .device_definition = MCM6876X
  },
  {
    MANUFACTURER("MOTOROLA"),
    NAME("MCM68766"),
    .device_definition = MCM6876X
  },
  {
    MANUFACTURER("TI"),
    NAME("TMS2532A"),
    .device_definition = {
      .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 18 },
      .ctrl_pins_read_h = { 21 },
      .vpp = VOLT(21, 0),
      .pgm_pw_us = 50000,
      .pgm_vcc_extra = 0,
      .pgm_intial_single_pass = 0,
      .pgm_overprogram_5v_vcc = 0,
      .pgm_overprogram_ignore_verify = 0,
      .pgm_overprogram_multiply_n = 0,
      .pgm_overprogram_after = 0,
      .pgm_overprogram_pw = 0,
      .pgm_vpp_pin = PGM_VPP_PIN_P21,
      .pgm_pulses = 1,
      .pgm_vpp_always_on = 0,
      .f_can_blank_check = 1,
      .f_blank_check_value = 1,
      .f_2364_compat_pinout = 0,
      .pgm_variant = PGM_VARIANT_P20_PULSED_NEGATIVE
    }
  },
  {
    MANUFACTURER("TI"),
    NAME("TMS2532"),
    .device_definition = GENERIC_2532
  },
  {
    MANUFACTURER("HITACHI"),
    NAME("HN462532"),
    .device_definition = GENERIC_2532
  },
  {
    MANUFACTURER("MOTOROLA"),
    NAME("MCM2532"),
    .device_definition = GENERIC_2532
  },
  {
    MANUFACTURER("ST"),
    NAME("M2532"),
    .device_definition = GENERIC_2532
  },
  {
    MANUFACTURER("WSI"),
    NAME("WS57C43"),
    .device_definition = WS57C43_WS57C43B
  },
  {
    MANUFACTURER("WSI"),
    NAME("WS57C43B"),
    .device_definition = WS57C43_WS57C43B
  },
  {
    MANUFACTURER("WSI"),
    NAME("WS57C43C"),
    .device_definition = {
      .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19 },
      .ctrl_pins_read_h = { 18 },
      .vpp = VOLT(12, 750),
      .pgm_pw_us = 100,
      .pgm_vcc_extra = VOLT(1, 250),
      .pgm_intial_single_pass = 0,
      .pgm_overprogram_5v_vcc = 0,
      .pgm_overprogram_ignore_verify = 0,
      .pgm_overprogram_multiply_n = 0,
      .pgm_overprogram_after = 0,
      .pgm_overprogram_pw = OVERPROGRAM_PW(10, 0),
      .pgm_vpp_pin = PGM_VPP_PIN_P20,
      .pgm_pulses = 25,
      .pgm_vpp_always_on = 0,
      .f_can_blank_check = 1,
      .f_blank_check_value = 1,
      .f_2364_compat_pinout = 0,
      .pgm_variant = PGM_VARIANT_VPP_PULSED_POSITIVE
    }
  },
  { //Algorithm observed on oscilloscope on ALL-03 programmer
    MANUFACTURER("ICT"),
    NAME("27CX321"),
    .device_definition = {
      .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19 },
      .ctrl_pins_read_h = { 18 },
      .vpp = VOLT(12, 500),
      .pgm_pw_us = 100,
      .pgm_vcc_extra = VOLT(1, 0),
      .pgm_intial_single_pass = 0,
      .pgm_overprogram_5v_vcc = 0,
      .pgm_overprogram_ignore_verify = 0,
      .pgm_overprogram_multiply_n = 1,
      .pgm_overprogram_after = 0,
      .pgm_overprogram_pw = OVERPROGRAM_PW(5, 5),
      .pgm_vpp_pin = PGM_VPP_PIN_P20,
      .pgm_pulses = 30,
      .pgm_vpp_always_on = 0,
      .f_can_blank_check = 0,
      .f_blank_check_value = 0,
      .f_2364_compat_pinout = 0,
      .pgm_variant = PGM_VARIANT_VPP_PULSED_POSITIVE
    }
  },
  { //Same algorithm as CY7C264
    MANUFACTURER("CYPRESS"),
    NAME("CY7C244"),
    .device_definition = {
      .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19 },
      .ctrl_pins_read_h = { 18 },
      .vpp = VOLT(12, 500),
      .pgm_pw_us = 200,
      .pgm_vcc_extra = 0,
      .pgm_intial_single_pass = 0,
      .pgm_overprogram_5v_vcc = 0,
      .pgm_overprogram_ignore_verify = 1,
      .pgm_overprogram_multiply_n = 1,
      .pgm_overprogram_after = 0,
      .pgm_overprogram_pw = OVERPROGRAM_PW(4, 0),
      .pgm_vpp_pin = PGM_VPP_PIN_P19,
      .pgm_pulses = 10,
      .pgm_vpp_always_on = 1,
      .f_can_blank_check = 1,
      .f_blank_check_value = 0,
      .f_2364_compat_pinout = 0,
      .pgm_variant = PGM_VARIANT_CYPRESS
    }
  },
  {
    MANUFACTURER("NATIONAL"),
    NAME("NMC27C32H"),
    .device_definition = {
      .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 21 },
      .ctrl_pins_read_h = {},
      .vpp = VOLT(25, 0),
      .pgm_pw_us = 10000,
      .pgm_vcc_extra = 0,
      .pgm_intial_single_pass = 0,
      .pgm_overprogram_5v_vcc = 0,
      .pgm_overprogram_ignore_verify = 0,
      .pgm_overprogram_multiply_n = 0,
      .pgm_overprogram_after = 0,
      .pgm_overprogram_pw = 0,
      .pgm_vpp_pin = PGM_VPP_PIN_P20,
      .pgm_pulses = 1,
      .pgm_vpp_always_on = 0,
      .f_can_blank_check = 1,
      .f_blank_check_value = 1,
      .f_2364_compat_pinout = 0,
      .pgm_variant = PGM_VARIANT_P18_PULSED_NEGATIVE
    }
  },
  {
    MANUFACTURER("NATIONAL"),
    NAME("NMC27C32B"),
    .device_definition = {
      .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 21 },
      .ctrl_pins_read_h = {},
      .vpp = VOLT(12, 750),
      .pgm_pw_us = 100,
      .pgm_vcc_extra = VOLT(1, 250),
      .pgm_intial_single_pass = 0,
      .pgm_overprogram_5v_vcc = 0,
      .pgm_overprogram_ignore_verify = 0,
      .pgm_overprogram_multiply_n = 0,
      .pgm_overprogram_after = 0,
      .pgm_overprogram_pw = 0,
      .pgm_vpp_pin = PGM_VPP_PIN_P20,
      .pgm_pulses = 25,
      .pgm_vpp_always_on = 0,
      .f_can_blank_check = 1,
      .f_blank_check_value = 1,
      .f_2364_compat_pinout = 0,
      .pgm_variant = PGM_VARIANT_P18_PULSED_NEGATIVE
    }
  },
  {
    MANUFACTURER("AMD"),
    NAME("AM2732B"),
    .device_definition = {
      .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 21 },
      .ctrl_pins_read_h = {},
      .vpp = VOLT(12, 500),
      .pgm_pw_us = 1000,
      .pgm_vcc_extra = VOLT(1, 0),
      .pgm_intial_single_pass = 0,
      .pgm_overprogram_5v_vcc = 1,
      .pgm_overprogram_ignore_verify = 0,
      .pgm_overprogram_multiply_n = 0,
      .pgm_overprogram_after = 1,
      .pgm_overprogram_pw = OVERPROGRAM_PW(2, 0),
      .pgm_vpp_pin = PGM_VPP_PIN_P20,
      .pgm_pulses = 15,
      .pgm_vpp_always_on = 0,
      .f_can_blank_check = 1,
      .f_blank_check_value = 1,
      .f_2364_compat_pinout = 0,
      .pgm_variant = PGM_VARIANT_P18_PULSED_NEGATIVE
    }
  },

  {
    MANUFACTURER("AMD"),
    NAME("AM2732"),
    .device_definition = GENERIC_2732
  },
  {
    MANUFACTURER("EUROTECHNIQUE"),
    NAME("ET2732Q"),
    .device_definition = GENERIC_2732
  },
  {
    MANUFACTURER("EUROTECHNIQUE"),
    NAME("ETC2732Q"),
    .device_definition = GENERIC_2732
  },
  {
    MANUFACTURER("FUJITSU"),
    NAME("MBM2732"),
    .device_definition = GENERIC_2732
  },
  {
    MANUFACTURER("FUJITSU"),
    NAME("MB8532"),
    .device_definition = GENERIC_2732
  },
  {
    MANUFACTURER("HITACHI"),
    NAME("HN462732G"),
    .device_definition = GENERIC_2732
  },
  {
    MANUFACTURER("INTEL"),
    NAME("2732"),
    .device_definition = GENERIC_2732
  },
  {
    MANUFACTURER("MITSUBISHI"),
    NAME("M5L2732K"),
    .device_definition = GENERIC_2732
  },
  {
    MANUFACTURER("RFT"),
    NAME("U2732"),
    .device_definition = GENERIC_2732
  },
  {
    MANUFACTURER("NEC"),
    NAME("D2732"),
    .device_definition = GENERIC_2732
  },
  {
    MANUFACTURER("TOSHIBA"),
    NAME("TMM2732D"),
    .device_definition = GENERIC_2732
  },
  {
    MANUFACTURER("NATIONAL"),
    NAME("NMC27C32"),
    .device_definition = GENERIC_2732
  },
  {
    MANUFACTURER("TI"),
    NAME("TMS2732A"),
    .device_definition = {
      .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 21 },
      .ctrl_pins_read_h = {},
      .vpp = VOLT(21, 0),
      .pgm_pw_us = 10000,
      .pgm_vcc_extra = 0,
      .pgm_intial_single_pass = 0,
      .pgm_overprogram_5v_vcc = 0,
      .pgm_overprogram_ignore_verify = 0,
      .pgm_overprogram_multiply_n = 0,
      .pgm_overprogram_after = 0,
      .pgm_overprogram_pw = 0,
      .pgm_vpp_pin = PGM_VPP_PIN_P20,
      .pgm_pulses = 1,
      .pgm_vpp_always_on = 0,
      .f_can_blank_check = 1,
      .f_blank_check_value = 1,
      .f_2364_compat_pinout = 0,
      .pgm_variant = PGM_VARIANT_P18_PULSED_NEGATIVE
    }
  },
  {
    MANUFACTURER("NTE"),
    NAME("NTE2732A"),
    .device_definition = GENERIC_2732A
  },
  {
    MANUFACTURER("AMD"),
    NAME("AM2732A"),
    .device_definition = GENERIC_2732A
  },
  {
    MANUFACTURER("FUJITSU"),
    NAME("MBM2732A"),
    .device_definition = GENERIC_2732A
  },
  {
    MANUFACTURER("HITACHI"),
    NAME("HN482732AG"),
    .device_definition = GENERIC_2732A
  },
  {
    MANUFACTURER("INTEL"),
    NAME("2732A"),
    .device_definition = GENERIC_2732A
  },
  {
    MANUFACTURER("NEC"),
    NAME("D2732A"),
    .device_definition = GENERIC_2732A
  },
  {
    MANUFACTURER("ROCKWELL"),
    NAME("R87C32"),
    .device_definition = GENERIC_2732A
  },
  {
    MANUFACTURER("SIEMENS"),
    NAME("SAB2732A"),
    .device_definition = GENERIC_2732A
  },
  {
    MANUFACTURER("ST"),
    NAME("M2732A"),
    .device_definition = GENERIC_2732A
  },
  {
    MANUFACTURER("FUJITSU"),
    NAME("MBM27C32A"),
    .device_definition = GENERIC_2732A
  },
  {
    MANUFACTURER("NATIONAL"),
    NAME("NMC27C16H"),
    .device_definition = {
      .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19 },
      .ctrl_pins_read_h = { 21 },
      .vpp = VOLT(25, 0),
      .pgm_pw_us = 10000,
      .pgm_vcc_extra = 0,
      .pgm_intial_single_pass = 0,
      .pgm_overprogram_5v_vcc = 0,
      .pgm_overprogram_ignore_verify = 0,
      .pgm_overprogram_multiply_n = 0,
      .pgm_overprogram_after = 0,
      .pgm_overprogram_pw = 0,
      .pgm_vpp_pin = PGM_VPP_PIN_P21,
      .pgm_pulses = 1,
      .pgm_vpp_always_on = 1,
      .f_can_blank_check = 1,
      .f_blank_check_value = 1,
      .f_2364_compat_pinout = 0,
      .pgm_variant = PGM_VARIANT_P18_PULSED_POSITIVE
    }
  },
  {
    MANUFACTURER("NATIONAL"),
    NAME("NMC27C16B"),
    .device_definition = {
      .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19 },
      .ctrl_pins_read_h = { 21 },
      .vpp = VOLT(12, 750),
      .pgm_pw_us = 100,
      .pgm_vcc_extra = VOLT(1, 250),
      .pgm_intial_single_pass = 0,
      .pgm_overprogram_5v_vcc = 0,
      .pgm_overprogram_ignore_verify = 0,
      .pgm_overprogram_multiply_n = 0,
      .pgm_overprogram_after = 0,
      .pgm_overprogram_pw = 0,
      .pgm_vpp_pin = PGM_VPP_PIN_P21,
      .pgm_pulses = 25,
      .pgm_vpp_always_on = 1,
      .f_can_blank_check = 1,
      .f_blank_check_value = 1,
      .f_2364_compat_pinout = 0,
      .pgm_variant = PGM_VARIANT_P18_PULSED_POSITIVE
    }
  },
  {
    MANUFACTURER("AMD"),
    NAME("AM2716B"),
    .device_definition = {
      .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19 },
      .ctrl_pins_read_h = { 21 },
      .vpp = VOLT(12, 500),
      .pgm_pw_us = 1000,
      .pgm_vcc_extra = VOLT(1, 0),
      .pgm_intial_single_pass = 0,
      .pgm_overprogram_5v_vcc = 1,
      .pgm_overprogram_ignore_verify = 0,
      .pgm_overprogram_multiply_n = 0,
      .pgm_overprogram_after = 1,
      .pgm_overprogram_pw = OVERPROGRAM_PW(2, 0),
      .pgm_vpp_pin = PGM_VPP_PIN_P21,
      .pgm_pulses = 15,
      .pgm_vpp_always_on = 1,
      .f_can_blank_check = 1,
      .f_blank_check_value = 1,
      .f_2364_compat_pinout = 0,
      .pgm_variant = PGM_VARIANT_P18_PULSED_POSITIVE
    }
  },
  {
    MANUFACTURER("AMD"),
    NAME("AM2716"),
    .device_definition = GENERIC_2716
  },
  {
    MANUFACTURER("EUROTECHNIQUE"),
    NAME("ET2716Q"),
    .device_definition = GENERIC_2716
  },
  {
    MANUFACTURER("EUROTECHNIQUE"),
    NAME("ETC2716Q"),
    .device_definition = GENERIC_2716
  },
  {
    MANUFACTURER("FUJUTSU"),
    NAME("MBM2716"),
    .device_definition = GENERIC_2716
  },
  {
    MANUFACTURER("FUJUTSU"),
    NAME("MB8516"),
    .device_definition = GENERIC_2716
  },
  {
    MANUFACTURER("HITACHI"),
    NAME("HN462716"),
    .device_definition = GENERIC_2716
  },
  {
    MANUFACTURER("INTEL"),
    NAME("2716"),
    .device_definition = GENERIC_2716
  },
  {
    MANUFACTURER("MITSUBISHI"),
    NAME("M5L2716K"),
    .device_definition = GENERIC_2716
  },
  {
    MANUFACTURER("RFT"),
    NAME("U2716C"),
    .device_definition = GENERIC_2716
  },
  {
    MANUFACTURER("MOTOROLA"),
    NAME("MCM2716"),
    .device_definition = GENERIC_2716
  },
  {
    MANUFACTURER("NATIONAL"),
    NAME("MM2716"),
    .device_definition = GENERIC_2716
  },
  {
    MANUFACTURER("NEC"),
    NAME("D2716"),
    .device_definition = GENERIC_2716
  },
  {
    MANUFACTURER("NTE"),
    NAME("NTE2716"),
    .device_definition = GENERIC_2716
  },
  {
    MANUFACTURER("OKI"),
    NAME("MSM2716AS"),
    .device_definition = GENERIC_2716
  },
  {
    MANUFACTURER("ST"),
    NAME("M2716"),
    .device_definition = GENERIC_2716
  },
  {
    MANUFACTURER("SOVIET"),
    NAME("573RF2"),
    .device_definition = GENERIC_2716
  },
  {
    MANUFACTURER("TESLA"),
    NAME("MHB2716C"),
    .device_definition = GENERIC_2716
  },
  {
    MANUFACTURER("TI"),
    NAME("TMS2516"),
    .device_definition = GENERIC_2716
  },
  {
    MANUFACTURER("THOMSON"),
    NAME("ET2716Q"),
    .device_definition = GENERIC_2716
  },
  {
    MANUFACTURER("ST"),
    NAME("ET2716Q"),
    .device_definition = GENERIC_2716
  },
  {
    MANUFACTURER("TOSHIBA"),
    NAME("TMM323D"),
    .device_definition = GENERIC_2716
  },
  {
    MANUFACTURER("ST"),
    NAME("M2716A"),
    .device_definition = GENERIC_2716
  },
  {
    MANUFACTURER("NATIONAL"),
    NAME("NMC27C16"),
    .device_definition = GENERIC_2716
  },
  {
    MANUFACTURER("MOSTEK"),
    NAME("MK2716"),
    .device_definition = GENERIC_2716
  },
  {
    MANUFACTURER("SIEMENS"),
    NAME("SAB2716"),
    .device_definition = GENERIC_2716
  },
  {
    MANUFACTURER("WSI"),
    NAME("WS57C191"),
    .device_definition = WS57C191_WS57C191B
  },
  {
    MANUFACTURER("WSI"),
    NAME("WS57C191B"),
    .device_definition = WS57C191_WS57C191B
  },
  {
    MANUFACTURER("WSI"),
    NAME("WS57C191C"),
    .device_definition = {
      .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21 },
      .ctrl_pins_read_h = { 19, 18 },
      .vpp = VOLT(12, 750),
      .pgm_pw_us = 100,
      .pgm_vcc_extra = VOLT(1, 250),
      .pgm_intial_single_pass = 0,
      .pgm_overprogram_5v_vcc = 0,
      .pgm_overprogram_ignore_verify = 0,
      .pgm_overprogram_multiply_n = 0,
      .pgm_overprogram_after = 0,
      .pgm_overprogram_pw = OVERPROGRAM_PW(10, 0),
      .pgm_vpp_pin = PGM_VPP_PIN_P20,
      .pgm_pulses = 25,
      .pgm_vpp_always_on = 0,
      .f_can_blank_check = 1,
      .f_blank_check_value = 1,
      .f_2364_compat_pinout = 0,
      .pgm_variant = PGM_VARIANT_VPP_PULSED_POSITIVE
    }
  },
  {
    MANUFACTURER("TI"),
    NAME("TMS27C292"),
    .device_definition = {
      .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21 },
      .ctrl_pins_read_h = { 19, 18 },
      .vpp = VOLT(13, 500),
      .pgm_pw_us = 100,
      .pgm_vcc_extra = 0,
      .pgm_intial_single_pass = 0,
      .pgm_overprogram_5v_vcc = 0,
      .pgm_overprogram_ignore_verify = 1,
      .pgm_overprogram_multiply_n = 1,
      .pgm_overprogram_after = 0,
      .pgm_overprogram_pw = OVERPROGRAM_PW(24, 0),
      .pgm_vpp_pin = PGM_VPP_PIN_P20,
      .pgm_pulses = 4,
      .pgm_vpp_always_on = 1,
      .f_can_blank_check = 0, //Special handling not implemented in hardware required for blank-check
      .f_blank_check_value = 0,
      .f_2364_compat_pinout = 0,
      .pgm_variant = PGM_VARIANT_P18_PULSED_NEGATIVE_P19_VERIFY
    }
  },
  {
    MANUFACTURER("AMD"),
    NAME("AM27C191"),
    .device_definition = {
      .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21 },
      .ctrl_pins_read_h = { 19, 18 },
      .vpp = VOLT(13, 500),
      .pgm_pw_us = 1000,
      .pgm_vcc_extra = VOLT(1, 0),
      .pgm_intial_single_pass = 0,
      .pgm_overprogram_5v_vcc = 1,
      .pgm_overprogram_ignore_verify = 0,
      .pgm_overprogram_multiply_n = 1,
      .pgm_overprogram_after = 0,
      .pgm_overprogram_pw = OVERPROGRAM_PW(2, 0),
      .pgm_vpp_pin = PGM_VPP_PIN_P20,
      .pgm_pulses = 25,
      .pgm_vpp_always_on = 0,
      .f_can_blank_check = 1,
      .f_blank_check_value = 1,
      .f_2364_compat_pinout = 0,
      .pgm_variant = PGM_VARIANT_VPP_PULSED_POSITIVE
    }
  },
  {
    MANUFACTURER("TI"),
    NAME("TMS2508"),
    .device_definition = GENERIC_2758_L
  },
  {
    MANUFACTURER("INTEL"),
    NAME("2758"),
    .device_definition = GENERIC_2758_L
  },
  {
    MANUFACTURER("NATIONAL"),
    NAME("MM2758-A"),
    .device_definition = GENERIC_2758_L
  },
  {
    MANUFACTURER("TI"),
    NAME("TMS2758-JL0"),
    .device_definition = GENERIC_2758_L
  },
  {
    MANUFACTURER("INTEL"),
    NAME("2758-S1865"),
    .device_definition = GENERIC_2758_H
  },
  {
    MANUFACTURER("NATIONAL"),
    NAME("MM2758-B"),
    .device_definition = GENERIC_2758_H
  },
  {
    MANUFACTURER("TI"),
    NAME("TMS2758-JL1"),
    .device_definition = GENERIC_2758_H
  }
};

uint8_t num_ics = sizeof(ics) / sizeof(ics[0]);