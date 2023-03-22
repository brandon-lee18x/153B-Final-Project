/*****************************************************************************
* lab2a.h for Lab2A of ECE 153a at UCSB
* Date of the Last Update:  October 23,2014
*****************************************************************************/

#ifndef GUI
#define GUI

#include "qpn_port.h"
#include "MY_ILI9341.h"

enum LCDSignals {
	INCREMENT = Q_USER_SIG,
	TIMEOUT_RST
};

extern struct LCD_TypeDef lcd;

void LCD_ctor(void);

#endif  
