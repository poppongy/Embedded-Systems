from timer_drv.c
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

// from state_machine.c
/*
 * state_machine.c
 *
 *  Created on: Feb 15, 2026
 *      Author: Prince
 */
#include "state_machine.h"
#include "gpio_drv.h"
#include "timer_drv.h"

/*static void delay(volatile uint32_t count)
{
    while (count--) { __asm__("NOP"); }
    //Repeat N times:
    	// wait one CPU cycle
}*/

//static uint32_t last_toggle = 0;
static soft_timer_t led_timer;

void state_machine_init(void)
{
	// later: set initial state
	//gpio_init_output(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN);
	gpio_init_output(GPIO_PORT_D, PIN_12);
	timer_start(&led_timer, 1000);
}

/*BLOCKING DELAY
 * void state_machine_run(void)
{
   // later: transition + actions
	gpio_toggle(GPIO_PORT_D, PIN_12);

	delay_ms(500); wait loop - blocking
	//delay(500000);

}*/

/*USING INTERRUPT
 * this logic uses time comparisons not loops making it non-blocking
 * The CPU: checks elapsed time
 *toggles if ready
 *immediately continues executing the rest of the program
 *There is no pause, no stall, and no polling loop holding execution hostage.
 */
void state_machine_run(void)
{
	//“How much time has passed since the last toggle?”
	//with the condition, you asking has 1s elapsed since last toggle?
    /*if ((timer_get_ticks() - last_toggle) >= 1000)
    {
        //Reset the reference point
    	last_toggle = timer_get_ticks();

         //Toggle PD12
        GPIOD->ODR ^= GPIO_ODR_OD12;
    }*/

	//cooperative scheduler primitive
	 if (timer_expired(&led_timer))
	    {
	        timer_reset(&led_timer);
	        GPIOD->ODR ^= GPIO_ODR_OD12;
	    }
}

