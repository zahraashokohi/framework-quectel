/*
 *  EC25
 *  Created on: 01.15.2019
 *      Author: Georgi Angelov
 */

#include <Arduino.h>

#include <HardwareSerial.h>
HardwareSerial  Serial(PORT_TTY_NATIVE(0));
HardwareSerial  Serial1(PORT_TTY_NATIVE(3));
HardwareSerial  SerialUSB(PORT_TTY_CDC(0));

