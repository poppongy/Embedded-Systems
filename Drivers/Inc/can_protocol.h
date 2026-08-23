/*
 * can_protocol.h
 *
 *  Created on: Jul 30, 2026
 *      Author: Prince
 */

#ifndef INC_CAN_PROTOCOL_H_
#define INC_CAN_PROTOCOL_H_

#define CAN_ID_HEARTBEAT      0x100U
#define CAN_ID_BUTTON_EVENT   0x123U
#define HEARTBEAT_ALIVE       0xAAU

//HEARTBEAT DETAILS
#define CAN_HEARTBEAT_DLC	  1U
#define HEARTBEAT_DATA1		  0xAAU

//BUTTON FRAME DETAILS
#define CAN_BUTTON_EVENT_DLC      3U
#define BTNFRAME_DATA0		      0x11U
#define BTNFRAME_DATA1		      0x22U
#define BTNFRAME_DATA2		      0x33U

#endif /* INC_CAN_PROTOCOL_H_ */
