#ifndef DEVICE_OPERATIONS_H
#define DEVICE_OPERATIONS

void device_operations_init();
void read_device();
void blank_check(uint8_t margin);
void compare_data(uint8_t margin);
void write_data();
void setVCC(uint8_t volt, uint8_t calibrated);
void setVPP(uint8_t volt, uint8_t calibrated);
void resetVCCandVPP();

#endif