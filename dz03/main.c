#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "serial.h"
#include "encoder.h"
#include "motor.h"
#include "counter.h"

volatile uint64_t ms = 0;

volatile int32_t P, I, D;

volatile int64_t err = 0;
volatile int64_t err_old = 0;
volatile int64_t err_acc = 0;
volatile int64_t err_diff = 0;

volatile int64_t gain = 0;
volatile int16_t speed = 0;
volatile int64_t max = 1023;

volatile int16_t tps = 0;
volatile int16_t tps_h = 0;
volatile int16_t tps_m = 0;

volatile int8_t sendToPlotter = 0;

volatile int64_t ref_speed1 = 1500;
volatile int64_t ref_speed2 = 1000;

volatile int64_t delay_sec = 2;
volatile int64_t delay_msec = 2000;

void reset_errors(void) {
	err = 0;
	err_old = 0;
	err_acc = 0;
	err_diff = 0;
}

ISR(TIMER0_COMPA_vect){
	++ms;
	if(ms % delay_msec == 0){
		if(tps_h == ref_speed1/delay_sec){
			tps = (ref_speed2-ref_speed1)/delay_sec;
			tps_h = ref_speed2/delay_sec;
		} else {
			tps = (ref_speed1/delay_sec) - tps_h;
			tps_h = ref_speed1/delay_sec;
		}
	}
		
	if(ms % 10 == 0)
		sendToPlotter = 1;
		
	if(ms % 10 == 0){
		tps_m = (ticks - ticks_old) * 1000 / 10;
		
		err = tps - tps_m;
		err_acc += err;
		err_diff = (err - err_old);
		gain = P * err + I * err_acc + D * err_diff;
		gain = (gain / 21);
		
		if (gain > 0){
			gain += 300;
			speed = gain > max ? max : gain;
			OCR1A = speed;
			OCR1B = 0;
		} else {
			gain -= 300;
			gain *= -1;
			speed = gain > max ? max : gain;
			OCR1B = speed;
			OCR1A = 0;
		}
		
		err_old = err;
		ticks_old = ticks;
	}
}


int main(void) {
	
	uart_init();
	encoder_init();
	motor_init();
	counter_init();
	
	P = 12;
	I = 4;
	D = 7;
	
	while(1) {
		if(sendToPlotter){
			uart_putint(tps);
			uart_putstr(" ");
			uart_putint(tps_m);
			uart_putstr("\n");
		}
	}
	
	return 0;
}