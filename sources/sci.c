
#include "main.h"
char *txbuffer_sci0; //text를 저장하는 버퍼이다. 
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
	/*sci를 초기화 하는 함수이다. bps와 문자열의 길이 그리고 송수신 interrupt를 사용할지 등을 결정한다.*/
	unsigned long oscclk, modclk;  //SCIBD register의 SBR을 설정하기 위한 변수
	unsigned short BR;    //우리 얻고자 하는 bps를 저장하는 변수
	oscclk = 16000000;	  //MCU의 주파수는 16 MHz이다.
	modclk = oscclk / 2;	  //Set SCI module clock = (Oscillator Freq.)/2
	BR = modclk / 16 / baud_rate;    //Set BR, SCI baud rate = (SCI module clock)/(16*BR)
									 /*위의 식들은 우리가 사용하는 MCU의 설정에 의해 정해지는 식이다. */

	Sci0.scibd.word = BR;  //설정한 BR 값을 SCIBD register에 저장
	Sci0.scicr1.byte = 0x00;  //SCI 설정 관련 register 초기화. Normal operation & 8 data bit format & Parity disabled
	Sci0.scicr2.byte = 0b00100100;  //SCI 설정 관련 register 초기화. reseive interrupt,reseive,transmit enabled
									/* 여기서 transmit interrpt는 보낼 때 interrupt를 발생시키기 때문에 초기화 해 줄 필요없다. */
	txoffset_sci0 = 0;
	rxbuffer_sci0 = txbuf;
	/*각각의 offset과 buffer를 초기화 한다.*/
}

/*********************************************************/
/*  입력받은 문자열을 시리얼 통신으로 전송한다.     */
/*  입력받은 문자열과 길이를 버퍼에 저장하고 송신 interrupt를 enable시켜    */
/*  MCU가 interrupt handler를 수행하도록 한다.        */
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
		Sci0.scidrl.byte = *txbuffer_sci0;    // scidrl에 송신할 데이터 저장          
		*txbuffer_sci0 = '\0';
		txbuffer_sci0++;

		if (*txbuffer_sci0 == '\0')
		{
		  Sci0.scicr2.byte &= 0x7F; // Transmitter Interrupt Enable Bit만 클리어
			//regiatar        
		}
	}
}
