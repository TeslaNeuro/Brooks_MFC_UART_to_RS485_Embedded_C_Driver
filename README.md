# Brooks MFC UART to RS485 Embedded C STM32 Driver
- Brooks S-Protocol Mass Flow Controller RS485 Driver for STM32F4 microcontrollers. 
- Ensure Brooks MFC last 8 digit Serial number or Tag ID is acquired in advance for the purpose of testing or implementation.
- RS485 main advantages as compared to other serial communication are tolerance to electrical noise, lengthy cable runs, multiple slaves in one connection, and fast data transmission speeds. Acting as a perfect candidate for industrial applications where reliability is critical.
- Typically requires a UART to RS485 chip for communication, preferably an isolated one (See [http://www.ti.com/lit/ds/symlink/iso3082.pdf](https://www.ti.com/lit/gpn/iso3082)).
- Ground must be shared with power supply and components used to avoid any communication or signalling issues.
- Before operating the device, ensure all fluid connections have been properly tightened and, where applicable, all electrical connections have been properly terminated.
- Brooks MFC RS485 Pins are: Pin #14 RS485 B (inverted driver side) Pin #15  A (non-inverted driver side)
- Always check for 120 Ohm (Low-tolerance) termination for RS485 A and B communication lines.
- See " RS485 Supplemental Manual " for more details of the driver implementation. (https://cdn.brooksinstrument.com/-/media/brooks/documentation/products/mass-flow-controllers/elastomer-sealed/sla5800/rs485-manual-sla5800-slamf.pdf)
- See Brooks MFC Alarm Codes for more information on how to implement and construct the correct set of bytes when enabling alarm codes.

## STM32 UART Required Settings
- UART Asynchronous must be enabled, Global interrupt must be enabled (Check NVIC Settings for STM32)
- Baud rate by default 19200 Bits/s (BROOKS MFC FACTORY DEFAULT BAUD RATE ! You can alter this by sending a command check documentation)
- Word Length 9 bits (including parity [8-bits])
- Parity Odd
- Stop Bits 1

## STM32 Global Interrupt Function
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

	
## Main.c Requirements

Ensure you have initialised BrooksProInit() function in int main(void) :

	BrooksProInit();
 
 	uint32_t brookspromsec = 0;

Ensure functions BrooksProTXHandler() and BrooksProHandler() are being called inside of your main loop While(1) :

	if((HAL_GetTick()-brookspromsec) >= BROOKSPRO_POLLRATE){
			BrooksProTXHandler();
			//HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
			brookspromsec = HAL_GetTick();
	}
	BrooksProHandler();
## Brooks Mass Flow Controller SLA58xxx RS485
  ![image](https://github.com/user-attachments/assets/c9ef4fb1-7d61-4363-b887-fa022184e216)

- Mass flow controllers typically require a stable and reliable power supply including product sensor calibration depending on the gas being used for your application (Hydrogen, Nitrogen and etc).
- Use Nitrogen Gas for testing purposes as less risks and higher personal safety is guaranteed. 

## License

This project includes firmware, software, hardware designs, and integration components and is licensed under the [Custom License Agreement](./LICENSE).

- **Non-Commercial Use**: You may use, modify, and distribute this project for non-commercial purposes.
- **Commercial Use**: To use this project for commercial purposes, please contact the author at [arshiakeshvariasl@gmail.com] to obtain full permission rights and discuss licensing terms.

See the [LICENSE](./LICENSE) file for full licensing details.

## Additional Notes
Should you want to use/modify/edit/reference/get creative inspiration from it for commercial, corporate and company needs you must gain full permission of usage and discuss licensing from myself. Otherwise, Feel free to play around with the code to make it custom to your own requirements. 

Code can be improved for better error handling or detection !
You are welcome to contact me should you have any general and technical questions.
