/* 
 * File:   Servos.c
 * Author: Juan Diego Villafuerte Pazos
 *
 * Created on 27 de abril de 2021, 10:57 AM
 */

#include <stdio.h>
#include <stdlib.h>

#define _XTAL_FREQ 8000000 //frecuencia de 8 MHz
#include <xc.h>
#pragma config FOSC=INTRC_NOCLKOUT //Oscilador interno sin salida
#pragma config WDTE=OFF           //Reinicio repetitivo del pic
#pragma config PWRTE=ON           //espera de 72 ms al iniciar el pic
#pragma config MCLRE=OFF          //El pin MCLR se utiliza como entrada/salida
#pragma config CP=OFF             //Sin protección de código
#pragma config CPD=OFF            //Sin protección de datos
    
#pragma config BOREN=OFF //Sin reinicio cuando el input voltage es inferior a 4V
#pragma config IESO=OFF  //Reinicio sin cambio de reloj de interno a externo
#pragma config FCMEN=OFF //Cambio de reloj externo a interno en caso de fallas
#pragma config LVP=ON    //Programación en low voltage permitida
    
//CONFIGURATION WORD 2
#pragma config WRT=OFF //Proteccion de autoescritura por el programa desactivada
#pragma config BOR4V=BOR40V //Reinicio abajo de 4V 

void __interrupt() isr(void){       //apartado de interrupciones
       
    if (PIR1bits.ADIF == 1){        //revisar la bandera de interrupcion por ADC

    if (ADCON0bits.CHS == 12){      //multiplexion entre los pot
        
        CCPR1L = (ADRESH>>1)+124;//para que el servo1 pueda girar 180 grados
        CCP1CONbits.DC1B1 = ADRESH & 0b01; //añadir precision/resolucion
        CCP1CONbits.DC1B0 = (ADRESL>>7);
        ADCON0bits.CHS = 10; //se cambia al canal del segundo pot

    }
    else if (ADCON0bits.CHS == 10){     //multiplexion
        
        CCPR2L = (ADRESH>>1)+124;//para que el servo0 pueda girar 180 grados
        CCP2CONbits.DC2B1 = ADRESH & 0b01;//añadir precision/resolucion
        CCP2CONbits.DC2B0 = (ADRESL>>7);
        ADCON0bits.CHS = 12;//se cambia a canal del primer pot
        
    }
    __delay_us(50);   //delay de 50 us
    PIR1bits.ADIF = 0;      //apagar la bandera de interrupcion por ADC
        ADCON0bits.GO = 1;      //set para el GO que permite hacer la conversion

    }
}
void main(void) {
    //configuraciones
    
    //configuracion reloj
    OSCCONbits.IRCF = 0b0111;//0111, Frecuencia de reloj 8 MHz
    OSCCONbits.SCS   = 1;//reloj interno
    
    //configuracion de entradas y salidas
    ANSELH = 0b010100;     //entradas/salidas digitales
    ANSEL  = 0;
    
    TRISB  = 3;     //entradas
    TRISA  = 0;     //salidas
    TRISC  = 0;
    TRISD  = 0;
    
    PORTA  = 0;     //borrar los puertos
    PORTB  = 0;
    PORTC  = 0;
    PORTD  = 0;
    
    //configuracion del ADC
    ADCON0 = 0b00110011; 
    ADCON1 = 0b00000000;
    
    __delay_us(50);   //delay de 50 us
    
    //configuracion pwm
    //ccp1
    TRISCbits.TRISC2 = 1;      //CCP1 como entrada;
    PR2 = 250;                 //valor para que el periodo pwm sea 2 ms 
    CCP1CONbits.P1M = 0;       //config pwm
    CCP1CONbits.CCP1M = 0b1100;
    CCPR1L = 0x0f;             //ciclo de trabajo inicial
    CCP1CONbits.DC1B = 0;
    //ccp2
    TRISCbits.TRISC1 = 1;      //CCP2 como entrada;
    CCP2CONbits.CCP2M = 0b1100;//config pwm
    CCPR2L = 0x0f;             //ciclo de trabajo inicial
    CCP2CONbits.DC2B1 = 0;
    
    //configuracion tmr2
    PIR1bits.TMR2IF = 0; //se apaga la bandera de interrupcion del tmr2
    T2CONbits.T2CKPS = 0b11;//prescaler 1:16
    T2CONbits.TMR2ON = 1;//se enciende el tmr2
    
    while(PIR1bits.TMR2IF == 0);//esperar un ciclo de tmr2
    PIR1bits.TMR2IF = 0;
    TRISCbits.TRISC2 = 0;//out pwm2
    TRISCbits.TRISC1 = 0;//out pwm1
    
    //configuracion interrupciones
    INTCONbits.GIE  = 1;
    INTCONbits.RBIE = 1;
    //INTCONbits.T0IE = 1;
    INTCONbits.PEIE = 1;
    PIE1bits.ADIE   = 1;

    while (1){}
}