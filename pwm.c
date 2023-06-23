#include <p30fxxxx.h>
#include "pwm.h"
#include "timer.h"
#include "outcompare.h"

void initPWM (void){
    
    
    TRISDbits.TRISD0=0; //pwm za pogon motora 1
    TRISDbits.TRISD1=0; //pwm za pogon motora 2
    
    TRISFbits.TRISF0=0; //za smer prvog motora
    TRISFbits.TRISF1=0; // za smer prvog motora
    
    ADPCFGbits.PCFG9=1; //Pinovi B9 i B10 moraju biti digitalni
    ADPCFGbits.PCFG10=1; //
    TRISBbits.TRISB9=0; // za smer drugog motora
    TRISBbits.TRISB10=0; // za smer drugog motora
   

}
void PWM1(int dutyC)
{
    PR3 = 500;//odredjuje frekvenciju po formuli
    OC1RS = dutyC;//postavimo pwm
    OC1R = 1000;//inicijalni pwm pri paljenju samo
    OC1CON  = OC_IDLE_CON & OC_TIMER3_SRC & OC_PWM_FAULT_PIN_DISABLE& T3_PS_1_256;//konfiguracija pwma
                   
    T3CONbits.TON = 1;//ukljucujemo timer koji koristi

 
}

void PWM2(int dutyC)
{
    PR3 = 500;//odredjuje frekvenciju po formuli
    OC2RS = dutyC;//postavimo pwm
    OC2R = 1000;//inicijalni pwm pri paljenju samo
    OC2CON = OC_IDLE_CON & OC_TIMER3_SRC & OC_PWM_FAULT_PIN_DISABLE& T3_PS_1_256;//konfiguracija pwma
             
    T3CONbits.TON = 1;//ukljucujemo timer koji koristi
}