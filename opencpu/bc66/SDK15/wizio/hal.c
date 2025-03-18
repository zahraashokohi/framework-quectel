////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Georgi Angelov
//      Basic library and helpers for Arduino
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
//
// Helper for fast access
// Usage: #include <hal.h>
//
////////////////////////////////////////////////////////////////////////////

#include <hal.h>

//////////////////////////////////////////////////////////////////////////////////////
//  GPIO
//
//      Befiore use init pin, call Ql_GPIO_Init();
//      pin = MTK_GPIO_ID (look arduino pin table)
//////////////////////////////////////////////////////////////////////////////////////

static GPIO_BASE_REGISTER_T *gpio_base = (GPIO_BASE_REGISTER_T *)(GPIO_BASE);

int hal_gpio_set_direction(uint32_t pin, uint32_t direction) // 0 = input, 1 = output
{
    if (pin > 36)
        return -2;
    uint32_t no = pin / 32;
    uint32_t remainder = pin % 32;
    if (direction)
        gpio_base->GPIO_DIR.SET[no] = (1 << remainder);
    else
        gpio_base->GPIO_DIR.CLR[no] = (1 << remainder);
    return 0;
}

int hal_gpio_get_input(uint32_t pin, uint32_t *p)
{
    if (pin > 36)
        return -2;
    if (!p)
        return -1;
    uint32_t no = pin / 32;
    uint32_t remainder = pin % 32;
    uint32_t temp = gpio_base->GPIO_DIN.R[no];
    *p = temp & (1 << remainder) ? 1 : 0;
    return 0;
}

int hal_gpio_set_output(uint32_t pin, uint32_t data)
{
    if (pin > 36)
        return -2;
    uint32_t no = pin / 32;
    uint32_t remainder = pin % 32;
    if (data)
        gpio_base->GPIO_DOUT.SET[no] = (1 << remainder);
    else
        gpio_base->GPIO_DOUT.CLR[no] = (1 << remainder);
    return 0;
}

int hal_gpio_set_driving_current(uint32_t pin, hal_gpio_driving_current_t driving)
{
    uint32_t *reg = (uint32_t *)0xA20C0000; // config_0.GPIO_DRV.RW[0]   0xA20D0000
    if (pin > 36)
        return -2;
    if (pin > 15)
    {
        if ((pin - 16) < 14)
        {
            pin -= 16;
            reg++; // config_0.GPIO_DRV.RW[1]
        }
        else
        {
            pin -= 30;
            reg += 0x10000; // config_1.GPIO_DRV.RW[0]   0xA20D0000
        }
    }
    *reg &= ~(3 << 2 * pin);
    *reg |= (driving << 2 * pin);
    return 0;
}

static uint8_t is_pin_with_pullsel(uint32_t gpio_pin)
{
    uint8_t result;
    if ((unsigned int)(gpio_pin - 24) <= 10 || gpio_pin == 2)
        result = 1;
    else
        result = (unsigned int)(gpio_pin - 5) <= 0;
    return result;
}

int hal_gpio_set_pupd_register(uint32_t gpio_pin, uint8_t gpio_pupd, uint8_t gpio_r0, uint8_t gpio_r1)
{
    volatile uint32_t v;
    if ((unsigned int)gpio_pin > 36 || is_pin_with_pullsel(gpio_pin))
        return -2;
    if (gpio_pin <= 1) // 1, 2
    {
    LABEL:
        v = 1 << gpio_pin;
        if (gpio_pupd)
            GPIO_CFG0_REGISTER->GPIO_PUPD.SET = v;
        else
            GPIO_CFG0_REGISTER->GPIO_PUPD.CLR = v;
        if (gpio_r0)
            GPIO_CFG0_REGISTER->GPIO_R0.SET = v;
        else
            GPIO_CFG0_REGISTER->GPIO_R0.CLR = v;
        if (gpio_r1)
            GPIO_CFG0_REGISTER->GPIO_R1.SET = v;
        else
            GPIO_CFG0_REGISTER->GPIO_R1.CLR = v;
        return 0;
    }
    if (gpio_pin - 3 <= 1) // 3, 4
    {
        gpio_pin -= 1;
        goto LABEL;
    }
    if (gpio_pin - 6 <= 17) // 6 ... 17
    {
        gpio_pin -= 2;
        goto LABEL;
    }
    gpio_pin -= 35;
    if (gpio_pin <= 1) // 35, 36
    {
        v = 1 << gpio_pin;
        if (gpio_pupd)
            GPIO_CFG1_REGISTER->GPIO_PUPD.SET = v;
        else
            GPIO_CFG1_REGISTER->GPIO_PUPD.CLR = v;
        if (gpio_r0)
            GPIO_CFG1_REGISTER->GPIO_R0.SET = v;
        else
            GPIO_CFG1_REGISTER->GPIO_R0.CLR = v;
        if (gpio_r1)
            GPIO_CFG1_REGISTER->GPIO_R1.SET = v;
        else
            GPIO_CFG1_REGISTER->GPIO_R1.CLR = v;
        return 0;
    }
    return -1;
}

int hal_gpio_pull_up(uint32_t gpio_pin)
{
    uint32_t v;
    if (gpio_pin <= 36)
    {
        if (!is_pin_with_pullsel(gpio_pin))
        {
            return hal_gpio_set_pupd_register(gpio_pin, 0, 1, 0);
        }
        if (gpio_pin > 29)
        {
            v = gpio_pin - 30;
            if (v <= 4)
            {
                v = 1 << v;
                GPIO_CFG1_REGISTER->GPIO_PD.CLR = v;
                GPIO_CFG1_REGISTER->GPIO_PU.SET = v;
            }
            return 0;
        }
        if (gpio_pin == 2)
        {
            v = 1;
        }
        else
        {
            if (gpio_pin != 5)
                goto exit;
            v = 2;
        }
        GPIO_CFG0_REGISTER->GPIO_PD.CLR = v;
        GPIO_CFG0_REGISTER->GPIO_PU.SET = v;
    exit:
        if (gpio_pin - 24 <= 5)
        {
            v = 1 << (gpio_pin - 22);
            GPIO_CFG0_REGISTER->GPIO_PD.CLR = v;
            GPIO_CFG0_REGISTER->GPIO_PU.SET = v;
        }
        return 0;
    }
    return -2;
}

int hal_gpio_pull_down(uint32_t gpio_pin)
{
    uint32_t v;
    if (gpio_pin <= 36)
    {
        if (!is_pin_with_pullsel(gpio_pin))
        {
            return hal_gpio_set_pupd_register(gpio_pin, 1, 1, 0);
        }
        if (gpio_pin > 29)
        {
            v = gpio_pin - 30;
            if (v <= 4)
            {
                v = 1 << v;
                GPIO_CFG1_REGISTER->GPIO_PU.CLR = v;
                GPIO_CFG1_REGISTER->GPIO_PD.SET = v;
            }
            return 0;
        }
        if (gpio_pin == 2)
        {
            v = 1;
        }
        else
        {
            if (gpio_pin != 5)
                goto exit;
            v = 2;
        }
        GPIO_CFG0_REGISTER->GPIO_PU.CLR = v;
        GPIO_CFG0_REGISTER->GPIO_PD.SET = v;
    exit:
        if (gpio_pin - 24 <= 5)
        {
            v = 1 << (gpio_pin - 22);
            GPIO_CFG0_REGISTER->GPIO_PU.CLR = v;
            GPIO_CFG0_REGISTER->GPIO_PD.SET = v;
        }
        return 0;
    }
    return -2;
}

int hal_gpio_disable_pull(uint32_t gpio_pin)
{
    uint32_t v;
    if (gpio_pin <= 36)
    {
        if (!is_pin_with_pullsel(gpio_pin))
        {
            return hal_gpio_set_pupd_register(gpio_pin, 0, 0, 0);
        }
        if (gpio_pin > 29)
        {
            v = gpio_pin - 30;
            if (v <= 4)
            {
                v = 1 << v;
                GPIO_CFG1_REGISTER->GPIO_PU.CLR = v;
                GPIO_CFG1_REGISTER->GPIO_PD.CLR = v;
            }
            return 0;
        }
        if (gpio_pin == 2)
        {
            v = 1;
        }
        else
        {
            if (gpio_pin != 5)
                goto exit;
            v = 2;
        }
        GPIO_CFG0_REGISTER->GPIO_PU.CLR = v;
        GPIO_CFG0_REGISTER->GPIO_PD.CLR = v;
    exit:
        if (gpio_pin - 24 <= 5)
        {
            v = 1 << (gpio_pin - 22);
            GPIO_CFG0_REGISTER->GPIO_PU.CLR = v;
            GPIO_CFG0_REGISTER->GPIO_PD.CLR = v;
        }
        return 0;
    }
    return -2;
}

int hal_pinmux_set_function(uint32_t pin, uint8_t function)
{
    if ((unsigned int)pin > 36)
        return -2;
    if (function > 8)
        return -1;
    uint32_t no = pin / 8;
    uint32_t remainder = pin % 8;
    gpio_base->GPIO_MODE.CLR[no] = 0xF << (remainder * 4);
    gpio_base->GPIO_MODE.SET[no] = function << (remainder * 4);
    char v = pin - 30;
    if (v <= 4)
    {
        if (function == 5)
            GPIO_CFG1_REGISTER->GPIO_G.CLR = 1 << v;
        else
            GPIO_CFG1_REGISTER->GPIO_G.SET = 1 << v;
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
//  GPT
//      GPT4 is userware, connected to 1Mhz
//      GPT5 is userware, connected to 32K
//////////////////////////////////////////////////////////////////////////////////////
void gpt_open_clock_source(void)
{
    if (REG_32(0xA21D0300) & 0x800000)
        REG_32(0xA21D0320) = 0x800000;
}

void gpt_start_free_run_timer(GPT_REGISTER_T *gpt, uint32_t clock_source, uint32_t divide) // (GPT4, 0, 12) for 1MHz
{
    uint32_t clk;
    gpt = gpt;
    clk = clock_source | divide;
    gpt_open_clock_source();
    gpt->GPT_CLK = clk;
    gpt->GPT_CON_UNION.GPT_CON = 0x301;
}

uint32_t gpt_convert_ms_to_32k_count(uint32_t ms)
{
    return 6 * ms / 100 + 7 * ms / 10 + 32 * ms + 8 * ms / 1000;
}

void gpt_delay_time(GPT_REGISTER_T *gpt, const uint32_t count)
{
    volatile uint32_t begin;
    volatile uint32_t tmp;
    begin = gpt->GPT_COUNT;
    tmp = count + begin + 1;
    if (tmp <= begin)
    {
        while (gpt->GPT_COUNT >= begin)
            ;
        while (gpt->GPT_COUNT < tmp)
            ;
    }
    else
    {
        while (gpt->GPT_COUNT >= begin && gpt->GPT_COUNT < tmp)
            ;
    }
}

//////////////////////////////////////////////////////////////////////////////////////
//  PWM
//      OpenCPU use PWM_1
//////////////////////////////////////////////////////////////////////////////////////
int hal_pwm_set_duty_cycle(PWM_REGISTER_T *pwm, uint32_t duty_cycle)
{
    if (0 == pwm)
        return -1;
    if (duty_cycle > pwm->PWM_COUNT)
        duty_cycle = pwm->PWM_COUNT;
    pwm->PWM_THRESH = duty_cycle;
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
//  UART
//
//////////////////////////////////////////////////////////////////////////////////////

static void hal_uart_put_char_block(UART_REGISTER_T *reg, uint8_t byte)
{
    while (!(reg->LSR & 0x20))
        ;
    reg->THR = byte;
}

void hal_uart_send_string_polling(UART_REGISTER_T *reg, const char *str)
{
    volatile uint32_t v = reg->DMA_CON_UNION.DMA_CON_CELLS.TX_DMA_EN;
    reg->DMA_CON_UNION.DMA_CON_CELLS.TX_DMA_EN = 0; // disable DMA
    while (*str)
        hal_uart_put_char_block(reg, *str++);
    reg->DMA_CON_UNION.DMA_CON_CELLS.TX_DMA_EN = v; // reastore DMA
}

void hal_uart_send_buffer_polling(UART_REGISTER_T *reg, const char *data, uint32_t size)
{
    volatile uint32_t v = reg->DMA_CON_UNION.DMA_CON_CELLS.TX_DMA_EN;
    reg->DMA_CON_UNION.DMA_CON_CELLS.TX_DMA_EN = 0;
    while (size--)
        hal_uart_put_char_block(reg, *data++);
    reg->DMA_CON_UNION.DMA_CON_CELLS.TX_DMA_EN = v;
}

void hal_uart_set_baudrate(UART_REGISTER_T *uartx, uint32_t actual_baudrate)
{
    uint32_t integer, remainder, fraction;
    uint32_t dll_dlm, sample_count, sample_point;
    uint32_t fraction_mapping[] = {0x00, 0x00, 0x20, 0x90, 0xa8, 0x154, 0x16c, 0x1ba, 0x1f6, 0x1fe};
    uartx->RATEFIX_UNION.RATEFIX = 0;
    uartx->FRACDIV = (~UART_FRACDIV_MASK); //0xFFFFFC00
    integer = 26000000 / (actual_baudrate * 256);
    remainder = ((26000000 * 10) / (actual_baudrate * 256)) % 10;
    if ((remainder != 0) || (integer == 0))
        integer += 1;
    dll_dlm = integer;
    sample_count = 26000000 / (actual_baudrate * dll_dlm);
    while (sample_count > 256)
    {
        dll_dlm++;
        sample_count = 26000000 / (actual_baudrate * dll_dlm);
    }
    fraction = ((26000000 * 10) / (actual_baudrate * dll_dlm)) % 10;
    sample_count -= 1;
    sample_point = (sample_count - 1) >> 1;
    uartx->HIGHSPEED = UART_HIGHSPEED_SPEED_MODE3;
    uartx->DLM_DLL = dll_dlm & 0x0000ffff;
    uartx->SAMPLE_REG_UNION.SAMPLE_REG = (sample_point << UART_SAMPLE_REG_SAMPLE_POINT_OFFSET) | sample_count;
    uartx->FRACDIV = fraction_mapping[fraction];
    if (actual_baudrate >= 3000000)
        uartx->GUARD = 0x12; /* delay 2 bits per byte. */
}

