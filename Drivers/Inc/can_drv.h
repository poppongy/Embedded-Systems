/*
 * can_drv.h
 *
 *  Created on: May 12, 2026
 *      Author: Prince
 */

#ifndef INC_CAN_DRV_H_
#define INC_CAN_DRV_H_

#include "gpio_drv.h"
#include "uart_drv.h"

void can1_init(void);
void can1_tx(void);
void can1_rx(void);
uint8_t can1_get_rx_event(void);
void can1_clear_rx_event(void);

#endif /* INC_CAN_DRV_H_ */
