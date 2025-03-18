/*
 *  EC25
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

#define DEV_CONSOLE             "/dev/console"

#define SYSFS_GPIO_PATH         "/sys/class/gpio/"
#define SYSFS_GPION_PATH        "/sys/class/gpio/gpio%d/"

#define DEV_UART_PATH           "/dev/ttyHSL%d"
#define DEV_USB_PATH            "/dev/ttyUSB0"

#define DEV_SPI_PATH            "/dev/spidev1.1"
#define DEV_I2C_PATH            "/dev/i2c-2"

#define PORT_SMD(n)             "/dev/smd"#n /* AT-8, NMEA-7*/
#define PORT_TTY_NATIVE(n)      "/dev/ttyHSL"#n
#define PORT_TTY_CDC(n)         "/dev/ttyGS"#n

#define SYSFS_ADC_PATH          "/dev/null"
#define SYSFS_PWM_PATH          "/dev/null"
#define SYSFS_PWMN_PATH         "/dev/null"

#ifdef __cplusplus
}
#endif //__cplusplus


#ifdef __cplusplus

#include <HardwareSerial.h>
extern HardwareSerial Serial;
extern HardwareSerial Serial1;
extern HardwareSerial SerialUSB;

#endif //__cplusplus

#endif /* __VARIANT_H__ */
