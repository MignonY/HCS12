
#include "main.h"
#include "sci.h"

/*****************************************************************/
/* 2. fill out (?) and //(?)function                             */
/* and fill out a part of code(line 303)                         */
/* and let it react once every time you press the button.        */
/*****************************************************************/


char ch=0;
char tx_data[16]="";
char tx_buffer[16]="";
int change=0;
int i=0;
int state=1;
int count=0; 
int position=0; 
int eng=0;
int change_count=0;
int select_alpha=0;
int button_state=0;
int lock=0;
int blink;

int screen_state=0;

char str[16]="";


void rti_handler(void)   // 리얼타임 핸들
{
   
   change_count++;
   count++;

   if(count==1000)
   {
     count=0;
     ADC_getatd(screen_state, lock);
     LED_blink(blink, screen_state, lock);
     Buzzer_timer();
   }
   
  /* 
   if(button_state!=10 && change==1 && change_count==5000 && tx_data[0]!='\0')
   {
      change=0;
      change_count=0;
      position++;
      ch=0;
      select_alpha=0;
      LCD_write_str(tx_data, 4, 52);
      button_state=10;
   }*/
  
  
   Crg.crgflg.bit.rtif = 1;
} 


void init_interrupt(void){

 Pim.ddrh.byte=0xF0;  //상위 4bit은 출력 하위 4bit은 입력으로 설정
 Pim.pieh.byte=0x07;  //하위 3 bit을 사용한다.
 Pim.ppsh.byte=0x00;  //interrupt flag 초기한다.
 linktx(tx_data);
} 


void interruptH_function()
{

    if(Pim.pifh.byte & 0b00000010)   
    {
      if((Pim.pth.byte & 0xe0) == 0xe0) //1
      {
        if(!screen_state && !lock) 
        {
          LED_screen();
          LCD_initial_line();
          screen_state=1;
        } 
        
        else if(screen_state==1 && !lock)
          LED_toggle1(); 
        
        else if(screen_state==2 && !lock)
          Buzzer_second(1);
      }
      else if((Pim.pth.byte & 0x70) == 0x70)  //4  
      {
        if(!screen_state && !lock) 
        {
          CAN_screen();
          LCD_initial_line();
          screen_state=4;
        } 
        
        else if(screen_state==1 && !lock) 
        {
          LED_erase_cursor();
          Down_cursor();
          LED_make_cursor();           
        }
        
        else if(screen_state==2 && !lock)
          Buzzer_off();
        
        else if(screen_state==5 && !lock)
          Acceptance_left();
        
        else if(screen_state==6 && !lock)
          Mask_left();

      }
      else if((Pim.pth.byte & 0xb0) == 0xb0)  //7  
      {

      }
      else if((Pim.pth.byte & 0xd0) == 0xd0)  //*  
      {
         if(screen_state==1 && !lock) 
         {          
           blink=!blink;
           LED_screen();
         }
      }
      		     
      Pim.pifh.byte |= 0b00000010;  
    } 
      		 				
    else if(Pim.pifh.byte & 0b00000001)   
    {
      if((Pim.pth.byte & 0xE0) == 0xE0) //2   
      {
        if(!screen_state && !lock) 
        {
          Buzzer_screen();
          LCD_initial_line();
          screen_state=2;
        }
        else if(screen_state==2 && !lock)
          Buzzer_second(2);
        
        else if((screen_state==4 || screen_state==5 || screen_state==6) && !lock)
          CAN_upcursor(&screen_state);
      }
      else if((Pim.pth.byte & 0x70) == 0x70)  //5  
      {
        if(screen_state==1 && !lock)
          LED_toggle();
                                                              
        else if(screen_state==2 && !lock)
          Buzzer_toggle();
        
        else if(screen_state==3 && !lock)
          ADC_toggle();
        
        else if(screen_state==4 && !lock)
          CAN_check(&screen_state);
        
        else if(screen_state==5 && !lock)
          Acceptance_toggle();
        
        else if(screen_state==6 && !lock)
          Mask_toggle();

      }
      else if((Pim.pth.byte & 0xb0) == 0xb0)  //8
      {
        if((screen_state==4 || screen_state==5 || screen_state==6)&& !lock)
          CAN_downcursor(&screen_state);
 
      }
      else if((Pim.pth.byte & 0xd0) == 0xd0)  //0
      {
        if(!lock) 
        {
          lock=1;
          LCD_clear();
          LCD_initial_line();          
        } 
        
        else 
        {
          if(!screen_state) 
          {
            Select_screen();     
          } 
          
          else if(screen_state==1) 
          {
            LED_screen();                       
          } 
          else if(screen_state==2)
            Buzzer_screen();
          else if(screen_state==3)
            ADC_screen();
          else if(screen_state==4)
            CAN_screen();
          else if(screen_state==5)
            Acceptance_screen();
          else if(screen_state==6)
            Mask_screen();
          
          lock=0;
          LCD_initial_line();
        }
        
      }
      		
     Pim.pifh.byte |= 0b00000001; 
     }
      		
     else if(Pim.pifh.byte & 0b00000100)   
     {
      	if((Pim.pth.byte & 0xe0) == 0xe0)  //3
      	{
      	  if(!screen_state && !lock) 
          {
            ADC_screen();
            LCD_initial_line();
            screen_state=3;
          }
          
          else if(screen_state==2 && !lock)
            Buzzer_second(3);

      	}
        else if((Pim.pth.byte & 0x70) == 0x70)  //6
      	{
        	if(screen_state==1 && !lock) 
          {
            LED_erase_cursor();
            Up_cursor();
            LED_make_cursor();           
          }
          
          else if(screen_state==5 && !lock)
            Acceptance_right();
          
          else if(screen_state==6 && !lock)
            Mask_right();

      	}
      	else if((Pim.pth.byte & 0xb0) == 0xb0)  //9  
      	{

        }
        else if((Pim.pth.byte & 0xd0) == 0xd0)  //#   
      	{
      	  if(screen_state && !lock) 
      	  { 
        	  Select_screen();
        	  LCD_initial_line();
        	  screen_state=0;      
      	  } 
      	  else if((screen_state==5 || screen_state==6) && !lock) 
      	  {
      	    CAN_screen();
            LCD_initial_line();
            screen_state=4;   
      	    
      	  }
      	}
      	
      Pim.pifh.byte |= 0b00000100; 			
      }
      
	    LCD_write_str(tx_data, 4, 52);
      LCD_initial_line();
      delay_1(10000);
  
}


void Keypad_Cheking() {  //메인에서 풀링으로 계속 돌려서 항상 값을 판단할 수 있게 해줌 

  KEYPAD_CHECK = FIRST_ROW;
  delay_1(100);
  KEYPAD_CHECK = SECOND_ROW;
  delay_1(100);
  KEYPAD_CHECK = THIRD_ROW;
  delay_1(100);
  KEYPAD_CHECK = FOURTH_ROW;
  delay_1(100);   
}