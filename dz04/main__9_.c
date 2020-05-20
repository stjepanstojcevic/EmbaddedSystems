#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


volatile uint8_t send_response = 1;
volatile uint16_t c = 0;
volatile uint8_t pin = MUX2;
volatile uint16_t us_measurement;

void send_byte(uint16_t c) {
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = (c >> 8) & 0xFF;
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = c & 0xFF;
}

ISR(ADC_vect) {
	if((1 << pin) == ((ADMUX & (1 << MUX0)) | (ADMUX & (1 << MUX1)) | (ADMUX & (1 << MUX2)) | (ADMUX & (1 << MUX3)))){
		us_measurement = ADC;
	}
}

void ir_measure(void) {
	ADMUX |= (1 << REFS0) | (1 << pin);
	ADCSRA |= (1 << ADSC) | (1 << ADIE);
}

int main(void) {
	
	DDRB |= (1 << DDB2);
	
	UBRR0 = 103;
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
	UCSR0B |= (1 << TXEN0);
	
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADCSRA |= (1 << ADEN);
	
	sei();
	
	while(1) {
		ir_measure();
		send_byte(us_measurement);
		_delay_ms(100);
	}
	
	return 0;
}