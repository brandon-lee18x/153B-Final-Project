#include "SPI.h"

// Note: When the data frame size is 8 bit, "SPIx->DR = byte_data;" works incorrectly. 
// It mistakenly send two bytes out because SPIx->DR has 16 bits. To solve the program,
// we should use "*((volatile uint8_t*)&SPIx->DR) = byte_data";

void SPI1_GPIO_Init(void) {
	// TODO: initialize SPI1 GPIO pins
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN; //enable port b clock
	GPIOB->MODER &= ~(GPIO_MODER_MODE3_0|GPIO_MODER_MODE4_0|GPIO_MODER_MODE5_0); //ALT FUNCTION FOR PINS 3, 4, 5
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL3|GPIO_AFRL_AFSEL4|GPIO_AFRL_AFSEL5); //clear AF bits
	GPIOB->AFR[0] |= (GPIO_AFRL_AFSEL3_2|GPIO_AFRL_AFSEL3_0|GPIO_AFRL_AFSEL4_2|GPIO_AFRL_AFSEL4_0|GPIO_AFRL_AFSEL5_2|GPIO_AFRL_AFSEL5_0); //set PB3-5 to AF5 (0101)
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT3|GPIO_OTYPER_OT4|GPIO_OTYPER_OT5); //clear OTYPER PB3-5 bits
	GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR3|GPIO_OSPEEDER_OSPEEDR4|GPIO_OSPEEDER_OSPEEDR5); //set PB3-5 to very high output speed
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD3|GPIO_PUPDR_PUPD4|GPIO_PUPDR_PUPD5); //set PB3-5 to no pull
}

void SPI1_Init(void){
	// TODO: initialize SPI1 peripheral as master
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; //enable SPI (I think this enables SPI clock but not sure)
	RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;//set RCC SPI reset bit
	RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST; //clear RCC SPI reset bit (resets SPI)
	SPI1->CR1 &= ~SPI_CR1_SPE; // disable SPI
	SPI1->CR1 &= ~SPI_CR1_RXONLY; // full-duplex
	SPI1->CR1 &= ~SPI_CR1_BIDIMODE; //two-line unidirectional data mode
	SPI1->CR1 &= ~SPI_CR1_BIDIOE; //disables output in bidirectional mode
	SPI1->CR1	&= ~SPI_CR1_LSBFIRST; // msb first
	SPI1->CR1 &= ~SPI_CR1_CRCL; // 8-bit data frame format
	SPI1->CR2 &= ~SPI_CR2_FRF; //use motorolla SPI mode
	SPI1->CR1 &= ~SPI_CR1_CPOL; //??? set to low polarity
	SPI1->CR1 &= ~SPI_CR1_CPHA; //??? set sampling edge to first transition?
	SPI1->CR1 &= ~SPI_CR1_BR; // clear baud rate control bits
	SPI1->CR1 |= (SPI_CR1_BR_0|SPI_CR1_BR_1); // set baud rate prescaler to 16
	SPI1->CR1 &= ~SPI_CR1_CRCEN; // disables hardware CRC calculation
	SPI1->CR1 |= SPI_CR1_MSTR; // set SPI1 to master mode
	SPI1->CR1 |= SPI_CR1_SSM; //enable software SSM
	SPI1->CR2 |= SPI_CR2_NSSP; //enable NSS pluse generation
	SPI1->CR1 |= SPI_CR1_SSI; //enable internal slave select (master = 1)
	SPI1->CR2 |= SPI_CR2_FRXTH; //??? set FIFO transmission to 1/4
	SPI1->CR1 |= SPI_CR1_SPE; // re-enable SPI
}

void SPI2_GPIO_Init(void) {
	// TODO: initialize SPI2 GPIO pins
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN; //enable port b clock
	GPIOB->MODER &= ~(GPIO_MODER_MODE13_0|GPIO_MODER_MODE14_0|GPIO_MODER_MODE15_0); //ALT FUNCTION FOR PINS 13, 14, 15
	GPIOB->AFR[1] &= ~(GPIO_AFRH_AFSEL13|GPIO_AFRH_AFSEL14|GPIO_AFRH_AFSEL15); //clear AF bits
	GPIOB->AFR[1] |= (GPIO_AFRH_AFSEL13_2|GPIO_AFRH_AFSEL13_0|GPIO_AFRH_AFSEL14_2|GPIO_AFRH_AFSEL14_0|GPIO_AFRH_AFSEL15_2|GPIO_AFRH_AFSEL15_0); //set PB13-15 to AF5 (0101)
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT13|GPIO_OTYPER_OT14|GPIO_OTYPER_OT15); //clear OTYPER PB13-15 bits
	GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR13|GPIO_OSPEEDER_OSPEEDR14|GPIO_OSPEEDER_OSPEEDR15); //set PB13-15 to very high output speed
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD13|GPIO_PUPDR_PUPD14|GPIO_PUPDR_PUPD15); //set PB13-5 to no pull
}

void SPI2_Init(void){
	// TODO: initialize SPI2 peripheral as slave
	RCC->APB1ENR1 |= RCC_APB1ENR1_SPI2EN; //enable SPI (I think this enables SPI clock but not sure)
	RCC->APB1RSTR1 |= RCC_APB1RSTR1_SPI2RST;//set RCC SPI reset bit
	RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_SPI2RST; //clear RCC SPI reset bit (resets SPI)
	SPI2->CR1 &= ~SPI_CR1_SPE; // disable SPI
	SPI2->CR1 &= ~SPI_CR1_RXONLY; // full-duplex
	SPI2->CR1 &= ~SPI_CR1_BIDIMODE; //two-line unidirectional data mode
	SPI2->CR1 &= ~SPI_CR1_BIDIOE; //disables output in bidirectional mode
	SPI2->CR1	&= ~SPI_CR1_LSBFIRST; // msb first
	SPI2->CR1 &= ~SPI_CR1_CRCL; // 8-bit data frame format
	SPI2->CR2 &= ~SPI_CR2_FRF; //use motorolla SPI mode
	SPI2->CR1 &= ~SPI_CR1_CPOL; // set to low polarity
	SPI2->CR1 &= ~SPI_CR1_CPHA; // set sampling edge to first transition?
	SPI2->CR1 &= ~SPI_CR1_BR; // clear baud rate control bits
	SPI2->CR1 |= (SPI_CR1_BR_0|SPI_CR1_BR_1); // set baud rate prescaler to 16
	SPI2->CR1 &= ~SPI_CR1_CPHA; //set sampling edge to first transition?
	SPI2->CR1 &= ~SPI_CR1_CRCEN; // disables hardware CRC calculation
	SPI2->CR1 &= ~SPI_CR1_MSTR; // set SPI2 to slave mode
	SPI2->CR1 |= SPI_CR1_SSM; //enable software SSM
	SPI2->CR2 |= SPI_CR2_NSSP; //enable NSS pluse generation
	SPI2->CR1 &= ~SPI_CR1_SSI; //enable internal slave select (slave = 0)
	SPI2->CR2 |= SPI_CR2_FRXTH; //??? set FIFO transmission to 1/4
	SPI2->CR1 |= SPI_CR1_SPE; // re-enable SPI
}
 
void SPI_Send_Byte(SPI_TypeDef* SPIx, uint8_t write_data) {
	// TODO: send data from SPI1
	while ((SPIx->SR & SPI_SR_TXE) != SPI_SR_TXE); //while transmit buffer empty flag is 0
	*((volatile uint8_t*)&SPIx->DR) = write_data; //write data to DR register
	while ((SPIx->SR & SPI_SR_BSY) == SPI_SR_BSY); //while busy bit is 1
}

void SPI_Receive_Byte(SPI_TypeDef* SPIx, uint8_t* read_data) {
	// TODO: receive data from SPI2
	while (!(SPIx->SR & SPI_SR_RXNE)); //while rx buffer is empty (0)
	*read_data = *((volatile uint8_t*)(&(SPIx->DR))) & SPI_DR_DR; 
}

