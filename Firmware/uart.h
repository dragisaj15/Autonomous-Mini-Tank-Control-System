#ifndef UART_H
#define	UART_H
#include <p30fxxxx.h>

void initUART1(void);
void WriteUART1(unsigned int data);
void WriteUART1dec2string(unsigned int data);
void RS232_putst(register const char *str);
void initUART2(void);
void WriteUART2(unsigned int data);
void WriteUART2dec2string(unsigned int data);
void RS232_putst2(register const char *str);

#endif	/* UART_H */
