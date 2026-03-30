#ifndef TIMER_DRV_H
#define TIMER_DRV_H

#include <stdint.h>

void timer2_init(void);
uint32_t timer_get_ticks(void);

/* Soft timer object */
typedef struct
{
    uint32_t start;
    uint32_t period;
} soft_timer_t;

/* Soft timer APIs */
void timer_start(soft_timer_t *t, uint32_t period_ms);
uint8_t timer_expired(soft_timer_t *t);
void timer_reset(soft_timer_t *t);
#endif
