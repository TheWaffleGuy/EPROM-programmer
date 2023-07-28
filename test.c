#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define HEX_DIGIT(n) ((char)((n) + (((n) < 10) ? '0' : ('A' - 10))))

uint8_t port_a;
uint8_t port_b;
uint8_t port_c;
uint8_t port_d;

uint8_t buffer[8192];

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

IC ics[] = {
    {
        .name = "WS57C49C",
        .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19, 18 }
    },
    {
        .name = "MCM68764",
        .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 18, 21 }
    },
    {
        .name = "2532",
        .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 18 },
        .ctrl_pins_read_h = { 21 }
    },
    {
        .name = "57C43",
        .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 21, 19 },
        .ctrl_pins_read_h = { 18 }
    },
    {
        .name = "2732",
        .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19, 21 }
    },
    {
        .name = "2716",
        .adr_pins = { 8, 7, 6, 5, 4, 3, 2, 1, 23, 22, 19 },
        .ctrl_pins_read_h = { 21 }
    }
};

uint8_t num_ics = sizeof(ics) / sizeof(ics[0]);

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

void write_byte(uint8_t byte) {
    uint8_t high_nibble = byte >> 4;
    uint8_t low_nibble = byte & 0x0FU;
    putchar(HEX_DIGIT(high_nibble));
    putchar(HEX_DIGIT(low_nibble));
}

void print_record(char *type, uint8_t *data, uint8_t length, uint16_t adress) {
    uint8_t chksum = length + 3 + (uint8_t) adress + (uint8_t) ( adress >> 8 );
    printf("%s%02hX%04hX", type, length + 3, adress);
    for (uint8_t i = 0; i < length; i++) {
        write_byte(data[i]);
        chksum += data[i];
    }
    write_byte(~chksum);
    putchar('\n');
}

void print_buffer() {
    char header[] = "HDR";
    print_record("S0", header, sizeof(header) - 1, 0);

    for (uint16_t i = 0; i < sizeof(buffer); i+=32) {
        print_record("S1", buffer + i, 32, i);
    }

    print_record("S5", NULL, 0, sizeof(buffer) / 32);
    print_record("S9", NULL, 0, 0);
}

void list_ics() {
    for (uint8_t i = 0; i < num_ics; i++) {
        printf("%u) %s\n", i, ics[i].name);
    }
}

void ic_size(IC *ic) {
    uint8_t i;
    for(i = 0; i < sizeof(ic->adr_pins); i++) {
        uint8_t adr_pin = ic->adr_pins[i];
        if (adr_pin == 0) break;
    }
    printf("Device size: %u\n", i);
}

int main() {
    IC WS57C49C = ics[0];
    IC MCM68764 = ics[1];
    IC _2532    = ics[2];
    IC _57C43   = ics[3];
    IC _2732    = ics[4];
    IC _2716    = ics[5];

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
    set_adress_and_print_ports(&_2732, 2047);

    set_adress_and_print_ports(&_57C43, 4095);
    set_adress_and_print_ports(&_57C43, 2047);

    set_adress_and_print_ports(&_2532, 0);
    set_adress_and_print_ports(&_2532, 8191);
    set_adress_and_print_ports(&_2532, 4095);
    set_adress_and_print_ports(&_2532, 2047);

    set_adress_and_print_ports(&_2716, 0);

    memcpy(buffer,
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed euismod, arcu id faucibus mattis, erat nisi consectetur nisi, nec blandit quam lorem sed lectus. Quisque magna elit, auctor non nibh vel, interdum aliquet ligula. Morbi odio elit, tristique sit amet commodo quis, varius vel lorem. Quisque vel gravida libero. Suspendisse sodales dapibus est a rhoncus. Nulla eu porttitor magna, et maximus ligula. Sed ex turpis, facilisis vitae nisi id, lobortis varius enim. Sed malesuada rutrum velit. Phasellus bibendum felis vitae velit imperdiet laoreet. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Vestibulum consectetur sem vel mattis pellentesque.\n"
        "Nullam scelerisque quam nisl, sit amet consequat enim aliquet nec. Vivamus tincidunt dolor nibh, id iaculis nisi maximus non. Sed leo nunc, vulputate id lorem eget, fermentum hendrerit mi. Aenean et eleifend justo, vitae semper sem. Aenean vulputate ex sed venenatis ultricies. Praesent laoreet ligula ligula, vestibulum consequat libero ornare sed. Praesent tincidunt ipsum at sagittis posuere. Interdum et malesuada fames ac ante ipsum primis in faucibus. Cras lobortis, velit et porta semper, nunc nulla pharetra nibh, eu faucibus augue felis at felis.\n"
        "Morbi consequat quam vel lacus vulputate rutrum. Morbi sollicitudin at augue vel dictum. Nullam elementum ligula quis tristique lobortis. Maecenas sapien lacus, congue ut ipsum et, mattis vestibulum magna. Proin id accumsan magna. Mauris lobortis ex id ex accumsan interdum. Mauris vulputate sem sed felis ultrices gravida. Donec egestas orci in sodales ultricies.\n"
        "Morbi consectetur fringilla purus, vel vehicula massa dignissim ut. Donec sed velit a arcu molestie ultricies vitae quis arcu. Nunc varius leo at commodo eleifend. Aliquam vel quam est. Donec vehicula pellentesque leo, eget ultrices augue interdum non. Quisque tempor nulla quis odio vestibulum, ac faucibus tortor pretium. Etiam nec sagittis odio. Sed commodo metus sed metus facilisis, tincidunt fringilla lectus egestas. Nullam volutpat id neque quis sodales. Nullam vitae vehicula ex. Pellentesque ullamcorper risus sed elit commodo, nec vehicula ex gravida.\n"
        "Praesent molestie elementum massa sed condimentum. Sed ante orci, pulvinar a imperdiet sit amet, faucibus accumsan justo. Donec quis tristique felis. Morbi ac tincidunt mauris, at iaculis quam. Ut dolor ipsum, vehicula id mattis in, auctor quis lectus. Vestibulum auctor augue a sagittis finibus. Nulla lobortis leo non odio laoreet faucibus. Integer eget consectetur ex. Donec augue quam, rutrum et velit sed, ultricies scelerisque metus. Duis dictum viverra suscipit. Maecenas varius augue in est congue, eget congue orci lacinia. Nulla nec semper orci. Cras viverra neque orci, vel sodales sem scelerisque fringilla. Nullam ligula dui, scelerisque non ipsum sit amet, luctus euismod nibh. Nam sem lectus, lobortis quis ultrices vitae, convallis interdum urna. Duis mi tellus, tempor in ex a, accumsan molestie eros.\n"
        "Pellentesque at urna non magna posuere venenatis. Proin hendrerit tortor ut magna ornare viverra. Sed sodales turpis a dui imperdiet pharetra. In tempus, dolor non ornare auctor, tortor eros finibus orci, a accumsan nulla orci quis nibh. Suspendisse eget blandit nulla. Vivamus sit amet lectus sem. Mauris pellentesque rhoncus faucibus. Phasellus et velit tristique, scelerisque enim vel, commodo elit. Sed dapibus cursus erat, sed imperdiet justo porttitor quis. Donec purus ligula, elementum ut sodales non, fermentum id odio. Maecenas vestibulum est vitae elit hendrerit laoreet. Pellentesque fermentum blandit nibh, ac maximus leo. Nullam orci dolor, mattis non lacinia sit amet, dignissim ut sem. Nam sit amet ante quis arcu bibendum egestas vitae eu nisi. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae;\n"
        "Pellentesque et libero quis nibh posuere consectetur non vel eros. Phasellus nec dolor mattis, efficitur justo pulvinar, placerat nisi. Duis eget ex ac turpis gravida vehicula a et nibh. Cras vel arcu quam. Pellentesque vehicula, magna sit amet cursus dignissim, est enim volutpat tellus, eu tempor magna odio eget diam. Praesent sodales, tellus non pulvinar condimentum, tortor est vestibulum ex, eget ultrices sapien velit in arcu. Nunc nisi lacus, auctor faucibus egestas ut, sagittis viverra justo.\n"
        "Sed eros augue, tincidunt a sem nec, blandit ornare est. Integer euismod imperdiet egestas. Donec lobortis, magna et sollicitudin fermentum, dolor lorem convallis orci, sit amet cursus dolor eros interdum ex. Nulla augue velit, efficitur ac orci eu, tincidunt egestas enim. Praesent tincidunt nulla non ornare convallis. Curabitur dapibus consectetur odio eget congue. Cras erat libero, faucibus id viverra ut, porttitor in mi. Curabitur tristique porttitor venenatis. Suspendisse justo risus, lobortis eu rutrum ac, pharetra nec velit. Nullam fringilla tortor nec tortor molestie, at ultricies nulla porta. Aenean feugiat volutpat nulla, id bibendum odio accumsan in.\n"
        "Integer posuere purus nec felis dapibus, eu sollicitudin dui tincidunt. Aenean vel eros dignissim, vulputate magna nec, facilisis ante. Nam id erat ut odio fermentum imperdiet at non libero. Donec finibus nibh nec massa pretium, a congue odio elementum. Nam et efficitur sem. Pellentesque non velit id magna iaculis porta in vehicula nisi. Quisque nec ex risus. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Mauris porta sed nisi ut dictum. Duis eu condimentum arcu, a elementum nisi.\n"
        "Proin sed arcu orci. Praesent dignissim dignissim diam eget accumsan. Sed varius a leo et tristique. Cras maximus eleifend ex, at feugiat ante convallis id. Aliquam sodales mauris sit amet arcu faucibus lobortis. Praesent sit amet ex sit amet massa hendrerit ornare. Cras elementum venenatis interdum. Aenean convallis, libero quis condimentum bibendum, quam tellus lobortis ligula, nec bibendum lacus erat vitae dolor. Phasellus gravida tortor iaculis, hendrerit risus eget, vehicula est. Pellentesque bibendum nec lacus vel mattis. In et viverra lacus, sit amet imperdiet metus. In vitae metus eu justo tempus rutrum mattis vel nulla. Duis sit amet nisl et ante accumsan laoreet in at nulla. Sed justo elit, cursus in eros sit amet, cursus ullamcorper nibh. Ut a tellus eu lacus placerat porttitor. Fusce efficitur nisi nisi, ac semper sem fringilla cursus.\n"
        "Donec hendrerit elit sit amet condimentum interdum. Donec id aliquam elit. Nulla facilisi. Sed tristique nisi id sapien feugiat lobortis. Vestibulum et velit sed arcu fermentum tempus ac vitae orci. Donec sit amet nulla gravida, aliquet ex at, varius lorem. Duis sit amet metus tellus. Aenean nisi sem, suscipit id tincidunt nec, rutrum in augue. Fusce in sapien tempus, lacinia ligula sed, suscipit felis. Vestibulum blandit velit et arcu semper venenatis. Aliquam magna ipsum, scelerisque ut fermentum at, volutpat at risus. Aliquam mollis sapien tempus sem iaculis, non gravida neque vehicula. Sed tincidunt lobortis congue. Nullam congue ex ligula, non sagittis ipsum sodales eget.\n"
        "Fusce maximus velit eu congue consectetur. Vivamus efficitur eros eu magna mollis egestas. Morbi commodo pretium orci sit amet facilisis. Fusce a nulla vitae magna semper pellentesque non quis nulla. Cras tellus ligula, egestas at lorem non, rhoncus dictum dolor. Curabitur ut dolor in ex convallis pretium. Suspendisse id lacinia metus. Proin facilisis, mi id molestie porta, quam tortor facilisis arcu, quis faucibus mauris odio in urna. Etiam finibus ornare ullamcorper. Donec sit amet dui eu magna maximus pellentesque vehicula vel eros.\n"
        "Etiam euismod malesuada finibus. Nulla sit amet semper nisi, ut semper ante. Cras laoreet ante ligula, vel tincidunt arcu interdum vel. Nam leo neque, mollis sit amet felis sagittis, finibus efficitur magna. Donec finibus condimentum erat vel consequat. Sed molestie blandit aliquam. Integer ac justo vitae mauris condimentum scelerisque finibus a massa. Sed rhoncus, tellus at scelerisque varius, orci purus ultrices justo, et posuere elit leo efficitur metus. In nec mi urna. Donec elementum nisi non augue convallis aliquam. Duis dapibus mauris vel arcu fri",
        sizeof(buffer));
    
    print_buffer();

    list_ics();

    ic_size(&WS57C49C);
    ic_size(&_2532);
    
    return 0;
}