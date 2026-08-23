/*
 * uart_drv.h
 *
 *  Created on: Apr 4, 2026
 *      Author: Prince
 */

#ifndef INC_UART_DRV_H_
#define INC_UART_DRV_H_

#include <stdint.h>
#include "stm32f407xx.h"

	void uart2_init(void);

	void uart2_write_char(char c);

	void uart2_write_string(const char *str);


#endif /* INC_UART_DRV_H_ */
