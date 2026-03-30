/*

 * timer_drv.c
 *
 *  Created on: Feb 15, 2026
 *      Author: Prince



#include "timer_drv.h"


 * Global system tick counter.
 *
 * 'volatile' is CRITICAL because:
 * - value changes inside an interrupt
 * - compiler must not optimize reads/writes

void TIM2_Timebase_Init(void)
{
     1. Enable timer clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

     2. Stop timer
    TIM2->CR1 &= ~TIM_CR1_CEN;

     3. Configure prescaler system clock at 84MHZ
    TIM2->PSC = 84000 - 1;

     4. Configure auto reload 1s overflow
    TIM2->ARR = 0xFFFFFFFF;//free running

     5. Apply configuration This simply loads PSC and ARR immediately so the timer starts correctly.
    TIM2->EGR |= TIM_EGR_UG;

     6. Reset counter
    TIM2->CNT = 0;

     7. clear Pending flag
    //TIM2->SR = 0;

     8. Start timer
    TIM2->CR1 |= TIM_CR1_CEN;
}
TIM2->PSC = 84000 - 1;   Used for testing
	TIM2->ARR = 0;
	TIM2->CNT = 0;
	TIM2->EGR = 1;
	TIM2->CR1 = 1;


void delay_ms(volatile uint32_t ms)
{
	//this implementation logic causes CPU to wait until desired time is reached
	uint32_t start = TIM2->CNT;//Remember current time
	while ((TIM2->CNT - start) < ms);//How much time has passed (TIM2->CNT - start)
	// < ms -- Loop until elapsed time = desired delay
}

*/

#include "timer_drv.h"
#include "stm32f407xx.h"

/******************************************************************************
 * Global system tick counter
 *
 * static   -> visible only inside this source file (encapsulation)
 * volatile -> prevents compiler optimization because value changes inside ISR
 * uint32_t -> 32-bit counter for long runtime before overflow
 *
 * This variable increments once every timer interrupt event.
 * With current configuration: increments once per ms.
 ******************************************************************************/
static volatile uint32_t ms_ticks = 0;


/******************************************************************************
 * timer2_init()
 *
 * Configures TIM2 to generate periodic update interrupts that create a
 * software timebase using an interrupt-driven tick counter.
 ******************************************************************************/
void timer2_init(void)
{
    /**********************************************************************
     * Enable clock access to TIM2 peripheral
     *
     * RCC  = Reset and Clock Control module
     * APB1ENR = APB1 peripheral clock enable register
     * TIM2EN  = Bit enabling TIM2 clock
     *
     * Without enabling this clock, TIM2 registers cannot be accessed.
     **********************************************************************/
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;


    /**********************************************************************
     * Set prescaler value
     *  // TIM2 input clock = 16 MHz
     * Timer clock = 16MHZ
     *
     * Prescaler divides timer input frequency:
     *
     * Timer counter frequency =
     *      Timer clock / (PSC + 1)
     *
     * = 16,000,000 / 16,000
     * = 1,000 Hz
     *
     * So timer counter increments once every 1 ms
     **********************************************************************/
    //TIM2->PSC = 16000 - 1 does not toggle led
    TIM2->PSC = 1600 - 1; //refactoring to industry standard. behaviour does not toggle led in previous configuration


    /**********************************************************************
     * Set auto-reload register (ARR)
     *
     * Timer resets when counter reaches ARR value.
     *
     * Interrupt period =
     *      (PSC + 1) × (ARR + 1) / Timer clock
     *
     * = 1600 × 10 / 16,000,000
     * = 1 ms
     *
     * Therefore:
     * One interrupt occurs every 1ms second
     * 10 software tick = 1 ms
     **********************************************************************/
    TIM2->ARR = 10 - 1; //this toggles led under previous nonblocking configuration
    //TIM2->ARR = 1 - 1; does not toggle led
    /*ARR = 0
     *
     	 update event never fires
	 	 interrupt fires once only
		 counter never increments properly
		 UIF flag not generated as expected
     */

    /**********************************************************************
     * Reset counter register
     *
     * Ensures timer starts counting from zero instead of a random value
     **********************************************************************/
    TIM2->CNT = 0;


    /**********************************************************************
     * Generate update event
     *
     * Forces immediate loading of:
     *   - prescaler value
     *   - auto-reload value
     *
     * Without this step, new values may not take effect immediately.
     **********************************************************************/
    TIM2->EGR |= TIM_EGR_UG;


    /**********************************************************************
     * Clear pending interrupt flags
     *
     * Prevents accidental immediate interrupt execution when enabling
     * interrupts in the next steps.
     **********************************************************************/
    TIM2->SR = 0;


    /**********************************************************************
     * Enable update interrupt
     *
     * DIER = DMA/Interrupt Enable Register
     * UIE  = Update Interrupt Enable
     *
     * Allows timer overflow events to trigger interrupt requests.
     **********************************************************************/
    TIM2->DIER |= TIM_DIER_UIE;


    /**********************************************************************
     * Enable TIM2 interrupt inside NVIC controller
     *
     * NVIC = Nested Vector Interrupt Controller
     *
     * Even if peripheral interrupt is enabled, CPU must also allow it.
     **********************************************************************/
    NVIC_EnableIRQ(TIM2_IRQn);


    /**********************************************************************
     * Start timer counting
     *
     * CR1 = Control Register 1
     * CEN = Counter Enable bit
     *
     * Timer begins counting after this instruction executes.
     **********************************************************************/
    TIM2->CR1 |= TIM_CR1_CEN;
}


/******************************************************************************
 * TIM2 interrupt handler
 *
 * Executes automatically every time timer reaches ARR value.
 * With current configuration: runs once every second.
 ******************************************************************************/
void TIM2_IRQHandler(void)
{
    /**********************************************************************
     * Check if update interrupt flag is set
     *
     * UIF = Update Interrupt Flag
     *
     * Ensures interrupt occurred due to timer overflow event.
     **********************************************************************/
    if (TIM2->SR & TIM_SR_UIF)
    {
        /*****************************************************************
         * Clear interrupt flag
         *
         * Required so next interrupt can occur properly.
         * Writing 0 clears all pending flags.
         *****************************************************************/
        TIM2->SR = 0;


        /*****************************************************************
         * Increment software tick counter
         *
         * With current configuration:
         * ms_ticks increments once per MS
         *****************************************************************/
        ms_ticks++;
    }
	//GPIOD->ODR |= GPIO_ODR_OD12;
}


/******************************************************************************
 * timer_get_ticks()
 *
 * Returns current tick count value.
 *
 * Used by application code to implement non-blocking timing logic such as:
 *
 * if ((timer_get_ticks() - last_toggle) >= 1000)
 *
 * With current configuration:
 * 10 ticks = 1ms
 ******************************************************************************/
uint32_t timer_get_ticks(void)
{
    return ms_ticks;
}

/*
 * Starts a timer by capturing current system tick
 * and storing desired expiration period
 */
void timer_start(soft_timer_t *t, uint32_t period_ms)
{
    t->start = timer_get_ticks();
    t->period = period_ms;
}

/*
 * Returns 1 if timer expired
 * Returns 0 otherwise
 */
uint8_t timer_expired(soft_timer_t *t)
{
    if ((timer_get_ticks() - t->start) >= t->period)
    {
        return 1;
    }

    return 0;
}

/*
 * Resets timer start point without changing duration
 */
void timer_reset(soft_timer_t *t)
{
    t->start = timer_get_ticks();
}
