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

unsigned char Tecla = 0; //Variable para leer Teclado
unsigned char Distancia; //Resultado del sensor
int Minimo;
int Minimo_Consola[5];
//Distancia que dispara la alarma

void __interrupt() ISR(void); //Interrupción para leer teclado
void Transmitir(unsigned char); //Transmitir por consola
unsigned int Recibir(void); //Recibir por consola
void TransmitirDatos(unsigned int);
void Recibir_Cadena(char*, unsigned int);
void Transmitir_Cadena(char *info);

void main(void) {
    BorraLCD();
    RBPU = 0;
    TRISB0 = 1;
    TRISD = 0;
    TRISC = 0b11010010;
    OSCCON = 0b01110000;
    __delay_ms(1);
    TXSTA = 0b00100000; //Configuraci?n del transmisor, habilitaci?n del transmisor y modo asincr?nico, bajas velocidades
    RCSTA = 0b10010000;
    BAUDCON = 0b00000000;
    SPBRG = 12;
    ADCON1 = 15;
    RCIE = 1;
    RCIF = 0;
    GIE = 1;
    INT0IE = 1;
    INT0IF = 0;
    INTEDG0 = 1;


    T1CON = 0b10010000;
    USBEN = 0;
    UTRDIS = 1; //inhabilita el transciever
    //Configuración del PWM
    CCP1CON = 0b00001100;
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
    MensajeLCD_Var("Ingrese Dist. de");
    DireccionaLCD(0xC0);
    MensajeLCD_Var("alarma: ");
    Recibir_Cadena(Minimo_Consola, 5);
    MensajeLCD_Var("cm");
    EscribeLCD_c(' ');
    Transmitir_Cadena("Distancia de alarma configurada: ");
    Transmitir(Minimo / 100 + 48);
    Transmitir((Minimo % 100) / 10 + 48);
    Transmitir(Minimo % 10 + 48);
    Transmitir_Cadena("cm \n\n");
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
    TMR2ON = 1;
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
        TransmitirDatos(Minimo);

        if (Distancia <= Minimo) {
            CCPR1L = 125;
        } else {
            CCPR1L = 0;
        }
    }
}

void Transmitir(unsigned char BufferT) {
    while (TRMT == 0);
    TXREG = BufferT;
}

void Transmitir_Cadena(char *info){          			// Funcion para transmitir una cadena de caracteres
    while(*info)
    {
        Transmitir(*info++);
    }
}

unsigned int Recibir(void) {
    while (RCIF == 0);
    if (OERR == 1) {
        CREN = 0;
        CREN = 1;
    }
    RCIF = 0;
    return RCREG;
}

void Recibir_Cadena(char* Buffer, unsigned int tamano) {
    unsigned int cont_buf = 0;
    char c;
    int valor = 0;
    Minimo = 0;
    for (int i = 0; i < 3; i++) {
        c = Recibir();
        Buffer[i] = c;
        valor = Buffer[i] - 48;
        EscribeLCD_c(c);
        if (i == 0) {
            valor = valor * 100;
            Minimo = Minimo + valor;
        } else if (i == 1) {
            valor = valor * 10;
            Minimo = Minimo + valor;
        } else if (i == 2) {
            Minimo = Minimo + valor;
        }
    }
    Buffer[tamano - 2] = '\0';
    cont_buf = 0;
}

void TransmitirDatos(unsigned int Minimo) {
    unsigned int DistC = Distancia, MinC = Minimo;
    Transmitir_Cadena("Distancia de alarma: ");

    Transmitir(Minimo / 100 + 48);
    Transmitir((Minimo % 100) / 10 + 48);
    Transmitir(Minimo % 10 + 48);
    Transmitir_Cadena("cm  ");
    
    
    Transmitir_Cadena("Distancia: ");

    Transmitir(Distancia / 100 + 48);
    Transmitir((Distancia % 100) / 10 + 48);
    Transmitir(Distancia % 10 + 48);
    Transmitir_Cadena("cm  ");
    
    if(Distancia<=Minimo){
        Transmitir_Cadena("���Alarma Activada!!!");
    }
    Transmitir('\n');

    DireccionaLCD(0xC7);
    EscribeLCD_c(Distancia / 100 + 48);
    EscribeLCD_c((Distancia % 100) / 10 + 48);
    EscribeLCD_c(Distancia % 10 + 48);
    MensajeLCD_Var("cm");
    EscribeLCD_c(' ');
    __delay_ms(200);
}

void __interrupt() ISR(void) {
    if (INT0IF == 1) {
        __delay_ms(1000);
        BorraLCD();
        MensajeLCD_Var("Nueva dist. de");
        DireccionaLCD(0xC0);
        MensajeLCD_Var("alarma: ");
        Recibir_Cadena(Minimo_Consola, 5);
        MensajeLCD_Var("cm");
        EscribeLCD_c(' ');
        __delay_ms(2000);
        BorraLCD();
        MensajeLCD_Var("Nueva Distancia");
        DireccionaLCD(0xC0);
        MensajeLCD_Var("Configurada");
        Transmitir_Cadena("\nNueva distancia de alarma configurada: ");
        Transmitir(Minimo / 100 + 48);
        Transmitir((Minimo % 100) / 10 + 48);
        Transmitir(Minimo % 10 + 48);
        Transmitir_Cadena("cm  \n\n");
        __delay_ms(1000);
        BorraLCD();
        MensajeLCD_Var("Distancia:");  
    }
    INT0IF = 0;
}