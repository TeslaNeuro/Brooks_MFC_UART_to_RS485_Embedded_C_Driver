#include <stdio.h>
#include "stdlib.h"
#include <stdint.h>

uint8_t crc(const uint8_t* command, size_t length){
    // Skip leading 'FF bytes'
    while (length > 0 && *command == 0xFF){
        command++;
        length--;
    }

    // Calculate checksum using bitwise XOR
    uint8_t result = 0;
    for (size_t i = 0; i < length; i++){
        result ^= command[i];
    }

    return result;
}

int main(){
    uint8_t BYTE_T[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x82, 0x80, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x06, 0xE3, 0x2D, 0xF1, 0xDF, 0x0C, 0xF0};
    // Modified array with checksum must be: FF FF FF FF FF 82 80 00 00 00 00 0B 06 E3 2D F1 DF 0C F0 13 
    size_t length = sizeof(BYTE_T) / sizeof(BYTE_T[0]);
    
    // Calculate checksum
    uint8_t checksum = crc(BYTE_T, length);

    // Append checksum to the array
    BYTE_T[length] = checksum;

    // Print the modified array
    printf("Modified array with checksum: ");
    for (size_t i = 0; i < length + 1; i++){
        printf("%02X ", BYTE_T[i]);
    }
    printf("\n");

    return 0;
}
