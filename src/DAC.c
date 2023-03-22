#include "stm32l476xx.h"

static void DAC_Pin_Init(void) {
	//PA4
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; //enable port A clock
	//mode set to analog by default (11)
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD4; //set to no PUPDR
}

void 
	DAC_Write_Value(uint32_t value) {
  // [TODO] Write DAC1Ch1 right-aligned 12-bit value
	DAC1->DHR12R1 &= ~DAC_DHR12R1_DACC1DHR;
	DAC1->DHR12R1 |= value & DAC_DOR1_DACC1DOR;
}

void DAC_Init(void) {
	DAC_Pin_Init();
	RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN; //enable DAC clock
	DAC1->CR &= ~DAC_CR_EN1; //disable DAC channel 1
	DAC1->CR |= DAC_CR_TSEL1; //enable software trigger mode
	DAC1->CR &= ~DAC_CR_TEN1; //disable trigger
	DAC1->CR &= ~DAC_CR_CEN1; //DAC1 channel in normal operating mode
	DAC1->MCR &= ~DAC_MCR_MODE1; //DAC1 connected in normal mode to external pin only with buffer enabled; both EN1 and CEN1 have to = 0 to write
	DAC1->CR |= DAC_CR_EN1; //enable DAC channel 1

  DAC_Write_Value(0);
}