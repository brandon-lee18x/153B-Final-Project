#include "lcd.h"

// Global variables
int fch;
int fcl;
int bch;
int bcl;
struct _current_font cfont;

void LCD_GPIO_init() {
	//initialize DC (PA10), RESET (PA2), CS(PA3)
	
	//Enable PortA GPIO clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	
	//Initialize DC (PA10) pin
	GPIOA->MODER &= ~(GPIO_MODER_MODE10|GPIO_MODER_MODE2|GPIO_MODER_MODE3); //set DC to output, RESET to output, CS to output
	GPIOA->MODER |= (GPIO_MODER_MODE10_0|GPIO_MODER_MODE2_0|GPIO_MODER_MODE3_0);
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT10|GPIO_OTYPER_OT2|GPIO_OTYPER_OT3);// set DC, RESET, CS to push-pull
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD10|GPIO_PUPDR_PUPD2|GPIO_PUPDR_PUPD3); // set DC and RESET to no pull-up, pull-down
	//GPIOA->PUPDR |= GPIO_PUPDR_PUPD3_0; // set CS to pull-up
	GPIOA->ODR |= GPIO_ODR_OD3;
	GPIOA->ODR |= GPIO_ODR_OD2;
	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR10|GPIO_OSPEEDER_OSPEEDR2|GPIO_OSPEEDER_OSPEEDR3);
	GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR10|GPIO_OSPEEDER_OSPEEDR2|GPIO_OSPEEDER_OSPEEDR3); // set DC, RESET, CS to high output speed
}
/*
// Write command to LCD controller
void LCD_Write_COM(uint8_t byte)
{
		GPIOA->ODR &= ~GPIO_ODR_OD3;
		// set DC to 0
    GPIOA->ODR &= ~GPIO_ODR_OD10;
    SPI_Send_Byte(SPI1, byte);
		GPIOA->ODR |= GPIO_ODR_OD3;
}


// Write 16-bit data to LCD controller
void LCD_Write_DATA16(uint8_t byteH, uint8_t byteL)
{
		GPIOA->ODR &= ~GPIO_ODR_OD3;
		// set DC to 1
    GPIOA->ODR |= GPIO_ODR_OD10;
		SPI_Send_Byte(SPI1, byteH);
		SPI_Send_Byte(SPI1, byteL);
    //Xil_Out32(SPI_DTR, byteH);
    //Xil_Out32(SPI_DTR, byteL);
		GPIOA->ODR |= GPIO_ODR_OD3;
}


// Write 8-bit data to LCD controller
void LCD_Write_DATA(uint8_t byteL)
{
		GPIOA->ODR &= ~GPIO_ODR_OD3;
		// set DC to 1
    GPIOA->ODR |= GPIO_ODR_OD10;
    SPI_Send_Byte(SPI1, byteL);
		GPIOA->ODR |= GPIO_ODR_OD3;
}


// Initialize LCD controller
void initLCD(void)
{
    int i; 

    // Reset
    LCD_Write_COM(0x01);
    for (i = 0; i < 500000; i++); //Must wait > 5ms


    LCD_Write_COM(0xCB);
    LCD_Write_DATA(0x39);
    LCD_Write_DATA(0x2C);
    LCD_Write_DATA(0x00);
    LCD_Write_DATA(0x34);
    LCD_Write_DATA(0x02);

    LCD_Write_COM(0xCF);
    LCD_Write_DATA(0x00);
    LCD_Write_DATA(0XC1);
    LCD_Write_DATA(0X30);

    LCD_Write_COM(0xE8);
    LCD_Write_DATA(0x85);
    LCD_Write_DATA(0x00);
    LCD_Write_DATA(0x78);

    LCD_Write_COM(0xEA);
    LCD_Write_DATA(0x00);
    LCD_Write_DATA(0x00);

    LCD_Write_COM(0xED);
    LCD_Write_DATA(0x64);
    LCD_Write_DATA(0x03);
    LCD_Write_DATA(0X12);
    LCD_Write_DATA(0X81);

    LCD_Write_COM(0xF7);
    LCD_Write_DATA(0x20);

    LCD_Write_COM(0xC0);   //Power control
    LCD_Write_DATA(0x23);  //VRH[5:0]

    LCD_Write_COM(0xC1);   //Power control
    LCD_Write_DATA(0x10);  //SAP[2:0];BT[3:0]

    LCD_Write_COM(0xC5);   //VCM control
    LCD_Write_DATA(0x3e);  //Contrast
    LCD_Write_DATA(0x28);

    LCD_Write_COM(0xC7);   //VCM control2
    LCD_Write_DATA(0x86);  //--

    LCD_Write_COM(0x36);   // Memory Access Control
    LCD_Write_DATA(0x48);

    LCD_Write_COM(0x3A);
    LCD_Write_DATA(0x55);

    LCD_Write_COM(0xB1);
    LCD_Write_DATA(0x00);
    LCD_Write_DATA(0x18);

    LCD_Write_COM(0xB6);   // Display Function Control
    LCD_Write_DATA(0x08);
    LCD_Write_DATA(0x82);
    LCD_Write_DATA(0x27);

    LCD_Write_COM(0x11);   //Exit Sleep
    for (i = 0; i < 100000; i++);

    LCD_Write_COM(0x29);   //Display on
    LCD_Write_COM(0x2c);

    //for (i = 0; i < 100000; i++);

    // Default color and fonts
    fch = 0xFF;
    fcl = 0xFF;
    bch = 0x00;
    bcl = 0x00;
    setFont(SmallFont);
}


// Set boundary for drawing
void setXY(int x1, int y1, int x2, int y2)
{
    LCD_Write_COM(0x2A);
    LCD_Write_DATA(x1 >> 8);
    LCD_Write_DATA(x1);
    LCD_Write_DATA(x2 >> 8);
    LCD_Write_DATA(x2);
    LCD_Write_COM(0x2B);
    LCD_Write_DATA(y1 >> 8);
    LCD_Write_DATA(y1);
    LCD_Write_DATA(y2 >> 8);
    LCD_Write_DATA(y2);
    LCD_Write_COM(0x2C);
}


// Remove boundry
void clrXY(void)
{
    setXY(0, 0, DISP_X_SIZE, DISP_Y_SIZE);
}


// Set foreground RGB color for next drawing
void setColor(uint8_t r, uint8_t g, uint8_t b)
{
    // 5-bit r, 6-bit g, 5-bit b
    fch = (r & 0x0F8) | g >> 5;
    fcl = (g & 0x1C) << 3 | b >> 3;
}


// Set background RGB color for next drawing
void setColorBg(uint8_t r, uint8_t g, uint8_t b)
{
    // 5-bit r, 6-bit g, 5-bit b
    bch = (r & 0x0F8) | g >> 5;
    bcl = (g & 0x1C) << 3 | b >> 3;
}


// Clear display
void clrScr(void)
{
    // Black screen
    setColor(0, 0, 0);

    fillRect(0, 0, DISP_X_SIZE, DISP_Y_SIZE);
}


// Draw horizontal line
void drawHLine(int x, int y, int l)
{
    int i;

    if (l < 0) {
        l = -l;
        x -= l;
    }

    setXY(x, y, x + l, y);
    for (i = 0; i < l + 1; i++) {
        LCD_Write_DATA16(fch, fcl);
    }

    clrXY();
}

void drawBackground(int x, int y, int w, int h) {
    if(w==0 || h==0)
    	return;

//	setColor(120, 180, 20);
//	setColorBg(70, 130, 10);
    setColorBg(96, 125, 139);
    setXY(x, y, x + w - 1, y + h - 1); // set bounding region to draw shapes in
    int val;

    for (int j = y; j < y+h; j++) {
        for (int i = x; i < x+w; i++) {
        	LCD_Write_DATA16(bch, bcl);
        }
    }
    clrXY();
}

// Fill a rectangular
void fillRect(int x1, int y1, int x2, int y2)
{
    int i;

    if (x1 > x2)
        swap(int, x1, x2);

    if (y1 > y2)
        swap(int, y1, y2);

    setXY(x1, y1, x2, y2);
    for (i = 0; i < (x2 - x1 + 1) * (y2 - y1 + 1); i++) {
    	LCD_Write_DATA16(fch, fcl);
    }

   clrXY();
}

void hideElement(int s) {
	if (s==MENU) {
		int start_x = DISP_X_SIZE/2 - (8+cfont.x_size*9)/2;
		int start_y = DISP_Y_SIZE/2 - (4+(cfont.y_size+4)*(sizeof(menu_items)/4))/2;
		drawBackground(start_x, start_y, 9+cfont.x_size*9, 5+(cfont.y_size+4)*(sizeof(menu_items)/4));
	} else if (s == HEADER) {
		drawBackground(0, 0, DISP_X_SIZE+1, 9+cfont.y_size);
	} else {

	}
}

void paintPageHeader(char* hd) {
	setFont(BigFont);
	setColor(207, 216, 220);
	fillRect(0,0, DISP_X_SIZE, cfont.y_size+8);
	setColor(0,0,0);
	setColorBg(207, 216, 220);
	short size = 0;
	short i = 0;
	while (hd[i++] != '\0') {
		size++;
	}
	lcdPrint(hd, DISP_X_SIZE/2 - (size*cfont.x_size)/2, 4);
}

void paintMenu(short selected) {
//	octave selection
//	Freq detect
//	histogram
	if (selected < 0) selected = 0;
	if (selected >= (sizeof(menu_items)/4)) selected = (sizeof(menu_items)/4) - 1;
	setFont(BigFont);

	int rect_center_x = DISP_X_SIZE/2 - (8+cfont.x_size*9)/2;
	int rect_center_y =  DISP_Y_SIZE/2 - (4+(cfont.y_size+4)*(sizeof(menu_items)/4))/2;

	setXY(rect_center_x, rect_center_y, rect_center_x+8+cfont.x_size*9, rect_center_y+4+(cfont.y_size+4)*(sizeof(menu_items)/4));
	setColor(255,255,255);
	fillRect(rect_center_x, rect_center_y, rect_center_x+8+cfont.x_size*9, rect_center_y+4+(cfont.y_size+4)*(sizeof(menu_items)/4));
	setColor(0, 0, 0);
	setColorBg(255, 255, 255);
	int center_x, center_y, size_word, j;
	for (int i= 0; i < sizeof(menu_items)/4; i++) {
		size_word = 0;
		j = 0;
		while (menu_items[i][j++] != '\0') {
			size_word++;
		}
		center_x = rect_center_x+(8+cfont.x_size*9)/2 - (size_word*cfont.x_size)/2;
		if (i == selected) {
			setColorBg(0, 176, 255);
			setColor(255,255,255);
			lcdPrint(menu_items[i], center_x, rect_center_y+4+(cfont.y_size+4)*i);
			setColorBg(255, 255, 255);
			setColor(0,0,0);
		} else {
			lcdPrint(menu_items[i], center_x, rect_center_y+4+(cfont.y_size+4)*i);
		}
	}
	clrXY();
}

void printWhiteBar(int x, int y, int width, int height) {
	setColor(255, 255, 255);
	fillRect(y, x, y + height, x+width);
}


void printBar(int x, int y, int width, int height) {
	setColor(50, 50, 255);
	fillRect(y, x, y + height, x+width);
}

// Select the font used by print() and printChar()
void setFont(uint8_t* font)
{
	cfont.font=font;
	cfont.x_size = font[0];
	cfont.y_size = font[1];
	cfont.offset = font[2];
	cfont.numchars = font[3];
}


// Print a character
void printChar(uint8_t c, int x, int y)
{
    uint8_t ch;
    int i, j, pixelIndex;


    setXY(x, y, x + cfont.x_size - 1,y + cfont.y_size - 1);

    pixelIndex =
            (c - cfont.offset) * (cfont.x_size >> 3) * cfont.y_size + 4;
    for(j = 0; j < (cfont.x_size >> 3) * cfont.y_size; j++) {
        ch = cfont.font[pixelIndex];
        for(i = 0; i < 8; i++) {
            if ((ch & (1 << (7 - i))) != 0)
                LCD_Write_DATA16(fch, fcl);
            else
                LCD_Write_DATA16(bch, bcl);
        }
        pixelIndex++;
    }

    clrXY();
}


// Print string
void lcdPrint(char *st, int x, int y)
{
    int i = 0;
    while(*st != '\0')
        printChar(*st++, x + cfont.x_size * i++, y);
}
*/
