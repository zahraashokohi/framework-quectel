/*
 *  Created on: 01.15.2019
 *      Author: Georgi Angelov
 */

#ifndef __VARIANT_H__
#define __VARIANT_H__

#include <interface.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define LED 0
#define LED_BUILTIN 0

#define PINS_MAX 14
    typedef struct
    {
        uint8_t arduino;
        uint8_t device;
        qapi_TLMM_Config_t tlmm;
        qapi_GPIO_ID_t id;
        void *eint;
    } PinDescription;
    extern PinDescription pinsMap[PINS_MAX];

#ifdef __cplusplus
}
#endif //__cplusplus




#ifdef __cplusplus

#include <HardwareSerial.h>
extern HardwareSerial Serial;
extern HardwareSerial Serial1;
extern HardwareSerial Serial2;

#endif //__cplusplus

#endif /* __VARIANT_H__ */
