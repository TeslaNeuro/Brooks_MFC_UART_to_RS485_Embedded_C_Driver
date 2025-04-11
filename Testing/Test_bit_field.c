#include <stdint.h>
#include <stdio.h>

// Function to initialize all bytes to zero
void initialize_bytes(uint8_t *bytes) {
    bytes[0] = 0x00;
    bytes[1] = 0x00;
    bytes[2] = 0x00;
    bytes[3] = 0x00;
}

// Function to set fixed bits according to the device specifications
void set_fixed_bits(uint8_t *bytes) {
    bytes[0] |= 0b00101010;  // Byte 0: Bits 0, 1, 3, 5 set to 1, Bits 2, 4, 6, 7 set to 0
    bytes[1] &= ~0b00111111; // Byte 1: Bits 0-5 set to 0
    bytes[2] &= ~0b01000000; // Byte 2: Bit 6 set to 0
    bytes[3] &= ~0b11111110; // Byte 3: Bits 1, 3-7 set to 0
}

// Function to enable specific alarms by setting their respective bits
void enable_alarm_bits(uint8_t *bytes) {
    // Enable only the Low Flow Alarm and High Flow Alarm
    bytes[2] |= 0b00000001;  // Byte 2, Bit 0: Low flow alarm
    bytes[2] |= 0b00000010;  // Byte 2, Bit 1: High flow alarm
}

int main() {
    uint8_t bytes[4];

    // Initialize the bytes
    initialize_bytes(bytes);

    // Set fixed bits according to the device specifications
    set_fixed_bits(bytes);

    // Enable specific alarms
    enable_alarm_bits(bytes);

    // Print the resulting bytes to verify correctness
    printf("Bytes to send (Big-Endian): 0x%02X 0x%02X 0x%02X 0x%02X\n",
           bytes[0], bytes[1], bytes[2], bytes[3]);

    // Here you would send the bytes to your device
    // send_bytes_to_device(bytes, sizeof(bytes));

    return 0;
}
