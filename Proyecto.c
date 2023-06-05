#include<xc.h> //Libreria para PIC
#include<stdio.h> //Libreria adicional para manejo de decimales
#include "LibLCDXC8.h" //Libreria para uso de LCD

#define _XTAL_FREQ 8000000
#pragma config FOSC=INTOSC_EC //Sentencia para usar oscilador externo
#pragma config WDT=OFF //Apagar el perro guardian
#pragma config PBADEN=OFF 
#pragma config LVP=OFF
#pragma config MCLRE=ON
#define TRIG RE0
#define ECHO RC2

unsigned char Tecla = 0; //Variable para leer Teclado
unsigned char Distancia;
unsigned char Minimo;


void interrupt() ISR(void);
unsigned char LeerTeclado(void);//Declarar funcion para lectura de matricial
void Transmitir(unsigned char);
unsigned char Recibir(void);


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
        /*while(ECHO==1){
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
    LATB=0b11111110;
    if(RB5==0) return '=';
    else if(RB6==0) return '0';
    else if(RB7==0) return 'C';
    else{
    LATB=0b11111101;
    if(RB5==0) return '9';
    else if(RB6==0) return '8';
    else if(RB7==0) return '7';
    else{
    LATB=0b11111011;
    if(RB5==0) return '6';
    else if(RB6==0) return '5';
    else if(RB7==0) return '4';
    else{
    LATB=0b11110111;
    if(RB5==0) return '3';
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

unsigned char Recibir(void){
    while(RCIF==0);
    return RCREG;
}

void TransmitirDatos(unsigned int Ent1, unsigned int Ent2) {
    unsigned int DistC=Distancia;
    Transmitir('D');
    Transmitir('i');
    Transmitir('s');
    Transmitir('t');
    Transmitir('a');
    Transmitir('n');
    Transmitir('c');
    Transmitir('i');
    Transmitir('a');
    Transmitir(':');
    Transmitir(' ');
    

    Transmitir(Distancia/100 + 48);
    Transmitir((Distancia%100)/10 + 48);
    Transmitir(Distancia%10 + 48);
    
    DireccionaLCD(0xC0);
    EscribeLCD_c(Distancia/100 + 48);
    EscribeLCD_c((Distancia%100)/10 + 48);
    EscribeLCD_c(Distancia%10 + 48);
    MensajeLCD_Var("cm");
    EscribeLCD_c(' ');

}

void interrupt ISR(void){
    if(RBIF==1){
        if(PORTB!=0b11110000){
            Tecla=16;
            LATB=0b11111110;
            if(RB5==0) Tecla="1";
            else if(RB6==0) Tecla="2";
            else if(RB7==0) Tecla="3";
            else{
                LATB=0b11111101;
                if(RB5==0) Tecla="5";
                else if(RB6==0) Tecla="6";
                else if(RB7==0) Tecla="7";
                else{
                    LATB=0b11111011;
                    if(RB5==0) Tecla="9";
                    else if(RB6==0) Tecla="10";
                    else if(RB7==0) Tecla="11";
                    else{
                        LATB=0b11110111;
                        if(RB5==0) Tecla="13";
                        else if(RB6==0) Tecla="14";
                        else if(RB7==0) Tecla="15";
                    }
                }
            }
            LATB=0b11110000;
        }
        __delay_ms(100);
        RBIF=0;
    }
}

