#include "exti_drv.h"

static volatile uint8_t btnEvent_flag = 0;
void exti_init(void)
{
	//Enable SYSCFG clock
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	//configure port B using syscfg reg
	//clear first
	SYSCFG->EXTICR[3] &= ~(SYSCFG_EXTICR4_EXTI12);
	//select port B
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI12_PB;

	//Falling Edge detection
	EXTI->RTSR &= ~EXTI_RTSR_TR12;
	EXTI->FTSR |=EXTI_FTSR_TR12;

	//clearing any pending register in the EXTI block
	EXTI->PR |= EXTI_PR_PR12;

	//Enable interrupt on the peripheral side --Unmask interrupt
	EXTI->IMR |= EXTI_IMR_MR12;

	//Enable NVIC interrupt
	NVIC_EnableIRQ(EXTI15_10_IRQn);
}
void EXTI15_10_IRQHandler(void)
{
	//check which exti line triggered
	if(EXTI->PR & EXTI_PR_PR12)
	{
		//clear pending flag or clear interrupt source
		EXTI->PR = EXTI_PR_PR12;

		//set the event flag --notify application
		btnEvent_flag = 1;
	}
}

uint8_t exti_get_button_event(void)
{
	return btnEvent_flag;
}

void exti_clear_button_event(void)
{
	btnEvent_flag = 0;
}


