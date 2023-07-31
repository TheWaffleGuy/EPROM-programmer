typedef struct IC {
  char name[11];
  uint8_t adr_pins[13];
  uint8_t ctrl_pins_read_h[2];
} IC;

#if __GNUC__ > 11
#define NAME(n) .name = n
#else
#define NAME(n) { .name = n }
#endif

IC ics[] = {
  {
    NAME("CY7C264"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 }
  },
  {
    NAME("AM27C49"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 }
  },
  {
    NAME("ICT27CX641"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 }
  },
  {
    NAME("M27HC641"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 }
  },
  {
    NAME("S27HC641"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 }
  },
  {
    NAME("AT27HC641"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 }
  },
  {
    NAME("AT27HC641R"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 }
  },
  {
    NAME("LH5749"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 }
  },
  {
    NAME("WS57C49"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 }
  },
  {
    NAME("WS57C49B"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 }
  },
  {
    NAME("WS57C49C"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 }
  },
  {
    NAME("MCM68764"),
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 18, 21 }
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