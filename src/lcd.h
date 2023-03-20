#ifndef LCD_H_
#define LCD_H_

#include "stm32l476xx.h"


#define cbi(reg, bitmask)       Xil_Out32(reg, Xil_In32(reg) & ~(u32)bitmask)
#define sbi(reg, bitmask)       Xil_Out32(reg, Xil_In32(reg) |= (u32)bitmask)
#define swap(type, i, j)        {type t = i; i = j; j = t;}

#define DISP_X_SIZE     239
#define DISP_Y_SIZE     329


struct _current_font
{
    uint8_t* font;
    uint8_t x_size;
    uint8_t y_size;
    uint8_t offset;
    uint8_t numchars;
};

enum IdleElement {
	MENU,
	HEADER
};


static char* menu_items[4] = {"tuner", "settings", "octave", "histogram"};

extern int fch; // Foreground color upper byte
extern int fcl; // Foreground color lower byte
extern int bch; // Background color upper byte
extern int bcl; // Background color lower byte

extern struct _current_font cfont;
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];

uint32_t LCD_Read(char VL);
void LCD_Write_COM(char VL);
void LCD_Write_DATA(char VL);
void LCD_Write_DATA16(char VH, char VL);
//void LCD_Write_DATA_(char VH, char VL);

void initLCD(void);
void setXY(int x1, int y1, int x2, int y2);
void setColor(uint8_t r, uint8_t g, uint8_t b);
void setColorBg(uint8_t r, uint8_t g, uint8_t b);
void clrXY(void);
void clrScr(void);

void drawHLine(int x, int y, int l);
void fillRect(int x1, int y1, int x2, int y2);
void drawBackground(int x, int y, int w, int h);

void setFont(uint8_t* font);
void printChar(uint8_t c, int x, int y);
void lcdPrint(char *st, int x, int y);
void hideElement(int s);
void paintMenu(short selected);
void paintPageHeader(char* hd);
void printBar(int x, int y, int width, int height);

#endif /* LCD_H_ */
