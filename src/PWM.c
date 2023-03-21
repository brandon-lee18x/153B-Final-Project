#include "PWM.h"

static volatile uint32_t ccr = 100;
static volatile short counting_up = 1;
 
void PWM_Init() {
	// Enable GPIO Port A Clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	
	// Enable TIM2 Clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	
	// Configure PA0
	GPIOA->MODER &= GPIO_MODER_MODE0_0; // set to AF (10)
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED0; // high output speed
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD0; // no PUPDR
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL0; // clear register
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL0_0; // AF 0001 (TIM2_CH1)
	
	// Configure PWM Output for TIM2 CH 1
	TIM2->CR1 &= ~TIM_CR1_DIR; //set to upcounting
	TIM2->PSC = 0x0003; //prescaler value
	TIM2->ARR = 1000;
	TIM2->CCMR1 &= ~TIM_CCMR1_OC1M; //clear compare mode bits
	TIM2->CCMR1 |= (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2); //0110
	TIM2->CCMR1 |= TIM_CCMR1_OC1PE; //enable output preload
	TIM2->CCER &= ~TIM_CCER_CC1P; //set output polarity to active high
	TIM2->CCER |= TIM_CCER_CC1E; //enable channel 1 output
	TIM2->CCR1 = ccr;
	TIM2->CR1 |= TIM_CR1_CEN; //enable counter
}