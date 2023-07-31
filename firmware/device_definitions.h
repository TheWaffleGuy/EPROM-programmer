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