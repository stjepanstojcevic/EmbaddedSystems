#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "serial.h"

volatile uint8_t pin = MUX2;
volatile uint8_t us_measurement;

void ir_measure(void) {
	ADMUX |= (1 << REFS0) | (1 << pin);
	ADCSRA |= (1 << ADSC) | (1 << ADIE);
}

ISR(ADC_vect) {
	if((1 << pin) == ((ADMUX & (1 << MUX0)) | (ADMUX & (1 << MUX1)) | (ADMUX & (1 << MUX2)) | (ADMUX & (1 << MUX3)))){
		us_measurement = ADC;
		uart_putint(us_measurement);
	}
}

int main(void) {
		
	uart_init();
		
	sei();
		
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADCSRA |= (1 << ADEN);

	while(1) {
		ir_measure();
		_delay_ms(500);
	}

	return 0;
}