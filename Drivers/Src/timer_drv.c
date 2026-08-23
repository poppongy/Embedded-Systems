#include "timer_drv.h"
#include "stm32f407xx.h"

static volatile uint32_t ms_ticks = 0;

void timer2_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->PSC = 1600 - 1; //refactoring to industry standard.

    TIM2->ARR = 10 - 1;

    TIM2->CNT = 0;

    //Forces immediate loading of:prescaler value, auto-reload value
    TIM2->EGR |= TIM_EGR_UG;

    //Clear pending interrupt flags
    TIM2->SR = 0;

    //Allows timer overflow events to trigger interrupt requests.
    TIM2->DIER |= TIM_DIER_UIE;

    //Enable TIM2 interrupt inside NVIC controller
    NVIC_EnableIRQ(TIM2_IRQn);

    TIM2->CR1 |= TIM_CR1_CEN;
}

void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF)
    {
        TIM2->SR = 0;
        ms_ticks++;
    }
}

uint32_t timer_get_ticks(void)
{
    return ms_ticks;
}


void timer_start(soft_timer_t *t, uint32_t period_ms)
{
    t->start = timer_get_ticks();
    t->period = period_ms;
}


uint8_t timer_expired(soft_timer_t *t)
{
    if ((timer_get_ticks() - t->start) >= t->period)
    {
        return 1;
    }

    return 0;
}

void timer_reset(soft_timer_t *t)
{
    t->start = timer_get_ticks();
}

uint8_t timer_expired_periodic(soft_timer_t *t)
{
    if ((timer_get_ticks() - t->start) >= t->period)
    {
        t->start += t->period;  // drift-free reset
        return 1;
    }
    return 0;
}
