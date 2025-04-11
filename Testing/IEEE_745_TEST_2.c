#include <stdint.h>
#include <string.h>
#include <stdio.h>
// Function to reverse bytes
void reverse_bytes(uint8_t* bytes, size_t length) {
    uint8_t temp;
    for (size_t i = 0; i < length / 2; ++i) {
        temp = bytes[i];
        bytes[i] = bytes[length - i - 1];
        bytes[length - i - 1] = temp;
    }
}

// Function to pack a float value into IEEE-754 representation as bytes
void ieee_pack(float value, uint8_t *ieee_bytes) {
    // Create a union to ensure proper alignment and avoid strict aliasing violation
    union {
        float f;
        uint32_t ieee;
    } u;
    u.f = value;

    // Copy IEEE-754 representation to byte array
    memcpy(ieee_bytes, &u.ieee, sizeof(uint32_t));

    // Reverse byte order if system is little-endian
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        reverse_bytes(ieee_bytes, sizeof(uint32_t));
    #endif
}

// Function to unpack IEEE-754 representation from bytes to float
float ieee_unpack(uint8_t* bytes, size_t length, int is_big_endian) {
    if (is_big_endian) {
        reverse_bytes(bytes, length);
    }

    float ieee_value;
    memcpy(&ieee_value, bytes, sizeof(float));
    return ieee_value;
}

int main() {
    float original_value = -120.456f;
    uint8_t ieee_bytes[4];

    // Pack the float value into IEEE-754 byte representation
    ieee_pack(original_value, ieee_bytes);

    // Print the packed bytes
    printf("Packed bytes: ");
    for (int i = 0; i < 4; ++i) {
        printf("%02X ", ieee_bytes[i]);
    }
    printf("\n");

    // Unpack the bytes back into a float value
    float unpacked_value = ieee_unpack(ieee_bytes, 4, 1);

    // Print the unpacked float value
    printf("Unpacked value: %f\n", unpacked_value);

    return 0;
}