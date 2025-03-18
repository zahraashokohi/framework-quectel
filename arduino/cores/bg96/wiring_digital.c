/*
  BG96 - digital
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

#define DEBUG_GPIO DBG

#define ARRAYLEN(x) (sizeof(x) / sizeof((x)[0]))
static PinDescription *getArduinoPin(uint8_t arduinoPin)
{
  for (int i = 0; i < ARRAYLEN(pinsMap); i++)
    if (pinsMap[i].arduino == arduinoPin)
      return &pinsMap[i];
  return NULL;
}

void pinMode(uint8_t pin, uint8_t mode)
{
  PinDescription *n = getArduinoPin(pin);
  if (n)
  {
    if (mode == CLOSE)
    {
      if (n->id)
        qapi_TLMM_Release_Gpio_ID(&n->tlmm, n->id);
      n->id = 0;
    }
    else
    {
      n->tlmm.dir = QAPI_GPIO_INPUT_E;
      n->tlmm.pull = QAPI_GPIO_NO_PULL_E;
      n->tlmm.drive = QAPI_GPIO_10MA_E;
      n->tlmm.func = 0;

      if (mode & INPUT_PULLUP)
        n->tlmm.pull = QAPI_GPIO_PULL_UP_E;

      if (mode & INPUT_PULLDOWN)
        n->tlmm.pull = QAPI_GPIO_PULL_DOWN_E;

      if ((mode & OUTPUT) || (mode & OUTPUT_LO) || (mode & OUTPUT_HI))
        n->tlmm.dir = QAPI_GPIO_OUTPUT_E;

      int r = qapi_TLMM_Get_Gpio_ID(&n->tlmm, &n->id);
      if (r)
      {
        DEBUG_GPIO("[ERROR] qapi_TLMM_Get_Gpio_ID[%d] %d\n", pin, r);
        return;
      }

      r = qapi_TLMM_Config_Gpio(n->id, &n->tlmm);
      if (r)
      {
        DEBUG_GPIO("[ERROR] qapi_TLMM_Config_Gpio[%d] %d\n", pin, r);
        return;
      }

      if ((mode & OUTPUT) || (mode & OUTPUT_LO) || (mode & OUTPUT_HI))
      {
        if (mode & OUTPUT_HI)
          digitalWrite(pin, 1);
        else
          digitalWrite(pin, 0);
      }
    }
  }
}

void digitalWrite(uint8_t pin, uint8_t val)
{
  PinDescription *n = getArduinoPin(pin);
  if (n)
  {
    int r = qapi_TLMM_Drive_Gpio(n->id, n->tlmm.pin, val ? QAPI_GPIO_HIGH_VALUE_E : QAPI_GPIO_LOW_VALUE_E);
    if (r)
    {
      DEBUG_GPIO("[ERROR] qapi_TLMM_Drive_Gpio[%d] %d\n", pin, r);
    }
  }
}

int digitalRead(uint8_t pin)
{
  PinDescription *n = getArduinoPin(pin);
  if (n)
  {
    qapi_GPIO_Value_t val;
    int r = qapi_TLMM_Read_Gpio(n->id, n->tlmm.pin, &val);
    if (r)
    {
      DEBUG_GPIO("[ERROR] qapi_TLMM_Read_Gpio[%d] %d\n", pin, r);
    }
    return val;
  }
  return -1;
}
