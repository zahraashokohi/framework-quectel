/*
  BC66 - analog
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

#include <variant.h>

#define PWM_MAX 8193

int analogRead(uint8_t pin)
{
    unsigned int value = 0;
    switch (pin)
    {
    case PIN_ADC0:
    case PIN_ADC1:
    case PIN_ADC2:
        Ql_ADC_Read(pin, &value);
        break;
    }
    return value;
}

void analogWrite(uint8_t pin, int val)
{
    Ql_PWM_Output((Enum_PinName)pin, val);
}

void analogClose(uint8_t pin)
{
    switch (pin)
    {
    case PWM0:
    case PWM1:
    case PWM2:
        Ql_PWM_Uninit((Enum_PinName)pin);
    }
}

void analogOpen(uint8_t pin, /* val, src, div */...)
{
    switch (pin)
    {
    case PWM0:
    case PWM1:
    case PWM2:
    {
        va_list list;
        va_start(list, pin);
        uint32_t val = va_arg(list, uint32_t);
        uint32_t pwmSrcClk = va_arg(list, uint32_t);
        uint32_t pwmDiv = va_arg(list, uint32_t);
        Ql_GPIO_Uninit((Enum_PinName)pin);
        uint32_t PWM_lowPulseNum = PWM_MAX / 2;
        uint32_t PWM_highPulseNum = PWM_MAX / 2;
        Ql_PWM_Init((Enum_PinName)pin, (Enum_PwmSource)pwmSrcClk, (Enum_PwmSourceDiv)pwmDiv, PWM_lowPulseNum, PWM_highPulseNum);
        Ql_PWM_Output((Enum_PinName)pin, val);
    }
    break;
    } //switch
}
