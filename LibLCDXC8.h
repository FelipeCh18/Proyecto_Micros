/* 
 * File:   LibLCDXC8.h
 * Author: Robin
 *
 * Created on 2 de septiembre de 2018, 08:15 PM
 */

#ifndef LIBLCDXC8_H
#define	LIBLCDXC8_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif
#include<xc.h>
#include<string.h>
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 20000000
#endif
#ifndef Datos
#define Datos LATD	//El puerto de conexi�n de los datos el cual se puede cambiar
#endif
#ifndef RS
#define RS LATD2	//Los pines de control al LCD los cuales se
#endif
#ifndef E
#define E LATD3	//pueden cambiar
#endif

unsigned char interfaz=8;

void ConfiguraLCD(unsigned char);
void RetardoLCD(unsigned char);
void EnviaDato(unsigned char);
void InicializaLCD(void);
void HabilitaLCD(void);
void BorraLCD(void);
void ComandoLCD(char);
void EscribeLCD_c(unsigned char);
void MensajeLCD_Var(char *);
void DireccionaLCD(unsigned char);
//void NuevoCaracter(unsigned char, unsigned char);

void ConfiguraLCD(unsigned char a){
	if(a==4 | a ==8)
		interfaz=a;	
}
void EnviaDato(unsigned char a){
	if(interfaz==4){
		Datos=(Datos & 0b00001111) | (a & 0b11110000);
		HabilitaLCD();
		RetardoLCD(1);
		Datos=(Datos & 0b00001111) | (a<<4);
	}else if(interfaz==8){
		Datos=a;
	}	
}
void InicializaLCD(void){
//Funci�n que inicializa el LCD caracteres
	RS=0;
	if(interfaz==4)
		Datos=(Datos & 0b00001111) | 0x30;
	else	
		Datos=0x3F;
	HabilitaLCD();
	RetardoLCD(1);
	if(interfaz==4)
		Datos=(Datos & 0b00001111) | 0x30;
	else	
		Datos=0x3F;
	HabilitaLCD();
	RetardoLCD(3);
	if(interfaz==4)
		Datos=(Datos & 0b00001111) | 0x30;
	else	
		Datos=0x3F;
	HabilitaLCD();
	RetardoLCD(4);
	if(interfaz==4){
		Datos=(Datos & 0b00001111) | 0x20;
		HabilitaLCD();
		RetardoLCD(4);
		EnviaDato(0x2F);
		HabilitaLCD();
		RetardoLCD(4);
	}
	BorraLCD();
	EnviaDato(0xF);
	HabilitaLCD();
	RetardoLCD(4);	
}
void HabilitaLCD(void){
//Funci�n que genera los pulsos de habilitaci�n al LCD 	
	E=1;
	__delay_us(40);
    //Delay1TCY();
	E=0;
}
void BorraLCD(void){
//Funci�n que borra toda la pantalla	
	RS=0;
	EnviaDato(0x01);
	HabilitaLCD();
	RetardoLCD(2);
}
/* void CursorAInicio(){
	//Funci�n que lleva el cursor a la primera posici�n o la de la
	//primera l�nea mas a la izquierda
	DireccionaLCD(0x80);	
	}
	*/
void EscribeLCD_c(unsigned char a){
//Funci�n que escribe un caracter en la pantalla
//a es un valor en codigo ascii
//Ejemplo EscribeLCD_c('A');
	RS=1;
	EnviaDato(a);
	HabilitaLCD();
	RetardoLCD(4);
}
/*void EscribeLCD_n8(unsigned char a,unsigned char b){
	//Funci�n que escribe un n�mero positivo de 8 bits en la pantalla
	//a es el n�mero a escribir, el cual debe estar en el rango de 0 a 255
	//b es el n�mero de digitos que se desea mostrar empezando desde las unidades
	//Ejemplo EscribeLCD_n8(204,3);	
		unsigned char centena,decena,unidad;
		RS=1;
		switch(b){
			case 1: unidad=a%10;
					EnviaDato(unidad+48);
					HabilitaLCD();
					RetardoLCD(4);
					break;
			case 2:	decena=(a%100)/10;
					unidad=a%10;
					EnviaDato(decena+48);
					HabilitaLCD();
					RetardoLCD(4);
					EnviaDato(unidad+48);
					HabilitaLCD();
					RetardoLCD(4);
					break;
			case 3: centena=a/100;
					decena=(a%100)/10;
					unidad=a%10;
					EnviaDato(centena+48);
					HabilitaLCD();
					RetardoLCD(4);
					EnviaDato(decena+48);
					HabilitaLCD();
					RetardoLCD(4);
					EnviaDato(unidad+48);
					HabilitaLCD();
					RetardoLCD(4);
					break;
			default: break;
		}
	}
	void EscribeLCD_n16(unsigned int a,unsigned char b){
	//Funci�n que escribe un n�mero positivo de 16 bits en la pantalla
	//a es el n�mero a escribir, el cual debe estar en el rango de 0 a 65535
	//b es el n�mero de digitos que se desea mostrar empezando desde las unidades
	//Ejemplo EscribeLCD_n16(12754,5);	
		unsigned char decena,unidad;
		unsigned int centena,millar;
		RS=1;
		switch(b){
			case 1: unidad=a%10;
					EscribeLCD_c(unidad+48);
					break;
			case 2:	decena=(a%100)/10;
					unidad=a%10;
					EscribeLCD_c(decena+48);
					EscribeLCD_c(unidad+48);
					break;
			case 3: centena=(a%1000)/100;
					decena=(a%100)/10;
					unidad=a%10;
					EscribeLCD_c(centena+48);
					EscribeLCD_c(decena+48);
					EscribeLCD_c(unidad+48);
					break;
			case 4: millar=(a%10000)/1000;
					centena=(a%1000)/100;
					decena=(a%100)/10;
					unidad=a%10;
					EscribeLCD_c(millar+48);
					EscribeLCD_c(centena+48);
					EscribeLCD_c(decena+48);
					EscribeLCD_c(unidad+48);
					break;
			case 5: EscribeLCD_c(a/10000 +48);
					millar=(a%10000)/1000;
					centena=(a%1000)/100;
					decena=(a%100)/10;
					unidad=a%10;
					EscribeLCD_c(millar+48);
					EscribeLCD_c(centena+48);
					EscribeLCD_c(decena+48);
					EscribeLCD_c(unidad+48);
					break;
			default: break;
		}	
	}
	void EscribeLCD_d(double num, unsigned char digi, unsigned char digd){
		
	}
	*/
void MensajeLCD_Var(char* a){
//Funci�n que escribe una cadena de caracteres variable en la pantalla
//a es una cadena de caracteres guardada en una variable *char
//Ejemplo: char aux[4]="Hola"; MensajeLCD_Var(aux);
    for(int i = 0; i<strlen(a); i++){
        if(a[i] == NULL){
            break;
        }
        EscribeLCD_c(a[i]);
    }
}
    
void DireccionaLCD(unsigned char a){
//Funci�n que ubica el cursor en una posici�n especificada
//a debe ser una direcci�n de 8 bits valida de la DDRAM o la CGRAM	
	RS=0;
	EnviaDato(a);
	HabilitaLCD();
	RetardoLCD(4);
}
/* void FijaCursorLCD(unsigned char fila,unsigned char columna){
	//Funci�n que ubica el cursor en una fila y columna especificadas
	//fila es un valor positivo que especifica la posici�n del cursor de 1 a 4
	//columna es un valor positivo que especifica la posici�n del cursor de 1 a 80
	//Si el display es de una fila, Ej: 8x1, tiene 80 columnas
	//Si el display es de dos filas, Ej: 20x2, tiene 40 columnas
	//Si el display es de cuatro filas, Ej: 20x4, tiene 20 columnas
		
	}
	void DesplazaPantallaD(void){
	//Funci�n que desplaza una sola vez la pantalla a la derecha	
		
	}
	void DesplazaPantallaI(void){
	//Funci�n que desplaza una sola vez la pantalla a la izquierda
		
	}
	void DesplazaCursorD(void){
	//Funci�n que desplaza una sola vez la pantalla a la derecha
		
	}
	void DesplazaCursorI(void){
	//Funci�n que desplaza una sola vez la pantalla a la izquierda
		
	}
 */

void RetardoLCD(unsigned char a){
	switch(a){
		case 1: __delay_ms(15);
                //Delay100TCYx(38); //Retardo de mas de 15 ms
				break;
		case 2: __delay_ms(1);
                __delay_us(640);
                //Delay10TCYx(41); //Retardo de mas de 1.64 ms
				break;
		case 3: __delay_us(100);
                //Delay10TCYx(3);	//Retardo de mas de 100 us
				break;
		case 4: __delay_us(40);
                //Delay10TCYx(1); //Retardo de mas de 40 us
				break;
		default:
				break;
	}
}
void ComandoLCD(char a){
//Funci�n que envia cualquier comando al LCD
	RS=0;
    Datos=a&0xF0;
    HabilitaLCD();
    Datos=(a&0xF0)<<4;
    HabilitaLCD();
    RetardoLCD(2);
    RetardoLCD(2);
}
/*void NuevoCaracter(unsigned char ubicacion, unsigned char mapeo[]){
    int i;
    ComandoLCD(0x40 + (ubicacion * 8));
	for (i=0;i<8;i++){
		EscribeLCD_c(mapeo [i]);
	}	
}*/
#endif	/* LIBLCDXC8_H */
