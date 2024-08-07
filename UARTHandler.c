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
