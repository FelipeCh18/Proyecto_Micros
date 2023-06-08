#include<xc.h> //Libreria para PIC
#include<stdio.h> //Libreria adicional para manejo de decimales
#include "LibLCDXC8.h" //Libreria para uso de LCD

#define _XTAL_FREQ 8000000
#pragma config FOSC=INTOSC_EC //Sentencia para usar oscilador externo
#pragma config WDT=OFF //Apagar el perro guardian
#pragma config PBADEN=OFF //Apaga funciones análogas
#pragma config LVP=OFF //Apaga modo de programación con bajo consumo
#pragma config MCLRE=ON //Habilita MCLR
#define TRIG RC0 //Pin de trigger del ultrasonido
#define ECHO RC1 //Pin de echo del ultrasonido
#define INT RE0
#define Led RA2

unsigned char Tecla = 0; //Variable para leer Teclado
unsigned char Distancia; //Resultado del sensor
unsigned char Minimo=10.0;
unsigned char en_entrada=0;
//Distancia que dispara la alarma

void __interrupt() ISR(void); //Interrupción para leer teclado
void Transmitir(unsigned char); //Transmitir por consola
unsigned int Recibir(void); //Recibir por consola
void TransmitirDatos(unsigned int);

void main(void) {
    BorraLCD();
    TRISB = 0b11110000;
    TRISA = 0;
    RBPU = 0;
    TRISD = 0;
    TRISC = 0b11000010;
    TRISE = 0b00001010;
    OSCCON = 0b01110000;
    __delay_ms(1);
    TXSTA = 0b00100000; //Configuraci?n del transmisor, habilitaci?n del transmisor y modo asincr?nico, bajas velocidades
    RCSTA = 0b10010000;
    BAUDCON = 0b00000000;
    SPBRG = 12;
    ADCON1 = 15;
    RBIF = 0;
    RBIE = 1;
    RCIE=1;
    RCIF=0;
    GIE = 1;

    T1CON = 0b10010000;
    UTRDIS = 1; //inhabilita el transciever
    //Configuración del PWM
    CCP1CON=0b00001100;
    T2CON = 0b00000010;
    TMR2 = 0; //Empieza a contar desde 0
    PR2 = 249;
    CCPR1L = 0;
        
    //Fin config PWM
    ConfiguraLCD(4);
    InicializaLCD();
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
    BorraLCD();
    MensajeLCD_Var("Distancia:"); 
    //redirigir el lcd al registro 0x8a
    TMR2ON=1;
    while (1) {
        CCP2CON = 0b00000100;
        TMR1 = 0;
        CCP2IF = 0;
        TRIG = 1;
        __delay_us(10);
        TRIG = 0;
        while (ECHO == 0);
        /*
    while(ECHO==1){
      __delay_us(10);
      t++;
    }
    d=t/5.8;*/
        TMR1ON = 1;
        while (CCP2IF == 0);
        TMR1ON = 0;
        Distancia = CCPR2 / 58;

        if (!RE1) {
            TransmitirDatos(Minimo);
//            if(en_entrada==1) Minimo=Minimo_Teclado;  
        } else {
            TransmitirDatos(Minimo);
//            if(en_entrada==1) Minimo=Minimo_Consola;
        }
        
        if (Distancia<=Minimo){
            CCPR1L=125;
        }else{
            CCPR1L=0;
        }


        //if (interruptor==1) Minimo=Recibir(); //1->Consola
        //else Minimo= teclado xd
    }
}

void Transmitir(unsigned char BufferT) {
    while (TRMT == 0);
    TXREG = BufferT;
}

unsigned int Recibir(void) {
    while (RCIF == 0);
    return RCREG;
}

void TransmitirDatos(unsigned int Minimo) {
    unsigned int DistC = Distancia, MinC = Minimo;
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

    Transmitir(Distancia / 100 + 48);
    Transmitir((Distancia % 100) / 10 + 48);
    Transmitir(Distancia % 10 + 48);
    Transmitir('c');
    Transmitir('m');
    Transmitir(' ');
    Transmitir('\n');

    DireccionaLCD(0xC7);
    EscribeLCD_c(Distancia / 100 + 48);
    EscribeLCD_c((Distancia % 100) / 10 + 48);
    EscribeLCD_c(Distancia % 10 + 48);
    MensajeLCD_Var("cm");
    EscribeLCD_c(' ');
}

unsigned char LeerTeclado(void) {
    LATB = 0b11111110;
    if (RB5 == 0) return '=';
    else if (RB6 == 0) return '0';
    else if (RB7 == 0) return 'C';
    else {
        LATB = 0b11111101;
        if (RB5 == 0) return '9';
        else if (RB6 == 0) return '8';
        else if (RB7 == 0) return '7';
        else {
            LATB = 0b11111011;
            if (RB5 == 0) return '6';
            else if (RB6 == 0) return '5';
            else if (RB7 == 0) return '4';
            else {
                LATB = 0b11110111;
                if (RB5 == 0) return '3';
                else if (RB6 == 0) return '2';
                else if (RB7 == 0) return '1';
            }
        }
    }
    LATB = 0b11110000;
}

void __interrupt() ISR(void) {
    /*if (RBIF == 1) {
        en_entrada=1;
        __delay_ms(10);
        Led = 1;
        Tecla = LeerTeclado();
        BorraLCD();
        DireccionaLCD(0x80);
        MensajeLCD_Var("Ingrese dist. de");
        DireccionaLCD(0xC0);
        MensajeLCD_Var("alarma: ");
        DireccionaLCD(0xC8);
        EscribeLCD_c(Tecla);
        __delay_ms(3000);
        BorraLCD();
        __delay_ms(10);
        DireccionaLCD(0x80);
        MensajeLCD_Var("Distancia: ");
        DireccionaLCD(0xC7);
        
    }
    __delay_ms(100);
    RBIF = 0;
    Led = 0;*/

    if (RCIF==1) {
        en_entrada=1;
        Minimo = Recibir();
        
        MensajeLCD_Var(Minimo);
        __delay_ms(1000);
    }
}