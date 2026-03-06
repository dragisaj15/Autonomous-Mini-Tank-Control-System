//TRIGER B5 i F2
#include "hc_senzor.h"

void ConfigureHC_sensor(void){

    ADPCFGbits.PCFG5=1; //TRIGER
    ADPCFGbits.PCFG8=1; //ECHO
    TRISBbits.TRISB5 = 0;   // izlazni triger signal 1
    TRISFbits.TRISF2 = 0;   // izlazni triger signal 2
    TRISBbits.TRISB8 = 1;   // ulazni echo signal 1
    TRISFbits.TRISF3 = 1;  // ulazni echo signal 2

    TRIGGER_PIN = 0;  
    TRIGGER_PIN_2 = 0;
    

}
