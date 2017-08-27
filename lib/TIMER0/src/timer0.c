/*
Copyright (c) 2017 NicoHood
See the readme for credit to other people.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

/* Pin functions for the Teensy and Teensy++
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2008-2010 PJRC.COM, LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "timer0.h"
#include "timer0_private.h"

void timer0_init(void)
{
	// Timer 0, fast pwm mode
	TCCR0A = (1<<WGM01) | (1<<WGM00);
	TCCR0B = (1<<CS01) | (1<<CS00);		// div 64 prescaler
	TIMSK0 |= (1 << TOIE0);
}

volatile unsigned long timer0_micros_count = 0;
volatile unsigned long timer0_millis_count = 0;
volatile unsigned char timer0_fract_count = 0;


ISR(TIMER0_OVF_vect, ISR_NAKED)
{
	asm volatile(
		"push	r24"				"\n\t"
		"in	r24, __SREG__"			"\n\t"
		"push	r24"				"\n\t"

		"lds	r24, timer0_fract_count"	"\n\t"
		"subi	r24, 256 - %0"			"\n\t"
		"cpi	r24, 125"			"\n\t"
		"brsh	L_%=_fract_roll"		"\n\t"

	"L_%=_fract_noroll:"				"\n\t"
		"sts	timer0_fract_count, r24"	"\n\t"
		"lds	r24, timer0_millis_count"	"\n\t"
		"subi	r24, 256 - %1"			"\n\t"
		"sts	timer0_millis_count, r24"	"\n\t"
		"brcs	L_%=_ovcount"			"\n\t"

	"L_%=_millis_inc_sext:"
		"lds	r24, timer0_millis_count+1"	"\n\t"
		"sbci	r24, 255"			"\n\t"
		"sts	timer0_millis_count+1, r24"	"\n\t"
		"brcs	L_%=_ovcount"			"\n\t"
		"lds	r24, timer0_millis_count+2"	"\n\t"
		"sbci	r24, 255"			"\n\t"
		"sts	timer0_millis_count+2, r24"	"\n\t"
		"brcs	L_%=_ovcount"			"\n\t"
		"lds	r24, timer0_millis_count+3"	"\n\t"
		"sbci	r24, 255"			"\n\t"
		"sts	timer0_millis_count+3, r24"	"\n\t"
		"rjmp	L_%=_ovcount"			"\n\t"

	"L_%=_fract_roll:"				"\n\t"
		"subi	r24, 125"			"\n\t"
		"sts	timer0_fract_count, r24"	"\n\t"
		"lds	r24, timer0_millis_count"	"\n\t"
		"subi	r24, 256 - %1 - 1"		"\n\t"
		"sts	timer0_millis_count, r24"	"\n\t"
		"brcc	L_%=_millis_inc_sext"		"\n\t"

	"L_%=_ovcount:"
		"lds	r24, timer0_micros_count"	"\n\t"
		"subi	r24, 256 - %2"			"\n\t"
		"sts	timer0_micros_count, r24"	"\n\t"
		"brcs	L_%=_end"			"\n\t"
		"lds	r24, timer0_micros_count+1"	"\n\t"
		"sbci	r24, 255"			"\n\t"
		"sts	timer0_micros_count+1, r24"	"\n\t"
		"brcs	L_%=_end"			"\n\t"
		"lds	r24, timer0_micros_count+2"	"\n\t"
		"sbci	r24, 255"			"\n\t"
		"sts	timer0_micros_count+2, r24"	"\n\t"

	"L_%=_end:"
		"pop	r24"				"\n\t"
		"out	__SREG__, r24"			"\n\t"
		"pop	r24"				"\n\t"
		"reti"
		:
		: "M" (TIMER0_FRACT_INC), "M" (TIMER0_MILLIS_INC),
		  "M" (TIMER0_MICROS_INC)
	);
}


void delay(uint32_t ms)
{
	uint16_t start = (uint16_t)micros();

	while (ms > 0) {
		if (((uint16_t)micros() - start) >= 1000) {
			ms--;
			start += 1000;
		}
	}
#if 0
	// This doesn't save a lot of power on Teensy, which
	// lacks the power saving flash memory of some newer
	// chips, and also usually consumes lots of power for
	// the USB port.  There is also some strange (probably
	// hardware) bug involving the A/D mux for the first
	// conversion after the processor wakes from idle mode.
	uint32_t start;
	if (!(SREG & 0x80)) {
		// if interrupts are disabled, busy loop
		while (ms--) delayMicroseconds(1000);
		return;
	}
	// if interrupt are enabled, use low power idle mode
	cli();
	start = timer0_millis_count;
	do {
		_SLEEP_CONTROL_REG = SLEEP_MODE_IDLE | _SLEEP_ENABLE_MASK;
		sei();
		sleep_cpu();
		_SLEEP_CONTROL_REG = SLEEP_MODE_IDLE;
		cli();
	} while (timer0_millis_count - start <= ms);
	sei();
#endif
}


uint32_t _micros(void)
{
	register uint32_t out asm("r22");
	asm volatile(
		"in	__tmp_reg__, __SREG__"		"\n\t"
		"cli"					"\n\t"
		"in	%A0, %2"			"\n\t"
		"in	__zero_reg__, %3"		"\n\t"
		"lds	%B0, timer0_micros_count"	"\n\t"
		"lds	%C0, timer0_micros_count+1"	"\n\t"
		"lds	%D0, timer0_micros_count+2"	"\n\t"
		"out	__SREG__, __tmp_reg__"		"\n\t"
		"sbrs	__zero_reg__, %4"		"\n\t"
		"rjmp	L_%=_skip"			"\n\t"
		"cpi	%A0, 255"			"\n\t"
		"breq	L_%=_skip"			"\n\t"
		"subi	%B0, 256 - %1"			"\n\t"
		"sbci	%C0, 255"			"\n\t"
		"sbci	%D0, 255"			"\n\t"
	"L_%=_skip:"
		"clr	__zero_reg__"			"\n\t"
		"clr	__tmp_reg__"			"\n\t"
#if F_CPU == 16000000L || F_CPU == 8000000L || F_CPU == 4000000L
		"lsl	%A0"				"\n\t"
		"rol	__tmp_reg__"			"\n\t"
		"lsl	%A0"				"\n\t"
		"rol	__tmp_reg__"			"\n\t"
#if F_CPU == 8000000L || F_CPU == 4000000L
		"lsl	%A0"				"\n\t"
		"rol	__tmp_reg__"			"\n\t"
#endif
#if F_CPU == 4000000L
		"lsl	%A0"				"\n\t"
		"rol	__tmp_reg__"			"\n\t"
#endif
		"or	%B0, __tmp_reg__"		"\n\t"
#endif
#if F_CPU == 1000000L || F_CPU == 2000000L
		"lsr	%A0"				"\n\t"
		"ror	__tmp_reg__"			"\n\t"
		"lsr	%A0"				"\n\t"
		"ror	__tmp_reg__"			"\n\t"
#if F_CPU == 2000000L
		"lsr	%A0"				"\n\t"
		"ror	__tmp_reg__"			"\n\t"
#endif
		"or	%B0, %A0"			"\n\t"
		"mov	%A0, __tmp_reg__"		"\n\t"
#endif
		: "=d" (out)
		: "M" (TIMER0_MICROS_INC),
		  "I" (_SFR_IO_ADDR(TCNT0)),
		  "I" (_SFR_IO_ADDR(TIFR0)),
		  "I" (TOV0)
		: "r0"
	);
	return out;
}
