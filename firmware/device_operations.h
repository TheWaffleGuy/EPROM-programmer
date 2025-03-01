#ifndef DEVICE_OPERATIONS_H
#define DEVICE_OPERATIONS

void device_operations_init();
void read_device();
void blank_check();
void compare_data();
void write_data();
void setVCC(uint8_t volt, uint8_t calibrated);
void setVPP(uint8_t volt, uint8_t calibrated);
void resetVCCandVPP();

#endif