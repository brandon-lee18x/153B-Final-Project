#include "stm32l476xx.h"
#include "accelerometer.h"
#include "SysClock.h"
#include "UART.h"
#include "PWM.h"
#include "DAC.h"
#include <stdio.h>
#include <string.h>

//determine thresholds
void accelerometer_fsm() {
    int state = 0;
    //0 = top, 1 = descent, 2 = pause, 3 = ascent
    switch(state) {
        case (0):
            state = 1;
            break;
        case (1):
            break;
        case (2):
            break;
        case (3):
            break;
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
	int i;
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
	while(1) {
		// Slave Address: Note the "<< 1" must be present because bit 0 is treated as a don't care in 7-bit addressing mode
		SlaveAddress = 0b1101000 << 1; // address of accelerometer if AD0 = 0
		float d = 16384;

    //x_accel
	  Data_Send = 0x3B;
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
    accel_y = accel_y_raw / d; //gives accel in g's

    //z_accel
    Data_Send = 0x3F;
    I2C_SendData(I2C1, SlaveAddress, &Data_Send, 1);
    I2C_ReceiveData(I2C1, SlaveAddress, &accel_z_h, 1);
    Data_Send = 0x40;
    I2C_SendData(I2C1, SlaveAddress, &Data_Send, 1);
    I2C_ReceiveData(I2C1, SlaveAddress, &accel_z_l, 1);
    accel_z_raw = (accel_z_h << 8) | accel_z_l;
    accel_z = accel_z_raw / d; //gives accel in g's
    printf("accel_x: %f", accel_x);
		printf("\taccel_y: %f", accel_y);
		printf("\taccel_z: %f", accel_z);
		printf("\n");
		DAC_Write_Value(2500);
    for(i = 0; i < 50000; ++i); 
   }

}