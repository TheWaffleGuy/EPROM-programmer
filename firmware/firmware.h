#ifndef FIRMWARE_H
#define FIRMWARE_H

typedef struct voltage_offsets {
  int16_t vcc_low;
  int16_t vcc_high;

  int16_t vpp_low;
  int16_t vpp_high;
} voltage_offsets;
static_assert(sizeof(voltage_offsets) == 8, "Error: unexpected size of voltage_offsets");

#define VCC_CAL_LOW VOLT(4, 500)
#define VCC_CAL_HIGH VOLT(6, 500)

#define VPP_CAL_LOW VOLT(12, 500)
#define VPP_CAL_HIGH VOLT(25, 0)

#define HEX_DIGIT(n) ((char)((n) + (((n) < 10) ? '0' : ('A' - 10))))

#endif