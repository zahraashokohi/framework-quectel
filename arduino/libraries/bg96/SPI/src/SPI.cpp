/*
 *  Created on: 01.15.2019
 *      Author: Georgi Angelov
 */

#include "SPI.h"

#define DEBUG_SPI /*DBG*/

SPIClass SPI(0); // hardware spi, all other is software

SPISettings::SPISettings(uint32_t clockFrequency, BitOrder bitOrder, SPIDataMode dataMode)
{
    clock = clockFrequency;
    order = bitOrder;
    mode = dataMode;
}

/* default SPI setting */
SPISettings::SPISettings()
{
    clock = 1000000; // 1Mhz = 1000000
    order = MSBFIRST;
    mode = SPI_MODE0;
}

////////////////////////////////////////////////////////////////////////////////////////////
void SPIClass::ctor()
{
    handle = NULL;
    config.SPIM_Mode = QAPI_SPIM_MODE_0_E;               // set the spi mode, determined by slave device
    config.SPIM_CS_Polarity = QAPI_SPIM_CS_ACTIVE_LOW_E; // set CS low as active, determined by slave device
    config.SPIM_endianness = SPI_LITTLE_ENDIAN;
    config.SPIM_Bits_Per_Word = 8;
    config.SPIM_Slave_Index = 0;
    config.Clk_Freq_Hz = 1000000; //config spi clk about 1Mhz
    config.SPIM_CS_Mode = QAPI_SPIM_CS_KEEP_ASSERTED_E;
    config.CS_Clk_Delay_Cycles = 0;     // don't care, set 0 is ok.
    config.Inter_Word_Delay_Cycles = 0; // don't care, set 0 is ok.
    config.loopback_Mode = 0;
    port = QAPI_SPIM_INSTANCE_6_E;

    order = MSBFIRST;
}

SPIClass::SPIClass()
{
    ctor();
}

SPIClass::SPIClass(uint32_t spi_port)
{
    ctor();
    port = (qapi_SPIM_Instance_t)spi_port;
}

SPIClass::SPIClass(uint32_t spi_port, int miso, int mosi, int clk, int cs)
{
    ctor();
    port = (qapi_SPIM_Instance_t)spi_port;
}

void SPIClass::cs(int level)
{
}

void SPIClass::setBitOrder(BitOrder bit_order)
{
    order = bit_order;
};

void SPIClass::setDataMode(uint8_t mode)
{
    config.SPIM_Mode = (qapi_SPIM_Shift_Mode_t)mode;
};

void SPIClass::setFrequency(uint32_t frequency)
{
    config.Clk_Freq_Hz = frequency; // 1Mhz = 1000000
};

void SPIClass::begin()
{
    if (handle)
        return; // already
    int r = qapi_SPIM_Open(port, &handle);
    if (r)
    {
        DEBUG_SPI("[ERROR] SPI[%d] Open: %d\n", port, r);
        abort();
    }
    r = qapi_SPIM_Power_On(handle);
    if (r)
    {
        DEBUG_SPI("[ERROR] SPI[%d] PowerOn: %d\n", port, r);
        abort();
    }
}

/* Disables the SPI bus (leaving pin modes unchanged) */
void SPIClass::end()
{
    if (handle)
    {
        qapi_SPIM_Power_Off(handle);
        qapi_SPIM_Close(handle);
    }
    handle = NULL;
}

/* Stop using the SPI bus. Normally this is called after de-asserting the chip select, to allow other libraries to use the SPI bus */
void SPIClass::endTransaction(void)
{
    end();
}

/* Initializes the SPI bus using the defined SPISettings */
void SPIClass::beginTransaction(SPISettings settings)
{
    setFrequency(settings.clock);
    setDataMode(settings.mode);
    setBitOrder(settings.order);
    begin();
}

int SPIClass::rewrite(uint8_t *tx, uint8_t *rx, size_t size)
{
    qapi_SPIM_Descriptor_t desc;
    desc.rx_buf = rx;
    desc.tx_buf = tx;
    desc.len = size;
    int r = qapi_SPIM_Full_Duplex(handle, &config, &desc, 1, callback, NULL, false);
    if (r)
    {
        DEBUG_SPI("[ERROR] SPI[%d] WriteRead: %d\n", port, r);
        return 0; 
    }
    return size;
}

uint8_t SPIClass::transfer(uint8_t tx)
{
    uint8_t rx;
    int r = 0;
    if (order == LSBFIRST)
        tx = __REV(__RBIT(tx));
    return rewrite(&tx, &rx, 1);
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
    if (order == LSBFIRST)
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
        if (order == MSBFIRST)
        {
            return rewrite(tx, tx, wLen);
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
    if (tx && rx && wLen && rLen)
    {
        if (order == MSBFIRST)
        {
            return rewrite(tx, rx, wLen); // MAX
        }
        else
        {
            for (int i = wLen; i; i--)
                rx[i - 1] = transfer(tx[i - 1]);
        }
    }
    return -1;
}
