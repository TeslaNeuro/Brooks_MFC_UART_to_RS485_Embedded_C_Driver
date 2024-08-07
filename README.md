# Brooks MFC UART to RS485 Embedded C STM32 Driver
- Brooks S-Protocol Mass Flow Controller RS485 Driver for STM32 microcontrollers. 
- Ensure Brooks MFC last 8 digit Serial number or Tag ID is acquired in advance for the purpose of testing or implementation.
- Typically requires a UART to RS485 chip for communication, preferably an isolated one (See http://www.ti.com/lit/ds/symlink/iso3082.pdf).
- Ground must be shared with power supply and HSXC to avoid any communication or signalling issues.
- Before operating the device, ensure all fluid connections have been properly tightened and, where applicable, all electrical connections have been properly terminated.
- Brooks MFC RS485 Pins are: Pin #14 RS485 B (inverted driver side) Pin #15  A (non-inverted driver side)
- See " RS485 Supplemental Manual " for more details of the driver implementation. (https://cdn.brooksinstrument.com/-/media/brooks/documentation/products/mass-flow-controllers/elastomer-sealed/sla5800/rs485-manual-sla5800-slamf.pdf?rev=-1&sc_lang=en&hash=10edc89cdf8c24ffaa1421f811dd958e)
- See Brooks MFC Alarm Codes for more information on how to implement and construct the correct set of bytes when enabling alarm codes.

# STM32 UART Required Settings
- UART Asynchronous must be enabled, Global interrupt must be enabled (Check NVIC Settings for STM32)
- Baud rate by default 19200 Bits/s (BROOKS MFC DEFAULT BAUD RATE)
- Word Length 9 bits (including parity)
- Parity Odd
- Stop Bits 1

# STM32 Global Interrupt Function
Depending on the STM32 UART peripheral you are using it is imperative the following global interrupt function is added to stm32fxxx_it.c . 
	
 	// @brief This function handles USART2 global interrupt.
	void USART2_IRQHandler(void)
	{
		  if(!__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE)){
			  HAL_UART_IRQHandler(&huart2);
			}
			else{
				UARTRXHandler(&huart2);
			}
	}

	
# Main.c Requirements

Ensure you have initialised BrooksProInit() function in int main(void) :

	BrooksProInit();
 
 	uint32_t brookspromsec = 0;

Ensure functions BrooksProTXHandler() and BrooksProHandler() are being called inside of While(1) :

	if((HAL_GetTick()-brookspromsec) >= BROOKSPRO_POLLRATE){
			BrooksProTXHandler();
			//HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
			brookspromsec = HAL_GetTick();
	}
	BrooksProHandler();
