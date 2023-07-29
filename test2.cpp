#include "mock_arduino.cpp"

#include "firmware/firmware.ino"

int main() {
    setup();
    while(true) {
        loop();
    }
}