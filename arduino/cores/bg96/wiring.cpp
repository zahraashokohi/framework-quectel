/*
 Copyright (c) 2011 Arduino.  All right reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 Modified 
 */

#include "Arduino.h"

void delay(unsigned int ms)
{
	qapi_Timer_Sleep(ms, QAPI_TIMER_UNIT_MSEC, 1);
}

void delayMicroseconds(unsigned int us)
{
	qapi_Timer_Sleep(us, QAPI_TIMER_UNIT_USEC, 1);
}

void yield(void)
{
	qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_TICK, 1);
}

unsigned int millis(void)
{
	return qurt_timer_get_ticks() / 19200;
}

unsigned int seconds(void)
{
	return millis() / 1000;
}

uint32_t getTime()
{
	qapi_time_get_t tt;
	qapi_time_get(QAPI_TIME_SECS, &tt);
	return (uint32_t)tt.time_secs + (10 * 365 * 24 * 60 * 60) + (7 * 24 * 60 * 60); // - (2 * 60 * 60); //315975640
}

boolean no_interrupt = 1;

void interrupts(void)
{
	no_interrupt = 1;
}

void noInterrupts(void)
{
	no_interrupt = 0;
}

boolean noStopInterrupts(void)
{
	return no_interrupt;
}

uint32_t clockCyclesPerMicrosecond(void)
{
	return 1200; // TODO
}

uint32_t clockCyclesToMicroseconds(uint32_t a)
{
	return a / clockCyclesPerMicrosecond();
}

uint32_t microsecondsToClockCycles(uint32_t a)
{
	return a * clockCyclesPerMicrosecond();
}
