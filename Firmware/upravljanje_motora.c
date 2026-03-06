#include "upravljanje_motora.h"
#include "pwm.h"
#include "uart.h"
#include <p30fxxxx.h>

void pravo()
{
    
    LATFbits.LATF0=1; //za smer prvog motora
    LATFbits.LATF1=0; 
        
    LATBbits.LATB9=1; //za smer drugog motora
    LATBbits.LATB10=0; 
    
    PWM1(450); //PWM 1 i PWM 2 nisu postavljeni na iste vrednosti, jer jedan motor malo slabije radi, pa smo empirjiski dosli do ovih optimalnih vrednosti
    PWM2(480);
    
    RS232_putst2("Idem pravo");
}

void levo()
{
    //DESNI MOTOR IDE NAPRED, A LEVI NAZAD
    LATFbits.LATF0=1; //za smer prvog motora
    LATFbits.LATF1=0; 
        
    LATBbits.LATB9=0; //za smer drugog motora
    LATBbits.LATB10=1; 
    
    PWM1(480); //PWM NIJE POSTAVLJEN JEDNAKO KAKO BI MOTOR BOLJE SKRETAO
    PWM2(450);
    
    RS232_putst2("Idem levo");
}

void desno()
{
    
    LATFbits.LATF0=0; //za smer prvog motora
    LATFbits.LATF1=1; 
        
    LATBbits.LATB9=1; //za smer drugog motora
    LATBbits.LATB10=0; 
    
    PWM1(470);
    PWM2(470);
     
    RS232_putst2("Idem desno");
    
}
void stani()
{
    //MOTORI SU UGASENI
    LATFbits.LATF0=0; 
    LATFbits.LATF1=0; 
        
    LATBbits.LATB9=0; 
    LATBbits.LATB10=0; 
    
    PWM1(300);
    PWM2(300);
    
}
