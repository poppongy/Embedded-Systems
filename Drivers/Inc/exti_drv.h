/*
 * exti_drv.h
 *
 *  Created on: Mar 31, 2026
 *      Author: Prince
 */

#ifndef INC_EXTI_DRV_H_
#define INC_EXTI_DRV_H_

#include <stdint.h>
#include "stm32f407xx.h"

	void exti_init(void);

	uint8_t exti_get_button_event(void);

	void exti_clear_button_event(void);

#endif /* INC_EXTI_DRV_H_ */
