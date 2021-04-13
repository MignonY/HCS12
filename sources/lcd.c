#include "main.h"
#include<stdio.h>

/*************************************************/
/* 3. make LCD_initial_line, LCD_blink function  */
/*   use only LCD_... function                   */
/*************************************************/

unsigned char LCD_saver[128][8];
void erase();
void Square(int x, int y, int width, int height, int fill);
void Erase_Square(int x, int y, int width, int height, int fill);
void ADC_value();
void Acceptance_screen();
void Mask_screen();

int LED_cursor;
int LED[10];
int LED_mask;
int LED_mask2;
int blink_state;
int square[8]={1, 2, 4, 8, 16, 32, 64, 128};

int buzzer;
int buzzer_timer;
char buzzer_str[10]="OFF";

int ADC_on;
int ADC_size;
int level;
int prev_level;

int CAN_cursor;
int CAN_toggle[5]={0, 0, 1, 0, 0};


int acceptance[12];
int acceptance_cursor;

int mask[12];
int mask_cursor;

void delay_1(int t) {
    int d;
    for(d = 0 ; d < t; d++);
}

void Select_screen() 
{
  LCD_clear();
  
  LCD_write_str("Select menu", 31, 3);  
  LCD_write_str("1. LED Control", 5, 14);
  LCD_write_str("2. Buzzer Control", 5, 24);
  LCD_write_str("3. ADC Control", 5, 34);
  LCD_write_str("4. CAN Control", 5, 44);
      
}

void Up_cursor() 
{
  if(LED_cursor<9)
    ++LED_cursor;    
}

void Down_cursor() 
{
  if(LED_cursor)
    --LED_cursor;
}

void LED_blink(int all_blink, int screen_state, int lock) 
{
  int i;
  if(all_blink) 
  {
    if(blink_state) 
    {
      Regs.portb.byte=0; 
      Pim.ptm.byte=0;
      
      if(screen_state==1 && !lock)
        for(i=0; i<10; i++)             
          Square(5+i*12, 20, 9, 30, 1);      
    } 
    
    else 
    {
      Regs.portb.byte=0xFF; 
      Pim.ptm.byte=0xFF;
      
      if(screen_state==1 && !lock)
        for(i=0; i<10; i++)             
          Square(5+i*12, 20, 9, 30, 0);    
    }
  } 
  else 
  {
    if(blink_state)
    { 
      Regs.portb.byte=~(LED_mask|LED_mask2);

      if(LED[8]==-1)
        Pim.ptm.byte&=0b10111111;
      if(LED[9]==-1)
        Pim.ptm.byte&=0b01111111;
      
      if(screen_state==1 && !lock)    
        for(i=0; i<10; i++)
          if(LED[i]==-1)             
            Square(5+i*12, 20, 9, 30, 1);      
    }
     
    else 
    {
      Regs.portb.byte=~(LED_mask);
      
      if(LED[8]==-1 || !LED[8])
        Pim.ptm.byte|=0b01000000;
      if(LED[9]==-1 || !LED[9])
        Pim.ptm.byte|=0b10000000;
    
      if(screen_state==1 && !lock)
       for(i=0; i<10; i++)
        if(LED[i]==-1)             
         Square(5+i*12, 20, 9, 30, 0);         
    }
  }
  
  blink_state=!blink_state;    
}

void LED_toggle1() 
{
  if(LED[LED_cursor]==-1) 
  {
    LED[LED_cursor]=0;
    
    if(LED_cursor<8)
      LED_mask2&=(~square[LED_cursor]);    
  } 
  
  else 
  {
    LED[LED_cursor]=-1;
    
    if(LED_cursor<8)
      LED_mask2|=square[LED_cursor];
  }
  Square(5+LED_cursor*12, 20, 10, 31, LED[LED_cursor]);  
}

void LED_toggle() 
{
  int position=4+LED_cursor*12;
  int y=19;
  int width=12;
  int height=33;

  LED[LED_cursor]=!LED[LED_cursor];
 
  
  
  if(LED[LED_cursor]) 
  {    
    Square(position+1, y+1, width-2, height-2, 1);
    
    if(LED_cursor<8)
      LED_mask |= square[LED_cursor];
    else if(LED_cursor==8)
      Pim.ptm.byte&=0b10111111;
    else if(LED_cursor==9)
      Pim.ptm.byte&=0b01111111;        
  }
  
  else 
  {    
    Square(position+1, y+1, width-2, height-2, 0);
    
    if(LED_cursor<8)
      LED_mask &= (~square[LED_cursor]);
    else if(LED_cursor==8)
      Pim.ptm.byte|=0b01000000;
    else if(LED_cursor==9)
      Pim.ptm.byte|=0b10000000;    
  }
  
  Regs.portb.byte=(~LED_mask);
}

void LED_make_cursor() 
{
  int position=4+LED_cursor*12;
  int y=19;
  int width=12;
  int height=33;
  
  Square(position, y, width, height, 2);
  Square(position+1, y+1, width-2, height-2, LED[LED_cursor]);    
}

void LED_erase_cursor() 
{
  int position=4+LED_cursor*12;
  int y=19;
  int width=12;
  int height=33;

  
  Erase_Square(position, y, width, height, 0);
  
  if(LED[LED_cursor])
    Square(position+1, y+1, width-2, height-2, 1);
  
  else
    Square(position+1, y+1, width-2, height-2, 0);    
}
  
  


void LED_screen() 
{
  int position=0;
  int i;

  LCD_clear();  
  LCD_write_str("1. LED Control", 26, 3);



  position=4+LED_cursor*12;
           
  for(i=0; i<10; i++) 
  {        
    Square(5+i*12, 20, 9, 30, LED[i]);
  }       
  
  LED_make_cursor();     
}

void Buzzer_screen() 
{
  char buzzer_sec[2]="";
  LCD_clear();
  
  LCD_write_str("2. Buzzer Control", 15, 3);
  LCD_write_str(buzzer_str, 70, 30);
  
  if(buzzer==-1)
    Square(40, 25, 15, 15, 1);
  else 
  {   
    Square(40, 25, 15, 15, 0);    
    if(buzzer) 
    {
      sprintf(buzzer_sec, "%d", buzzer); 
      LCD_write_str(buzzer_sec, 45, 30);
    }
  }
//  LCD_write_str("OFF", 70, 30);  
}

void Buzzer_toggle() 
{
  if(buzzer!=-1) 
  {
    Square(40, 25, 15, 15, 1);
    
    sprintf(buzzer_str, "ON");    
    LCD_write_str("          ", 70, 30);
    LCD_write_str(buzzer_str, 70, 30);

    buzzer=-1;
    Regs.porta.byte=0x7F;    
  } 
  else 
  {
    Square(40, 25, 15, 15, 0);
    
    sprintf(buzzer_str, "OFF");    
    LCD_write_str("          ", 70, 30);
    LCD_write_str(buzzer_str, 70, 30);
    
    buzzer=0;
    Regs.porta.byte=0xFF;     
  }   
}

void Buzzer_off() 
{
  Square(40, 25, 15, 15, 0);
    
  sprintf(buzzer_str, "OFF");    
  LCD_write_str("          ", 70, 30);
  LCD_write_str(buzzer_str, 70, 30);

  buzzer=0;
  buzzer_timer=0;
  Regs.porta.byte=0xFF;    
}

void Buzzer_timer() 
{
  if(buzzer<=0)
    return;
  
  ++buzzer_timer;
  if(buzzer_timer==buzzer) 
  {
    buzzer_timer=0;
    Regs.porta.byte^=0b10000000;
  }
}

void Buzzer_second(int s) 
{
  char buzzer_sec[2]="";
  
  Square(40, 25, 15, 15, 0);
  buzzer=s;
  buzzer_timer=0;  
  sprintf(buzzer_sec, "%d", s);
  LCD_write_str(" ", 45, 30);
  LCD_write_str(buzzer_sec, 45, 30);
  
  sprintf(buzzer_str, "%d second", s);
  LCD_write_str("          ", 70, 30);
  LCD_write_str(buzzer_str, 70, 30);    
}

void ADC_screen() 
{
  LCD_clear();
  
  LCD_write_str("3. ADC Control", 26, 3);

  Square(45, 15, 8, 8, ADC_on);
  LCD_write_str("enable", 6, 15);
  LCD_write_str("Value:", 6, 50);
  ADC_value();  
}

void ADC_toggle() 
{
  int i;
  ADC_on=!ADC_on;
  Square(45, 15, 8, 8, ADC_on);
  
  if(ADC_on)
    for(i=0; i<level; ++i)
      Square(45+i*5, 50, 3, 8, 1);

  else
    for(i=0; i<level; ++i)
      Erase_Square(45+i*5, 50, 3, 8, 1);    
}

void ADC_value() 
{
  int i;
  
  if(!ADC_on)
    return;
  
  if(prev_level<level)  
    for(i=prev_level; i<level; ++i) 
      Square(45+i*5, 50, 3, 8, 1);
  
  else if(prev_level>level)
    for(i=prev_level-1; i>=level; --i)
      Erase_Square(45+i*5, 50, 3, 8, 1);
      
  else
    for(i=0; i<level; ++i)
      Square(45+i*5, 50, 3, 8, 1);         
}
                       
void ADC_getatd(int screen_state, int lock) 
{
  double temp;
  if(!ADC_on || screen_state!=3)
    return;
  
  temp=(double)get_atd0(0)/90;
             
  if(temp<=6)
    level=1;
  else if(temp<=6.1)
    level=2;
  else if(temp<=6.2)
    level=3;
  else if(temp<=6.3)
    level=4;
  else if(temp<=6.4)
    level=5;
  else if(temp<=6.5)
    level=6;
  else if(temp<=6.6)
    level=7;
  else if(temp<=6.7)
    level=8;
  else if(temp<=6.8)
    level=9;
  else
    level=10;
  
  if(prev_level!=level && !lock) 
    ADC_value();
      
  prev_level=level;
}



void CAN_screen() 
{
  LCD_clear();
  
  LCD_write_str("4. CAN Control", 26, 3);        
  LCD_write_str("acceptance", 5, 14);
  LCD_write_str("mask", 5, 24);
  LCD_write_str("echo mode", 5, 34);
  LCD_write_str("A->a a->A", 5, 44);
  LCD_write_str("enable", 5, 54);
     
  Square(80, 34, 8, 8, CAN_toggle[2]);
  Square(80, 44, 8, 8, CAN_toggle[3]);
  Square(80, 54, 8, 8, CAN_toggle[4]);
  
  if(CAN_cursor<=1)
    Square(2, 13+CAN_cursor*10, 65, 9, 2);
  else
    Square(79, 13+CAN_cursor*10, 10, 10, 2);
}

void CAN_downcursor(int *screen_state) 
{
  if(CAN_cursor==4)
    return;
  
  if(CAN_cursor<=1)
  {
    if(*screen_state==4)
      Erase_Square(2, 13+CAN_cursor*10, 65, 9, 0);
    
    else if(*screen_state==5 || *screen_state==6) 
    {
      *screen_state=4; 
      CAN_screen();
      return;
    }
  }
  else 
  {
    Erase_Square(79, 13+CAN_cursor*10, 10, 10, 0);
    Square(80, 14+CAN_cursor*10, 8, 8, CAN_toggle[CAN_cursor]); 
  }
  
  ++CAN_cursor;
  
  if(CAN_cursor<=1)
    Square(2, 13+CAN_cursor*10, 65, 9, 2);
  
  else 
    Square(79, 13+CAN_cursor*10, 10, 10, CAN_toggle[CAN_cursor]);    
}

void CAN_upcursor(int *screen_state) 
{
  if(CAN_cursor==0)
    return;
   
  if(CAN_cursor<=1) 
  {
    if(*screen_state==4)
      Erase_Square(2, 13+CAN_cursor*10, 65, 9, 0);
  
    else if(*screen_state==5 || *screen_state==6) 
    {
      *screen_state=4;
      CAN_screen();
      return;
    }
  
  }
  else 
  {
    Erase_Square(79, 13+CAN_cursor*10, 10, 10, 0);
    Square(80, 14+CAN_cursor*10, 8, 8, CAN_toggle[CAN_cursor]); 
  }
  
  --CAN_cursor;
  
  if(CAN_cursor<=1)
    Square(2, 13+CAN_cursor*10, 65, 9, 2);
  
  else 
    Square(79, 13+CAN_cursor*10, 10, 10, CAN_toggle[CAN_cursor]);    
}

void CAN_check(int *screen_state) 
{
  if(!CAN_cursor) 
  {
    if(*screen_state==4) 
    {     
      Acceptance_screen();
      *screen_state=5;
    } 
  } 
  else if(CAN_cursor==1) 
  {
    if(*screen_state==4) 
    {
      Mask_screen();
      *screen_state=6;        
    } 
  }
  
  else if(CAN_cursor>1) 
  { 
    
    if(CAN_cursor==2 || CAN_cursor==3) 
    {
      CAN_toggle[2]=!CAN_toggle[2];
      CAN_toggle[3]=!CAN_toggle[3]; 
      Square(80, 34, 8, 8, CAN_toggle[2]);
      Square(80, 44, 8, 8, CAN_toggle[3]);    
    }
    
    else if(CAN_cursor==4) 
    {
      CAN_toggle[4]=!CAN_toggle[4];
      if(CAN_toggle[4])  
        write_can0(0, "CAN OK!", 7);
      Square(80, 54, 8, 8, CAN_toggle[4]);
    }
  }
  
}

void Acceptance_screen() 
{
  int i=0;
  char bit[15]="";
  LCD_clear();
  
  LCD_write_str("4-1. acceptance", 26, 3);
  for(i=0; i<11; ++i)
    Square(18+8*i, 18, 8, 8, acceptance[i]);
  
  Square(18+8*acceptance_cursor, 17, 8, 10, 2);  
  
  LCD_write_str("MSB", 10, 33);
  LCD_write_str("LSB", 90, 33);
  
  sprintf(bit, "0b%d%d%d%d%d%d%d%d%d%d%d", acceptance[0], acceptance[1]
  , acceptance[2], acceptance[3], acceptance[4], acceptance[5]
  , acceptance[6], acceptance[7], acceptance[8], acceptance[9]
  , acceptance[10]);
  
  LCD_write_str(bit, 25, 50);
}

void Acceptance_right() 
{

  if(acceptance_cursor==10)
    return;
  
  Erase_Square(18+8*acceptance_cursor, 17, 8, 10, 1);
  Square(18+8*acceptance_cursor, 18, 8, 8, acceptance[acceptance_cursor]);
  
  ++acceptance_cursor;
  Square(18+8*acceptance_cursor, 17, 8, 10, acceptance[acceptance_cursor]);   
}

void Acceptance_left() 
{

  if(acceptance_cursor==0)
    return;
  
  Erase_Square(18+8*acceptance_cursor, 17, 8, 10, 1);
  Square(18+8*acceptance_cursor, 18, 8, 8, acceptance[acceptance_cursor]);
  
  --acceptance_cursor;
  Square(18+8*acceptance_cursor, 17, 8, 10, acceptance[acceptance_cursor]);   
}

void Acceptance_toggle() 
{
  char bit[15];
  int temp;
  int i;
  
  acceptance[acceptance_cursor]=!acceptance[acceptance_cursor];   
  
  
  	/* place CAN in init mode */
	while ( (Can0.canctl1.byte & 0x1) == 0) {
		Can0.canctl0.byte |= 0x1;
	}
  
  
  if(acceptance_cursor<8) 
  {
    if(acceptance[acceptance_cursor])
      Can0.canid[0].canidar.b[0] |= square[7-acceptance_cursor];
    else
      Can0.canid[0].canidar.b[0] &= ~(square[7-acceptance_cursor]);
  }                            
  else 
  {
    if(acceptance[acceptance_cursor])
      Can0.canid[0].canidar.b[1] |= square[15-acceptance_cursor];
    else
      Can0.canid[0].canidar.b[1] &= ~(square[15-acceptance_cursor]);      
  }
  
  /* Can out of init mode */
	Can0.canctl0.byte &= 0xfe;
	Can0.canrier.byte = 0b00000001;
  
  Square(18+8*acceptance_cursor, 18, 8, 8, acceptance[acceptance_cursor]);
  
  sprintf(bit, "0b%d%d%d%d%d%d%d%d%d%d%d", acceptance[0], acceptance[1]
  , acceptance[2], acceptance[3], acceptance[4], acceptance[5]
  , acceptance[6], acceptance[7], acceptance[8], acceptance[9]
  , acceptance[10]);
  
  LCD_write_str(bit, 25, 50);
/*  
  temp=Can0.canid[0].canidar.b[0];
  for(i=7; i>=0; --i) 
  {  
    bit[i]=temp%2+'0';
    temp/=2;     
  }
  
  
  temp=Can0.canid[0].canidar.b[1]/32;
  bit[10]=temp%2+'0';
  temp/=2;
  bit[9]=temp%2+'0';
  temp/=2;
  bit[8]=temp%2+'0';
  temp/=2;
  bit[11]=0;
  
  LCD_write_str(bit, 25, 40);*/  
}


void Mask_screen() 
{
  int i=0;
  char bit[15]="";
  LCD_clear();
  
  LCD_write_str("4-2. mask", 26, 3);
  for(i=0; i<11; ++i)
    Square(18+8*i, 18, 8, 8, mask[i]);
  
  Square(18+8*mask_cursor, 17, 8, 10, 2);  
  
  LCD_write_str("MSB", 10, 33);
  LCD_write_str("LSB", 90, 33);
  
  sprintf(bit, "0b%d%d%d%d%d%d%d%d%d%d%d", mask[0], mask[1]
  , mask[2], mask[3], mask[4], mask[5], mask[6], mask[7]
  , mask[8], mask[9], mask[10]);
  
  LCD_write_str(bit, 25, 50);
}

void Mask_right() 
{

  if(mask_cursor==10)
    return;
  
  Erase_Square(18+8*mask_cursor, 17, 8, 10, 1);
  Square(18+8*mask_cursor, 18, 8, 8, mask[mask_cursor]);
  
  ++mask_cursor;
  Square(18+8*mask_cursor, 17, 8, 10, mask[mask_cursor]);   
}

void Mask_left() 
{

  if(mask_cursor==0)
    return;
  
  Erase_Square(18+8*mask_cursor, 17, 8, 10, 1);
  Square(18+8*mask_cursor, 18, 8, 8, mask[mask_cursor]);
  
  --mask_cursor;
  Square(18+8*mask_cursor, 17, 8, 10, mask[mask_cursor]);   
}

void Mask_toggle() 
{
  char bit[15];
  int temp;
  int i;
  
  mask[mask_cursor]=!mask[mask_cursor];
  
    	/* place CAN in init mode */
	while ( (Can0.canctl1.byte & 0x1) == 0) {
		Can0.canctl0.byte |= 0x1;
	}
  
  
  if(mask_cursor<8) 
  {
    if(mask[mask_cursor])
      Can0.canid[0].canidmr.b[0] |= square[7-mask_cursor];
    else
      Can0.canid[0].canidmr.b[0] &= ~(square[7-mask_cursor]);
  } 
  else 
  {
    if(mask[mask_cursor])
      Can0.canid[0].canidmr.b[1] |= square[15-mask_cursor];
    else
      Can0.canid[0].canidmr.b[1] &= ~(square[15-mask_cursor]);      
  }
  
    /* Can out of init mode */
	Can0.canctl0.byte &= 0xfe;
	Can0.canrier.byte = 0b00000001;
     
  Square(18+8*mask_cursor, 18, 8, 8, mask[mask_cursor]);
  
  sprintf(bit, "0b%d%d%d%d%d%d%d%d%d%d%d", mask[0], mask[1]
  , mask[2], mask[3], mask[4], mask[5]
  , mask[6], mask[7], mask[8], mask[9], mask[10]);
  
  LCD_write_str(bit, 25, 50);
/*  
  temp=Can0.canid[0].canidmr.b[0];
  for(i=7; i>=0; --i) 
  {  
    bit[i]=temp%2+'0';
    temp/=2;     
  }
  
  temp=Can0.canid[0].canidmr.b[1]/32;
  bit[10]=temp%2+'0';
  temp/=2;
  bit[9]=temp%2+'0';
  temp/=2;
  bit[8]=temp%2+'0';
  temp/=2;
  bit[11]=0;
  
  LCD_write_str(bit, 25, 40);*/  
}

void CAN_communication(int id, char *str, int length) 
{
  int i;
  
  if(!CAN_toggle[4])
    return;
  
  if(CAN_toggle[3]) 
  {
    for(i=0; i<length; ++i) 
    {
      if(str[i]>='A' && str[i]<='Z')
        str[i]+=('a'-'A');
      
      else if(str[i]>='a' && str[i]<='z')
        str[i]-=('a'-'A');      
    }    
  }
    
  write_can0(id, str , length);
}



void Erase_Square(int x, int y, int width, int height, int fill) 
{
  int i, j;
  for(i=x; i<x+width; ++i) 
  {
    LCD_write_hole(i, y);
    LCD_write_hole(i, y+height-1);    
  }
  
  for(j=y; j<y+height; ++j) 
  {
    LCD_write_hole(x, j);
    LCD_write_hole(x+width-1, j); 
  }
  
  if(fill==1) 
  {
    for(j=y+1; j<y+height-1; ++j) 
    {
      
      for(i=x+1; i<x+width-1; ++i) 
      {
         LCD_write_hole(i, j);    
      }
    }
  }
     
}

void Square(int x, int y, int width, int height, int fill) 
{
  int i, j;
  for(i=x; i<x+width; ++i) 
  {
    LCD_write_dot(i, y);
    LCD_write_dot(i, y+height-1);    
  }
  
  for(j=y; j<y+height; ++j) 
  {
    LCD_write_dot(x, j);
    LCD_write_dot(x+width-1, j); 
  }
  
  if(fill==1) 
  {
    for(j=y+1; j<y+height-1; ++j) 
    {
      
      for(i=x+1; i<x+width-1; ++i) 
      {
         LCD_write_dot(i, j);    
      }
    }
  } 
  else if(fill==-1) 
  {
    for(j=y+4; j<y+height-4; ++j)
      LCD_write_dot(x+width/2, j);         
  }
  else if(!fill) 
  {
    for(j=y+1; j<y+height-1; ++j) 
    {
      
      for(i=x+1; i<x+width-1; ++i) 
      {
         LCD_write_hole(i, j);    
      }
    }
  }        
}

void LCD_initial_line(void) //LCD의 테두리를 그려주는 함수.
{
  int i;
  for(i=2; i<128; ++i)
  {
    LCD_write_dot(i, 0);
    LCD_write_dot(i, 1);
    
    LCD_write_dot(i, 11);
    LCD_write_dot(i, 12);

    LCD_write_dot(i, 62);
    LCD_write_dot(i, 63);
  }
    
  for(i=0; i<64; ++i)
  {  	
    LCD_write_dot(0, i);
//    LCD_write_dot(1, i);	
  	
    LCD_write_dot(125, i);
//    LCD_write_dot(126, i);
  }
  
}

void LCD_clear(void)
{
    int a,b,c;
    for(a = 0; a < 2; a++)
    {
      if(a == 0)
      {Pim.ptp.byte = 0x40;}
      else
      {Pim.ptp.byte = 0x80;}
      for (b = 0; b < 8; b++)
      {
          for(c = 0; c < 64; c++)
          {   delay_1(5);              
              Set_addressYX(c,b);
              write_1(0x00);}  
       }
  } 
  
  for(b=0;b<128;b++)
  {
    for(c=0;c<8;c++)
    {
      LCD_saver[b][c]=0;
    }
  }
}



void init_LCD(void)  
{   
    int a, b, c;

    Pim.ddrt.byte = 0xFF;
    Pim.ddrp.byte = 0xFF;
    Pim.ppst.byte = 0x00;
    Pim.ppsp.byte = 0x00;
    Pim.ptp.byte = 0x20;
    Display_ON(0x3F);
     
}


void LCD_write_dot(int x,int y)
{
  int block, byte;
  
  block=y/8;
  byte=y%8;
  
  if(x<64)
  {
    Pim.ptp.byte=0x40;
    LCD_saver[x][block]|=(0x01<<byte);
    delay_1(5); 
    Set_addressYX(x,block);
    write_1(LCD_saver[x][block]); 
  }
  else
 {
   Pim.ptp.byte=0x80;
   LCD_saver[x][block]|=(0x01<<byte);
   delay_1(5);
   Set_addressYX(x-64,block);
   write_1(LCD_saver[x][block]);
  } 
  delay_1(5);
}

void LCD_write_hole(int x,int y)
{
  int block, byte;
  
  block=y/8;
  byte=y%8;
  
  if(x<64)
  {
    Pim.ptp.byte=0x40;
    LCD_saver[x][block]&=~(0x01<<byte);
    delay_1(5); 
    Set_addressYX(x,block);
    write_1(LCD_saver[x][block]); 
  }
  else
 {
   Pim.ptp.byte=0x80;
   LCD_saver[x][block]&=~(0x01<<byte);
   delay_1(5);
   Set_addressYX(x-64,block);
   write_1(LCD_saver[x][block]);
  } 
  delay_1(5);
}

void LCD_blink(int x,char ch,int state) // 커서의 깜박임을 표시해 주는 함수
{
  int i, j;
  x*=6;
  x+=4;
  if(state==1)
  {
    if(ch) 
    {
       LCD_write_char(ch, x, 52);
    } 
    else
    {
      
      for(i=x; i<x+5; i++)
       for(j=52; j<59; j++)
         LCD_write_hole(i, j);
    }
  }
  else 
  { 
    for(i=x; i<x+5; i++)
       for(j=52; j<59; j++)
         LCD_write_dot(i, j);    
  }
}

void LCD_write_char(char ch,int x, int y)
{
  char dot_t[5],i,j;
  
  translation(ch,dot_t);
  
  for(j=0;j<5;j++)
  {
    for(i=0;i<8;i++)
    {
      LCD_write_hole(x+j,y+i); 
    } 
  }
  
  for(j=0;j<5;j++)
  {
    for(i=0;i<8;i++)
    {
      if(dot_t[j]&(0x01<<i))
      {
        LCD_write_dot(x+j,y+i); 
        LCD_write_hole(x+j+1,y+i);
      } 
    } 
  }
}

void LCD_write_str(char *str,int x,int y)
{
  int i;
  
  for(i=0;i<strlen(str);i++)
  {
    LCD_write_char(str[i],x+i*6, y);
  }
}



void write_1(BYTE tport)   
{
    Pim.ptp.byte = (Pim.ptp.byte & 0xC0)|0x30;
    Pim.ptt.byte = tport;
    delay_1(5);
    Pim.ptp.bit.ptp5 = 0;
    delay_1(5);
    Pim.ptp.bit.ptp5 = 1;
    delay_1(5);
}

void Display_ON(BYTE tport) 
{
    Pim.ptp.byte = (Pim.ptp.byte | 0xC0)|0x20; 
    Pim.ptt.byte = tport;
    delay_1(5);
    Pim.ptp.bit.ptp5 = 0;
    delay_1(5);
    Pim.ptp.bit.ptp5 = 1;
    delay_1(5);
}

void Set_addressYX(int tporty, int tportx) 
{
    Pim.ptp.byte = (Pim.ptp.byte & 0xC0)|0x20;
    Pim.ptt.byte = 0x40;
    Pim.ptt.byte |= tporty;
    delay_1(5);
    Pim.ptp.bit.ptp5 = 0;
    delay_1(5);
    Pim.ptp.bit.ptp5 = 1;
    delay_1(5);

    Pim.ptp.byte = (Pim.ptp.byte & 0xC0)|0x20;
    Pim.ptt.byte = 0xB8;
    Pim.ptt.byte |= tportx;
    delay_1(5);
    Pim.ptp.bit.ptp5 = 0;
    delay_1(5);
    Pim.ptp.bit.ptp5 = 1;
    delay_1(5);
}


void translation(char chch, char *dot_t) 
{
    if(chch==45)
    {dot_t[0] = 0x08;dot_t[1] = 0x08;dot_t[2] = 0x08;dot_t[3] = 0x08;dot_t[4] = 0x08;}
    else if(chch == 46)
    {dot_t[0] = 0x00;dot_t[1] = 0x40;dot_t[2] = 0x00;dot_t[3] = 0x00;dot_t[4] = 0x00;}
    else if(chch ==47)
    {dot_t[0] = 0x02;dot_t[1] = 0x04;dot_t[3] = 0x08;dot_t[4] = 0x10;dot_t[5] = 0x20;}
    else if(chch == 48)
    {dot_t[0] = 0x3E;dot_t[1] = 0x51;dot_t[2] = 0x49;dot_t[3] = 0x45;dot_t[4] = 0x3E;}
    else if (chch == 49)
    {dot_t[0] = 0x00;dot_t[1] = 0x42;dot_t[2] = 0x7F;dot_t[3] = 0x40;dot_t[4] = 0x00;}
    else if (chch == 50)
    {dot_t[0] = 0x62;dot_t[1] = 0x51;dot_t[2] = 0x49;dot_t[3] = 0x49;dot_t[4] = 0x46;}
    else if (chch == 51)
    {dot_t[0] = 0x22;dot_t[1] = 0x49;dot_t[2] = 0x49;dot_t[3] = 0x49;dot_t[4] = 0x36;}
    else if (chch == 52)
    {dot_t[0] = 0x18;dot_t[1] = 0x14;dot_t[2] = 0x12;dot_t[3] = 0x7F;dot_t[4] = 0x10;}
    else if (chch == 53)
    {dot_t[0] = 0x27;dot_t[1] = 0x45;dot_t[2] = 0x45;dot_t[3] = 0x45;dot_t[4] = 0x39;}
    else if (chch == 54)
    {dot_t[0] = 0x3C;dot_t[1] = 0x4A;dot_t[2] = 0x49;dot_t[3] = 0x49;dot_t[4] = 0x30;}
    else if (chch == 55)
    {dot_t[0] = 0x01;dot_t[1] = 0x71;dot_t[2] = 0x09;dot_t[3] = 0x05;dot_t[4] = 0x03;}
    else if (chch == 56)
    {dot_t[0] = 0x36;dot_t[1] = 0x49;dot_t[2] = 0x49;dot_t[3] = 0x49;dot_t[4] = 0x36;}
    else if (chch == 57)
    {dot_t[0] = 0x06;dot_t[1] = 0x49;dot_t[2] = 0x49;dot_t[3] = 0x29;dot_t[4] = 0x1E;}
    else if (chch == 32)
    {dot_t[0] = 0x00;dot_t[1] = 0x00;dot_t[2] = 0x00;dot_t[3] = 0x00;dot_t[4] = 0x00;}
    else if (chch == 65)
    {dot_t[0] = 0x7E;dot_t[1] = 0x09;dot_t[2] = 0x09;dot_t[3] = 0x09;dot_t[4] = 0x7E;}
    else if (chch == 66)
    {dot_t[0] = 0x7F;dot_t[1] = 0x49;dot_t[2] = 0x49;dot_t[3] = 0x49;dot_t[4] = 0x36;}
    else if (chch == 67)
    {dot_t[0] = 0x3E;dot_t[1] = 0x41;dot_t[2] = 0x41;dot_t[3] = 0x41;dot_t[4] = 0x22;}
    else if (chch == 68)
    {dot_t[0] = 0x7F;dot_t[1] = 0x41;dot_t[2] = 0x41;dot_t[3] = 0x41;dot_t[4] = 0x3E;}
    else if (chch == 69)
    {dot_t[0] = 0x7F;dot_t[1] = 0x49;dot_t[2] = 0x49;dot_t[3] = 0x49;dot_t[4] = 0x41;}
    else if (chch == 70)
    {dot_t[0] = 0x7F;dot_t[1] = 0x09;dot_t[2] = 0x09;dot_t[3] = 0x09;dot_t[4] = 0x01;}
    else if (chch == 71)
    {dot_t[0] = 0x3E;dot_t[1] = 0x41;dot_t[2] = 0x51;dot_t[3] = 0x51;dot_t[4] = 0x72;}
    else if (chch == 72)
    {dot_t[0] = 0x7F;dot_t[1] = 0x08;dot_t[2] = 0x08;dot_t[3] = 0x08;dot_t[4] = 0x7F;}
    else if (chch == 73)
    {dot_t[0] = 0x00;dot_t[1] = 0x41;dot_t[2] = 0x7F;dot_t[3] = 0x41;dot_t[4] = 0x00;}
    else if (chch == 74)
    {dot_t[0] = 0x20;dot_t[1] = 0x40;dot_t[2] = 0x40;dot_t[3] = 0x40;dot_t[4] = 0x3F;}
    else if (chch == 75)
    {dot_t[0] = 0x7F;dot_t[1] = 0x08;dot_t[2] = 0x14;dot_t[3] = 0x22;dot_t[4] = 0x41;}
    else if (chch == 76)
    {dot_t[0] = 0x7F;dot_t[1] = 0x40;dot_t[2] = 0x40;dot_t[3] = 0x40;dot_t[4] = 0x40;}
    else if (chch == 77)
    {dot_t[0] = 0x7F;dot_t[1] = 0x02;dot_t[2] = 0x0C;dot_t[3] = 0x02;dot_t[4] = 0x7F;}
    else if (chch == 78)
    {dot_t[0] = 0x7F;dot_t[1] = 0x04;dot_t[2] = 0x08;dot_t[3] = 0x10;dot_t[4] = 0x7F;}
    else if (chch == 79)
    {dot_t[0] = 0x3E;dot_t[1] = 0x41;dot_t[2] = 0x41;dot_t[3] = 0x41;dot_t[4] = 0x3E;}
    else if (chch == 80)
    {dot_t[0] = 0x7F;dot_t[1] = 0x09;dot_t[2] = 0x09;dot_t[3] = 0x09;dot_t[4] = 0x06;}
    else if (chch == 81)
    {dot_t[0] = 0x3E;dot_t[1] = 0x41;dot_t[2] = 0x51;dot_t[3] = 0x21;dot_t[4] = 0x5E;}
    else if (chch == 82)
    {dot_t[0] = 0x7F;dot_t[1] = 0x09;dot_t[2] = 0x19;dot_t[3] = 0x29;dot_t[4] = 0x46;}
    else if (chch == 83)
    {dot_t[0] = 0x26;dot_t[1] = 0x49;dot_t[2] = 0x49;dot_t[3] = 0x49;dot_t[4] = 0x32;}
    else if (chch == 84)
    {dot_t[0] = 0x01;dot_t[1] = 0x01;dot_t[2] = 0x7F;dot_t[3] = 0x01;dot_t[4] = 0x01;}
    else if (chch == 85)
    {dot_t[0] = 0x3F;dot_t[1] = 0x40;dot_t[2] = 0x40;dot_t[3] = 0x40;dot_t[4] = 0x3F;}
    else if (chch == 86)
    {dot_t[0] = 0x07;dot_t[1] = 0x18;dot_t[2] = 0x60;dot_t[3] = 0x18;dot_t[4] = 0x07;}
    else if (chch == 87)
    {dot_t[0] = 0x7F;dot_t[1] = 0x20;dot_t[2] = 0x18;dot_t[3] = 0x20;dot_t[4] = 0x7F;}
    else if (chch == 88)
    {dot_t[0] = 0x63;dot_t[1] = 0x14;dot_t[2] = 0x08;dot_t[3] = 0x14;dot_t[4] = 0x63;}
    else if (chch == 89)
    {dot_t[0] = 0x03;dot_t[1] = 0x04;dot_t[2] = 0x78;dot_t[3] = 0x04;dot_t[4] = 0x03;}
    else if (chch == 90)
    {dot_t[0] = 0x61;dot_t[1] = 0x51;dot_t[2] = 0x49;dot_t[3] = 0x45;dot_t[4] = 0x43;}
    else if (chch == 33)
    {dot_t[0] = 0x7F;dot_t[1] = 0x7F;dot_t[2] = 0x7F;dot_t[3] = 0x7F;dot_t[4] = 0x7F;}
    
    else if(chch==97)
  	{dot_t[0]=32;dot_t[1]=84;dot_t[2]=84;dot_t[3]=120;dot_t[4]=64;}
  	else if(chch==98)
  	{dot_t[0]=127;dot_t[1]=72;dot_t[2]=68;dot_t[3]=68;dot_t[4]=56;}
  	else if(chch==99)
  	{dot_t[0]=56;dot_t[1]=68;dot_t[2]=68;dot_t[3]=68;dot_t[4]=32;}
  	else if(chch==100)
  	{dot_t[0]=56;dot_t[1]=68;dot_t[2]=68;dot_t[3]=72;dot_t[4]=127;}
  	else if(chch==101)	//e
  	{dot_t[0]=56;dot_t[1]=84;dot_t[2]=84;dot_t[3]=84;dot_t[4]=8;}
  	else if(chch==102)	//f
  	{dot_t[0]=8;dot_t[1]=126;dot_t[2]=9;dot_t[3]=2;dot_t[4]=0;}
  	else if(chch==103)	//g
  	{dot_t[0]=56;dot_t[1]=324;dot_t[2]=324;dot_t[3]=324;dot_t[4]=248;}
  	else if(chch==104)	//h
  	{dot_t[0]=127;dot_t[1]=8;dot_t[2]=4;dot_t[3]=4;dot_t[4]=120;}
  	else if(chch==105)
  	{dot_t[0]=0;dot_t[1]=68;dot_t[2]=125;dot_t[3]=64;dot_t[4]=0;}
  	else if(chch==106)	//j
  	{dot_t[0]=192;dot_t[1]=256;dot_t[2]=260;dot_t[3]=253;dot_t[4]=0;}
  	else if(chch==107)
  	{dot_t[0]=127;dot_t[1]=16;dot_t[2]=40;dot_t[3]=68;dot_t[4]=0;}
  	else if(chch==108)
  	{dot_t[0]=0;dot_t[1]=65;dot_t[2]=127;dot_t[3]=64;dot_t[4]=0;}
  	else if(chch==109)
  	{dot_t[0]=120;dot_t[1]=4;dot_t[2]=24;dot_t[3]=4;dot_t[4]=120;}
  	else if(chch==110)	//n
  	{dot_t[0]=62;dot_t[1]=4;dot_t[2]=2;dot_t[3]=2;dot_t[4]=60;}
  	else if(chch==111)
  	{dot_t[0]=28;dot_t[1]=34;dot_t[2]=34;dot_t[3]=34;dot_t[4]=28;}
  	else if(chch==112)
  	{dot_t[0]=254;dot_t[1]=10;dot_t[2]=18;dot_t[3]=18;dot_t[4]=12;}
  	else if(chch==113)
  	{dot_t[0]=12;dot_t[1]=18;dot_t[2]=10;dot_t[3]=254;dot_t[4]=128;}
  	else if(chch==114)	//r
  	{dot_t[0]=62;dot_t[1]=4;dot_t[2]=2;dot_t[3]=2;dot_t[4]=0;}
  	else if(chch==115)	//s
  	{dot_t[0]=36;dot_t[1]=42;dot_t[2]=42;dot_t[3]=42;dot_t[4]=18;}
  	else if(chch==116)
  	{dot_t[0]=2;dot_t[1]=31;dot_t[2]=34;dot_t[3]=16;dot_t[4]=0;}
  	else if(chch==117)	//u
  	{dot_t[0]=30;dot_t[1]=32;dot_t[2]=32;dot_t[3]=16;dot_t[4]=62;}
  	else if(chch==118)	//v
  	{dot_t[0]=14;dot_t[1]=16;dot_t[2]=32;dot_t[3]=16;dot_t[4]=14;}
  	else if(chch==119)	//w
  	{dot_t[0]=30;dot_t[1]=32;dot_t[2]=24;dot_t[3]=32;dot_t[4]=30;}
  	else if(chch==120)
  	{dot_t[0]=34;dot_t[1]=20;dot_t[2]=8;dot_t[3]=20;dot_t[4]=34;}
  	else if(chch==121)
  	{dot_t[0]=130;dot_t[1]=68;dot_t[2]=56;dot_t[3]=4;dot_t[4]=2;}
  	else if(chch==122)
  	{dot_t[0]=34;dot_t[1]=50;dot_t[2]=42;dot_t[3]=38;dot_t[4]=34;} 
    else if(chch==62)
  	{dot_t[0]=65;dot_t[1]=34;dot_t[2]=20;dot_t[3]=8;dot_t[4]=0;} 
      
     
    else 
    {dot_t[0] = 0x00;dot_t[1] = 0x00;dot_t[2] = 0x00;dot_t[3] = 0x00;dot_t[4] = 0x00;}
}