/*
  BG96 - analog
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

static qapi_ADC_Handle_t adc_handle = NULL;

typedef struct
{
  int arduino;
  const char *name;
  qapi_Adc_Input_Properties_Type_t properties;
} AdcDescription;

static AdcDescription adcMap[] = {
    {0,   ADC_INPUT_ADC0,     {0, 0}  },
    {1,   ADC_INPUT_ADC1,     {0, 0}  },
    {2,   ADC_INPUT_BATT_ID,  {0, 0}  }
};

#define ARRAYLEN(x) (sizeof(x) / sizeof((x)[0]))
static AdcDescription *adc_get(uint8_t channel)
{
  for (int i = 0; i < ARRAYLEN(adcMap); i++)
    if (adcMap[i].arduino == channel)
      return &adcMap[i];
  return NULL;
}

void analogOpen(uint8_t pin)
{
  if (NULL == adc_handle)
    if (qapi_ADC_Open(&adc_handle, 0))
      return;

  if (adc_handle)
  {
    AdcDescription *a = adc_get(pin);
    if (a)
    {
      qapi_ADC_Get_Input_Properties(adc_handle, a->name, strlen(a->name) + 1, &a->properties);
    }
  }
}

void analogClose(uint8_t pin)
{
  //if (adc_handle) qapi_ADC_Close(adc_handle, 0);
}

int analogRead(uint8_t pin)
{
  uint32_t result = -1;
  if (adc_handle)
  {
    AdcDescription *a = adc_get(pin);
    if (a)
    {
      qapi_ADC_Read_Channel(adc_handle, &a->properties, &result);
    }
  }
  return result;
}

void analogWrite(uint8_t pin, int val)
{
  //THERE IS NOT PWM
}