#ifndef LCD_H        /*prevent duplicated includes*/
#define LCD_H

/*Includes*/
#include "projectglobals.h"

typedef unsigned char BYTE;
typedef unsigned int WORD;

void Select_screen(void);
void LED_screen(void);
void Buzzer_screen(void);
void ADC_screen(void);
void CAN_screen(void);

void LED_erase_cursor(void);
void LED_make_cursor(void);
void LED_toggle(void);
void Up_cursor(void);
void Down_cursor(void);

void Buzzer_toggle(void);
void Buzzer_second(int);

void init_LCD(void);
void write_1(BYTE tport);
void Display_ON(BYTE tport);
void Set_addressYX(int tporty, int tportx);
void translation(char chch, char *dot_t);
void delay_1(int t);

void LCD_write_dot(int x,int y);
void LCD_write_char(char ch,int x,int y);
void LCD_write_str(char *str,int x,int y);
void LCD_initial_line(void);
void LCD_clear(void);
void LCD_write_hole(int x,int y);
void LCD_blink(int x,char ch,int state);


#endif /* LCD_H */