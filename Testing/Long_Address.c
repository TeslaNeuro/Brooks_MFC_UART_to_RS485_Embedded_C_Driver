#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Function to extract long address
void extract_long_address(const uint8_t* response, uint8_t* long_address) {
    // Manufacturer code is the 17th byte in the response
    uint8_t manufacturer_code = response[16] | 0x80;  // Logical OR with 0x80

    // Device type code is the 18th byte in the response
    uint8_t device_type_code = response[17];

    // Device ID is the 25nd, 26rd, and 27th bytes in the response
    uint8_t device_id[3] = {response[24], response[25], response[26]};

    // Combine to form the long address
    long_address[0] = manufacturer_code;
    long_address[1] = device_type_code;
    memcpy(&long_address[2], device_id, 3);
}

int main() {
    // Example response
    const uint8_t response[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0x86, 0x80, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x0e, 0x00, 0x00, 0xfe, 0x0a, 0x32, 0x05, 0x05, 0x01, 0x03, 0x08, 0x01, 0x7e, 0x37, 0x72, 0xd5};

    // Array to hold the long address
    uint8_t long_address[5];

    // Extract the long address
    extract_long_address(response, long_address);

    // Print the long address
    printf("Long Address: ");
    for (int i = 0; i < 5; i++) {
        printf("%02X ", long_address[i]);
    }
    printf("\n");

    return 0;
}
