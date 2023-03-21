#define AO_LAB2A

#include "gui.h"

typedef struct LCD_TypeDef {               //LCD State machine
	QActive super;
	int a;
} Lcd;

/* Setup state machines */
/**********************************************************************/
static QState LCD_initial (Lcd* me);
static QState LCD_on      (Lcd* me);

/**********************************************************************/


Lcd lcd;


void LCD_ctor(void)  {
	Lcd *me = &lcd;
	QActive_ctor(&me->super, (QStateHandler)&LCD_initial);
}


QState LCD_initial(Lcd *me) {
//	xil_printf("\n\rInitialization");
    return Q_TRAN(&LCD_on);
}


QState LCD_on(Lcd *me) {
	switch (Q_SIG(me)) {
		case Q_ENTRY_SIG: {
			return Q_HANDLED();
			}
		
		case Q_INIT_SIG: {
			return Q_HANDLED();
			}
		}
	
	return Q_SUPER(&QHsm_top);
}
