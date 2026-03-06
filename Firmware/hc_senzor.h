#ifndef HCSR_H
#define	HCSR_H

///library for MCU
#include <p30fxxxx.h>


#define TRIGGER_PIN LATBbits.LATB5
#define ECHO_PIN PORTBbits.RB8
#define TRIGGER_PIN_2 LATFbits.LATF2
#define ECHO_PIN_2 PORTFbits.RF3


void ConfigureHC_sensor(void);

#endif	/* HCSR_H */
