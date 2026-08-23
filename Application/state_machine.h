/*
 * state_machine.h
 *
 *  Created on: Feb 16, 2026
 *      Author: Prince
 */

#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_


#define NODE_PRODUCER      1U
#define NODE_RECEIVER      2U

#define NODE_ROLE          NODE_PRODUCER

#define LED_PERIOD_MS                 		1000U
#define HEARTBEAT_TX_PERIOD_MS         		100U
#define HEARTBEAT_CHECK_PERIOD_MS       	150U
#define HEARTBEAT_TIMEOUT_MS           		500U
#define BTN_DEBOUNCE_MS						50U
//Heartbeat status
#define NODE_A_UNKNOWN   	  				0U
#define NODE_A_ALIVE          				1U
#define NODE_A_OFFLINE        				2U

void state_machine_init(void);
void state_machine_run(void);

#endif /* STATE_MACHINE_H_ */
