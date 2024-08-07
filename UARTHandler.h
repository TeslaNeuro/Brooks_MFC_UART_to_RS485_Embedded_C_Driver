/*
 * UARTHandler.h
 *
 *  Created on: May 13, 2024
 *      Author: Arshia Keshvari
 */

#ifndef UARTHANDLER_H_
#define UARTHANDLER_H_

#include "main.h"
#include "RingBuffer.h"


extern UART_HandleTypeDef huart2;

void UARTInit(void);
void UARTRXHandler(UART_HandleTypeDef *h);

uint8_t UARTGetChar(uint8_t uart, char *c);
void UARTFlush(uint8_t uart);


#endif /* UARTHANDLER_H_ */
