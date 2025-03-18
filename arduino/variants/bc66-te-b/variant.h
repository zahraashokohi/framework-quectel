/*
  BC66 - variant
    Created on: 08.03.2020
    Author: Georgi Angelov

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA   
 */

#ifndef __VARIANT_H__
#define __VARIANT_H__

#include <interface.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define LED 4 /* PINNAME_NETLIGHT */
#define LED_BUILTIN 4

#define ADC0 PIN_ADC0
#define ADC1 PIN_ADC1
#define ADC2 PIN_ADC2

#define PWM0 PINNAME_NETLIGHT
#define PWM1 PINNAME_RTS_AUX
#define PWM2 PINNAME_GPIO3

    typedef struct
    {
        uint8_t ql;
        uint8_t mk;
        void *eint;
    } PinDescription;
    extern PinDescription pinsMap[PINNAME_END];

    PinDescription *getPin(uint8_t arduinoPin);

    void digitalWriteFast(uint8_t pin, uint8_t val);
    int digitalReadFast(uint8_t pin);

    bool eintMode(uint8_t pin, Enum_EintType type, Callback_EINT_Handle cb, uint32_t hwDebounce, uint32_t swDebounce, bool automask);

    void analogReference(uint8_t mode) __attribute__((weak));
    void analogOpen(uint8_t pin, /* val, src, div */...);
    void analogClose(uint8_t pin);
    int analogRead(uint8_t pin);
    void analogWrite(uint8_t pin, int val);

#ifdef __cplusplus
}

void initVariant();

#include <HardwareSerial.h>
extern HardwareSerial Serial;
extern HardwareSerial Serial1;
extern HardwareSerial Serial2;

#include <LDEV.h>
extern LDEV Dev;

#include <LRTC.h>
extern LRTC Rtc;

#include <LPSM.h>
extern LPSM Psm;

#endif //__cplusplus

#endif /* __VARIANT_H__ */
