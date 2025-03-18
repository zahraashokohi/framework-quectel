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
#include <time.h>

struct timespec prog_start_time;

void delayMicroseconds(unsigned int us)
{
	usleep(us);
}

void delay(unsigned int ms)
{
	usleep(ms * 1000);
}

unsigned int micros(void)
{
	struct timespec gettime_now;
	clock_gettime(CLOCK_MONOTONIC, &gettime_now);
	return ((gettime_now.tv_sec - prog_start_time.tv_sec) * 1000000 +
			(gettime_now.tv_nsec - prog_start_time.tv_nsec) / 1000);
}

unsigned int millis()
{
	return micros() / 1000;
}

unsigned int seconds(void)
{
	return millis() / 1000;
}

//#include <sched.h>
void yield(void) { 
	//sched_yield(); 
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
