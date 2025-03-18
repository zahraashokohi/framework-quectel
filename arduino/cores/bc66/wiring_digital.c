/*
  BC66 - digital
    Created on: 01.01.2019
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

#include <Arduino.h>
#include <variant.h>
#include <os_eint.h>
#include <os_gpio.h>

#define ARRAYLEN(x) (sizeof(x) / sizeof((x)[0]))
PinDescription *getPin(uint8_t arduinoPin)
{
    return (arduinoPin >= ARRAYLEN(pinsMap)) ? NULL : &pinsMap[arduinoPin];
}

void pinMode(uint8_t pin, uint8_t mode)
{
    PinDescription *n = getPin(pin);
    if (n)
    {
        if (mode == CLOSE)
        {
            Ql_GPIO_Uninit(n->ql);
            n->ql = PINNAME_END;
        }
        else
        {            
            Enum_PinPullSel pull = PINPULLSEL_DISABLE;
            Enum_PinDirection dir = PINDIRECTION_IN;
            Enum_PinLevel level = PINLEVEL_LOW;
            if (mode & INPUT_PULLUP)
                pull = PINPULLSEL_PULLUP;
            if (mode & INPUT_PULLDOWN)
                pull = PINPULLSEL_PULLDOWN;
            if ((mode & OUTPUT) || (mode & OUTPUT_LO) || (mode & OUTPUT_HI))
                dir = PINDIRECTION_OUT;
            if (mode & OUTPUT_HI)
                level = PINLEVEL_HIGH;
            Ql_GPIO_Uninit(n->ql);    
            Ql_GPIO_Init(n->ql, dir, level, pull);
        }
    }
}

void digitalWrite(uint8_t pin, uint8_t val)
{
    PinDescription *n = getPin(pin);
    if (n)
        Ql_GPIO_SetLevel(n->ql, (Enum_PinLevel)val & 1);
}

int digitalRead(uint8_t pin)
{
    PinDescription *n = getPin(pin);
    if (n)
        return Ql_GPIO_GetLevel(n->ql);
    return -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void digitalWriteFast(uint8_t pin, uint8_t val)
{
    volatile GPIO_BASE_REGISTER_T *base = (GPIO_BASE_REGISTER_T *)GPIO_BASE;
    PinDescription *n = getPin(pin);
    if (n)
    {
        if (val)
            base->GPIO_DOUT.SET[n->mk >> 5] = 1 << (n->mk & 0x1F);
        else
            base->GPIO_DOUT.CLR[n->mk >> 5] = 1 << (n->mk & 0x1F);
    }
}

int digitalReadFast(uint8_t pin)
{
    volatile GPIO_BASE_REGISTER_T *base = (GPIO_BASE_REGISTER_T *)GPIO_BASE;
    PinDescription *n = getPin(pin);
    if (n)
        return (1 << (n->mk & 0x1F)) & base->GPIO_DIN.R[n->mk >> 5];
    return -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool eintMode(uint8_t pin, Enum_EintType type, Callback_EINT_Handle callback, uint32_t hwDebounce, uint32_t swDebounce, bool automask)
{
    int res = -1;
    PinDescription *n = getPin(pin);
    if (n)
    {
        if (END_OF_HAL_EINT == type)
        {
            if (0 == (res = eint_close(n->eint)))
            {
                free(n->eint);
                n->eint = NULL;
            }
        }
        else if (callback)
        {
            n->eint = eint_open(n->ql, type, callback, hwDebounce, swDebounce, automask);
            res = n->eint ? 0 : -1;
        }
    }
    return res == 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//void interrupts(void) {}
//void noInterrupts(void) {}