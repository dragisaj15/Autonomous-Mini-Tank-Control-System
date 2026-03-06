
#ifndef PWM_H
#define	PWM_H

///library for MCU
#include <p30fxxxx.h>


void initPWM();

void PWM1(int dutyC);
void PWM2(int dutyC);

#endif	/* PWM_H */
