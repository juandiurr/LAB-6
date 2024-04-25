/*
 * pre lab 6.c
 *
 * Created: 18/04/2024 16:48:16
 * Author : diego
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "Librerias/ADC.h"
volatile uint8_t uart = 0;
volatile uint8_t adc_h = 0;
volatile uint8_t letras = 0;
uint8_t reinicio = 0;
uint8_t ax = 0;
uint8_t bx = 0;
uint8_t cx = 0;
//const uint8_t mylist[] = {};

void initUART(void); //9600
void writetextUART(char* texto);
void writeUART(char caracter);

int main(void)
{
    cli();
	initUART();
	DDRB = 0xFF;
	PORTB = 0; //ESTABLECE TODO EL PUERTO D COMO SALIDA Y LO PONE EN 0
	ADC_(128);
	ADC2();
	
	sei();
	
	//writeUART('\n');
    while (1) 
    {
		//PORTB = boo;
		
		habilitar_conversion();
		if (reinicio == 0){
			writetextUART("1. Leer potenciometro");
			writeUART(10);
			writeUART(13);
			writetextUART("2. Enviar Ascii");
			writeUART(10);
			writeUART(13);
			reinicio = 1;
		}
		if (uart == '\r'){
			if (letras == 49){ //leer potenciometro (1)
				habilitar_conversion();
				ax = adc_h/100 + 48;//centecimas
				bx = (adc_h%100)/10 + 48;//decimas
				cx = adc_h%10 + 48;//unidades
				writeUART(10);
				writeUART(13);
				writetextUART("valor de potenciometro: ");
				writeUART(ax);
				writeUART(bx);
				writeUART(cx);
				writeUART(10);
				writeUART(13);
				reinicio = 0;
				letras = 0;
				} else if (letras == 50){ //mandar ASCII
					//uart = 0;
					writeUART(10);
					writeUART(13);
					do{
						
					}while(uart != '\n');
					writeUART(10);
					writeUART(13);
					reinicio = 0;
					letras = 0;
				}
				else if (letras != 0 && letras != 49 && letras != 50){
					writetextUART("error");
					writeUART(10);
					writeUART(13);
					letras = 0;
					reinicio = 0;
				}
			} else {
			letras = uart;
		}
    }
}

void initUART(void){
	DDRD &= (1<<DDD0); //RX
	DDRD |= (1<<DDD1); //TX
	//Configurar UCSR0A
	UCSR0A = 0;
	//Configurar UCSR0B
	UCSR0B = 0;
	UCSR0B |= (1<<RXCIE0); //Habilitar interrupcion de recepcion
	//UCSR0B |= (1<<TXCIE0); //Habilitar interrupcion de transmicion
	UCSR0B |= (1<<RXEN0); //Habilitar RX (recepcion)
	UCSR0B |= (1<<TXEN0); //Habilitar TX (transmición)
	//Configurar UCSR0C
	UCSR0C = 0;
	//UCSR0C &= ~((1<<UMSEL01) | (1<<UMSEL00)); //ASINCRONO
	//UCSR0C &= ~((1<<UPM01) | (1<<UPM00)); //NO PARIEDAD
	//UCSR0C &= ~(1<<USBS0); //UN BIT DE STOP, 2 BITS -> 1
	UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00); //8 BITS DE ESPACIO
	UBRR0 = 103;//Configurar velocidad del baudrate 9600
}
void writetextUART(char* texto){
	uint8_t i;
	for (i=0; texto[i]!='\0';i++){
		while (!(UCSR0A & (1<<UDRE0)));//se queda en el while hasta que la bandera UDRE0 este en 1
		UDR0 = texto[i];
	}
}

void writeUART(char caracter){
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = caracter;
}
ISR(USART_RX_vect){
	uart = UDR0;
	UDR0 = uart;
	
	if (letras == 50){
		while (!(UCSR0A & (1<<UDRE0)));
		PORTB = uart;
	} 
}
	
/*ISR(USART_TX_vect){
	UDR0 = boo;
}*/
ISR(ADC_vect){
	adc_h = ADCH;
	ADCSRA |= (1<<ADIF); //apagar bandera
}
