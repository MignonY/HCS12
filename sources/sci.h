#ifndef SCI_H        /*prevent duplicated includes*/
#define SCI_H


/*Includes*/
#include "projectglobals.h"

void linktx(char*);
void init_sci0(int, char *);
void write_sci0(char *text);
void sci0_handler(void);



#endif /*SCI_H*/