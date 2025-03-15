#ifndef DEVICE_H
#define DEVICE_H

#define VOLT(a,b) ( a * 8 + (b * 8) / 1000 ) //In 0.125 steps
#define OVERPROGRAM_PW(a,b) ( a * 2 + b / 5) //In 0.5 steps

#define PGM_VARIANT_VPP_PULSED_POSITIVE            0
#define PGM_VARIANT_P18_PULSED_NEGATIVE            1
#define PGM_VARIANT_P20_PULSED_NEGATIVE            2
#define PGM_VARIANT_P18_PULSED_NEGATIVE_P19_VERIFY 3
#define PGM_VARIANT_P18_PULSED_POSITIVE            4
#define PGM_VARIANT_CYPRESS                        5

#define PGM_VPP_PIN_P19 0
#define PGM_VPP_PIN_P20 1
#define PGM_VPP_PIN_P21 2

typedef struct device_def {
  uint8_t adr_pins[13];
  uint8_t ctrl_pins_read_h[2];

  uint8_t vpp;
  uint16_t pgm_pw_us;

  uint8_t pgm_vcc_extra : 4;
  uint8_t pgm_intial_single_pass : 1;
  uint8_t pgm_overprogram_5v_vcc : 1;
  uint8_t pgm_overprogram_ignore_verify : 1;
  uint8_t pgm_overprogram_multiply_n : 1;

  uint8_t pgm_overprogram_after : 1;
  uint8_t pgm_overprogram_pw : 6;
  uint8_t : 0;

  uint8_t pgm_vpp_pin : 2;
  uint8_t pgm_pulses : 6;

  uint8_t pgm_vpp_always_on: 1;
  uint8_t f_can_blank_check : 1;
  uint8_t f_blank_check_value : 1;
  uint8_t f_2364_compat_pinout : 1;
  uint8_t pgm_variant: 3;
} device_def;

typedef struct IC {
  char manufacturer[10];
  char name[11];
  device_def device_definition;
} IC;

#endif