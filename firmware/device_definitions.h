typedef struct IC {
  char manufacturer[10];
  char name[11];
  uint8_t adr_pins[13];
  uint8_t ctrl_pins_read_h[2];

  uint8_t f_can_blank_check : 1;
  uint8_t f_blank_check_value : 1;
} IC;

#if __GNUC__ > 11
#define MANUFACTURER(n) .manufacturer = n
#define NAME(n) .name = n
#else
#define NAME(n) { .name = n }
#define MANUFACTURER(n) { .manufacturer = n }
#endif

const IC ics[] PROGMEM = {
  {
    MANUFACTURER("CYPRESS"),
    NAME("CY7C264"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
    .ctrl_pins_read_h = {},
    .f_can_blank_check = 1,
    .f_blank_check_value = 0
  },
  {
    MANUFACTURER("AMD"),
    NAME("AM27C49"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
    .ctrl_pins_read_h = {},
    .f_can_blank_check = 1,
    .f_blank_check_value = 1
  },
  {
    MANUFACTURER("ICT"),
    NAME("27CX641"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
    .ctrl_pins_read_h = {},
    .f_can_blank_check = 0,
    .f_blank_check_value = 0
  },
  {
    MANUFACTURER("MICROCHIP"),
    NAME("27HC641"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
    .ctrl_pins_read_h = {},
    .f_can_blank_check = 1,
    .f_blank_check_value = 1
  },
  {
    MANUFACTURER("SIGNETICS"),
    NAME("27HC641"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
    .ctrl_pins_read_h = {},
    .f_can_blank_check = 0,
    .f_blank_check_value = 0
  },
  {
    MANUFACTURER("ATMEL"),
    NAME("AT27HC641"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
    .ctrl_pins_read_h = {},
    .f_can_blank_check = 1,
    .f_blank_check_value = 1
  },
  {
    MANUFACTURER("ATMEL"),
    NAME("AT27HC641R"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
    .ctrl_pins_read_h = {},
    .f_can_blank_check = 1,
    .f_blank_check_value = 1
  },
  {
    MANUFACTURER("SHARP"),
    NAME("LH5749"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
    .ctrl_pins_read_h = {},
    .f_can_blank_check = 1,
    .f_blank_check_value = 1
  },
  {
    MANUFACTURER("WSI"),
    NAME("WS57C49"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
    .ctrl_pins_read_h = {},
    .f_can_blank_check = 1,
    .f_blank_check_value = 1
  },
  {
    MANUFACTURER("WSI"),
    NAME("WS57C49B"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
    .ctrl_pins_read_h = {},
    .f_can_blank_check = 1,
    .f_blank_check_value = 1
  },
  {
    MANUFACTURER("WSI"),
    NAME("WS57C49C"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
    .ctrl_pins_read_h = {},
    .f_can_blank_check = 1,
    .f_blank_check_value = 1
  },
  {
    MANUFACTURER("MOTOROLA"),
    NAME("MCM68764"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 18, 21 },
    .ctrl_pins_read_h = {},
    .f_can_blank_check = 1,
    .f_blank_check_value = 1
  },
  {
    MANUFACTURER("MOTOROLA"),
    NAME("MCM68766"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 18, 21 },
    .ctrl_pins_read_h = {},
    .f_can_blank_check = 1,
    .f_blank_check_value = 1
  },
  {
    MANUFACTURER("TI"),
    NAME("TMS2532A"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 18 },
    .ctrl_pins_read_h = { 21 }
  },
  {
    MANUFACTURER("TI"),
    NAME("TMS2532"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 18 },
    .ctrl_pins_read_h = { 21 }
  },
  {
    MANUFACTURER("HITACHI"),
    NAME("HN462532"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 18 },
    .ctrl_pins_read_h = { 21 }
  },
  {
    MANUFACTURER("MOTOROLA"),
    NAME("MCM2532"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 18 },
    .ctrl_pins_read_h = { 21 }
  },
  {
    MANUFACTURER("ST"),
    NAME("M2532"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 18 },
    .ctrl_pins_read_h = { 21 }
  },
  {
    MANUFACTURER("WSI"),
    NAME("WS57C43"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19 },
    .ctrl_pins_read_h = { 18 }
  },
  {
    MANUFACTURER("WSI"),
    NAME("WS57C43B"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19 },
    .ctrl_pins_read_h = { 18 }
  },
  {
    MANUFACTURER("WSI"),
    NAME("WS57C43C"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19 },
    .ctrl_pins_read_h = { 18 }
  },
  {
    MANUFACTURER("ICT"),
    NAME("27CX321"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19 },
    .ctrl_pins_read_h = { 18 }
  },
  {
    MANUFACTURER("CYPRESS"),
    NAME("CY7C244"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19 },
    .ctrl_pins_read_h = { 18 }
  },
  {
    MANUFACTURER("GENERIC"),
    NAME("2732"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 21 }
  },
  {
    MANUFACTURER("GENERIC"),
    NAME("2716"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19 },
    .ctrl_pins_read_h = { 21 }
  },
  {
    MANUFACTURER("WSI"),
    NAME("WS57C191"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21 },
    .ctrl_pins_read_h = { 19, 18 }
  },
  {
    MANUFACTURER("WSI"),
    NAME("WS57C191B"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21 },
    .ctrl_pins_read_h = { 19, 18 }
  },
  {
    MANUFACTURER("WSI"),
    NAME("WS57C191C"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21 },
    .ctrl_pins_read_h = { 19, 18 }
  },
  {
    MANUFACTURER("SHARP"),
    NAME("LH57191"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21 },
    .ctrl_pins_read_h = { 19, 18 }
  },
  {
    MANUFACTURER("TI"),
    NAME("TMS27C292"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21 },
    .ctrl_pins_read_h = { 19, 18 }
  },
  {
    MANUFACTURER("AMD"),
    NAME("AM27C191"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21 },
    .ctrl_pins_read_h = { 19, 18 }
  }
};

uint8_t num_ics = sizeof(ics) / sizeof(ics[0]);