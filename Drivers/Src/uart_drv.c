/*
 * uart_drv.c
 *
 *  Created on: Apr 4, 2026
 *      Author: Prince
 */

#include "uart_drv.h"
#include "gpio_drv.h"

void uart2_init(void)
{
	gpio_init(GPIO_PORT_A, PIN_2,  GPIO_MODE_AF, NO_PUPD, AF7);
	//enable usart peripheral clock
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

	//set baud rate-- 16 MHz / 115200
	USART2->BRR = 0x008B;

	//enable transmitter
	USART2->CR1 |= USART_CR1_TE;

	//enable usart
	USART2->CR1 |= USART_CR1_UE;
}

void uart2_write_char(char c)
{
	/*while (!(USART2->SR & USART_SR_TXE))
	{
		USART2->DR = c;//
	}*/
	// Correct
	while (!(USART2->SR & USART_SR_TXE));  // wait until transmit data register is empty
	USART2->DR = c;                         // then write outside the loop
}

void uart2_write_string(const char *str)
{
    while (*str)
    {
        uart2_write_char(*str++);
    }
}
