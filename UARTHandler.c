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
 * UARTHandler.c
 *
 *  Created on: May 13, 2024
 *      Author: Arshia Keshvari
 */

#include "UARTHandler.h"
#include "string.h"

//UART receive handler
void UARTRXHandler(UART_HandleTypeDef *h){
	if(h == &huart2){
		uint8_t c = h->Instance->DR;
		ring_buffer_put(0, &c);
	}
}

//Initialize UART - doesn't use HAL
void UARTInit(void){
	rb_attr_t attr1 = {sizeof(uint8_t), UART_FIFOBUFLEN, rb1buf};
	ring_buffer_init(&rbd, &attr1);

	__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
}

/*
 * Get character from UART FIFO
 * Returns 1 if character available, otherwise 0
 */
uint8_t UARTGetChar(uint8_t uart, char *c){
	char ch = -1;
	int8_t ret = 0;

	if(uart == 2){
		ret = ring_buffer_get(0, &ch);
	}

	*c = ch;
	if(ret == -1) return 0;
	return 1;
}

void UARTFlush(uint8_t uart){
	char c;
	while(UARTGetChar(uart, &c));
}
