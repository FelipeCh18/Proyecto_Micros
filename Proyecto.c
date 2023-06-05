#include<xc.h> //Libreria para PIC
#include<stdio.h> //Libreria adicional para manejo de decimales
#include "LibLCDXC8.h" //Libreria para uso de LCD

#define _XTAL_FREQ 8000000
#pragma config FOSC=INTOSC_EC //Sentencia para usar oscilador externo
#pragma config WDT=OFF //Apagar el perro guardian
#pragma config PBADEN=OFF //Apaga funciones análogas
#pragma config LVP=OFF //Apaga modo de programación con bajo consumo
#pragma config MCLRE=ON //Habilita MCLR
#define TRIG RE0 //Pin de trigger del ultrasonido
#define ECHO RC2 //Pin de echo del ultrasonido

unsigned char Tecla = 0; //Variable para leer Teclado
unsigned char Distancia; //Resultado del sensor
unsigned char Minimo; //Distancia que dispara la alarma

void interrupt() ISR(void); //Interrupción para leer teclado
void Transmitir(unsigned char); //Transmitir por consola
unsigned char Recibir(void); //Recibir por consola


void main(void) {
    TRISB=0;
    TRISD=0;
    TRISC=0;
    TRISE=0;
    OSCCON=0b01111110;
    __delay_ms(1);
    TXSTA=0b00100000;//Config transmisión
    RCSTA=0b10000000; //Config recepción
    BAUDCON=0b00000000; //Velocidad de comunicación
    SPBRG=12; //1.201 rate con error de -0.16%
    T1CON=0b10011000; //Timer 1
    //Configuración del PWM
    TMR2 = 0x0 //Empieza a contar desde 0

    //Fin config PWM
    DireccionaLCD(0x80);
    MensajeLCD_Var("Bienvenido al");
    DireccionaLCD(0xC0);
    MensajeLCD_Var("sistema de");
    __delay_ms(1500);
    BorraLCD();
    MensajeLCD_Var("deteccion de");
    DireccionaLCD(0xC0);
    MensajeLCD_Var("distancias");
    __delay_ms(1500);
    BorraLCD();
    MensajeLCD_Var("Iniciando");
    __delay_ms(500);
    EscribeLCD_c('.');
    __delay_ms(500);
    EscribeLCD_c('.');
    __delay_ms(500);
    EscribeLCD_c('.');
    __delay_ms(500);
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
        if (interruptor==1) Minimo=Recibir(); //1->Consola
        //else Minimo= teclado xd
    }  
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
            if(RB5==0) Tecla='=';
            else if(RB6==0) Tecla='0';
            else if(RB7==0) Tecla='C';
            else{
                LATB=0b11111101;
                if(RB5==0) Tecla='9';
                else if(RB6==0) Tecla='8';
                else if(RB7==0) Tecla='7';
                else{
                    LATB=0b11111011;
                    if(RB5==0) Tecla='6';
                    else if(RB6==0) Tecla='5';
                    else if(RB7==0) Tecla='4';
                    else{
                        LATB=0b11110111;
                        if(RB5==0) Tecla='3';
                        else if(RB6==0) Tecla='2';
                        else if(RB7==0) Tecla='1';
                    }
                }
            }
            LATB=0b11110000;
        }
        __delay_ms(100);
        RBIF=0;
    }
}