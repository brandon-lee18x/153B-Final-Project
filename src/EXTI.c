#include "EXTI.h"

#include "DAC.h"

void EXTI_Init(void) {
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN; //enable port C clock

	// Initialize User Button
	GPIOC->MODER &= ~GPIO_MODER_MODE13; //set PC13 to input mode
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD13; // set GPIO A to have no pull up or pull down
	
	// Configure SYSCFG EXTI
	// [TODO]
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;
	
	
	// Configure EXTI Trigger
	// [TODO]
	//EXTI->RTSR1 |= EXTI_RTSR1_RT13;
	EXTI->FTSR1 |= EXTI_FTSR1_FT13;

	
	// Enable EXTI
	// [TODO]
	EXTI->IMR1 |= EXTI_IMR1_IM13;
	
	// Configure and Enable in NVIC
	// [TODO]
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	NVIC_SetPriority(EXTI15_10_IRQn, 0);
}

#define DAC_MIN 0
#define DAC_MAX 4095
#define DAC_INCREMENT 256

static uint32_t dac_value = 0;
static enum {
    DOWN,
    UP
} direction = UP;

// [TODO] Interrupt handler
void EXTI15_10_IRQHandler(void) {
	EXTI->PR1 |= EXTI_PR1_PIF13; // Clear interrupt pending bit
	DAC_Write_Value(2500);
	for(int i = 0; i < 5000; i++);
	

}