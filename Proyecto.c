#include<xc.h> //Libreria para PIC
#include<stdio.h> //Libreria adicional para manejo de decimales
#include "LibLCDXC8.h" //Libreria para uso de LCD

#define _XTAL_FREQ 8000000
#pragma config FOSC=INTOSC_EC //Sentencia para usar oscilador externo
#pragma config WDT=OFF //Apagar el perro guardian
#pragma config PBADEN=OFF 
#pragma config LVP=OFF
#define TRIG RE0
#define ECHO RC2

unsigned char Boton = 0; //Variable para leer Teclado
unsigned char Distancia;


unsigned char LeerTeclado(void);//Declarar funcion para lectura de matricial
void Transmitir(unsigned char);

void main(void) {
    TRISB=0;
    TRISD=0;
    TRISC=0;
    TRISE=0;
    OSCCON=0b01111110;
    __delay_ms(1);
    TXSTA=0b00100000;
    RCSTA=0b10000000;
    BAUDCON=0b00000000;
    SPBRG=12;
    T1CON=0b10011000;
    DireccionaLCD(0x80);
    MensajeLCD_Var("Bienvenido al");
    DireccionaLCD(0xC0);
    MensajeLCD_Var("Sistema de");
    __delay_ms(1500);
    BorraLCD();
    MensajeLCD_Var("Deteccion de");
    DireccionaLCD(0xC0);
    MensajeLCD_Var("distancias");
    __delay_ms(1500);
    BorraLCD();
    MensajeLCD_Var("Iniciando");
    __delay_ms(1000);
    EscribeLCD_c('.');
    __delay_ms(1000);
    EscribeLCD_c('.');
    __delay_ms(1000);
    EscribeLCD_c('.');
    __delay_ms(1000);
    MensajeLCD_Var("Distancia:");//redirigir el lcd al registro 0x8a
    while(1){
        CCP1CON=0b00000100;
        TMR1=0;
        CCP1IF=0;
        TRIG=1;
        __delay_us(10);
        TRIG=0;
        while(ECHO==0);
        /*
        while(ECHO==1){
            __delay_us(10);
            t++;
        }
        d=t/5.8;*/
        TMR1ON=1;
        while(CCP1IF==0);
        TMR1ON=0;
        Distancia=CCPR1/58;
        Transmitir(Distancia/100 + 48);
        Transmitir((Distancia%100)/10 + 48);
        Transmitir(Distancia%10 + 48);
        __delay_ms(1000);    
    }  
}

unsigned char LeerTeclado(void){
    while(RB4==1 && RB5==1 && RB6==1 && RB7==1);
    Verificador = 1;
    LATB=0b11111110;
    if(RB4==0){
        if(!Potencia) return '+';
        else if(Potencia) return '^';
    }
    else if(RB5==0) return '=';
    else if(RB6==0) return '0';
    else if(RB7==0) return 'C';
    else{
    LATB=0b11111101;
    if(RB4==0){
        if(!Factorial) return '-';
        else if(Factorial) return '!';
    }
    else if(RB5==0) return '9';
    else if(RB6==0) return '8';
    else if(RB7==0) return '7';
    else{
    LATB=0b11111011;
    if(RB4==0) return 'x';
    else if(RB5==0) return '6';
    else if(RB6==0) return '5';
    else if(RB7==0) return '4';
    else{
    LATB=0b11110111;
    if(RB4==0) return '/';
    else if(RB5==0) return '3';
    else if(RB6==0) return '2';
    else if(RB7==0) return '1';
    }
    }
    }
    return '?';
}

void Transmitir(unsigned char BufferT){
  while(TRMT==0);
  TXREG=BufferT;  
}
