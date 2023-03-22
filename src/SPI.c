#include "SPI.h"
//#include "SysTimer.h"
#include "stm32l476xx.h"
// Note: When the data frame size is 8 bit, "SPIx->DR = byte_data;" works incorrectly. 
// It mistakenly send two bytes out because SPIx->DR has 16 bits. To solve the program,
// we should use "*((volatile uint8_t*)&SPIx->DR) = byte_data";

// LSM303C eCompass (a 3D accelerometer and 3D magnetometer module) SPI Interface: 
//   MAG_DRDY = PC2   MEMS_SCK  = PD1 (SPI1_SCK)   XL_CS  = PE0             
//   MAG_CS   = PC0   MEMS_MOSI = PD4 (SPI1_MOSI)  XL_INT = PE1       
//   MAG_INT  = PC1 
//
// L3GD20 Gyro (three-axis digital output) SPI Interface: 
//   MEMS_SCK  = PD1 (SPI1_SCK)    GYRO_CS   = PD7 (GPIO)
//   MEMS_MOSI = PD4 (SPI1_MOSI)   GYRO_INT1 = PD2
//   MEMS_MISO = PD3 (SPI1_MISO)   GYRO_INT2 = PB8

extern uint8_t Rx1_Counter;
extern uint8_t Rx2_Counter;

void SPI1_GPIO_Init(void) {
	// [TODO]
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;//enable gpio d
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	GPIOB->MODER &= ~(GPIO_MODER_MODE3 | GPIO_MODER_MODE4 | GPIO_MODER_MODE5);
	GPIOB->MODER |= (GPIO_MODER_MODE3_1 | GPIO_MODER_MODE4_1 | GPIO_MODER_MODE5_1);
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT3 | GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5);
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD3 | GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5);
	GPIOB->AFR[0] |= (GPIO_AFRL_AFSEL3_0 | GPIO_AFRL_AFSEL3_2) | (GPIO_AFRL_AFSEL4_0 | GPIO_AFRL_AFSEL4_2) | (GPIO_AFRL_AFSEL5_0 | GPIO_AFRL_AFSEL5_2);//set pins 13,14,15 to alt function 5 for spi
	//GPIOE->AFR[1] |= (GPIO_AFRH_AFSEL14_0 | GPIO_AFRH_AFSEL14_2);

	GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR3 | GPIO_OSPEEDER_OSPEEDR4 | GPIO_OSPEEDER_OSPEEDR5);//set to very high speed


}



//OLD CODE ON DISCO BOARD
void SPI_Init(void){
	// [TODO] 
	SPI1->CR1 &= ~SPI_CR1_SPE;//disable spi
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;//enable spi clock
	RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;//reset spi
	RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;
	
	SPI1->CR1 &= ~SPI_CR1_RXONLY;//set to full duplex

	SPI1->CR1 &= ~SPI_CR1_LSBFIRST;//msb first
	SPI1->CR2 |= SPI_CR2_DS;//set to 8 bit data
	SPI1->CR2 &= ~SPI_CR2_DS_3;
	SPI1->CR2 &= ~SPI_CR2_FRF;//motorola mode
	SPI1->CR1 &= ~SPI_CR1_CPOL;//0 when clock is idle
	SPI1->CR1 &= ~SPI_CR1_CPHA;//first clock transition on first capture edge
	SPI1->CR1 |= (SPI_CR1_BR_0);//baud rate prescaler set to 2 for 40MHz
	//SPI1->CR1 &= ~SPI_CR1_BR;
	SPI1->CR1 &= ~SPI_CR1_CRCEN;//crc calc disabled
	SPI1->CR1 |= SPI_CR1_MSTR;//set master config
	SPI1->CR1 |= SPI_CR1_SSM;//set software slave management
	SPI1->CR2 &= ~SPI_CR2_NSSP;//set NSS pulse management
	SPI1->CR1 |= SPI_CR1_SSI;//set internal slave select bit
	SPI1->CR2 |= SPI_CR2_FRXTH;//FIFO reception threshold 1/4th
	SPI1->CR1 |= SPI_CR1_SPE;//enable SPI

	

	
}

void SPI_Write(SPI_TypeDef * SPIx, uint8_t *txBuffer, uint8_t * rxBuffer, int size) {
	volatile uint32_t tmpreg; 
	int i = 0;
	for (i = 0; i < size; i++) {
		while( (SPIx->SR & SPI_SR_TXE ) != SPI_SR_TXE );  // Wait for TXE (Transmit buffer empty)
		*((volatile uint8_t*)&SPIx->DR) = *((uint8_t *)txBuffer);
		
			//	while((SPI1->SR & SPI_SR_RXNE ) != SPI_SR_RXNE); 
		//pData[i] = SPI1->DR;
			while( (SPIx->SR & SPI_SR_BSY) == SPI_SR_BSY ); // Wait for BSY flag cleared
	}

	
	
}

void SPI_Read(SPI_TypeDef * SPIx, uint8_t *rxBuffer, int size) {
	int i = 0;
	for (i = 0; i < size; i++) {
		while( (SPIx->SR & SPI_SR_TXE ) != SPI_SR_TXE ); // Wait for TXE (Transmit buffer empty)
		*((volatile uint8_t*)&SPIx->DR) = rxBuffer[i];	
		// The clock is controlled by master. Thus the master has to send a byte
		// data to the slave to start the clock. 
		while((SPIx->SR & SPI_SR_RXNE ) != SPI_SR_RXNE); 
		rxBuffer[i] = *((__IO uint8_t*)&SPIx->DR);
	}
	while( (SPIx->SR & SPI_SR_BSY) == SPI_SR_BSY ); // Wait for BSY flag cleared
}
//just a SW delay
void SPI_Delay(uint32_t us) {
	uint32_t i, j;
	for (i = 0; i < us*1000; i++) {
		for (j = 0; j < 18; j++) // This is an experimental value.
			(void)i;
	}
}
