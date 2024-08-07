/*

## Custom License Agreement

This project includes software, hardware, and system integration and is licensed under the following terms:

### License Overview

- **Non-Commercial Use**: You are free to use, modify, and distribute the software, hardware designs, and integration components for non-commercial purposes, provided that you comply with the conditions outlined below.
- **Commercial Use**: If you wish to use, modify, or distribute the software, hardware designs, or integration components for commercial purposes, including but not limited to incorporating them into products or services for sale, you must obtain explicit permission from the author.

### Terms for Non-Commercial Use

1. **Attribution**: You must provide appropriate credit, including a link to the original repository and a notice indicating any changes made.
2. **Non-Commercial Use**: The use, modification, and distribution of this project are permitted only for non-commercial purposes. Commercial use requires obtaining a separate license.

### Terms for Commercial Use

1. **Contact Requirement**: To use the software, hardware designs, or integration components for commercial purposes, you must contact the author at [arshiakeshvariasl@gmail.com] to obtain permission and discuss licensing terms.
2. **Additional Licensing**: Commercial use will be subject to additional terms agreed upon between you and the author. This may include financial arrangements or other conditions.

### Integration with Other Projects

1. **Commercial Integration**: If you wish to integrate the software, hardware designs, or other components with other commercial projects or products, you must contact the author for permission and to discuss any additional licensing terms.
2. **Non-Commercial Integration**: Integration with non-commercial projects is permitted under the same terms as outlined for non-commercial use.

### Disclaimer

This project is provided "as-is" without warranty of any kind. The author is not liable for any damages arising from the use of this project, whether commercial or non-commercial. By using this project, you acknowledge that you assume all risks associated with its use.

### How to Contact

For commercial use inquiries or any other questions regarding this license, please contact the author at [arshiakeshvariasl@gmail.com].

By using this project, you agree to abide by the terms set forth in this license.

*/

/*
 * BrooksPro.c
 *
 *  Created on: Feb 21, 2024
 *      Author: Arshia Keshvari
 */

#include "BrooksPro.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"
#include "stddef.h"

/*
 * Source for commands: https://cdn.brooksinstrument.com/-/media/brooks/documentation/products/mass-flow-controllers/elastomer-sealed/sla5800/rs485-manual-sla5800-slamf.pdf
 * STM32F4xxx or ARM CORTEX-M is LITTLE ENDIAN =====>>>> BROOKS RS485 S-Protocol is BIG ENDIAN
 * =========================================================================================================================================================================
 * @NOTE: BROOKS SLA58xx MFC NEW MODELS Must be used for setting negative flow alarm limits
 * The Old BROOKS SLA58xx MFC MODELS does not support negative flow alarm limits
 * Any alarm code that gets enabled is typically volatile meaning it will be reset back to default(OFF / FALSE) after a power cycle (OFF/ON)
 */

#define REQUESTMADE					1
#define REQUESTNOTMADE					0

#define HEADER_LENGTH 					6
#define LONG_ADDRESS_LENGTH 				5
#define ALARM_ENABLE_LENGTH				4
#define FLOW_UNIT_REF_LENGTH				2

#define STATUS_REQUEST_SIZE 				28
#define MEASURE_REQUEST_SIZE 				21
#define SETPOINT_SEND_RESPONSE_SIZE 			26
#define SETPOINT_REQUEST_SIZE				26
#define ALARM_REQUEST_SIZE 				24
#define ALARM_SEND_RESPONSE_SIZE 			24
#define ENABLE_ALARM_RESPONSE_SIZE			20
#define FLOW_UNIT_SEND_RESPONSE_SIZE			18

uint8_t respbuf[BROOKSPRO_RESPBUFLEN] = {0};
uint8_t resppos = 0;
uint32_t measuretime = 0, measuretimelast = 0;
uint16_t bp_measurelast = 0;

extern UART_HandleTypeDef huart2;
//float soc = 0.0f;

BrooksProStatus bpstatus = NOREQUEST;

uint16_t bp_reqstatus = 0, bp_setpoint = 0, bp_reqsetpoint = 0, bp_reqmeasure = 0;
uint32_t trequest = 0, resptime = 0;

int32_t bpmaxerror = 32000, bpminerror = 0, bpmaxposchangeerror = 1000, bpmaxnegchangeerror = -1000;

//char* tag_name_str = "82717030";     // Tag name usually lasts 8 digits of BROOKS MFC serial number / ID
char* tag_name_str = "40300411";
const uint8_t command_init = 0x0B;
const uint8_t byte_count_init = 0x06;
uint8_t BROOKSPRO_HEADER_RESPONSE[HEADER_LENGTH] = {0xff, 0xff, 0xff, 0xff, 0xff, 0x86}; // Fixed header pattern
uint8_t long_address[LONG_ADDRESS_LENGTH] = {0}; // Global array to store the modified long address
uint8_t alarm_enable_bytes[ALARM_ENABLE_LENGTH] = {0};
uint8_t flow_unit_ref_bytes[FLOW_UNIT_REF_LENGTH] = {0};
//uint8_t bytearray[4] = {number>>24, (number>>16)&0xff, (number>>8)&0xff, number&0xff};

uint8_t st = 0;

float bp_flow = 0.0f;
float bp_setpoint_value = 0.0f;
float setpoint_val = 0.0f;
float send_setpoint = 0.0f;
float LOWER_LIMIT_ALARM_REQUEST = 0.0f;
float HIGHER_LIMIT_ALARM_REQUEST = 0.0f;
float LOWER_LIMIT_ALARM_SENT = 0.0f;
float HIGHER_LIMIT_ALARM_SENT = 0.0f;

void BrooksProInit(void){
	//MemGetSoC(&soc);
	UARTInit();
}

// Initial command message we send to brooks MFC to extract long address
uint8_t BrooksProReqStatus(void){
    uint8_t command_11[20];
	if(bpstatus == NOREQUEST){

		UARTFlush(BROOKSPRO_UART);

    		HAL_GPIO_WritePin(R485_DE_GPIO_Port, R485_DE_Pin, 1);
		HAL_GPIO_WritePin(R485_NRE_GPIO_Port, R485_NRE_Pin, 1);

        	brooks_init_command_message(command_11, tag_name_str); // Initialise the command message
        	HAL_UART_Transmit(&huart2, command_11, 20, 100);

		HAL_GPIO_WritePin(R485_DE_GPIO_Port, R485_DE_Pin, 0);
		HAL_GPIO_WritePin(R485_NRE_GPIO_Port, R485_NRE_Pin, 0);

		bpstatus = STATUSREQUEST;
		trequest = HAL_GetTick();

		return REQUESTMADE;
	}
	return REQUESTNOTMADE;
}

// Sends a command that selects what we want to set the flow unit and reference code to
uint8_t BrooksProSendFlowUnit(void){
	uint8_t command_196[18];
	if(bpstatus == NOREQUEST){

		UARTFlush(BROOKSPRO_UART);

		HAL_GPIO_WritePin(R485_DE_GPIO_Port, R485_DE_Pin, 1);
		HAL_GPIO_WritePin(R485_NRE_GPIO_Port, R485_NRE_Pin, 1);

		brooks_send_select_flow_unit_ref_command_message(command_196);
		HAL_UART_Transmit(&huart2, command_196, 18, 100);

		HAL_GPIO_WritePin(R485_DE_GPIO_Port, R485_DE_Pin, 0);
		HAL_GPIO_WritePin(R485_NRE_GPIO_Port, R485_NRE_Pin, 0);

		bpstatus = SENDFLOWUNIT;
		trequest = HAL_GetTick();

		return REQUESTMADE;
	}
    return REQUESTNOTMADE;
}


// Request to see the current flow measurement
uint8_t BrooksProReqMeasure(void){
    uint8_t command_1[14];
    if(bpstatus == NOREQUEST){

		UARTFlush(BROOKSPRO_UART);

    		HAL_GPIO_WritePin(R485_DE_GPIO_Port, R485_DE_Pin, 1);
		HAL_GPIO_WritePin(R485_NRE_GPIO_Port, R485_NRE_Pin, 1);

		brooks_request_flow_command_message(command_1);
		HAL_UART_Transmit(&huart2, command_1, 14, 100);

		HAL_GPIO_WritePin(R485_DE_GPIO_Port, R485_DE_Pin, 0);
		HAL_GPIO_WritePin(R485_NRE_GPIO_Port, R485_NRE_Pin, 0);

		bpstatus = MEASUREREQUEST;
		trequest = HAL_GetTick();

		return REQUESTMADE;
    }
    return REQUESTNOTMADE;
}

// Request to see what the current set-point is set to
uint8_t BrooksProReqSetpoint(void){
    uint8_t command_235[14];
    if(bpstatus == NOREQUEST){

		UARTFlush(BROOKSPRO_UART);

    		HAL_GPIO_WritePin(R485_DE_GPIO_Port, R485_DE_Pin, 1);
		HAL_GPIO_WritePin(R485_NRE_GPIO_Port, R485_NRE_Pin, 1);

		brooks_request_setpoint_command_message(command_235);
		HAL_UART_Transmit(&huart2, command_235, 14, 100);

		HAL_GPIO_WritePin(R485_DE_GPIO_Port, R485_DE_Pin, 0);
		HAL_GPIO_WritePin(R485_NRE_GPIO_Port, R485_NRE_Pin, 0);

		bpstatus = SETPOINTREQUEST;
		trequest = HAL_GetTick();

		return REQUESTMADE;
    }
    return REQUESTNOTMADE;
}

// Sending set-point value to MFC in percentage of full-scale
uint8_t BrooksProSendSetpoint(void){
    uint8_t command_236[19];
    if(bpstatus == NOREQUEST){

		UARTFlush(BROOKSPRO_UART);

    		HAL_GPIO_WritePin(R485_DE_GPIO_Port, R485_DE_Pin, 1);
		HAL_GPIO_WritePin(R485_NRE_GPIO_Port, R485_NRE_Pin, 1);

		brooks_send_setpoint_command_message(command_236, bp_setpoint_value);

		HAL_UART_Transmit(&huart2, command_236, 19, 100);

		HAL_GPIO_WritePin(R485_DE_GPIO_Port, R485_DE_Pin, 0);
		HAL_GPIO_WritePin(R485_NRE_GPIO_Port, R485_NRE_Pin, 0);

		bpstatus = SETPOINTSEND;
		trequest = HAL_GetTick();

		return REQUESTMADE;
    }
    return REQUESTNOTMADE;
}

// Requesting to see current flow alarm limits that are set by the MFC
uint8_t BrooksProReqAlarm(void){
	uint8_t command_247[14];
	if(bpstatus == NOREQUEST){

		UARTFlush(BROOKSPRO_UART);

		HAL_GPIO_WritePin(R485_DE_GPIO_Port, R485_DE_Pin, 1);
		HAL_GPIO_WritePin(R485_NRE_GPIO_Port, R485_NRE_Pin, 1);

		brooks_request_alarm_command_message(command_247);
		HAL_UART_Transmit(&huart2, command_247, 14, 100);

		HAL_GPIO_WritePin(R485_DE_GPIO_Port, R485_DE_Pin, 0);
		HAL_GPIO_WritePin(R485_NRE_GPIO_Port, R485_NRE_Pin, 0);

		bpstatus = FLOWLIMITALARMREQUEST;
		trequest = HAL_GetTick();

		return REQUESTMADE;
	}
    return REQUESTNOTMADE;
}

// Sending the alarm flow limits
uint8_t BrooksProSendAlarm(void){
	uint8_t command_248[22];
	if(bpstatus == NOREQUEST){

		UARTFlush(BROOKSPRO_UART);

		HAL_GPIO_WritePin(R485_DE_GPIO_Port, R485_DE_Pin, 1);
		HAL_GPIO_WritePin(R485_NRE_GPIO_Port, R485_NRE_Pin, 1);

		brooks_send_alarm_command_message(command_248, -5.0f, 100.0f);
		HAL_UART_Transmit(&huart2, command_248, 22, 100);

		HAL_GPIO_WritePin(R485_DE_GPIO_Port, R485_DE_Pin, 0);
		HAL_GPIO_WritePin(R485_NRE_GPIO_Port, R485_NRE_Pin, 0);

		bpstatus = SENDFLOWLIMITALARM;
		trequest = HAL_GetTick();

		return REQUESTMADE;
	}
    return REQUESTNOTMADE;
}

// @ need to add logic to extract what alarm is currently on
// Enabling certain alarms like low/high flow limits to the MFC
uint8_t BrooksProEnableAlarm(void){
	uint8_t command_246[18];
	if(bpstatus == NOREQUEST){

		UARTFlush(BROOKSPRO_UART);

		HAL_GPIO_WritePin(R485_DE_GPIO_Port, R485_DE_Pin, 1);
		HAL_GPIO_WritePin(R485_NRE_GPIO_Port, R485_NRE_Pin, 1);

		brooks_send_enable_alarm_command_message(command_246);
		HAL_UART_Transmit(&huart2, command_246, 18, 100);

		HAL_GPIO_WritePin(R485_DE_GPIO_Port, R485_DE_Pin, 0);
		HAL_GPIO_WritePin(R485_NRE_GPIO_Port, R485_NRE_Pin, 0);

		bpstatus = ENABLEALARM;
		trequest = HAL_GetTick();

		return REQUESTMADE;
	}
    return REQUESTNOTMADE;
}

// String to 6-bit ascii hex bytes
void pack_ascii_hex(char* input_string, unsigned char* result){
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

// Calculates Software Checksum for every Brooks S-Protocol command and returns in bytes
uint8_t crc(uint8_t* command, size_t length) {
    // Skip leading 'FF' bytes
    while (length > 0 && *command == 0xFF) {
        command++;
        length--;
    }

    // Calculate Software Checksum using bitwise XOR
    uint8_t result = 0;
    for (size_t i = 0; i < length; i++) {
        result ^= command[i];
    }
    return result;
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

    return;
}

// Function to reverse bytes
void reverse_bytes(uint8_t* bytes, size_t length) {
    uint8_t temp;
    for (size_t i = 0; i < length / 2; ++i) {
        temp = bytes[i];
        bytes[i] = bytes[length - i - 1];
        bytes[length - i - 1] = temp;
    }
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

// Brooks Initial command #11
void brooks_init_command_message(uint8_t* message, char* tag_name_str) {
    // Fixed parts of the message
    const uint8_t preambles_and_delimeter[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x82}; 	// Pre-ambles and de-limeter
    const uint8_t address[] = {0x80, 0x00, 0x00, 0x00, 0x00}; 							// Initialisation Address
    const uint8_t command_number_init = 0x0B;     			  							// Command number #11
    const uint8_t byte_count_init = 0x06;  					  // Byte Count == number of data bytes we want to send for the message
    unsigned char tag_name[6];     							  // Array to hold the tag name in bytes

    // Convert the tag name string to bytes
    pack_ascii_hex(tag_name_str, tag_name);

    // Copy preambles and delimeter to the message
    memcpy(message, preambles_and_delimeter, sizeof(preambles_and_delimeter));

    // Copy address to the message
    memcpy(message + sizeof(preambles_and_delimeter), address, sizeof(address));

    // Add command number and byte count to the message
    message[sizeof(preambles_and_delimeter) + sizeof(address)] = command_number_init;
    message[sizeof(preambles_and_delimeter) + sizeof(address) + 1] = byte_count_init;

    // Copy tag name to the message
    memcpy(message + sizeof(preambles_and_delimeter) + sizeof(address) + 2, tag_name, byte_count_init);

    // Calculate and add checksum to the message
    message[sizeof(preambles_and_delimeter) + sizeof(address) + 2 + byte_count_init] = crc(message, sizeof(preambles_and_delimeter) + sizeof(address) + 2 + byte_count_init);
}

// Function to send command #1 which request flow from BROOKS MFC
void brooks_request_flow_command_message(uint8_t* message) {
    // Fixed parts of the message
    const uint8_t preambles_and_delimiter[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x82};
    const uint8_t command_number = 0x01;
    const uint8_t byte_count = 0x00;

    // Copy preambles and delimiter to the message
    memcpy(message, preambles_and_delimiter, sizeof(preambles_and_delimiter));

    // Copy long address to the message
    memcpy(message + sizeof(preambles_and_delimiter), long_address, LONG_ADDRESS_LENGTH);

    // Add command number and byte count to the message
    message[sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH] = command_number;
    message[sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH + 1] = byte_count;

    // Calculate and add checksum to the message
    size_t message_length = sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH + 2;
    message[message_length] = crc(message, message_length);
}

// Function to send command #235 which reads current set-point usually as a percentage
void brooks_send_setpoint_command_message(uint8_t* message, float setpoint_value) {
    // Fixed parts of the message
    const uint8_t preambles_and_delimiter[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x82};
    const uint8_t command_number = 0xEC;
    const uint8_t byte_count = 0x05;
    const uint8_t units_code = 0x11;  // Unit Code 17: L/MIN  Unit Code 39: Percentage (Full-scale) Unit Code: 171 mL/MIN

    // Copy preambles and delimiter to the message
    memcpy(message, preambles_and_delimiter, sizeof(preambles_and_delimiter));

    // Copy long address to the message
    memcpy(message + sizeof(preambles_and_delimiter), long_address, LONG_ADDRESS_LENGTH);

    // Add command number, byte count, and units code to the message
    message[sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH] = command_number;
    message[sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH + 1] = byte_count;
    message[sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH + 2] = units_code;

    // Pack the float setpoint value into 4 bytes and add to the message
    uint8_t float_bytes[4];
    ieee_pack(setpoint_value, float_bytes);
    memcpy(message + sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH + 3, float_bytes, 4);

    // Calculate and add checksum to the message
    size_t message_length = sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH + 3 + 4;
    message[message_length] = crc(message, message_length);
}

// Function to send command #2 which request flow from BROOKS MFC
void brooks_request_setpoint_command_message(uint8_t* message) {
    // Fixed parts of the message
    const uint8_t preambles_and_delimiter[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x82};
    const uint8_t command_number = 0xEB;
    const uint8_t byte_count = 0x00;

    // Copy preambles and delimiter to the message
    memcpy(message, preambles_and_delimiter, sizeof(preambles_and_delimiter));

    // Copy long address to the message
    memcpy(message + sizeof(preambles_and_delimiter), long_address, LONG_ADDRESS_LENGTH);

    // Add command number and byte count to the message
    message[sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH] = command_number;
    message[sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH + 1] = byte_count;

    // Calculate and add checksum to the message
    size_t message_length = sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH + 2;
    message[message_length] = crc(message, message_length);
}

// Function to send command #247 which requests to read low and high alarm limits from BROOKS MFC
void brooks_request_alarm_command_message(uint8_t* message) {
    // Fixed parts of the message
    const uint8_t preambles_and_delimiter[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x82};
    const uint8_t command_number = 0xF7;
    const uint8_t byte_count = 0x00;

    // Copy preambles and delimiter to the message
    memcpy(message, preambles_and_delimiter, sizeof(preambles_and_delimiter));

    // Copy long address to the message
    memcpy(message + sizeof(preambles_and_delimiter), long_address, LONG_ADDRESS_LENGTH);

    // Add command number and byte count to the message
    message[sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH] = command_number;
    message[sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH + 1] = byte_count;

    // Calculate and add checksum to the message
    size_t message_length = sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH + 2;
    message[message_length] = crc(message, message_length);
}

// Command #248 Send Low and High Flow Alarm Limits from BROOKS MFC
void brooks_send_alarm_command_message(uint8_t* message, float Lower_Limit_Value, float Higher_Limit_Value){
    // Fixed parts of the message
    const uint8_t preambles_and_delimiter[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x82};
    const uint8_t command_number = 0xF8;
    const uint8_t byte_count = 0x08;

    // Copy preambles and delimiter to the message
    memcpy(message, preambles_and_delimiter, sizeof(preambles_and_delimiter));

    // Copy long address to the message
    memcpy(message + sizeof(preambles_and_delimiter), long_address, LONG_ADDRESS_LENGTH);

    // Add command number and byte count to the message
    message[sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH] = command_number;
    message[sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH + 1] = byte_count;

    // Pack the Low and High Flow Limit float values into 4 bytes and add to the message
    uint8_t float_bytes_1[4];
    ieee_pack(Lower_Limit_Value, float_bytes_1);
    uint8_t float_bytes_2[4];
    ieee_pack(Higher_Limit_Value, float_bytes_2);

    // Copy float bytes to the message
    memcpy(message + sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH + 2, float_bytes_1, 4);
    memcpy(message + sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH + 2 + 4, float_bytes_2, 4);

    // Calculate and add checksum to the message
    size_t message_length = sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH + 2 + 8; // 8 bytes for the two float values

    message[message_length] = crc(message, message_length);
}

/*
 * 	SLA5800 Models SHyGaN Project Default Alarm Codes
 *	Byte 0	0x2b
 *	Byte 1	0x40
 *	Byte 2	0x00 ====> 0x03 (Enables LOW/HIGH FLOW ALARM LIMITS see Brooks Alarm Codes on how to enable specific alarms)
 *	Byte 3	0x04
*/

// Function to send command #246 which requests to read low and high alarm limits from BROOKS MFC
void brooks_send_enable_alarm_command_message(uint8_t* message){
    // Fixed parts of the message
    const uint8_t preambles_and_delimiter[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x82};
    const uint8_t command_number = 0xF6;
    const uint8_t byte_count = 0x04;

    /* This is where we set the alarm bytes which are masked by their bit-field representation
     * See brooks mfc alarm codes in the RS485 Brooks S-Protocol supplement manual
     * */
    const uint8_t Alarm_Byte_0 = 0x2b; // binary 0010 1011 ==> Program memory corrupt, RAM test failure, Non-volatile memory failure and Internal power supply failure is set to TRUE
    const uint8_t Alarm_Byte_1 = 0x40; // binary 0010 0000 ==> set-point deviation (controller error is set to TRUE
    const uint8_t Alarm_Byte_2 = 0x03; // binary 0000 0011 ==> Low flow alarm and High flow alarm is set to TRUE
    const uint8_t Alarm_Byte_3 = 0x04; // binary 0000 0100 ==> No-flow Indication is set to TRUE

    // Copy preambles and delimiter to the message
    memcpy(message, preambles_and_delimiter, sizeof(preambles_and_delimiter));

    // Copy long address to the message
    memcpy(message + sizeof(preambles_and_delimiter), long_address, LONG_ADDRESS_LENGTH);

    // Add command number and byte count to the message
    message[sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH] = command_number;
    message[sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH + 1] = byte_count;

    // Add the alarm bytes to the message we want to construct
    message[sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH + 2] = Alarm_Byte_0;
    message[sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH + 3] = Alarm_Byte_1;
    message[sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH + 4] = Alarm_Byte_2;
    message[sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH + 5] = Alarm_Byte_3;

    // Calculate and add checksum to the message
    size_t message_length = sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH + 6;
    message[message_length] = crc(message, message_length);
}

// Function to send command #196 which selects the flow reference and unit from the BROOKS MFC
void brooks_send_select_flow_unit_ref_command_message(uint8_t* message){
    // Fixed parts of the message
    const uint8_t preambles_and_delimiter[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x82};
    const uint8_t command_number = 0xC4;
    const uint8_t byte_count = 0x02;

    /* This is where we set what units we want to select and the flow reference
     * See brooks flow unit/reference codes in the RS485 Brooks S-Protocol supplement manual
     * */
    const uint8_t Flow_Ref_Byte_0 = 0x02; // 2 - Gas calibraition as a reference
    const uint8_t Flow_Unit_Byte_1 = 0x11;

    // Copy preambles and delimiter to the message
    memcpy(message, preambles_and_delimiter, sizeof(preambles_and_delimiter));

    // Copy long address to the message
    memcpy(message + sizeof(preambles_and_delimiter), long_address, LONG_ADDRESS_LENGTH);

    // Add command number and byte count to the message
    message[sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH] = command_number;
    message[sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH + 1] = byte_count;

    // Add the Flow reference and unit bytes to the message we want to construct
    message[sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH + 2] = Flow_Ref_Byte_0;
    message[sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH + 3] = Flow_Unit_Byte_1;

    // Calculate and add checksum to the message
    size_t message_length = sizeof(preambles_and_delimiter) + LONG_ADDRESS_LENGTH + 4;
    message[message_length] = crc(message, message_length);
}

//Handle Brooks Protocol communications and reception
void BrooksProHandler(void){
	char c;               // Character received via UART
	uint8_t cu8;          // Unsigned 8-bit version of the received character

	static uint8_t doerror = 0; // Flag to track if errors should be processed

	if(HAL_GetTick()>SNS_NOERRORTIME){
		doerror = 1; // Enable error processing after a certain time has elapsed
	}

	if(bpstatus != NOREQUEST){
		//Request has been made, check for response or see if timeout happened
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 0);
		if(UARTGetChar(BROOKSPRO_UART, &c)){
			cu8 = (uint8_t)c; // Convert character to unsigned 8-bit integer (byte)
			if(resppos<BROOKSPRO_RESPBUFLEN){
				respbuf[resppos] = cu8; // Store received byte in buffer
				resppos++;
			}

	            	// Determine the expected response length based on the current status
	            	uint8_t expected_length = 0;
	            	if(bpstatus == STATUSREQUEST){
	                	expected_length = STATUS_REQUEST_SIZE;
	            	} else if(bpstatus == MEASUREREQUEST){
	                	expected_length = MEASURE_REQUEST_SIZE;
	            	} else if(bpstatus == SENDFLOWUNIT){
	                	expected_length = FLOW_UNIT_SEND_RESPONSE_SIZE;
	            	} else if(bpstatus == SETPOINTREQUEST){
	            		expected_length = SETPOINT_REQUEST_SIZE;
	            	} else if(bpstatus == SETPOINTSEND){
	            		expected_length = SETPOINT_SEND_RESPONSE_SIZE;
	            	} else if(bpstatus == FLOWLIMITALARMREQUEST){
	            		expected_length = ALARM_REQUEST_SIZE;
	            	} else if(bpstatus == ENABLEALARM){
	            		expected_length = ENABLE_ALARM_RESPONSE_SIZE;
	            	} else if(bpstatus == SENDFLOWLIMITALARM){
	            		expected_length = ALARM_SEND_RESPONSE_SIZE;
	            	}

			if(resppos == expected_length){

				if(doerror){
					bpflow.lostcomms = 0;
				}
				resppos = 0;
				if(memcmp(respbuf, BROOKSPRO_HEADER_RESPONSE, HEADER_LENGTH) == 0){
					// Start of packet
					resptime = HAL_GetTick() - trequest;

					if(bpstatus == STATUSREQUEST){

						// Extract long address and modify the first byte
                        			//long_address[0] = respbuf[16] | 0x80; // Can use OR with 0x80 which yields 0x8a works the same
						long_address[0] = respbuf[16]; // 0x0a at index 16
						long_address[1] = respbuf[17]; // 0x32 at index 17
						long_address[2] = respbuf[24]; // 0x7e at index 24
						long_address[3] = respbuf[25]; // 0x37 at index 25
						long_address[4] = respbuf[26]; // 0x72 at index 26

			                        // Store the first two bytes of the long address into bp_reqstatus
			                        bp_reqstatus = (long_address[0] << 8) | long_address[1];
			
			                        memset(respbuf, 0, 28);
			
			                        bpstatus = NOREQUEST;
					}

					else if(bpstatus == SENDFLOWUNIT){
						// During SEND FLOW UNIT ensure the acknowledgement is stored inside of flow_unit_ref_bytes
						flow_unit_ref_bytes[0] = respbuf[15]; // flow reference byte #0 at index 15
						flow_unit_ref_bytes[1] = respbuf[16]; // flow unit byte #1 at index 16

						bpstatus = NOREQUEST;
					}

					else if(bpstatus == MEASUREREQUEST){
						// Extract the flow value from command #1 response bytes from specific indexes
						bp_flow = ieee_unpack(&respbuf[16], 4, 1);

                        			bp_reqmeasure = bp_flow;
						bp_measurelast = bp_reqmeasure;

                        			measuretimelast = measuretime;
                        			measuretime = HAL_GetTick();

						if(doerror){
							if(bp_reqmeasure > bpmaxerror) bpflow.toohigh = 1;
							else bpflow.toohigh = 0;

							if(bp_reqmeasure < bpminerror) bpflow.toolow = 1;
							else bpflow.toolow = 0;

							if((bp_reqmeasure - bp_measurelast) > bpmaxposchangeerror) bpflow.abruptrise = 1;
							else bpflow.abruptrise = 0;

							if((bp_reqmeasure - bp_measurelast) < bpmaxnegchangeerror) bpflow.abruptfall = 1;
							else bpflow.abruptfall = 0;
						}

						bpstatus = NOREQUEST;
					}

					else if(bpstatus == SETPOINTREQUEST){

						setpoint_val = ieee_unpack(&respbuf[21], 4, 1);

						bp_reqsetpoint = setpoint_val;

						bpstatus = NOREQUEST;
					}

					else if(bpstatus == SETPOINTSEND){
						// Store the sent_setpoint as a response and store in send_setpoint variable
						send_setpoint = ieee_unpack(&respbuf[21], 4, 1);

						bp_setpoint = send_setpoint;

						bpstatus = NOREQUEST;
					}

					else if(bpstatus == FLOWLIMITALARMREQUEST){
						// Store the LOW / HIGH flow alarm limits from specific indexes inside of the following variables
						LOWER_LIMIT_ALARM_REQUEST = ieee_unpack(&respbuf[15], 4, 1);
						HIGHER_LIMIT_ALARM_REQUEST = ieee_unpack(&respbuf[19], 4, 1);

						bpstatus = NOREQUEST;
					}

					else if(bpstatus == SENDFLOWLIMITALARM){
						// Store the LOW / HIGH flow alarm limits from specific indexes inside of the following variables
						LOWER_LIMIT_ALARM_SENT = ieee_unpack(&respbuf[15], 4, 1);
						HIGHER_LIMIT_ALARM_SENT = ieee_unpack(&respbuf[19], 4, 1);

						bpstatus = NOREQUEST;
					}

					else if(bpstatus == ENABLEALARM){
						// During ENABLE ALARM ensure the acknowledgement is stored inside of alarm_enable_bytes
						alarm_enable_bytes[0] = respbuf[15]; // Alarm enable byte #0 at index 15
						alarm_enable_bytes[1] = respbuf[16]; // Alarm enable byte #1 at index 16
						alarm_enable_bytes[2] = respbuf[17]; // Alarm enable byte #2 at index 17
						alarm_enable_bytes[3] = respbuf[18]; // Alarm enable byte #3 at index 18

						bpstatus = NOREQUEST;
					}

					else{
						bpstatus = BPERROR;
						bp_flow = -10.0f;
						send_setpoint = -10.0f;
						setpoint_val = -10.0f;
					}

					if(bpstatus == BPERROR){
						//PushLog("BrooksPro Error");
						bpstatus = NOREQUEST;
					}
				}
			}
		}
		else if((HAL_GetTick() - trequest) > BROOKSPRO_RESPONSETIMEOUT){
			bpstatus = BPERROR;
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 1);
			//PushLog("Flow controller timeout");
			if(doerror){
				bpflow.lostcomms = 1;
			}
			if(HAL_GetTick() - trequest > 5000){
				//Completely flush fifo as it could be full of garbage
				UARTFlush(BROOKSPRO_UART);
				bpstatus = NOREQUEST;
				st = 0;
				resppos = 0;
				trequest = HAL_GetTick();
			}
		}
	}
}

//Small state machine to select which message to send
void BrooksProTXHandler(void){
	//static uint8_t st = 0;
	static uint8_t startupdone = 0;
	//Continuously send requests
	if(startupdone){
		switch(st){
		case 0:
			if(BrooksProReqStatus() == REQUESTMADE){
				st++;
			}
			break;

		case 1:
			if(BrooksProSendFlowUnit() == REQUESTMADE){
				st++;
			}
			break;

		case 2:
			if(BrooksProReqMeasure() == REQUESTMADE){
				st++;
			}
			break;

		case 3:
			if(BrooksProReqSetpoint() == REQUESTMADE){
				st++;
			}
			break;

		case 4:
			if(BrooksProSendSetpoint() == REQUESTMADE){
				st++;
			}
			break;

		case 5:
			if(BrooksProReqAlarm() == REQUESTMADE){
				st++;
			}
			break;

		case 6:
			if(BrooksProSendAlarm() == REQUESTMADE){
				st++;
			}
			break;

		case 7:
			if(BrooksProEnableAlarm() == REQUESTMADE){
				st = 0;
			}
			break;
		}
	}
	else{
		//Allow startup time for the sensor
		if(HAL_GetTick()>BROOKSPRO_STARTUPTIME) startupdone = 1;
	}
}
