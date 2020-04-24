#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "serial.h"


int main(void) {
	
	uart_init();
	
	
	
	ADCSRB |= (1 << ACME);
	ADCSRA &= ~(1 << ADEN);
	ADMUX |= (1 << REFS1) | (1 << REFS0) | (1 << MUX1) | (1 << MUX0);
	ACSR |= (1 << ACBG);
	
	while(1) {
		
		if(ACSR & (1 << ACO))
			uart_putstr("Ispod 1.1V\n");
		else
			uart_putstr("Iznad 1.1V\n");
		
		_delay_ms(500);
	}
	
	return 0;
}