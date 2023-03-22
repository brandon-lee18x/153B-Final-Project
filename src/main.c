#include "stm32l476xx.h"
#include "accelerometer.h"
#include "SysClock.h"
#include "UART.h"
#include "PWM.h"
#include "DAC.h"
#include <stdio.h>
#include <string.h>
#include "gui.h"
#include "SPI.h"
#include <math.h>
#include "MY_ILI9341.h"
#include "SysTimer.h"


#define STATE_REINFORCMENT 3
#define REP_DISPLAY_SIZE 10
#define REP_DISPLAY_X (ILI9341_WIDTH/2 - 6*REP_DISPLAY_SIZE/2)
#define REP_DISPLAY_Y (ILI9341_HEIGHT/2 - 8*REP_DISPLAY_SIZE/2)


#define DOWNSAMPLE 4

volatile float rolling_avg = 0;
volatile short inactive = 0;

typedef enum Typedef_States {
	top, 
	descent, 
	pause, 
	ascent
} States;

States state = top;
short numreps = 0;
short reps = 0;

//determine thresholds
void fsm() {
    switch(state) {
        case (top): {
					DAC_Write_Value(0);
					if (inactive) {
						reps = 0;
						ILI9341_drawChar(REP_DISPLAY_X, REP_DISPLAY_Y, (char)(reps+48), COLOR_WHITE, COLOR_OLIVE, REP_DISPLAY_SIZE);
					}
					
					if (fabs(rolling_avg) > ABS_MOVE_THRESHOLD) {
							if (++numreps > STATE_REINFORCMENT) {
								state = descent;
								numreps = 0;
							}
					} 
					break;
				}
        case (descent): {
					if (fabs(rolling_avg) < ABS_REST_THRESHOLD) {
							if (++numreps > STATE_REINFORCMENT) {
								state = pause;
								DAC_Write_Value(2000);
								numreps = 0;
							}
					}
					break;
				}
        case (pause): {
/*            short isDone = 0;
					while (!isDone) {
							for (int i = 0; i < 8000000; i++) {
							}
					}*/
					//for (int i = 0; i < 8000000; i++);
					if (fabs(rolling_avg) > ABS_MOVE_THRESHOLD) {
						if (++numreps > STATE_REINFORCMENT) {
							state = ascent;
							DAC_Write_Value(0);
							numreps = 0;
						}
					}
					//START TIMER while accelerometer at a certain pause threshold
					break;
				}
        case (ascent): {
					if (fabs(rolling_avg) < ABS_REST_THRESHOLD) {
						if (++numreps > STATE_REINFORCMENT) {
							state = top;
							numreps = 0;
							reps++;
							ILI9341_drawChar(REP_DISPLAY_X, REP_DISPLAY_Y, (char)(reps+48), COLOR_WHITE, COLOR_OLIVE, REP_DISPLAY_SIZE);
						}
					}
					break;
				}
    }
}

// Initializes USARTx
// USART2: UART Communication with Termite
// USART1: Bluetooth Communication with Phone
void Init_USARTx(int x) {
	if(x == 1) {
		UART1_Init();
		UART1_GPIO_Init();
		USART_Init(USART1);
	} else if(x == 2) {
		UART2_Init();
		UART2_GPIO_Init();
		USART_Init(USART2);
	} else {
		// Do nothing...
	}
}

int main(void){
	System_Clock_Init(); // System Clock = 80 MHz
	//PWM_Init();
	// Initialize I2C
	DAC_Init();
	Init_USARTx(2);
	I2C_GPIO_Init();
	I2C_Initialization();
	accelerometer_init();
	
	SPI1_GPIO_Init();
	SPI_Init();
	SysTick_Init();
	
	ILI9341_Init(SPI1, GPIOB, 8, GPIOB, 9, GPIOA, 6);
	ILI9341_Fill(COLOR_OLIVE);
	ILI9341_drawChar(REP_DISPLAY_X, REP_DISPLAY_Y, (char)(reps+48), COLOR_WHITE, COLOR_OLIVE, REP_DISPLAY_SIZE);
	
	int num_insert_accel = 0;
	Rolling_buffer z_arr;
	uint8_t SlaveAddress;
	float accel_x;
	int16_t accel_x_raw;
	int8_t accel_x_h;
	int8_t accel_x_l;
			
	float accel_y;
	int16_t accel_y_raw;
	int8_t accel_y_h;
	int8_t accel_y_l;
			
	float accel_z;
	int16_t accel_z_raw;
	int8_t accel_z_h;
	int8_t accel_z_l;
	uint8_t Data_Send;
	static float sum = 0;

	short num_samples = 0;
	float ravg;

	while(1) {
		// Slave Address: Note the "<< 1" must be present because bit 0 is treated as a don't care in 7-bit addressing mode
		SlaveAddress = 0b1101000 << 1; // address of accelerometer if AD0 = 0
		float d = 16384;

        //x_accel
/*	    Data_Send = 0x3B;
	    I2C_SendData(I2C1, SlaveAddress, &Data_Send, 1);
	    I2C_ReceiveData(I2C1, SlaveAddress, &accel_x_h, 1);
        Data_Send = 0x3C;
        I2C_SendData(I2C1, SlaveAddress, &Data_Send, 1);
        I2C_ReceiveData(I2C1, SlaveAddress, &accel_x_l, 1);
        accel_x_raw = (accel_x_h << 8) | accel_x_l;
        accel_x = accel_x_raw / d; //gives accel in g's

        //y_accel
        Data_Send = 0x3D;
        I2C_SendData(I2C1, SlaveAddress, &Data_Send, 1);
        I2C_ReceiveData(I2C1, SlaveAddress, &accel_y_h, 1);
        Data_Send = 0x3E;
       I2C_SendData(I2C1, SlaveAddress, &Data_Send, 1);
        I2C_ReceiveData(I2C1, SlaveAddress, &accel_y_l, 1);
        accel_y_raw = (accel_y_h << 8) | accel_y_l;
        accel_y = accel_y_raw / d; //gives accel in g's*/

        //z_accel
        Data_Send = 0x3F;
        I2C_SendData(I2C1, SlaveAddress, &Data_Send, 1);
        I2C_ReceiveData(I2C1, SlaveAddress, &accel_z_h, 1);
        Data_Send = 0x40;
        I2C_SendData(I2C1, SlaveAddress, &Data_Send, 1);
        I2C_ReceiveData(I2C1, SlaveAddress, &accel_z_l, 1);
        accel_z_raw = (accel_z_h << 8) | accel_z_l;
        accel_z = accel_z_raw / d; //gives accel in g's
        insert_accel(accel_z, &z_arr);
        if (num_insert_accel < SIZE_OF_BUFFER) {
            num_insert_accel++;
        } else { //num_insert_accel >= 10
					if ((++num_samples % DOWNSAMPLE) == (DOWNSAMPLE-1)){
						ravg = get_rolling_avg(&z_arr) + 0.50;
						rolling_avg =  250  * (ravg * ravg * ravg * ravg * ravg * ravg * ravg ); // / 0.5;
						if (ravg < 0) {
							//rolling_avg *= -1;
						}
						num_samples = 0;
					}
        }
				
        /*printf("accel_x: %f", accel_x);
        printf("\taccel_y: %f", accel_y);
        printf("\taccel_z: %f", accel_z);
        printf("\n");*/
				if (fabs(rolling_avg) > ABS_MOVE_THRESHOLD){
					printf("avg accel_z: %f\r\n", rolling_avg);
				}
				fsm();
        for(int i = 0; i < 50000; ++i);
   }

}