

#ifndef TIMER_H
#define	TIMER_H

///library for MCU
#include <p30fxxxx.h>

#define TMR1_period 10000   // Tosc = 1/Fosc = 1/10000000 = 0.1us , 0.1us * 10000 = 1ms
#define TMR2_period 10      // Tosc = 1/Fosc = 1/10000000 = 0.1us , 0.1us * 10 = 1us
#define TMR3_period 100    // Tpwm = 40us = (PR3 + 1) * Tosc * Prescale_Value, Prescale_Value = 1, fpwm = 25KHz
#define TMR4_period 40000
#define TMR5_period 40000 
#define T3_PS_1_256         0xffff  


void Init_T1(void);
void Init_T2(void);
void Init_T3(void);
void Init_T4(void);
void Init_T5(void);

#endif	/* TIMER_H */


