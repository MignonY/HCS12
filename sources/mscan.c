#include <stdio.h>
#include <string.h>
#include "mscan.h"
 

#ifdef CAN0


/*********** can0 ***************/
static char *can0_rx_data;
static int can0_rx_length;
static long can0_rx_id;

static char *can0_tx_data;
static int can0_tx_length;
static long can0_tx_id;


static char can0_string_ID[128];
static char can0_string_DATA[128];


void setup_rx_can0() {
	Can0.canidac.byte |= 0x10;

	/* Identifier Mask Register */
	Can0.canid[0].canidmr.b[0] = 0;
	Can0.canid[0].canidmr.b[1] = 0x07;


	/* Identifier Acceptance Register */
	Can0.canid[0].canidar.b[0] = 0;
  Can0.canid[0].canidar.b[1] = 0x07;

}                                 

/* initialize CAN0 module to only send message */
void init_can0(char *buf) {
	can0_rx_data = buf;                             
	can0_rx_length = 0;
	can0_rx_id = -1;

	Can0.canctl1.byte |= 0x80; // enable CAN
	Can0.canctl1.byte &= 0xef; // turn off Listen mode

	/* place CAN in init mode */
	while ( (Can0.canctl1.byte & 0x1) == 0) {
		Can0.canctl0.byte |= 0x1;
	}
  
  /* for 1Mbps
	 * SJW : 1 -> 00b , Prescaler : 2 -> 000001b
	 * Sampling : 1 -> 0 , TSEG2 : 3 -> 010b , TSEG1 : 4 -> 0011b
	 *********************************/
	 //속도 조절을 위한 설정
	Can0.canbtr0.byte = 0b00000001;   
	Can0.canbtr1.byte = 0b00100011;
    	
	setup_rx_can0();
	
	/* Can out of init mode */
	Can0.canctl0.byte &= 0xfe; 

	/* Receiver Interrupt of msCAN module can be initialized 
	 * when out of init mode */
	Can0.canrier.byte = 0b00000001; // 메시지 수신 인터럽트 활성화

	
  // wait for Sync
	while ( (Can0.canctl0.byte & SYNCH) == 0)
		continue;
}

void write_can0(long id, char *data, unsigned int length) {
		can0_tx_id = id;
		can0_tx_data = data;
		can0_tx_length = length;

	Can0.cantier.byte = 0b00000001;           //송신 인터럽트 활성화  
}

/* send message */
void can0_tx_handler(void) {
	int i;
		
	/* select tx buffer 0 */
	Can0.cantbsel.byte = TX0;
	
	/* set ID of data */
	// store only 11 bits of ID
	Can0.txbuf.id.b[0] = (unsigned char)(can0_tx_id >> 3);
	Can0.txbuf.id.b[1] = (unsigned char)( (can0_tx_id & 0x7) << 5);
	
 	/* set data */
  for (i = 0; i < can0_tx_length; i++)
	  Can0.txbuf.dsr[i] = can0_tx_data[i];
	
  Can0.txbuf.dlr = (unsigned char)can0_tx_length;
  Can0.txbuf.tbpr = 0; // no priority, sending order is 0, 1, 2
		
 	// send data in buffer
 	Can0.cantflg.byte |= 0b00000001;    // 메시지 전송 시작
 	
 	// send TX 0
 	Can0.cantarq.byte = ABTRQ1 + ABTRQ2;   //TX1,TX2는 전송하지 않음 

	// mask interrupt
	Can0.cantier.byte = 0;      // 송신 인터럽트를 비활성화한다.
}

void can0_rx_handler(void) {
	read_can0();

	// clear flag bit
	Can0.canrflg.byte |= 0x1;
}


void read_can0(void) {
	unsigned int id = 0;
	int i;
	int j=128;                      	
	unsigned int LED_mask = 0x1;

  static unsigned char temp = 0;
	
	// extract ID
	id = Can0.rxbuf.id.b[0];
	id <<= 3;
	id |= (Can0.rxbuf.id.b[1] >> 5);
  can0_rx_id = id;

	can0_rx_length = Can0.rxbuf.dlr;
		
		
  //CANKing 으로부터의 데이터 값을 그대로 복사한 후, CANKing으로 재전송 한다. 
  /******************************/
  for(i=0; i<can0_rx_length; i++) 
  {    
    can0_rx_data[i]=Can0.rxbuf.dsr[i];
  }
  
  CAN_communication(can0_rx_id, can0_rx_data , can0_rx_length);  
//  write_can0(can0_rx_id, can0_rx_data , can0_rx_length);
  
   
	can0_rx_length = 0;
}                   

#endif


