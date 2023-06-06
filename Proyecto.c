#include<xc.h> //Libreria para PIC
#include<stdio.h> //Libreria adicional para manejo de decimales
#include "LibLCDXC8.h" //Libreria para uso de LCD

#define _XTAL_FREQ 8000000
#pragma config FOSC=INTOSC_EC //Sentencia para usar oscilador externo
#pragma config WDT=OFF //Apagar el perro guardian
#pragma config PBADEN=OFF //Apaga funciones análogas
#pragma config LVP=OFF //Apaga modo de programación con bajo consumo
#pragma config MCLRE=ON //Habilita MCLR
#define TRIG RC1 //Pin de trigger del ultrasonido
#define ECHO RC2 //Pin de echo del ultrasonido
#define INT RE0

unsigned char Tecla = 0; //Variable para leer Teclado
unsigned char Distancia; //Resultado del sensor
unsigned char Minimo_Teclado=200;
unsigned char Minimo_Consola;
//Distancia que dispara la alarma

void __interrupt() ISR(void);
void __interrupt(low_priority) ISRL(void);//Interrupción para leer teclado
void Transmitir(unsigned char); //Transmitir por consola
unsigned char Recibir(void); //Recibir por consola
void TransmitirDatos(unsigned int);


void main(void) {
    TRISB=0b11110000;
    RBPU=0;
    TRISD=0;
    TRISC = 0b11000100;
    TRISE=0b00001010;
    OSCCON=0b01110000;
    __delay_ms(1);
    TXSTA = 0b00100000; //Configuraci?n del transmisor, habilitaci?n del transmisor y modo asincr?nico, bajas velocidades
    RCSTA = 0b10010000;
    BAUDCON=0b00000000;
    SPBRG=12;
    
    T0CON=0b00001011;//No habilita timer0, 16 bits de resolucion, reloj interno
    TMR0IF=0;// apaga bandera
    TMR0=3036; // valor pre carga
    TMR0IE=1; //Habilita la interrupcion 
    GIE=1; //habilita interrupciones globales
    TMR0ON=1;//Habilita la interrupcion Timer0, primer bit de T0CON
    
    T1CON=0b10010000;
    UTRDIS = 1;//inhabilita el transciever
    //Configuración del PWM
    TMR2 = 0; //Empieza a contar desde 0

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
        
        if(!RE1){
            
            TransmitirDatos(Minimo_Teclado);
        }else{
            TransmitirDatos(Minimo_Consola);
        }
        
        
        //if (interruptor==1) Minimo=Recibir(); //1->Consola
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

void TransmitirDatos(unsigned int Minimo) {
    unsigned int DistC=Distancia, MinC=Minimo;
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
    Transmitir('c');
    Transmitir('m');
    Transmitir(' ');
    
    DireccionaLCD(0xC7);
    EscribeLCD_c(Distancia/100 + 48);
    EscribeLCD_c((Distancia%100)/10 + 48);
    EscribeLCD_c(Distancia%10 + 48);
    MensajeLCD_Var("cm");
    EscribeLCD_c(' ');
}

unsigned char LeerTeclado(void){
    while(RB4==1 && RB5==1 && RB6==1 && RB7==1);
    Verificador = 1;
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
}

void __interrupt() ISR(void){
    if(TMR0IF){
        TMR0IF=0;
        RE2 = !RE2;
        TMR0 = 3036;//Precarga 2^n - Tsobreflujo*Fbus_Timer0/PreScaler
        //Tuvo que usarse una resolucion de 16 bits y un PS de  para lograr el valor deseado
    }
    if(RCIF){
        Minimo_Consola=Recibir();
        __delay_ms(100); 
    }            
}

void __interrupt(low_priority) ISRL(void){
    if(RBIF==1){
        unsigned char DistAlarma=[];
        BorraLCD();
        MensajeLCD_Var("Ingrese dist. de");
        DireccionaLCD(0xC0);
        MensajeLCD_Var("alarma: ");
        Tecla=LeerTeclado();
        DireccionaLCD(0xC8);
        EscribeLCD_c(Tecla);
        }
        __delay_ms(100);
        RBIF=0;
    }
}