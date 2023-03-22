/*
 * ECE 153B - Winter 2021
 *
 * Name(s):
 * Section:
 * Lab: 2B
 */

#include "SysTimer.h"
#include "stm32l476xx.h"

uint32_t volatile msTicks;
extern volatile float rolling_avg;
extern volatile short inactive;
volatile int inactive_ticks = INACTIVE_PERIOD;

void SysTick_Init(void) {
	// SysTick Control & Status Register
	SysTick->CTRL = 0; // Disable SysTick IRQ and SysTick Counter
	
	// SysTick Reload Value Register
	SysTick->LOAD = 79999; // [TODO] - Enter the correct LOAD value that will give us a 1 ms period
	
	// SysTick Current Value Register
	SysTick->VAL = 0;
	
	// Configure and Enable SysTick interrupt in NVIC
	NVIC_EnableIRQ(SysTick_IRQn);
	NVIC_SetPriority(SysTick_IRQn, 1); // Set Priority to 1
	
	// Enables SysTick exception request
	// 1 = counting down to zero asserts the SysTick exception request
	// 0 = counting down to zero does not assert the SysTick exception request
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	
	// Select clock source
	// If CLKSOURCE = 0, the external clock is used. The frequency of SysTick clock is the frequency of the AHB clock divided by 8.
	// If CLKSOURCE = 1, the processor clock is used.
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;		

	
	// Enable SysTick IRQ and SysTick Timer
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;  
}

void SysTick_Handler(void) {
		++msTicks;
	
		if (rolling_avg < ABS_REST_THRESHOLD) {
			if (!inactive){
				inactive_ticks--;
			}
		} else {
			inactive_ticks = INACTIVE_PERIOD;
			inactive = 0;
		}
		
		if (inactive_ticks==0){
			inactive = 1;
			inactive_ticks = INACTIVE_PERIOD;
		}
}

void delay(uint32_t T) {
	uint32_t currentTicks; // Hint: It may be helpful to keep track of what the current tick count is
	//clock_t startTime = clock();
	// [TODO] - Implement function that waits until a time specified by argument T
	//while (clock() < startTime + T);
	for (uint32_t i= 0; i < T * 80;i++);
}