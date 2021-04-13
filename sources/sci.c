
#include "main.h"
char *txbuffer_sci0; //text�� �����ϴ� �����̴�. 
int txoffset_sci0, number = 0;
char *rxbuffer_sci0;
char rx_buffer[16];
char data_saver[5][16];

char *textwindow;

/*********************************************************/
/* 4. fill out (?)function and registar setting.         */
/*********************************************************/

void linktx(char *tx) 
{
  textwindow=tx;  
}

void init_sci0(int baud_rate, char *txbuf)
{
	/*sci�� �ʱ�ȭ �ϴ� �Լ��̴�. bps�� ���ڿ��� ���� �׸��� �ۼ��� interrupt�� ������� ���� �����Ѵ�.*/
	unsigned long oscclk, modclk;  //SCIBD register�� SBR�� �����ϱ� ���� ����
	unsigned short BR;    //�츮 ����� �ϴ� bps�� �����ϴ� ����
	oscclk = 16000000;	  //MCU�� ���ļ��� 16 MHz�̴�.
	modclk = oscclk / 2;	  //Set SCI module clock = (Oscillator Freq.)/2
	BR = modclk / 16 / baud_rate;    //Set BR, SCI baud rate = (SCI module clock)/(16*BR)
									 /*���� �ĵ��� �츮�� ����ϴ� MCU�� ������ ���� �������� ���̴�. */

	Sci0.scibd.word = BR;  //������ BR ���� SCIBD register�� ����
	Sci0.scicr1.byte = 0x00;  //SCI ���� ���� register �ʱ�ȭ. Normal operation & 8 data bit format & Parity disabled
	Sci0.scicr2.byte = 0b00100100;  //SCI ���� ���� register �ʱ�ȭ. reseive interrupt,reseive,transmit enabled
									/* ���⼭ transmit interrpt�� ���� �� interrupt�� �߻���Ű�� ������ �ʱ�ȭ �� �� �ʿ����. */
	txoffset_sci0 = 0;
	rxbuffer_sci0 = txbuf;
	/*������ offset�� buffer�� �ʱ�ȭ �Ѵ�.*/
}

/*********************************************************/
/*  �Է¹��� ���ڿ��� �ø��� ������� �����Ѵ�.     */
/*  �Է¹��� ���ڿ��� ���̸� ���ۿ� �����ϰ� �۽� interrupt�� enable����    */
/*  MCU�� interrupt handler�� �����ϵ��� �Ѵ�.        */
/*********************************************************/
void write_sci0(char *text)
{
	/* This function transmit the recevied string to PC
	It calls SCI interrupt handler by generating a transmit interrupt */

	txbuffer_sci0 = text;           // Store a string in transmit buffer

	Sci0.scicr2.byte |= 0b10001000;     // Enable TIE & TE

}

/*****************************************************************************************/
/*  Function to be executed when transmit or receive interrupt of SCI module is occured  */
/*  Interrupt is occered each time a character of one byte sent or received              */
/*  It is called TDRE and RDRF of SCI module                                             */
/*  Therefore, this function have to be registered in projectvectors.c                   */
/*  Transmit/receive state is determined according to the pattern of the register        */
/*****************************************************************************************/

void sci0_handler(void)
{
	static int i = 0;
	int j, k, m, n;
	
	if (Sci0.scisr1.bit.rdrf == 1)  
	{
		rx_buffer[i++] = Sci0.scidrl.byte;
		if (rx_buffer[i - 1] == 0x0D)
		{
			for (j = i; j<15; j++)
			{
				rx_buffer[j] = ' ';
			}
			if (number<5)
			{
				//(?)function
				LCD_write_str(rx_buffer, 4, 4+number*9);
				for (j = 0; j<16; j++)
				{
					data_saver[number][j] = rx_buffer[j];
				}
				number++;
			}
			else
			{
				for (j = 0; j<4; j++)
				{
					for (k = 0; k<16; k++)
					{
						data_saver[j][k] = data_saver[j + 1][k];
					}
					LCD_write_str(data_saver[j], 4, 4+j*9);
				}

				for (j = 0; j<16; j++)
				{
					data_saver[4][j] = rx_buffer[j];
				}

        LCD_write_str(rx_buffer, 4, 40);
				//(?)function
			}

			LCD_initial_line();

			for (j = 0; j<16; j++)
			{
				rx_buffer[j] = '\0';
			}
			i = 0;
		} 
		
	}
	else if (Sci0.scisr1.bit.tdre == 1)  //registar state 
	{

		//registar
		Sci0.scidrl.byte = *txbuffer_sci0;    // scidrl�� �۽��� ������ ����          
		*txbuffer_sci0 = '\0';
		txbuffer_sci0++;

		if (*txbuffer_sci0 == '\0')
		{
		  Sci0.scicr2.byte &= 0x7F; // Transmitter Interrupt Enable Bit�� Ŭ����
			//regiatar        
		}
	}
}
