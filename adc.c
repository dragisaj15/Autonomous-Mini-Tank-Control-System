#include<p30fxxxx.h>

#include "adc.h"


void ConfigureADCPins(void)
{
	
    ADPCFGbits.PCFG4=0; //Za GP2Y analogni senzor
	TRISBbits.TRISB4=1;
    
   
}

void ADCinit(void)
{

ADCON1bits.ADSIDL=0;
ADCON1bits.FORM=0;
ADCON1bits.SSRC=7;
//na kraju  ADCON1bits.ASAM=1;
ADCON1bits.SAMP=1;
//ADCON1bits.ADON=1;   


ADCON2bits.VCFG=7;
ADCON2bits.CSCNA=1;
ADCON2bits.SMPI=0;  //1 konverzija
ADCON2bits.BUFM=0;
ADCON2bits.ALTS=0;



ADCON3bits.SAMC=31;
ADCON3bits.ADRC=1;
ADCON3bits.ADCS=31;

ADCHSbits.CH0NB=0;
ADCHSbits.CH0NA=0;

ADCHSbits.CH0SA=0;
ADCHSbits.CH0SB=0;



/*ADCSSL: A/D Input Scan Select Register

bit 15-0 CSSL<15:0>: A/D Input Pin Scan Selection bits
	1 = Select ANx for input scan
	0 = Skip ANx for input scan*/
//ADCSSL=0b0001111111111111;

    ADCSSL=0b0000000000010000;  //koristim 4 za gp2y
  //ADCSSL=0b0001001001111111; //koristimo 6 7 i 12 6pir 7mq3 12foto
         //  FEDCBA9876543210
                  
  
ADCON1bits.ASAM=1;

IFS0bits.ADIF=1;
IEC0bits.ADIE=1;
}