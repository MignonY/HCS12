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
        for (i=1; i != 0; i++);     // �ƹ��͵� ���� �����鼭 �ð� ����, i �����÷ο���� for��
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
	Crg.rtictl.byte = 0b0010111;     //����Ÿ�� ���ͷ�Ʈ�� �ӵ� ����(0.5ms �� �Ͻÿ�)  ���� Ŭ���� 16MHz
	Crg.crgint.byte |= 0b10000000;		//����Ÿ�� ���ͷ�Ʈ enable;
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
