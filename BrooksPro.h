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
 * BrooksPro.h
 *
 *  Created on: Feb 21, 2024
 *      Author: Arshia Keshvari
 */

#ifndef INC_BROOKSPRO_H_
#define INC_BROOKSPRO_H_

#include "MemFS.h"
#include "main.h"
#include "stdint.h"
#include "stddef.h"
#include "UARTHandler.h"
#include "SensorIO.h"

#define BROOKSPRO_RESPBUFLEN		128
#define BROOKSPRO_RESPONSETIMEOUT	100
#define BROOKSPRO_POLLRATE		150
#define BROOKSPRO_STARTUPTIME		5000

#define BROOKSPRO_MAXFLOWVALUE		32000

#define BROOKSPRO_UART			2

extern float soc;
extern int32_t bpmaxerror, bpminerror, bpmaxposchangeerror, bpmaxnegchangeerror;
extern uint16_t bp_setpoint, bp_reqsetpoint, bp_reqmeasure;
typedef enum{
	NOREQUEST = 0,
	SETPOINTSEND = 2,
	SENDFLOWLIMITALARM = 3,
	SETPOINTREQUEST = 4,
	FLOWLIMITALARMREQUEST = 5,
	ENABLEALARM = 6,
	SENDFLOWUNIT = 7,
	MEASUREREQUEST = 8,
	STATUSREQUEST = 16,
	BPERROR = 1,
} BrooksProStatus;

void pack_ascii_hex(char* input_string, unsigned char* result);
uint8_t crc(uint8_t* command, size_t length);
void ieee_pack(float value, uint8_t *ieee_bytes);
void reverse_bytes(uint8_t* bytes, size_t length);
float ieee_unpack(uint8_t* bytes, size_t length, int is_big_endian);

void BrooksProInit(void);

void brooks_init_command_message(uint8_t* message, char* tag_name_str);
void brooks_request_flow_command_message(uint8_t* message);
void brooks_send_setpoint_command_message(uint8_t* message, float setpoint_value);
void brooks_request_setpoint_command_message(uint8_t* message);
void brooks_request_alarm_command_message(uint8_t* message);
void brooks_send_alarm_command_message(uint8_t* message, float Lower_Limit_Value, float Higher_Limit_Value);
void brooks_send_enable_alarm_command_message(uint8_t* message);
void brooks_send_select_flow_unit_ref_command_message(uint8_t* message);

uint8_t BrooksProSendSetpoint(void);
uint8_t BrooksProReqSetpoint(void);
uint8_t BrooksProReqMeasure(void);
uint8_t BrooksProReqStatus(void);
uint8_t BrooksProReqAlarm(void);
uint8_t BrooksProSendAlarm(void);
uint8_t BrooksProEnableAlarm(void);
uint8_t BrooksProSendFlowUnit(void);

void BrooksProTXHandler(void);
void BrooksProHandler(void);

#endif /* INC_BROOKSPRO_H_*/
