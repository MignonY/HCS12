#ifndef MSCAN_H        /*prevent duplicated includes*/
#define MSCAN_H

//#include <stdio.h>
#include "projectglobals.h"
#include "projectvectors.h"
#include "LCD.h"
#include "mscan.h"
#include "stdlib.h"
#include "stdio.h"

#define  CAN0  1  // effects projectvectors.c


#ifdef CAN0

void setup_rx_can0(void);
void init_can0(char *);
void can0_rx_handler(void);
void can0_tx_handler(void);
void read_can0(void);
void write_can0(long id, char *data, unsigned int length);


#endif


int __atoi(char *p);



#endif /*MAIN_H*/
