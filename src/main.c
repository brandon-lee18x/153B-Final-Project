#include "stm32l476xx.h"

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

int main(void){
	
}
