#include<p30fxxxx.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "adc.h"
#include "timer.h"
#include "uart.h"
#include "hc_senzor.h"
#include "outcompare.h"
#include "pwm.h"
#include "upravljanje_motora.h"

_FOSC(CSW_FSCM_OFF & XT_PLL4);//instruction takt je isti kao i kristal 10MHz
_FWDT(WDT_OFF);

#define SPEED_OF_SOUND (0.0343) // cm/s
#define INSTRUCTION_CLOCK_PERIOD (0.1) // us


unsigned int sirovi0, gp2y, flag;
unsigned int broj,broj1,broj2,n,tempRX,temp0,analog;
unsigned int analogni, echo;
int buf[1];
static unsigned char prekoraceno_vreme_2=0;
static float izmerena_distanca_2 = 0;
static unsigned char prekoraceno_vreme_1=0;
static float izmerena_distanca_1 = 0;
static float distanca_gp2y = 0;
static float napon_gp2y = 0;
static int brojac_us,brojac_ms;



//PREKIDNA RUTINA TAJMERA 1 - na svakih 1ms
void __attribute__ ((__interrupt__, no_auto_psv)) _T1Interrupt(void) 
{
    TMR1 = 0;
    brojac_ms++;
    IFS0bits.T1IF = 0;     
}

//PREKIDNA RUTINA TAJMERA 2 - na svakih 1us
void __attribute__ ((__interrupt__, no_auto_psv)) _T2Interrupt(void) 
{
    TMR2 = 0;
    brojac_us++;
    IFS0bits.T2IF = 0;     
}

//PREKIDNA RUTINA TAJMERA 3 - za PWM
void __attribute__ ((__interrupt__, no_auto_psv)) _T3Interrupt(void) 
{
    TMR3 = 0;
    IFS0bits.T3IF = 0;     
}

//PREKIDNA RUTINA TAJMERA 4 - za drugi digitalni senzor 
void __attribute__ ((__interrupt__, no_auto_psv)) _T4Interrupt(void) 
{
    TMR4 = 0;
    ECHO_PIN_2 = 0;
    prekoraceno_vreme_2 = 1;
    IFS1bits.T4IF = 0;     
}

//PREKIDNA RUTINA TAJMERA 5 - za prvi digitalni senzor 
void __attribute__ ((__interrupt__, no_auto_psv)) _T5Interrupt(void) 
{
    TMR5 = 0;
    ECHO_PIN = 0;  
    prekoraceno_vreme_1 = 1;
    IFS1bits.T5IF = 0;     
}


// FUNKCIJA ZA KASNJENJE U ms
static void DelayMs (int vreme);
// FUNKCIJA ZA KASNJENJE U us
static void DelayUs (int vreme);


//PREKIDNA RUTINA ZA UART2
void __attribute__((__interrupt__)) _U2RXInterrupt(void) 
{
    IFS1bits.U2RXIF = 0;
    tempRX=U2RXREG;
    if(tempRX!=0){
        buf[n] = tempRX;
        if(n<2) {
            n++;
        } 
        else {
            n = 0;
        }
    }
} 

//PREKIDNA RUTINA ZA AD KONVERZIJU
void __attribute__((__interrupt__)) _ADCInterrupt(void) 
{
							

										sirovi0=ADCBUF0; //GP2Y
                                        analog=sirovi0;
                                      
										
										

    IFS0bits.ADIF = 0;

} 


//KASNJENJE U ms
static void DelayMs (int vreme)
{
    brojac_ms = 0;
    T1CONbits.TON = 1;
    while(brojac_ms < vreme);
    T1CONbits.TON = 0;
}
             
                
//KASNJENJE U us             
static void DelayUs (int vreme)
{
    brojac_us = 0;
    T2CONbits.TON = 1;
    while(brojac_us < vreme);
    T2CONbits.TON = 0;
}      
                



// CITANJE VREDNOSTI NAPONA SA ANALOGNOG SENZORA GP2Y I KONVERTOVANJE U DISTANCU(U CENTIMETRIMA)
float ReadDistance1GP2Y(){  
    
        
        // Pretvaranje analognog signala u napon
        napon_gp2y = analog * (5.0 / 4095.0);

         // Pretvaranje napona u udaljenost (u centimetrima)
        distanca_gp2y = 27.86 * powf(napon_gp2y, -1.15);
  
}




// Funkcija za citanje vrednosti prvog senzora
float ReadDistance1() {
    // Generisi 10us triger signal
    TRIGGER_PIN = 1;
    DelayUs(3);
    TRIGGER_PIN = 0;
    DelayUs(3);
    
    

    // Sacekaj da echo pin postane visok
    while (ECHO_PIN == 0);
    TMR5=0;              
    IFS1bits.T5IF = 0;
    T5CONbits.TON = 1;    //ukljucuje se tajmer 5
    
    // Merenje sirine echo signala
    while (ECHO_PIN == 1);
        T5CONbits.TON = 0;  // iskljucuje se T5 i zavrsava se merenje
    unsigned int izmereno_vreme_1; 
    if(prekoraceno_vreme_1 == 1)     // desi se vreme prekoracenja
    {
        izmereno_vreme_1 = TMR5_period;
        prekoraceno_vreme_1 = 0;
    }
    else                            // signal se vratio nazad
    {
        izmereno_vreme_1 = TMR5;
    }
    TMR5 = 0;
   
    izmerena_distanca_1 = (izmereno_vreme_1*INSTRUCTION_CLOCK_PERIOD)/2*SPEED_OF_SOUND;        
} 






// Funkcija za citanje vrednosti drugog senzora
float ReadDistance2() {
    // Generisi 10us triger signal
    TRIGGER_PIN_2 = 1;
    DelayUs(3);
    TRIGGER_PIN_2 = 0;
    DelayUs(3);
    
    

    // Sacekaj da echo pin postane visok
    while (ECHO_PIN_2 == 0);
    TMR4=0;
    IFS1bits.T4IF = 0;
    T4CONbits.TON = 1; 
    // Merenje sirine echo signala
    while (ECHO_PIN_2 == 1);
        T4CONbits.TON = 0;  // iskljucuje se T5 i zavrsava se merenje
    unsigned int izmereno_vreme_2; 
    if(prekoraceno_vreme_2 == 1)     // desi se vreme prekoracenja
    {
        izmereno_vreme_2 = TMR4_period;
        prekoraceno_vreme_2 = 0;
    }
    else                            // signal se vratio nazad
    {
        izmereno_vreme_2 = TMR4;
    }
    TMR4 = 0;

    izmerena_distanca_2 = (izmereno_vreme_2*INSTRUCTION_CLOCK_PERIOD)/2*SPEED_OF_SOUND;        
} 

//Funkcija za pokretanje sistema - kada se sa bluetooth-a posalje ON, sistem se pokrece
void start(){
    
    if(buf[0] == 'O' && buf[1] == 'N') {
            //GLCD_ClrScr();
            flag=1;
            WriteUART2(10);
            RS232_putst2("KRECEM");
            DelayMs(2000);
    }
}


int main(int argc, char** argv) {
    
    for(broj1=0;broj1<10000;broj1++);
    for(broj=0;broj<60000;broj++);
    
    flag = 0;
    Init_T1();
    Init_T2();
    Init_T3();
    Init_T4();
    Init_T5();
    ConfigureADCPins();
    ADCinit();
    ADCON1bits.ADON=1;//pocetak Ad konverzije 
    ConfigureHC_sensor();
    initUART1();
    initUART2();
    initPWM();
    
       
        
	while(1)
		{
        
        start();    
        
        
        if (flag == 1) {
        distanca_gp2y = ReadDistance1GP2Y();
        WriteUART2(10);
        
        izmerena_distanca_1 = ReadDistance1();
        izmerena_distanca_2 = ReadDistance2();
        
        
        //ISPISIVANJE IZMERENIH VREDNOSTI SA SENZORA NA BT
//        char buffer[20];
//        int i;

//        //Ispis senzora 1 na bt
//          sprintf(buffer, "Senzor 1: %.2f cm\r", (double)izmerena_distanca_1);
//          for (i = 0; i < 20; i++) {
//              WriteUART2(buffer[i]);
//         }
//          
////        //Ispis senzora 2 na bt  
//        sprintf(buffer, "Senzor 2: %.2f cm\r\n", (double)izmerena_distanca_2);
//        for (i = 0; i < 20; i++) {
//            WriteUART2(buffer[i]);
//        } 
//        
//        //Ispis senzora GP2Y na bt
//        sprintf(buffer, "Senzor GP2Y: %.2f cm\r", (double)distanca_gp2y);
//        for (i = 0; i < 20; i++) {
//            WriteUART2(buffer[i]);
//        } 
  
       
 
        
        
        //IDE PRAVO DOK GOD NE NAIDJE NA PREPREKU, KADA NAIDJE NA PREPREKU SA PREDNJE STRANE, U ZAVISNOSTI OD VREDNOSTI SENZORA SA LEVE I DESNE STRANE, ODLUCUJE GDE CE DA SKRENE
        if(distanca_gp2y >= 12) {
            pravo();
            
        } else if (distanca_gp2y < 12 && (izmerena_distanca_2 <= izmerena_distanca_1)){
            stani();
            DelayMs(500);
            levo();
            DelayMs(1200);
            stani();
            DelayMs(100);
            
            
        } else if (distanca_gp2y < 12 && (izmerena_distanca_1 < izmerena_distanca_2)) {
            stani();
            DelayMs(500);
            
            desno();
            DelayMs(1200);
            stani();
            DelayMs(100);
            
            
        } else {
            stani();
        } 
        
        }
         
        

         
        }
    
     return (EXIT_SUCCESS);

                  
    }//while    
    
    
  
