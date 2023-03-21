#include "accelerometer.h"

extern void Error_Handler(void);

// Inter-integrated Circuit Interface (I2C)
// up to 100 Kbit/s in the standard mode, 
// up to 400 Kbit/s in the fast mode, and 
// up to 3.4 Mbit/s in the high-speed mode.

// Recommended external pull-up resistance is 
// 4.7 kOmh for low speed, 
// 3.0 kOmh for the standard mode, and 
// 1.0 kOmh for the fast mode

void I2C_GPIO_Init(void) {
	//USE PB6 & PB7 FOR I2C1_SCL AND I2C1_SDA RESPECTIVELY
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN; //enable port B clk
	GPIOB->OTYPER |= GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7; //open drain
	GPIOB->MODER &= ~(GPIO_MODER_MODE6_0 | GPIO_MODER_MODE7_0); //clear lsb => moder bits [1:0] = 10 (alt function)
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL6|GPIO_AFRL_AFSEL7);
	GPIOB->AFR[0] |= (GPIO_AFRL_AFSEL6_2|GPIO_AFRL_AFSEL7_2); // set alt function to AF4 (0100)
	GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR6|GPIO_OSPEEDER_OSPEEDR7); //set output speed to very high (11) 
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD6|GPIO_PUPDR_PUPD7); // clear PUPDR bits
	GPIOB->PUPDR |= (GPIO_PUPDR_PUPD6_0|GPIO_PUPDR_PUPD7_0);// set lsb (01 = pull up)
}
	
#define I2C_TIMINGR_PRESC_POS	28
#define I2C_TIMINGR_SCLDEL_POS	20
#define I2C_TIMINGR_SDADEL_POS	16
#define I2C_TIMINGR_SCLH_POS	8
#define I2C_TIMINGR_SCLL_POS	0

void I2C_Initialization(void){
	uint32_t OwnAddr = 0x52;
	RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN; //enable clock for I2C1
	RCC->CCIPR &= ~(RCC_CCIPR_I2C1SEL); //clear bits associated with setting clock src for I2C1
	RCC->CCIPR |= RCC_CCIPR_I2C1SEL_0; //set clock src to system clock = 80 MHz
	RCC->APB1RSTR1 |= RCC_APB1RSTR1_I2C1RST; //reset I2C1
	RCC->APB1RSTR1 &= ~(RCC_APB1RSTR1_I2C1RST); //clear bits associated w/ resetting I2C1
	I2C1->CR1 &= ~(I2C_CR1_PE); //disable I2C
	I2C1->CR1 &= ~(I2C_CR1_ANFOFF | I2C_CR1_DNF | I2C_CR1_NOSTRETCH); // enable analog noise filter, disable digital noise filter, enable clock stretching?
	I2C1->CR1 |= I2C_CR1_ERRIE; // enable error interrupts
	I2C1->CR2 &= ~I2C_CR2_ADD10; //set master to 7-bit addressing mode; accelerometer slave address is 7 bits
	I2C1->CR2 |= I2C_CR2_AUTOEND | I2C_CR2_NACK; //enable automatic end mode and NACK generation??
	//CHANGE TIMINGR VALUES TO FIT ACCELEROMETER
	I2C1->TIMINGR |= (uint32_t)7 << I2C_TIMINGR_PRESC_POS;
	I2C1->TIMINGR |= (uint32_t)0 << I2C_TIMINGR_SCLDEL_POS;
	I2C1->TIMINGR |= (uint32_t)0 << I2C_TIMINGR_SDADEL_POS;
	I2C1->TIMINGR |= (uint32_t)5 << I2C_TIMINGR_SCLH_POS;
	I2C1->TIMINGR |= (uint32_t)12 << I2C_TIMINGR_SCLL_POS;
	I2C1->OAR1 &= ~(I2C_OAR1_OA1EN); // disables Own address 1
	I2C1->OAR2 &= ~(I2C_OAR2_OA2EN); // disables Own address 2
	I2C1->OAR1 &= ~(I2C_OAR1_OA1MODE); // set own address 1 as 7-bit address
	I2C1->OAR1 |= OwnAddr; //set own address in OAR1
	I2C1->OAR1 |= I2C_OAR1_OA1EN; // re-enable Own address 1
	//SYSCFG->CFGR1 |= (SYSCFG_CFGR1_I2C_PB6_FM | SYSCFG_CFGR1_I2C_PB6_FM);
	I2C1->CR1 |= I2C_CR1_PE; //re-enable I2C
}

//init steps: https://controllerstech.com/how-to-interface-mpu6050-gy-521-with-stm32/
int accelerometer_init(void) {
	uint8_t SlaveAddress = 0b11010000;
	/*
	uint8_t Data_Receive;
	uint8_t who_reg_address = 0x75;
	//sensor is valid if who_am_i reg (addres 0x75) return 0x68
	I2C_SendData(I2C1, SlaveAddress, &who_reg_address, 1);
	I2C_ReceiveData(I2C2, SlaveAddress, &Data_Receive, 1);
	if (Data_Receive != 0x68) {
		return -1;
	}*/
	
	//get accelerometer out of sleep mode
	//uint8_t sleep_mode[2] = {0x6B, 0b00000000};
	uint8_t pdata[2] = {0x6B, 0};
	//uint8_t sleep_mode_addr = 0x6B;
	I2C_SendData(I2C1, SlaveAddress, pdata, 2); //turn on accelerometer; change clk source to internal clk w/ freq of 8 MHZ; FINALIZE DECISION OF CLK SRC
	//uint8_t sleep_mode_data = 0;
	//I2C_SendData(I2C1, SlaveAddress, &sleep_mode_data, 1);
	
	//set SMPLRT_DIV, which effectively changes sample rate (sample rate = Gyroscope output rate / (1 + SMPLRT_DIV)). Rn gyroscope output rate is 8khz sample rate is 1 Khz
	pdata[0] = 0x19;
	pdata[1] = 0x07;
	I2C_SendData(I2C1, SlaveAddress, pdata, 1); //set SMPLRT_DIV TO 7

	//configure accel config regs
	pdata[0] = 0x1C;
	pdata[1] = 0x00;
	//uint8_t accel_config[2] = {0x1C, 0x00};
	I2C_SendData(I2C1, SlaveAddress, pdata, 2); //set acceleration range to +/- 2g; can change later
	return 0;
}

/*
states for I2C communication:
Free: SCL and SDA high
Start/Restart: SCL remains high while SDA transitions from high to low
Change: data bit to be transferred when SCL line is low
Data: SDA bit has to be stable when SCL line is high
Stop: SDA transitions from low to high while SCL line stays high
timing conditions:
- setup time: SDA has to be stable when SCL line is low for a certain amount of time
- hold time: time for SDA to hold data after SCL transitions from high to low
- from these timing conditions, can change min low clock period and min high clock period
other remarks for I2C:
- I2C address of peripheral sent out in first byte after start state
- ack bit sent every 8 bits (0 = ack, 1 = nack). Sent in the other direction (e.g if master sending data to peripheral, peripheral sends ACK/NACK bit)
- part name: GY-521 MPU-6050
to-do's from documentation:
- find slave address of accelerometer
- find delay times for accelerometer (setup time, hold time)
- get command byte to ask for acceleration value from accelerometer
slave address: 0x68 if AD0 = 0, 0x69 if AD0 = 1 (I think it's 0x68 based on video tutorial); 7 bits long 
pins to use: Vcc, gnd, SCL, SDA, AD0 (connect to gnd)
address of register to put accelerometer out of sleep mode: 0x6B
address of accelerometer acceleration configuration register (set range of accelerometer to +/-2g, 4g, 8g, 16g): 0x1C
addresses of accelerometer acceleration data registers: 0x3B - -0x40
register 25 changes sample rate of acceleration
*/

//===============================================================================
//                           I2C Start
// Master generates START condition
//    -- Slave address: 7 bits
//    -- Automatically generate a STOP condition after all bytes have been transmitted 
// Direction = 0: Master requests a write transfer
// Direction = 1: Master requests a read transfer
//===============================================================================
int8_t I2C_Start(I2C_TypeDef * I2Cx, uint32_t DevAddress, uint8_t Size, uint8_t Direction) {	
	
	// Direction = 0: Master requests a write transfer
	// Direction = 1: Master requests a read transfer
	
	uint32_t tmpreg = 0;
	
	// This bit is set by software, and cleared by hardware after the Start followed by the address
	// sequence is sent, by an arbitration loss, by a timeout error detection, or when PE = 0.
	tmpreg = I2Cx->CR2;
	
	tmpreg &= (uint32_t)~((uint32_t)(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP));
	
	if (Direction == READ_FROM_SLAVE)
		tmpreg |= I2C_CR2_RD_WRN;  // Read from Slave
	else
		tmpreg &= ~I2C_CR2_RD_WRN; // Write to Slave
		
	tmpreg |= (uint32_t)(((uint32_t)DevAddress & I2C_CR2_SADD) | (((uint32_t)Size << 16 ) & I2C_CR2_NBYTES));
	
	tmpreg |= I2C_CR2_START;
	
	I2Cx->CR2 = tmpreg; 
	
   	return 0;  // Success
}

//===============================================================================
//                           I2C Stop
//=============================================================================== 
void I2C_Stop(I2C_TypeDef * I2Cx){
	// Master: Generate STOP bit after the current byte has been transferred 
	I2Cx->CR2 |= I2C_CR2_STOP;								
	// Wait until STOPF flag is reset
	while( (I2Cx->ISR & I2C_ISR_STOPF) == 0 ); 
}

//===============================================================================
//                           Wait for the bus is ready
//=============================================================================== 
void I2C_WaitLineIdle(I2C_TypeDef * I2Cx){
	// Wait until I2C bus is ready
	while( (I2Cx->ISR & I2C_ISR_BUSY) == I2C_ISR_BUSY );	// If busy, wait
}

//===============================================================================
//                           I2C Send Data
//=============================================================================== 
int8_t I2C_SendData(I2C_TypeDef * I2Cx, uint8_t DeviceAddress, uint8_t *pData, uint8_t Size) {
	int i;
	
	if (Size <= 0 || pData == NULL) return -1;
	
	I2C_WaitLineIdle(I2Cx);
	
	if (I2C_Start(I2Cx, DeviceAddress, Size, WRITE_TO_SLAVE) < 0 ) return -1;

	// Send Data
	// Write the first data in DR register
	// while((I2Cx->ISR & I2C_ISR_TXE) == 0);
	// I2Cx->TXDR = pData[0] & I2C_TXDR_TXDATA;  

	for (i = 0; i < Size; i++) {
		// TXE is set by hardware when the I2C_TXDR register is empty. It is cleared when the next
		// data to be sent is written in the I2C_TXDR register.
		while( (I2Cx->ISR & I2C_ISR_TXE) == 0 ); //STUCK HERE ON 2ND ITERATION OF SENDDATA()

		// TXIS bit is set by hardware when the I2C_TXDR register is empty and the data to be
		// transmitted must be written in the I2C_TXDR register. It is cleared when the next data to be
		// sent is written in the I2C_TXDR register.
		// The TXIS flag is not set when a NACK is received.
		//while((I2Cx->ISR & I2C_ISR_TXIS) == 0 );
		I2Cx->TXDR = pData[i] & I2C_TXDR_TXDATA;  // TXE is cleared by writing to the TXDR register. Data not being sent
	}
	
	// Wait until TC flag is set 
	while((I2Cx->ISR & I2C_ISR_TC) == 0 && (I2Cx->ISR & I2C_ISR_NACKF) == 0);
	
	if( (I2Cx->ISR & I2C_ISR_NACKF) != 0 ) return -1;

	I2C_Stop(I2Cx);
	return 0;
}


//===============================================================================
//                           I2C Receive Data
//=============================================================================== 
int8_t I2C_ReceiveData(I2C_TypeDef * I2Cx, uint8_t DeviceAddress, uint8_t *pData, uint8_t Size) {
	int i;
	
	if(Size <= 0 || pData == NULL) return -1;

	I2C_WaitLineIdle(I2Cx);

	I2C_Start(I2Cx, DeviceAddress, Size, READ_FROM_SLAVE); // 0 = sending data to the slave, 1 = receiving data from the slave
						  	
	for (i = 0; i < Size; i++) {
		// Wait until RXNE flag is set 	
		while( (I2Cx->ISR & I2C_ISR_RXNE) == 0 );
		pData[i] = I2Cx->RXDR & I2C_RXDR_RXDATA;
	}
	
	// Wait until TCR flag is set 
	while((I2Cx->ISR & I2C_ISR_TC) == 0);
	
	I2C_Stop(I2Cx);
	
	return 0;
}