#ifndef __STM32L476R_NUCLEO_SPI_H
#define __STM32L476R_NUCLEO_SPI_H

#include "stm32l476xx.h"

void SPI1_GPIO_Init(void);
void SPI2_GPIO_Init(void);
void SPI1_Init(void);
void SPI2_Init(void);
void SPI_Send_Byte(SPI_TypeDef* SPIx, uint8_t write_data);
void SPI_Receive_Byte(SPI_TypeDef* SPIx, uint8_t* read_data);

#endif 
