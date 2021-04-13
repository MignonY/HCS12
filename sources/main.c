#include "main.h"

/*****************************************************************/
/*  1. make main.c file(initialize peripharlas and initial page  */
/*****************************************************************/
char TX[16];

char can0_rx_buffer[16];

void delay(unsigned int n) 
{
    unsigned int i;
    
    while (n > 0) {
        for (i=1; i != 0; i++);     // 아무것도 하지 않으면서 시간 지연, i 오버플로우까지 for문
        n--;
    }
}

void main ()
{

  Regs.ddrb.byte = 0xFF;    //LED
  Regs.portb.byte = 0xFF;  
  Pim.ddrm.byte = 0xC0;
  Pim.ptm.byte = 0xC0;
  
  Regs.ddra.byte = 0x80;  //Buzzer
  Regs.porta.byte = 0xFF;
      
  init_LCD();
  init_atd0();
  
  int_disable();
//  init_sci0(19200,TX);
  init_can0(can0_rx_buffer);
	Crg.rtictl.byte = 0b0010111;     //리얼타임 인터럽트의 속도 결정(0.5ms 로 하시오)  보드 클럭은 16MHz
	Crg.crgint.byte |= 0b10000000;		//리얼타임 인터럽트 enable;
  init_interrupt();
  int_enable();
 
//  write_sci0("SCI OK\n");
  Select_screen();
  LCD_initial_line();
	for (;;) 
	{
     Keypad_Cheking();
	}
}
