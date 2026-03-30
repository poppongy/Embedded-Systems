/*
 * app.c
 *
 *  Created on: Feb 15, 2026
 *      Author: Prince
 */

/*
 * Hardware implementation
 */
#include "app.h"
#include "state_machine.h"

void app_init(void)
{
    state_machine_init();
}

void app_loop(void)
{
    state_machine_run();
}
