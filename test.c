#include <stdio.h>
#include <stdint.h>

uint8_t port_a;
uint8_t port_b;
uint8_t port_c;
uint8_t port_d;

typedef struct pin {
    uint8_t *port;
    uint8_t index;
} pin;

pin pins[] = {
    { .port = &port_a, 0 },
    { .port = &port_a, 1 },
    { .port = &port_a, 2 },
    { .port = &port_a, 3 },
    { .port = &port_a, 4 },
    { .port = &port_a, 5 },
    { .port = &port_a, 6 },
    { .port = &port_a, 7 },
    { .port = &port_c, 0 }, //Data 0
    { .port = &port_c, 1 }, //Data 1
    { .port = &port_c, 2 }, //Data 2
    { .port = NULL, 0 },    //GND
    { .port = &port_c, 3 }, //Data 3
    { .port = &port_c, 4 }, //Data 4
    { .port = &port_c, 5 }, //Data 5
    { .port = &port_c, 6 }, //Data 6
    { .port = &port_c, 7 }, //Data 7
    { .port = &port_d, 7 },
    { .port = &port_d, 6 },
    { .port = NULL, 0 },    //#OE
    { .port = &port_d, 5 },
    { .port = &port_d, 4 },
    { .port = &port_d, 3 },
    { .port = NULL, 0 },    //VCC
};

typedef struct IC {
    char name[9];
    uint8_t adr_pins[13];
    uint8_t ctrl_pins_read_h[2];
} IC;

IC WS57C49C = {
    .name = "WS57C49C",
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 }
};

IC MCM68764 = {
    .name = "MCM68764",
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 18, 21 }
};

IC _2532 = {
    .name = "2532",
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 18 },
    .ctrl_pins_read_h = { 21 }
};

IC _2732 = {
    .name = "2732",
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 21 }
};

IC _2716 = {
    .name = "2716",
    .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19 },
    .ctrl_pins_read_h = { 21 }
};

void print_binary(uint8_t number) {
    printf("0b%d%d%d%d%d%d%d%d\n",
       (number >> 7) & 1, (number >> 6) & 1, (number >> 5) & 1, (number >> 4) & 1,
       (number >> 3) & 1, (number >> 2) & 1, (number >> 1) & 1, (number >> 0) & 1);
}

void set_adress(IC *ic, uint16_t adress) {
    port_a = 0;
    port_d &= 0b00000111; //Upper 5 bits used for adress

    for(uint8_t i = 0; i < sizeof(ic->adr_pins); i++) {
        uint8_t adr_pin = ic->adr_pins[i];
        if (adr_pin == 0) break;
        pin p = pins[adr_pin - 1];
        *(p.port) |= ( adress & 1 ) << p.index;
        adress = adress >> 1;
    }

    for(uint8_t i = 0; i < sizeof(ic->ctrl_pins_read_h); i++) {
        uint8_t ctrl_pin = ic->ctrl_pins_read_h[i];
        if (ctrl_pin == 0) break;
        pin p = pins[ctrl_pin - 1];
        *(p.port) |= 1 << p.index;
    }
}

void print_ports() {
    printf("Port A: ");
    print_binary(port_a);
    printf("Port D: ");
    print_binary(port_d);
}

void set_adress_and_print_ports(IC *ic, uint16_t adress) {
    printf("Setting adress for %s: %hu\n", ic->name, adress);
    set_adress(ic, adress);
    print_ports();
    printf("\n");
}

int main() {
    set_adress_and_print_ports(&WS57C49C, 0);
    set_adress_and_print_ports(&WS57C49C, 1);
    set_adress_and_print_ports(&WS57C49C, 2);
    set_adress_and_print_ports(&WS57C49C, 3);
    set_adress_and_print_ports(&WS57C49C, 256);

    set_adress_and_print_ports(&WS57C49C, 1024);
    set_adress_and_print_ports(&MCM68764, 1024);

    set_adress_and_print_ports(&WS57C49C, 8191);

    set_adress_and_print_ports(&_2732, 0);
    set_adress_and_print_ports(&_2732, 8191);
    set_adress_and_print_ports(&_2732, 4095);

    set_adress_and_print_ports(&_2532, 0);
    set_adress_and_print_ports(&_2532, 8191);
    set_adress_and_print_ports(&_2532, 4095);
    set_adress_and_print_ports(&_2532, 2047);

    set_adress_and_print_ports(&_2716, 0);
    
    return 0;
}