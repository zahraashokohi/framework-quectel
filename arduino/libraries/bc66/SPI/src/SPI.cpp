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

#include <Arduino.h>
#include "SPI.h"

SPIClass SPI(0);

#define DEBUG_SPI

SPISettings::SPISettings(uint32_t clockFrequency, BitOrder bitOrder, SPIDataMode dataMode)
{
    clock = clockFrequency;
    order = bitOrder;
    mode = dataMode;
}

/* default SPI setting */
SPISettings::SPISettings()
{
    clock = 1000000;
    order = MSBFIRST;
    mode = SPI_MODE0;
}

////////////////////////////////////////////////////////////////////////////////////////////

void SPIClass::set_hard_pins()
{
    _miso = PINNAME_SPI_MISO; //spi MISO pin, can not change to another pin.
    _mosi = PINNAME_SPI_MOSI; //spi MOSI pin, can not change to another pin.
    _clk = PINNAME_SPI_SCLK;  //spi CLK pin, can not change to another pin.
    _cs = PINNAME_SPI_CS;     //spi CS pin, user can change this pin to annother pin.
    _type = 1;                //spi hardware
    _order = MSBFIRST;
    _config = 0;
    _cpha = 0;
    _cpol = 0;
    _port = 0;
}

SPIClass::SPIClass()
{
    set_hard_pins();
}

SPIClass::SPIClass(uint32_t port)
{
    _port = port;
    _config = _type = _cpha = _cpol = 0;
    _clock = 1000;

    if (0 == port)
        set_hard_pins();
}

SPIClass::SPIClass(uint32_t port, int miso, int mosi, int clk, int cs)
{
    _port = port;
    _config = _cpha = _cpol = 0;
    _clock = 1000;

    if (0 == port)
    {
        set_hard_pins();
    }
    else
    {
        _type = 0;
        PinDescription *p;

        p = getPin(miso);
        if (p)
            _miso = (Enum_PinName)p->ql;

        p = getPin(mosi);
        if (p)
            _mosi = (Enum_PinName)p->ql;

        p = getPin(clk);
        if (p)
            _clk = (Enum_PinName)p->ql;

        p = getPin(cs);
        if (p)
        {
            _cs = (Enum_PinName)p->ql;
            if (PINNAME_SPI_CS != _cs)
                Ql_GPIO_Init(_cs, PINDIRECTION_OUT, PINLEVEL_HIGH, PINPULLSEL_PULLUP); // CS high
        }
    }
}

void SPIClass::cs(int level)
{
    if (PINNAME_SPI_CS != _cs)
        Ql_GPIO_SetLevel(_cs, (Enum_PinLevel)level);
}

void SPIClass::setBitOrder(BitOrder order)
{
    _order = order;
};

void SPIClass::setDataMode(uint8_t mode)
{
    if (_cpha != (bool)(mode & 1))
    {
        _cpha = (bool)mode & 1;
        _config = 0;
    }
    if (_cpol != (bool)(mode & 2))
    {
        _cpol = (bool)mode & 2;
        _config = 0;
    }
};

void SPIClass::setFrequency(uint32_t frequency)
{
    if (_clock != frequency / 1000)
    {
        _clock = frequency / 1000;
        _config = 0;
    }
};

void SPIClass::begin()
{
    if (!_config)
    {
        Ql_SPI_Uninit(_port);
        int res = Ql_SPI_Init(_port, _clk, _miso, _mosi, _cs, _type);
        if (res != QL_RET_OK)
        {
            DEBUG_SPI("[SPI] Ql_SPI_Init: %d\n", res);
        }
        res = Ql_SPI_Config(_port, 1, _cpol, _cpha, _clock);
        if (res != QL_RET_OK)
        {
            DEBUG_SPI("[SPI] Ql_SPI_Config: %d\n", res);
        }
        _config = 1;
    }
}

/* Disables the SPI bus (leaving pin modes unchanged) */
void SPIClass::end()
{
    _config = 0;
    Ql_SPI_Uninit(_port);
    if (PINNAME_SPI_CS != _cs)
        Ql_GPIO_Uninit(_cs);
}

void SPIClass::endTransaction(void) {}

/* Initializes the SPI bus using the defined SPISettings */
void SPIClass::beginTransaction(SPISettings settings)
{
    setFrequency(settings.clock);
    setDataMode(settings.mode);
    setBitOrder(settings.order);
    begin();
}

uint8_t SPIClass::transfer(uint8_t tx)
{
    // Quectel spi_config.bit_order = HAL_SPI_MASTER_MSB_FIRST = 1;
    uint8_t rx = 0;
    int res;

    if (_order == LSBFIRST)
        tx = __REV(__RBIT(tx));
    if (_type)
        res = Ql_SPI_Write(_port, &tx, 1);
    else
        res = Ql_SPI_WriteRead_Ex(_port, &tx, 1, &rx, 1);
    if (res != 1)
    {
        DEBUG_SPI("[SPI] Ql_SPI_Write byte res: %d\n", res);
    }
    return res == 1 ? rx : 0;
}

uint16_t SPIClass::transfer16(uint16_t _data)
{
    union {
        uint16_t val;
        struct
        {
            uint8_t lsb;
            uint8_t msb;
        };
    } t;
    t.val = _data;
    if (_order == LSBFIRST)
    {
        t.lsb = transfer(t.lsb);
        t.msb = transfer(t.msb);
    }
    else
    {
        t.msb = transfer(t.msb);
        t.lsb = transfer(t.lsb);
    }
    return t.val;
}

int SPIClass::transfer(uint8_t *tx, uint32_t wLen)
{
    if (tx && wLen)
    {
        if (_order == MSBFIRST)
        {
            int res = Ql_SPI_Write(_port, tx, wLen);
            if (res != (int)wLen)
            {
                DEBUG_SPI("[SPI] Ql_SPI_Write len: %d res: %d\n", wLen, res);
            }
            return res;
        }
        else
        {
            for (int i = wLen; i; i--)
                tx[i - 1] = transfer(tx[i - 1]);
        }
    }
    return -1;
}

int SPIClass::transfer(uint8_t *tx, uint32_t wLen, uint8_t *rx, uint32_t rLen)
{
    int res;
    if (tx && rx && wLen && rLen)
    {
        if (_order == MSBFIRST)
        {
            if (_type)
                res = Ql_SPI_WriteRead(_port, tx, wLen, rx, rLen); // Half-Duplex Transfer!
            else
                res = Ql_SPI_WriteRead_Ex(_port, tx, wLen, rx, rLen); // Full-Duplex Transfer!
            if (res != (int)rLen)
            {
                DEBUG_SPI("[SPI] Ql_SPI_WriteRead txlen: %d rxlen: %d res: %d\n", wLen, rLen, res);
            }
            return res;
        }
        else
        {
            for (int i = wLen; i; i--)
                rx[i - 1] = transfer(tx[i - 1]);
        }
    }
    return -1;
}
