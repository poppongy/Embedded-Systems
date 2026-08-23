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

#define CAN_TX_TIMEOUT_MS    100U

typedef struct
{
    uint32_t id;
    uint8_t  dlc;
    uint8_t  data[8];
} can_frame_t;

typedef enum
{
    CAN_STATUS_OK = 0,
    CAN_STATUS_NULL_POINTER,
    CAN_STATUS_INVALID_DLC,
    CAN_STATUS_TIMEOUT,
    CAN_STATUS_TX_FAILED,
    CAN_STATUS_NO_MESSAGE
} can_status_t;

void can1_init(void);
void can1_tx(void);
void can1_rx(void);
uint8_t can1_get_rx_event(void);
void can1_clear_rx_event(void);

void can1_heartbeat(void);
uint8_t can1_send(const can_frame_t *frame);
uint8_t can1_read(can_frame_t *frame);

#endif /* INC_CAN_DRV_H_ */
