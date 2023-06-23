#include <p30fxxxx.h>
#include "uart.h"

void initUART1(void)
{
    U1BRG=0x0040;//baud rate 9600
    U1MODEbits.ALTIO = 1; //Koristimo UART1 sa alternativnim pinovima RC13 i RC14
    IEC0bits.U1RXIE = 1;
    U1STA&=0xfffc;
    U1MODEbits.UARTEN=1;
    U1STAbits.UTXEN=1;
}

/*********************************************************************
* Ime funkcije      : WriteUART1                            		 *
* Opis              : Funkcija upisuje podatke u registar U1TXREG,   *
*                     za slanje podataka    						 *
* Parameteri        : unsigned int data-podatak koji zelimo poslati  *
* Povratna vrednost : Nema                                           *
*********************************************************************/

void WriteUART1(unsigned int data)
{


	while (U1STAbits.TRMT==0);
    if(U1MODEbits.PDSEL == 3)
        U1TXREG = data;
        
    else 
        U1TXREG = data & 0xFF;
        
}

/***********************************************************************
* Ime funkcije      : WriteUART1dec2string                     		   *
* Opis              : Funkcija salje 4-cifrene brojeve (cifru po cifru)*
* Parameteri        : unsigned int data-podatak koji zelimo poslati    *
* Povratna vrednost : Nema                                             *
************************************************************************/
void WriteUART1dec2string(unsigned int data)
{
	unsigned char temp;

	temp=data/1000;
	WriteUART1(temp+'0');
	data=data-temp*1000;
	temp=data/100;
	WriteUART1(temp+'0');
	data=data-temp*100;
	temp=data/10;
	WriteUART1(temp+'0');
	data=data-temp*10;
	WriteUART1(data+'0');
}

//Ispis stringa na uart-u
void RS232_putst(register const char *str)
{
    while((*str!=0))
    {
        WriteUART1(*str);
        if(*str == 13) WriteUART1(10);
        if(*str == 10) WriteUART1(13);
        str++;
    }
}




//UART 2 - ZA BLUETOOTH
void initUART2(void)
{
    U2BRG=0x0040;//baud rate 9600
    //U2MODEbits.ALTIO = 1; //Koristimo UART1 sa alternativnim pinovima RC13 i RC14
    IEC1bits.U2RXIE=1; 
    U2STA&=0xfffc;
    U2MODEbits.UARTEN=1;
    U2STAbits.UTXEN=1;
}

/*********************************************************************
* Ime funkcije      : WriteUART1                            		 *
* Opis              : Funkcija upisuje podatke u registar U1TXREG,   *
*                     za slanje podataka    						 *
* Parameteri        : unsigned int data-podatak koji zelimo poslati  *
* Povratna vrednost : Nema                                           *
*********************************************************************/

void WriteUART2(unsigned int data)
{


	while (U2STAbits.TRMT==0);
    if(U2MODEbits.PDSEL == 3)
        U2TXREG = data;
        
    else 
        U2TXREG = data & 0xFF;
        
}

/***********************************************************************
* Ime funkcije      : WriteUART1dec2string                     		   *
* Opis              : Funkcija salje 4-cifrene brojeve (cifru po cifru)*
* Parameteri        : unsigned int data-podatak koji zelimo poslati    *
* Povratna vrednost : Nema                                             *
************************************************************************/
void WriteUART2dec2string(unsigned int data)
{
	unsigned char temp;

	temp=data/1000;
	WriteUART2(temp+'0');
	data=data-temp*1000;
	temp=data/100;
	WriteUART2(temp+'0');
	data=data-temp*100;
	temp=data/10;
	WriteUART2(temp+'0');
	data=data-temp*10;
	WriteUART2(data+'0');
}

//Ispis stringa na uart-u
void RS232_putst2(register const char *str)
{
    while((*str!=0))
    {
        WriteUART2(*str);
        if(*str == 13) WriteUART2(10);
        if(*str == 10) WriteUART2(13);
        str++;
    }
}