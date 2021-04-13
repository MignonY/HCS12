#ifndef INTERRUPT_H        /*prevent duplicated includes*/
#define INTERRUPT_H

#define xint_enable() {asm andcc #0xBF;} // CCR 'X' bit set

/*Includes*/
#include "projectglobals.h"
#include "sci.h"
#include "lcd.h"

void init_interrupt(void);
void interruptH_function(void);  
void rti_handler(void);

#define KEYPAD_CHECK Pim.pth.byte

#define FIRST_ROW    0xe0   // 1Ca A��A��
#define SECOND_ROW   0x70		// 2Ca A��A��
#define THIRD_ROW    0xb0		// 3Ca A��A��
#define FOURTH_ROW   0xd0		// 4Ca A��A��

#define SW_TIME      0.5 
#endif 
