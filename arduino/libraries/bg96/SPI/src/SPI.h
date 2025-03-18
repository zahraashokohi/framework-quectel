/*
 *  Created on: 01.15.2019
 *      Author: Georgi Angelov
 */

#ifndef __SPI_H__
#define __SPI_H__

#include <Arduino.h>

typedef enum
{
  SPI_MODE0 = 0,
  SPI_MODE1,
  SPI_MODE2,
  SPI_MODE3,
} SPIDataMode;

class SPISettings
{
public:
  SPISettings(uint32_t clockFrequency, BitOrder bitOrder, SPIDataMode dataMode);
  SPISettings();

private:
  uint32_t clock;
  BitOrder order;
  SPIDataMode mode;
  friend class SPIClass;
};

class SPIClass
{
public:
  SPIClass();
  SPIClass(uint32_t chnnl);
  SPIClass(uint32_t chnnl, int miso, int mosi, int clk, int cs);
  void begin();
  void end();

  void setBitOrder(BitOrder order);
  void setDataMode(uint8_t mode);
  void setFrequency(uint32_t frequency);

  void beginTransaction(SPISettings settings);
  void endTransaction(void);
  uint8_t transfer(uint8_t data);
  uint16_t transfer16(uint16_t _data);
  int transfer(uint8_t *tx, uint32_t wLen);
  int transfer(uint8_t *tx, uint32_t wLen, uint8_t *rx, uint32_t rLen);
  void cs(int level);

  void setClockDivider(uint8_t){};
  void setClockDivider(uint8_t, uint8_t) {}
  void attachInterrupt(){};
  void detachInterrupt(){};

private:
  bool order;
  qapi_SPIM_Instance_t port;
  qapi_SPIM_Config_t config;
  void *handle = NULL;
  static void callback(uint32 status, void *cb_para) {}
  void ctor();
  int rewrite(uint8_t *tx, uint8_t *rx, size_t size);
};

extern SPIClass SPI;

#endif