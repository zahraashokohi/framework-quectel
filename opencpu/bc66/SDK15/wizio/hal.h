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

#ifndef HAL_H_
#define HAL_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include <mt2625.h>

#define REG_32(addr)                    (*(volatile uint32_t*)(addr))
#define REG_16(addr)                    (*(volatile uint16_t*)(addr))

//////////////////////////////////////////////////////////////////////////////////////
//  SYSTICK
// 		Oscilator   = 13M ???
//		ReloadValue = 0x10000000;
//////////////////////////////////////////////////////////////////////////////////////
#define GET_SYS_TICK 					SysTick.VAL	
	
//////////////////////////////////////////////////////////////////////////////////////
//  GPIO
//
//      Befiore use init pin, call Ql_GPIO_Init();  
//      pin = MTK_GPIO_ID (look arduino pin table)
//////////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    HAL_GPIO_0 = 0x0,
    HAL_GPIO_1 = 0x1,
    HAL_GPIO_2 = 0x2,
    HAL_GPIO_3 = 0x3,
    HAL_GPIO_4 = 0x4,
    HAL_GPIO_5 = 0x5,
    HAL_GPIO_6 = 0x6,
    HAL_GPIO_7 = 0x7,
    HAL_GPIO_8 = 0x8,
    HAL_GPIO_9 = 0x9,
    HAL_GPIO_10 = 0xA,
    HAL_GPIO_11 = 0xB,
    HAL_GPIO_12 = 0xC,
    HAL_GPIO_13 = 0xD,
    HAL_GPIO_14 = 0xE,
    HAL_GPIO_15 = 0xF,
    HAL_GPIO_16 = 0x10,
    HAL_GPIO_17 = 0x11,
    HAL_GPIO_18 = 0x12,
    HAL_GPIO_19 = 0x13,
    HAL_GPIO_20 = 0x14,
    HAL_GPIO_21 = 0x15,
    HAL_GPIO_22 = 0x16,
    HAL_GPIO_23 = 0x17,
    HAL_GPIO_24 = 0x18,
    HAL_GPIO_25 = 0x19,
    HAL_GPIO_26 = 0x1A,
    HAL_GPIO_27 = 0x1B,
    HAL_GPIO_28 = 0x1C,
    HAL_GPIO_29 = 0x1D,
    HAL_GPIO_30 = 0x1E,
    HAL_GPIO_31 = 0x1F,
    HAL_GPIO_32 = 0x20,
    HAL_GPIO_33 = 0x21,
    HAL_GPIO_34 = 0x22,
    HAL_GPIO_35 = 0x23,
    HAL_GPIO_36 = 0x24,
    HAL_GPIO_MAX = 0x25,
} hal_gpio_pin_t;


#define HAL_GPIO_0_GPIO0    0   
#define HAL_GPIO_0_PWM0    3    
#define HAL_GPIO_0_KP_COL0    4 
#define HAL_GPIO_0_CLKO0    5           
#define HAL_GPIO_0_DEBUGMON0    8

#define HAL_GPIO_1_GPIO1    0   
#define HAL_GPIO_1_UART1_RTS    1   
#define HAL_GPIO_1_I2C0_SDA    2    
#define HAL_GPIO_1_I2S0_RX    3 
#define HAL_GPIO_1_KP_COL1    4 
#define HAL_GPIO_1_PWM0    5    
#define HAL_GPIO_1_GPS_BLANKING    6    
#define HAL_GPIO_1_EINT1    7   
#define HAL_GPIO_1_DEBUGMON1    8

#define HAL_GPIO_2_GPIO2    0   
#define HAL_GPIO_2_UART0_RXD    1                       
#define HAL_GPIO_2_EINT2    7   
#define HAL_GPIO_2_DEBUGMON2    8

#define HAL_GPIO_3_GPIO3    0   
#define HAL_GPIO_3_UART2_RXD    1   
#define HAL_GPIO_3_I2C1_SCL    2    
#define HAL_GPIO_3_I2S0_CK    3 
#define HAL_GPIO_3_KP_ROW0    4 
#define HAL_GPIO_3_UART1_RXD    5   
#define HAL_GPIO_3_CLKO0    6   
#define HAL_GPIO_3_EINT3    7   
#define HAL_GPIO_3_DEBUGMON3    8

#define HAL_GPIO_4_GPIO4    0   
#define HAL_GPIO_4_UART2_TXD    1   
#define HAL_GPIO_4_I2C1_SDA    2    
#define HAL_GPIO_4_I2S0_MCLK    3   
#define HAL_GPIO_4_KP_ROW1    4 
#define HAL_GPIO_4_UART1_TXD    5   
#define HAL_GPIO_4_PWM0    6    
#define HAL_GPIO_4_EINT4    7   
#define HAL_GPIO_4_DEBUGMON4    8

#define HAL_GPIO_5_GPIO5    0   
#define HAL_GPIO_5_UART0_TXD    1                       
#define HAL_GPIO_5_EINT5    7   
#define HAL_GPIO_5_DEBUGMON5    8

#define HAL_GPIO_6_GPIO6    0   
#define HAL_GPIO_6_SDIO_SLV0_DAT1    1  
#define HAL_GPIO_6_SPI_MST0_SIO3    2   
#define HAL_GPIO_6_I2S0_WS    3 
#define HAL_GPIO_6_I2C0_SCL    4    
#define HAL_GPIO_6_NBIOT_PRI    5   
#define HAL_GPIO_6_GPS_BLANKING    6    
#define HAL_GPIO_6_EINT6    7   
#define HAL_GPIO_6_DEBUGMON6    8

#define HAL_GPIO_7_GPIO7    0   
#define HAL_GPIO_7_SDIO_SLV0_CMD    1   
#define HAL_GPIO_7_SPI_MST0_SIO2    2   
#define HAL_GPIO_7_I2S0_TX    3 
#define HAL_GPIO_7_I2C0_SDA    4    
#define HAL_GPIO_7_PWM1    5    
#define HAL_GPIO_7_GPS_FRAME_SYNC    6  
#define HAL_GPIO_7_EINT7    7   
#define HAL_GPIO_7_DEBUGMON7    8

#define HAL_GPIO_8_GPIO8    0   
#define HAL_GPIO_8_SDIO_SLV0_DAT0    1  
#define HAL_GPIO_8_SPI_MST0_CS    2 
#define HAL_GPIO_8_I2S0_CK    3 
#define HAL_GPIO_8_PWM1    4    
#define HAL_GPIO_8_NBIOT_ACT    5   
#define HAL_GPIO_8_PAD_DFUNC_SFCK    6  
#define HAL_GPIO_8_EINT8    7   
#define HAL_GPIO_8_DEBUGMON8    8

#define HAL_GPIO_9_GPIO9    0   
#define HAL_GPIO_9_SDIO_SLV0_CLK    1   
#define HAL_GPIO_9_SPI_MST0_MISO    2   
#define HAL_GPIO_9_I2S0_MCLK    3   
#define HAL_GPIO_9_RF_CONFL_DET_IRQ_B    4  
#define HAL_GPIO_9_WLAN_ACT    5    
#define HAL_GPIO_9_PAD_DFUNC_SFCS0    6 
#define HAL_GPIO_9_EINT9    7   
#define HAL_GPIO_9_DEBUGMON9    8

#define HAL_GPIO_10_GPIO10    0 
#define HAL_GPIO_10_SDIO_SLV0_DAT3    1 
#define HAL_GPIO_10_SPI_MST0_MOSI    2  
#define HAL_GPIO_10_I2C1_SCL    3   
#define HAL_GPIO_10_GPS_FRAME_SYNC    4 
#define HAL_GPIO_10_PMU_RGU_RSTB    5   
#define HAL_GPIO_10_CLKO1    6  
#define HAL_GPIO_10_EINT10    7 
#define HAL_GPIO_10_DEBUGMON10    8

#define HAL_GPIO_11_GPIO11    0 
#define HAL_GPIO_11_SDIO_SLV0_DAT2    1 
#define HAL_GPIO_11_SPI_MST0_SCK    2       
#define HAL_GPIO_11_GPS_BLANKING    4   
#define HAL_GPIO_11_PMU_GOTO_SLEEP    5 
#define HAL_GPIO_11_PWMSG0    6 
#define HAL_GPIO_11_EINT11    7 
#define HAL_GPIO_11_DEBUGMON11    8

#define HAL_GPIO_12_GPIO12    0 
#define HAL_GPIO_12_MSDC0_DAT1    1 
#define HAL_GPIO_12_SPI_SLV0_CS    2    
#define HAL_GPIO_12_UART1_RXD    3  
#define HAL_GPIO_12_CLKO1    4  
#define HAL_GPIO_12_PMU_GOTO_SLEEP    5 
#define HAL_GPIO_12_PAD_DFUNC_SWP    6  
#define HAL_GPIO_12_EINT12    7 
#define HAL_GPIO_12_DEBUGMON12    8

#define HAL_GPIO_13_GPIO13    0 
#define HAL_GPIO_13_MSDC0_CMD    1  
#define HAL_GPIO_13_SPI_SLV0_MOSI    2  
#define HAL_GPIO_13_UART1_TXD    3  
#define HAL_GPIO_13_PWM2    4   
#define HAL_GPIO_13_PWMSG0    5 
#define HAL_GPIO_13_PAD_DFUNC_SHOLD    6    
#define HAL_GPIO_13_EINT13    7 
#define HAL_GPIO_13_DEBUGMON13    8

#define HAL_GPIO_14_GPIO14    0 
#define HAL_GPIO_14_MSDC0_DAT0    1 
#define HAL_GPIO_14_SPI_SLV0_MISO    2  
#define HAL_GPIO_14_UART1_RTS    3  
#define HAL_GPIO_14_UART3_RXD    4  
#define HAL_GPIO_14_CLKO2    5  
#define HAL_GPIO_14_PAD_DFUNC_SIN    6  
#define HAL_GPIO_14_EINT14    7 
#define HAL_GPIO_14_DEBUGMON14    8

#define HAL_GPIO_15_GPIO15    0 
#define HAL_GPIO_15_MSDC0_CLK    1  
#define HAL_GPIO_15_SPI_SLV0_SCK    2   
#define HAL_GPIO_15_UART1_CTS    3  
#define HAL_GPIO_15_UART3_TXD    4  
#define HAL_GPIO_15_PWM2    5   
#define HAL_GPIO_15_PAD_DFUNC_SOUT    6 
#define HAL_GPIO_15_EINT15    7 
#define HAL_GPIO_15_DEBUGMON15    8

#define HAL_GPIO_16_GPIO16    0 
#define HAL_GPIO_16_MSDC0_DAT2    1 
#define HAL_GPIO_16_SPI_SLV0_SIO2    2  
#define HAL_GPIO_16_I2C1_SCL    3   
#define HAL_GPIO_16_UART1_RXD    4  
#define HAL_GPIO_16_GPS_FRAME_SYNC    5 
#define HAL_GPIO_16_PWMSG0    6 
#define HAL_GPIO_16_EINT16    7 
#define HAL_GPIO_16_DEBUGMON4    8

#define HAL_GPIO_17_GPIO17    0 
#define HAL_GPIO_17_MSDC0_DAT3    1 
#define HAL_GPIO_17_SPI_SLV0_SIO3    2  
#define HAL_GPIO_17_I2C1_SDA    3   
#define HAL_GPIO_17_UART1_TXD    4  
#define HAL_GPIO_17_XO_WAKEUP    5      
#define HAL_GPIO_17_EINT17    7 
#define HAL_GPIO_17_DEBUGMON5    8

#define HAL_GPIO_18_GPIO18    0 
#define HAL_GPIO_18_MSDC1_DAT2    1 
#define HAL_GPIO_18_SPI_MST1_SIO2    2  
#define HAL_GPIO_18_UART2_RXD    3  
#define HAL_GPIO_18_UART1_CTS    4  
#define HAL_GPIO_18_GPS_BLANKING    5   
#define HAL_GPIO_18_RF_CONFL_DET_IRQ_B    6 
#define HAL_GPIO_18_EINT18    7 
#define HAL_GPIO_18_DEBUGMON10    8

#define HAL_GPIO_19_GPIO19    0 
#define HAL_GPIO_19_MSDC1_DAT1    1 
#define HAL_GPIO_19_SPI_MST1_CS    2    
#define HAL_GPIO_19_MD_JTMS    3    
#define HAL_GPIO_19_CLKO3    4  
#define HAL_GPIO_19_GSM_TIMING_SYNC    5        
#define HAL_GPIO_19_EINT19    7 
#define HAL_GPIO_19_DEBUGMON11    8

#define HAL_GPIO_20_GPIO20    0 
#define HAL_GPIO_20_MSDC1_CMD    1  
#define HAL_GPIO_20_SPI_MST1_MISO    2  
#define HAL_GPIO_20_MD_JTCK    3    
#define HAL_GPIO_20_PWM3    4   
#define HAL_GPIO_20_GSM_RF_CONFL_DET_EN    5        
#define HAL_GPIO_20_EINT20    7 
#define HAL_GPIO_20_DEBUGMON0    8

#define HAL_GPIO_21_GPIO21    0 
#define HAL_GPIO_21_MSDC1_DAT0    1 
#define HAL_GPIO_21_SPI_MST1_MOSI    2          
#define HAL_GPIO_21_PWMSG1    5     
#define HAL_GPIO_21_EINT21    7 
#define HAL_GPIO_21_DEBUGMON1    8

#define HAL_GPIO_22_GPIO22    0 
#define HAL_GPIO_22_MSDC1_CLK    1  
#define HAL_GPIO_22_SPI_MST1_SCK    2   
#define HAL_GPIO_22_I2C1_SCL    3   
#define HAL_GPIO_22_PMU_GOTO_SLEEP    4 
#define HAL_GPIO_22_UART2_TXD    5      
#define HAL_GPIO_22_EINT22    7 
#define HAL_GPIO_22_DEBUGMON2    8

#define HAL_GPIO_23_GPIO23    0 
#define HAL_GPIO_23_MSDC1_DAT3    1 
#define HAL_GPIO_23_SPI_MST1_SIO3    2  
#define HAL_GPIO_23_I2C1_SDA    3   
#define HAL_GPIO_23_UART2_TXD    4  
#define HAL_GPIO_23_UART1_RTS    5      
#define HAL_GPIO_23_EINT0    7  
#define HAL_GPIO_23_DEBUGMON3    8

#define HAL_GPIO_24_GPIO24    0 
#define HAL_GPIO_24_I2S0_RX    1    
#define HAL_GPIO_24_UART1_RXD    2  
#define HAL_GPIO_24_PWMSG1    3 
#define HAL_GPIO_24_XO_WAKEUP    4  
#define HAL_GPIO_24_CLKO3    5      
#define HAL_GPIO_24_EINT1    7  
#define HAL_GPIO_24_DEBUGMON3    8

#define HAL_GPIO_25_GPIO25    0 
#define HAL_GPIO_25_I2S0_TX    1    
#define HAL_GPIO_25_UART1_TXD    2      
#define HAL_GPIO_25_PMU_RGU_RSTB    4   
#define HAL_GPIO_25_PWMSG1    5 
#define HAL_GPIO_25_GSM_TIMING_SYNC    6    
#define HAL_GPIO_25_EINT2    7  
#define HAL_GPIO_25_DEBUGMON4    8

#define HAL_GPIO_26_GPIO26    0 
#define HAL_GPIO_26_I2S0_WS    1    
#define HAL_GPIO_26_UART2_RTS    2  
#define HAL_GPIO_26_UART3_RXD    3  
#define HAL_GPIO_26_PMU_GOTO_SLEEP    4 
#define HAL_GPIO_26_PWM3    5   
#define HAL_GPIO_26_GSM_RF_CONFL_DET_EN    6    
#define HAL_GPIO_26_EINT3    7  
#define HAL_GPIO_26_DEBUGMON5    8

#define HAL_GPIO_27_GPIO27    0 
#define HAL_GPIO_27_I2S0_CK    1    
#define HAL_GPIO_27_UART2_CTS    2  
#define HAL_GPIO_27_UART3_TXD    3  
#define HAL_GPIO_27_NBIOT_PRI    4  
#define HAL_GPIO_27_GPS_FRAME_SYNC    5 
#define HAL_GPIO_27_RF_CONFL_DET_IRQ_B    6 
#define HAL_GPIO_27_EINT4    7  
#define HAL_GPIO_27_DEBUGMON6    8

#define HAL_GPIO_28_GPIO28    0 
#define HAL_GPIO_28_I2S0_MCLK    1  
#define HAL_GPIO_28_UART2_RXD    2  
#define HAL_GPIO_28_I2C2_SCL    3   
#define HAL_GPIO_28_NBIOT_ACT    4  
#define HAL_GPIO_28_GPS_BLANKING    5       
#define HAL_GPIO_28_EINT5    7  
#define HAL_GPIO_28_DEBUGMON0    8

#define HAL_GPIO_29_GPIO29    0 
#define HAL_GPIO_29_UART2_TXD    2  
#define HAL_GPIO_29_I2C2_SDA    3   
#define HAL_GPIO_29_WLAN_ACT    4   
#define HAL_GPIO_29_CLKO4    5      
#define HAL_GPIO_29_EINT6    7  
#define HAL_GPIO_29_DEBUGMON1    8

#define HAL_GPIO_30_GPIO30    0 
#define HAL_GPIO_30_I2S1_RX    1    
#define HAL_GPIO_30_AP_JTDI    2    
#define HAL_GPIO_30_CLKO3    3  
#define HAL_GPIO_30_MD_JTMS    4    
#define HAL_GPIO_30_AUXADC0    5        
#define HAL_GPIO_30_EINT7    7  
#define HAL_GPIO_30_DEBUGMON2    8

#define HAL_GPIO_31_GPIO31    0 
#define HAL_GPIO_31_I2S1_TX    1    
#define HAL_GPIO_31_AP_JRSTB    2   
#define HAL_GPIO_31_UART2_RXD    3  
#define HAL_GPIO_31_MD_JTCK    4    
#define HAL_GPIO_31_AUXADC1    5        
#define HAL_GPIO_31_EINT8    7  
#define HAL_GPIO_31_DEBUGMON3    8

#define HAL_GPIO_32_GPIO32    0 
#define HAL_GPIO_32_I2S1_WS    1    
#define HAL_GPIO_32_AP_JTDO    2    
#define HAL_GPIO_32_UART2_TXD    3  
#define HAL_GPIO_32_CLKO4    4  
#define HAL_GPIO_32_AUXADC2    5    
#define HAL_GPIO_32_PWM3    6   
#define HAL_GPIO_32_EINT9    7  
#define HAL_GPIO_32_DEBUGMON6    8

#define HAL_GPIO_33_GPIO33    0 
#define HAL_GPIO_33_I2S1_CK    1    
#define HAL_GPIO_33_AP_JTMS    2    
#define HAL_GPIO_33_UART3_RXD    3  
#define HAL_GPIO_33_I2C2_SCL    4   
#define HAL_GPIO_33_AUXADC3    5        
#define HAL_GPIO_33_EINT10    7 

#define HAL_GPIO_34_GPIO34    0 
#define HAL_GPIO_34_I2S1_MCLK    1  
#define HAL_GPIO_34_AP_JTCK    2    
#define HAL_GPIO_34_UART3_TXD    3  
#define HAL_GPIO_34_I2C2_SDA    4   
#define HAL_GPIO_34_AUXADC4    5    
#define HAL_GPIO_34_WLAN_ACT    6   
#define HAL_GPIO_34_EINT11    7 

#define HAL_GPIO_35_GPIO35    0 
#define HAL_GPIO_35_UART2_RXD    1  
#define HAL_GPIO_35_I2C2_SCL    2       
#define HAL_GPIO_35_KP_COL2    4        
#define HAL_GPIO_35_CLKO0    6  
#define HAL_GPIO_35_EINT12    7 
#define HAL_GPIO_35_DEBUGMON7    8

#define HAL_GPIO_36_GPIO36    0 
#define HAL_GPIO_36_UART2_TXD    1  
#define HAL_GPIO_36_I2C2_SDA    2       
#define HAL_GPIO_36_KP_ROW2    4    
#define HAL_GPIO_36_GPS_BLANKING    5   
#define HAL_GPIO_36_RF_CONFL_DET_IRQ_B    6 
#define HAL_GPIO_36_EINT13    7 
#define HAL_GPIO_36_DEBUGMON8    8

int hal_gpio_set_direction(uint32_t pin, uint32_t direction); // 0 = input, 1 = output
int hal_gpio_get_input(uint32_t pin, uint32_t *pData);
int hal_gpio_set_output(uint32_t pin, uint32_t data);

typedef enum
{
	HAL_GPIO_DRIVING_CURRENT_4MA    = 0x0,
	HAL_GPIO_DRIVING_CURRENT_8MA    = 0x1,
	HAL_GPIO_DRIVING_CURRENT_12MA   = 0x2,
	HAL_GPIO_DRIVING_CURRENT_16MA   = 0x3,
} hal_gpio_driving_current_t;
int hal_gpio_set_driving_current(uint32_t pin, hal_gpio_driving_current_t driving);
int hal_gpio_set_pupd_register(uint32_t pin, uint8_t pupd, uint8_t r0, uint8_t r1);
int hal_gpio_pull_up(uint32_t gpio_pin);
int hal_gpio_pull_down(uint32_t gpio_pin);
int hal_gpio_disable_pull(uint32_t gpio_pin);
int hal_pinmux_set_function(uint32_t gpio_pin, uint8_t function_index); 
	
//////////////////////////////////////////////////////////////////////////////////////
//  GPT
//      GPT4 is userware, connected to 1Mhz
//      GPT5 is userware, connected to 32K
//////////////////////////////////////////////////////////////////////////////////////
#define GPT_CURRENT_COUNT(G)     G->GPT_COUNT
uint32_t gpt_convert_ms_to_32k_count(uint32_t ms);
void gpt_delay_time(GPT_REGISTER_T *gpt /*GPT4, GPT5*/, const uint32_t count);

//////////////////////////////////////////////////////////////////////////////////////
//  PWM
//      OpenCPU use PWM_1
//////////////////////////////////////////////////////////////////////////////////////
int hal_pwm_set_duty_cycle(PWM_REGISTER_T *pwm, uint32_t duty_cycle);	

//////////////////////////////////////////////////////////////////////////////////////
//  UART
//
//////////////////////////////////////////////////////////////////////////////////////	
void hal_uart_send_string_polling(UART_REGISTER_T *reg, const char *str);
void hal_uart_send_buffer_polling(UART_REGISTER_T *reg, const char *data, uint32_t size);
void hal_uart_set_baudrate(UART_REGISTER_T *uartx, uint32_t actual_baudrate);
	
#ifdef __cplusplus
}
#endif
#endif
