#include <stdint.h>
#include <stdio.h>
#include <string.h>

void pack_ascii_hex(const char* input_string, unsigned char* result){
    unsigned char klaf[48];
    memset(klaf, 0, sizeof(klaf));

    for (int i = 0; i < 8; ++i){
        char s = input_string[i];
        klaf[i * 6 + 0] = ((unsigned char) s >> 5) & 1;
        klaf[i * 6 + 1] = ((unsigned char) s >> 4) & 1;
        klaf[i * 6 + 2] = ((unsigned char) s >> 3) & 1;
        klaf[i * 6 + 3] = ((unsigned char) s >> 2) & 1;
        klaf[i * 6 + 4] = ((unsigned char) s >> 1) & 1;
        klaf[i * 6 + 5] = ((unsigned char) s >> 0) & 1;
    }

    for (int i = 0; i < 6; ++i){
        unsigned char chunk = 0;
        for (int j =0; j < 8; ++j){
            chunk |= klaf[i * 8 + j] << (7 - j);
        }
        result[i] = chunk;
    }
}

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

void brooks_init_command_message(uint8_t command_number, uint8_t byte_count, const char* tag_name_str, uint8_t* message) {
    // Preambles and delimeter
    const uint8_t preambles_and_delimeter[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x82};

    // Address
    const uint8_t address[] = {0x80, 0x00, 0x00, 0x00, 0x00};

    // Array to hold the tag name in bytes
    unsigned char tag_name[6];

    // Convert the tag name string to bytes
    pack_ascii_hex(tag_name_str, tag_name);

    // Copy preambles and delimeter to the message
    memcpy(message, preambles_and_delimeter, sizeof(preambles_and_delimeter));

    // Copy address to the message
    memcpy(message + sizeof(preambles_and_delimeter), address, sizeof(address));

    // Add command number and byte count to the message
    message[sizeof(preambles_and_delimeter) + sizeof(address)] = command_number;
    message[sizeof(preambles_and_delimeter) + sizeof(address) + 1] = byte_count;

    // Copy tag name to the message
    memcpy(message + sizeof(preambles_and_delimeter) + sizeof(address) + 2, tag_name, byte_count);

    // Calculate and add checksum to the message
    message[sizeof(preambles_and_delimeter) + sizeof(address) + 2 + byte_count] = crc(message, sizeof(preambles_and_delimeter) + sizeof(address) + 2 + byte_count);
}

int main() {
    // Example command number and tag name
    uint8_t command_number = 0x0B;
    uint8_t byte_count = 0x06;

    //const char* tag_name_str = "82717030";
    const char* tag_name_str = "82719007";

    // Array to hold the message
    uint8_t message[20];

    // Construct the message
    brooks_init_command_message(command_number, byte_count, tag_name_str, message);

    // Print the message
    printf("Brooks Command #11: ");
    printf("\n");

    for (int i = 0; i < sizeof(message); i++) {
        printf("%02X ", message[i]);
    }
    printf("\n");

    return 0;
}
