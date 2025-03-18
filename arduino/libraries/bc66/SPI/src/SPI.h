////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Georgi Angelov
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////

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
    uint32_t _port;
    uint32_t _clock;

    bool _order;
    bool _cpol;
    bool _cpha;
    bool _type;
    bool _config;

    Enum_PinName _miso;
    Enum_PinName _mosi;
    Enum_PinName _clk;
    Enum_PinName _cs;
    void set_hard_pins();
};

extern SPIClass SPI;

#endif