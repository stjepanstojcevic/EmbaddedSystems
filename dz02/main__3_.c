#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "serial.h"

volatile uint8_t flag = 0;

void beep(void) {
	if(flag == 1){
		PORTD ^= (1 << PORTD4);
	} else
		PORTD &= ~(1 << PORTD4);
	_delay_ms(100);
}

ISR(ANALOG_COMP_vect) {
	if(ACSR & (1 << ACO))
		flag = 1;
	else
		flag = 0;
}


int main(void) {
	
	uart_init();
	sei();
	
	DDRD |= (1 << DDD4);
	
	ACSR |= (1 << ACIE);
	
	while(1) {
		beep();
	}
	
	return 0;
}