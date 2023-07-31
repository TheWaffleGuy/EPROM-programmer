typedef struct IC {
  char name[11];
  uint8_t adr_pins[13];
  uint8_t ctrl_pins_read_h[2];

  uint8_t f_can_blank_check : 1;
  uint8_t f_blank_check_value : 1;
} IC;

#if __GNUC__ > 11
#define NAME(n) .name = n
#else
#define NAME(n) { .name = n }
#endif

IC ics[] = {
  {
    NAME("CY7C264"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
    .ctrl_pins_read_h = {},
    .f_can_blank_check = 1,
    .f_blank_check_value = 0
  },
  {
    NAME("AM27C49"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
    .ctrl_pins_read_h = {},
    .f_can_blank_check = 1,
    .f_blank_check_value = 1
  },
  {
    NAME("ICT27CX641"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
    .ctrl_pins_read_h = {},
    .f_can_blank_check = 0,
    .f_blank_check_value = 0
  },
  {
    NAME("M27HC641"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
    .ctrl_pins_read_h = {},
    .f_can_blank_check = 1,
    .f_blank_check_value = 1
  },
  {
    NAME("S27HC641"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
    .ctrl_pins_read_h = {},
    .f_can_blank_check = 0,
    .f_blank_check_value = 0
  },
  {
    NAME("AT27HC641"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
    .ctrl_pins_read_h = {},
    .f_can_blank_check = 1,
    .f_blank_check_value = 1
  },
  {
    NAME("AT27HC641R"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
    .ctrl_pins_read_h = {},
    .f_can_blank_check = 1,
    .f_blank_check_value = 1
  },
  {
    NAME("LH5749"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
    .ctrl_pins_read_h = {},
    .f_can_blank_check = 1,
    .f_blank_check_value = 1
  },
  {
    NAME("WS57C49"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
    .ctrl_pins_read_h = {},
    .f_can_blank_check = 1,
    .f_blank_check_value = 1
  },
  {
    NAME("WS57C49B"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
    .ctrl_pins_read_h = {},
    .f_can_blank_check = 1,
    .f_blank_check_value = 1
  },
  {
    NAME("WS57C49C"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 },
    .ctrl_pins_read_h = {},
    .f_can_blank_check = 1,
    .f_blank_check_value = 1
  },
  {
    NAME("MCM68764"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 18, 21 },
    .ctrl_pins_read_h = {},
    .f_can_blank_check = 1,
    .f_blank_check_value = 1
  },
  {
    NAME("MCM68766"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 18, 21 },
    .ctrl_pins_read_h = {},
    .f_can_blank_check = 1,
    .f_blank_check_value = 1
  },
  {
    NAME("2532"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 18 },
    .ctrl_pins_read_h = { 21 }
  },
  {
    NAME("57C43"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19 },
    .ctrl_pins_read_h = { 18 }
  },
  {
    NAME("2732"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 21 }
  },
  {
    NAME("2716"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19 },
    .ctrl_pins_read_h = { 21 }
  },
  {
    NAME("57C191"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21 },
    .ctrl_pins_read_h = { 19, 18 }
  }
};

uint8_t num_ics = sizeof(ics) / sizeof(ics[0]);