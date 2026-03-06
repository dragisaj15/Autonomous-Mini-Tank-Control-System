#include <p30fxxxx.h>
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

_FOSC(CSW_FSCM_OFF & XT_PLL4); // Instruction clock is the same as the 10MHz crystal
_FWDT(WDT_OFF);                // Disable Watchdog Timer

#define SPEED_OF_SOUND (0.0343) // cm/s
#define INSTRUCTION_CLOCK_PERIOD (0.1) // us

unsigned int raw_adc0, gp2y_flag, sys_flag;
unsigned int temp_delay, temp_delay1, temp_delay2, rx_index, tempRX, temp0, analog_val;
unsigned int analog_read, echo_pulse;
int rx_buf[1];

static unsigned char timeout_sensor_2 = 0;
static float measured_distance_2 = 0;
static unsigned char timeout_sensor_1 = 0;
static float measured_distance_1 = 0;
static float distance_gp2y = 0;
static float voltage_gp2y = 0;
static int counter_us, counter_ms;

// TIMER 1 INTERRUPT ROUTINE - Every 1ms
void __attribute__ ((__interrupt__, no_auto_psv)) _T1Interrupt(void) 
{
    TMR1 = 0;
    counter_ms++;
    IFS0bits.T1IF = 0;     
}

// TIMER 2 INTERRUPT ROUTINE - Every 1us
void __attribute__ ((__interrupt__, no_auto_psv)) _T2Interrupt(void) 
{
    TMR2 = 0;
    counter_us++;
    IFS0bits.T2IF = 0;     
}

// TIMER 3 INTERRUPT ROUTINE - For PWM generation
void __attribute__ ((__interrupt__, no_auto_psv)) _T3Interrupt(void) 
{
    TMR3 = 0;
    IFS0bits.T3IF = 0;     
}

// TIMER 4 INTERRUPT ROUTINE - For the second digital ultrasonic sensor
void __attribute__ ((__interrupt__, no_auto_psv)) _T4Interrupt(void) 
{
    TMR4 = 0;
    ECHO_PIN_2 = 0;
    timeout_sensor_2 = 1;
    IFS1bits.T4IF = 0;     
}

// TIMER 5 INTERRUPT ROUTINE - For the first digital ultrasonic sensor
void __attribute__ ((__interrupt__, no_auto_psv)) _T5Interrupt(void) 
{
    TMR5 = 0;
    ECHO_PIN = 0;  
    timeout_sensor_1 = 1;
    IFS1bits.T5IF = 0;     
}

// FUNCTION PROTOTYPE - Delay in ms
static void DelayMs(int time_ms);
// FUNCTION PROTOTYPE - Delay in us
static void DelayUs(int time_us);

// UART2 INTERRUPT ROUTINE - For Bluetooth communication
void __attribute__((__interrupt__)) _U2RXInterrupt(void) 
{
    IFS1bits.U2RXIF = 0;
    tempRX = U2RXREG;
    if(tempRX != 0){
        rx_buf[rx_index] = tempRX;
        if(rx_index < 2) {
            rx_index++;
        } 
        else {
            rx_index = 0;
        }
    }
} 

// ADC INTERRUPT ROUTINE
void __attribute__((__interrupt__)) _ADCInterrupt(void) 
{
    raw_adc0 = ADCBUF0; // Read from GP2Y sensor channel
    analog_val = raw_adc0;
                                      
    IFS0bits.ADIF = 0;
} 

// DELAY FUNCTION - milliseconds
static void DelayMs(int time_ms)
{
    counter_ms = 0;
    T1CONbits.TON = 1;
    while(counter_ms < time_ms);
    T1CONbits.TON = 0;
}
                
// DELAY FUNCTION - microseconds           
static void DelayUs(int time_us)
{
    counter_us = 0;
    T2CONbits.TON = 1;
    while(counter_us < time_us);
    T2CONbits.TON = 0;
}      

// READ VOLTAGE FROM ANALOG GP2Y SENSOR AND CONVERT TO DISTANCE (IN CM)
float ReadDistance1GP2Y(){  
    // Convert analog ADC value to voltage
    voltage_gp2y = analog_val * (5.0 / 4095.0);

    // Convert voltage to distance in centimeters (based on GP2Y datasheet curve)
    distance_gp2y = 27.86 * powf(voltage_gp2y, -1.15);
    
    return distance_gp2y;
}

// READ VALUE FROM FIRST ULTRASONIC SENSOR
float ReadDistance1() {
    // Generate a 10us trigger pulse
    TRIGGER_PIN = 1;
    DelayUs(3);
    TRIGGER_PIN = 0;
    DelayUs(3);
    
    // Wait for ECHO pin to go high
    while (ECHO_PIN == 0);
    TMR5 = 0;              
    IFS1bits.T5IF = 0;
    T5CONbits.TON = 1;    // Start Timer 5
    
    // Measure the width of the ECHO signal
    while (ECHO_PIN == 1);
    T5CONbits.TON = 0;  // Stop Timer 5, measurement complete
    
    unsigned int measured_time_1; 
    if(timeout_sensor_1 == 1)     // If timeout occurred (no object in range)
    {
        measured_time_1 = TMR5_period;
        timeout_sensor_1 = 0;
    }
    else                            // Signal returned successfully
    {
        measured_time_1 = TMR5;
    }
    TMR5 = 0;
   
    // Calculate distance based on speed of sound
    measured_distance_1 = (measured_time_1 * INSTRUCTION_CLOCK_PERIOD) / 2.0 * SPEED_OF_SOUND;        
    return measured_distance_1;
} 

// READ VALUE FROM SECOND ULTRASONIC SENSOR
float ReadDistance2() {
    // Generate a 10us trigger pulse
    TRIGGER_PIN_2 = 1;
    DelayUs(3);
    TRIGGER_PIN_2 = 0;
    DelayUs(3);
    
    // Wait for ECHO pin to go high
    while (ECHO_PIN_2 == 0);
    TMR4 = 0;
    IFS1bits.T4IF = 0;
    T4CONbits.TON = 1; 
    
    // Measure the width of the ECHO signal
    while (ECHO_PIN_2 == 1);
    T4CONbits.TON = 0;  // Stop Timer 4, measurement complete
    
    unsigned int measured_time_2; 
    if(timeout_sensor_2 == 1)     // If timeout occurred
    {
        measured_time_2 = TMR4_period;
        timeout_sensor_2 = 0;
    }
    else                            // Signal returned successfully
    {
        measured_time_2 = TMR4;
    }
    TMR4 = 0;

    // Calculate distance based on speed of sound
    measured_distance_2 = (measured_time_2 * INSTRUCTION_CLOCK_PERIOD) / 2.0 * SPEED_OF_SOUND;        
    return measured_distance_2;
} 

// START SYSTEM - System activates when "ON" is received via Bluetooth
void start(){
    if(rx_buf[0] == 'O' && rx_buf[1] == 'N') {
            sys_flag = 1;
            WriteUART2(10);
            RS232_putst2("STARTING"); // Notify via Bluetooth
            DelayMs(2000);
    }
}


int main(int argc, char** argv) {
    
    // Initial stabilization delays
    for(temp_delay1=0; temp_delay1<10000; temp_delay1++);
    for(temp_delay=0; temp_delay<60000; temp_delay++);
    
    sys_flag = 0;
    
    // Hardware Initializations
    Init_T1();
    Init_T2();
    Init_T3();
    Init_T4();
    Init_T5();
    
    ConfigureADCPins();
    ADCinit();
    ADCON1bits.ADON = 1; // Start AD conversion 
    
    ConfigureHC_sensor();
    initUART1();
    initUART2(); // Bluetooth UART
    initPWM();   // Motor control PWM
    
	while(1)
	{
        start();    
        
        if (sys_flag == 1) {
            
            distance_gp2y = ReadDistance1GP2Y();
            WriteUART2(10);
            
            measured_distance_1 = ReadDistance1();
            measured_distance_2 = ReadDistance2();
        
            /*
            // PRINT MEASURED SENSOR VALUES TO BLUETOOTH
            char buffer[20];
            int i;

            // Print Sensor 1 to BT
            sprintf(buffer, "Sensor 1: %.2f cm\r", (double)measured_distance_1);
            for (i = 0; i < 20; i++) {
                WriteUART2(buffer[i]);
            }
              
            // Print Sensor 2 to BT  
            sprintf(buffer, "Sensor 2: %.2f cm\r\n", (double)measured_distance_2);
            for (i = 0; i < 20; i++) {
                WriteUART2(buffer[i]);
            } 
            
            // Print GP2Y to BT
            sprintf(buffer, "Sensor GP2Y: %.2f cm\r", (double)distance_gp2y);
            for (i = 0; i < 20; i++) {
                WriteUART2(buffer[i]);
            } 
            */
       
            // NAVIGATION ALGORITHM:
            // The tank moves forward until an obstacle is detected in front.
            // When an obstacle is detected, it compares the values of the left and right sensors 
            // to decide the best direction to turn.
            
            if(distance_gp2y >= 12) {
                pravo(); // Move forward
                
            } else if (distance_gp2y < 12 && (measured_distance_2 <= measured_distance_1)){
                stani(); // Stop
                DelayMs(500);
                levo();  // Turn left
                DelayMs(1200);
                stani(); // Stop
                DelayMs(100);
                
            } else if (distance_gp2y < 12 && (measured_distance_1 < measured_distance_2)) {
                stani(); // Stop
                DelayMs(500);
                desno(); // Turn right
                DelayMs(1200);
                stani(); // Stop
                DelayMs(100);
                
            } else {
                stani(); // Stop default
            } 
        } // end if sys_flag
    } // end while    
    
    return (EXIT_SUCCESS);
}
