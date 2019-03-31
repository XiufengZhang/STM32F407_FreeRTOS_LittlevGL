#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "misc.h"
#include "gpio.h"


FlagStatus RealayRunStatus = RESET;//继电器运行状态，继电器无故障的情况下，SET运行，RESET停止
ErrorStatus RealayStatus = SUCCESS;//继电器状态，MCU控制IO与检测IO状态不符时泵故障报警，ERROR故障报警，SUCCESS正常
ErrorStatus TemperStatus = SUCCESS;//温度开关状态，可控硅超温报警，ERROR故障报警，SUCCESS正常


//IO初始化配置
void  GpioInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    //FSMC SRAM SSD1963 GPIO配置
    //数据线
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;                               //复用GPIO_Mode_AF = 0x02
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                              //推挽输出GPIO_OType_PP = 0x00
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;                           //无上拉下拉GPIO_PuPd_NOPULL = 0x00
    
    GPIO_InitStructure.GPIO_Pin = FSMC_D0_GPIO_PIN;
    GPIO_Init(FSMC_D0_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_D0_GPIO_PORT, FSMC_D0_PINSOURCE, FSMC_D0_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_D1_GPIO_PIN;
    GPIO_Init(FSMC_D1_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_D1_GPIO_PORT, FSMC_D1_PINSOURCE, FSMC_D1_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_D2_GPIO_PIN;
    GPIO_Init(FSMC_D2_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_D2_GPIO_PORT, FSMC_D2_PINSOURCE, FSMC_D2_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_D3_GPIO_PIN;
    GPIO_Init(FSMC_D3_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_D3_GPIO_PORT, FSMC_D3_PINSOURCE, FSMC_D3_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_D4_GPIO_PIN;
    GPIO_Init(FSMC_D4_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_D4_GPIO_PORT, FSMC_D4_PINSOURCE, FSMC_D4_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_D5_GPIO_PIN;
    GPIO_Init(FSMC_D5_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_D5_GPIO_PORT, FSMC_D5_PINSOURCE, FSMC_D5_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_D6_GPIO_PIN;
    GPIO_Init(FSMC_D6_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_D6_GPIO_PORT, FSMC_D6_PINSOURCE, FSMC_D6_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_D7_GPIO_PIN;
    GPIO_Init(FSMC_D7_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_D7_GPIO_PORT, FSMC_D7_PINSOURCE, FSMC_D7_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_D8_GPIO_PIN;
    GPIO_Init(FSMC_D8_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_D8_GPIO_PORT, FSMC_D8_PINSOURCE, FSMC_D8_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_D9_GPIO_PIN;
    GPIO_Init(FSMC_D9_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_D9_GPIO_PORT, FSMC_D9_PINSOURCE, FSMC_D9_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_D10_GPIO_PIN;
    GPIO_Init(FSMC_D10_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_D10_GPIO_PORT, FSMC_D10_PINSOURCE, FSMC_D10_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_D11_GPIO_PIN;
    GPIO_Init(FSMC_D11_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_D11_GPIO_PORT, FSMC_D11_PINSOURCE, FSMC_D11_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_D12_GPIO_PIN;
    GPIO_Init(FSMC_D12_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_D12_GPIO_PORT, FSMC_D12_PINSOURCE, FSMC_D12_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_D13_GPIO_PIN;
    GPIO_Init(FSMC_D13_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_D13_GPIO_PORT, FSMC_D13_PINSOURCE, FSMC_D13_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_D14_GPIO_PIN;
    GPIO_Init(FSMC_D14_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_D14_GPIO_PORT, FSMC_D14_PINSOURCE, FSMC_D14_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_D15_GPIO_PIN;
    GPIO_Init(FSMC_D15_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_D15_GPIO_PORT, FSMC_D15_PINSOURCE, FSMC_D15_AF);
    
    //地址线
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;                               //复用GPIO_Mode_AF = 0x02
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                              //推挽输出GPIO_OType_PP = 0x00
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;                           //无上拉下拉GPIO_PuPd_NOPULL = 0x00

    GPIO_InitStructure.GPIO_Pin = FSMC_A0_GPIO_PIN;
    GPIO_Init(FSMC_A0_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_A0_GPIO_PORT, FSMC_A0_PINSOURCE, FSMC_A0_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_A1_GPIO_PIN;
    GPIO_Init(FSMC_A1_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_A1_GPIO_PORT, FSMC_A1_PINSOURCE, FSMC_A1_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_A2_GPIO_PIN;
    GPIO_Init(FSMC_A2_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_A2_GPIO_PORT, FSMC_A2_PINSOURCE, FSMC_A2_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_A3_GPIO_PIN;
    GPIO_Init(FSMC_A3_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_A3_GPIO_PORT, FSMC_A3_PINSOURCE, FSMC_A3_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_A4_GPIO_PIN;
    GPIO_Init(FSMC_A4_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_A4_GPIO_PORT, FSMC_A4_PINSOURCE, FSMC_A4_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_A5_GPIO_PIN;
    GPIO_Init(FSMC_A5_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_A5_GPIO_PORT, FSMC_A5_PINSOURCE, FSMC_A5_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_A6_GPIO_PIN;
    GPIO_Init(FSMC_A6_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_A6_GPIO_PORT, FSMC_A6_PINSOURCE, FSMC_A6_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_A7_GPIO_PIN;
    GPIO_Init(FSMC_A7_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_A7_GPIO_PORT, FSMC_A7_PINSOURCE, FSMC_A7_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_A8_GPIO_PIN;
    GPIO_Init(FSMC_A8_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_A8_GPIO_PORT, FSMC_A8_PINSOURCE, FSMC_A8_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_A9_GPIO_PIN;
    GPIO_Init(FSMC_A9_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_A9_GPIO_PORT, FSMC_A9_PINSOURCE, FSMC_A9_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_A10_GPIO_PIN;
    GPIO_Init(FSMC_A10_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_A10_GPIO_PORT, FSMC_A10_PINSOURCE, FSMC_A10_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_A11_GPIO_PIN;
    GPIO_Init(FSMC_A11_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_A11_GPIO_PORT, FSMC_A11_PINSOURCE, FSMC_A11_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_A12_GPIO_PIN;
    GPIO_Init(FSMC_A12_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_A12_GPIO_PORT, FSMC_A12_PINSOURCE, FSMC_A12_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_A13_GPIO_PIN;
    GPIO_Init(FSMC_A13_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_A13_GPIO_PORT, FSMC_A13_PINSOURCE, FSMC_A13_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_A14_GPIO_PIN;
    GPIO_Init(FSMC_A14_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_A14_GPIO_PORT, FSMC_A14_PINSOURCE, FSMC_A14_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_A15_GPIO_PIN;
    GPIO_Init(FSMC_A15_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_A15_GPIO_PORT, FSMC_A15_PINSOURCE, FSMC_A15_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_A16_GPIO_PIN;
    GPIO_Init(FSMC_A16_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_A16_GPIO_PORT, FSMC_A16_PINSOURCE, FSMC_A16_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_A17_GPIO_PIN;
    GPIO_Init(FSMC_A17_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_A17_GPIO_PORT, FSMC_A17_PINSOURCE, FSMC_A17_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_A18_GPIO_PIN;
    GPIO_Init(FSMC_A18_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_A18_GPIO_PORT, FSMC_A18_PINSOURCE, FSMC_A18_AF);  
    
    //控制信号线
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;                               //复用GPIO_Mode_AF = 0x02
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                              //推挽输出GPIO_OType_PP = 0x00
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;                           //无上拉下拉GPIO_PuPd_NOPULL = 0x00
    
    GPIO_InitStructure.GPIO_Pin = FSMC_NOE_GPIO_PIN;
    GPIO_Init(FSMC_NOE_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_NOE_GPIO_PORT, FSMC_NOE_PINSOURCE, FSMC_NOE_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_NWE_GPIO_PIN;
    GPIO_Init(FSMC_NWE_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_NWE_GPIO_PORT, FSMC_NWE_PINSOURCE, FSMC_NWE_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_NBL0_GPIO_PIN;
    GPIO_Init(FSMC_NBL0_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_NBL0_GPIO_PORT, FSMC_NBL0_PINSOURCE, FSMC_NBL0_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_NBL1_GPIO_PIN;
    GPIO_Init(FSMC_NBL1_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_NBL1_GPIO_PORT, FSMC_NBL1_PINSOURCE, FSMC_NBL1_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_ENRAM_GPIO_PIN;
    GPIO_Init(FSMC_ENRAM_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_ENRAM_GPIO_PORT, FSMC_ENRAM_PINSOURCE, FSMC_ENRAM_AF);
    
    GPIO_InitStructure.GPIO_Pin = FSMC_ENLCD_GPIO_PIN;
    GPIO_Init(FSMC_ENLCD_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(FSMC_ENLCD_GPIO_PORT, FSMC_ENLCD_PINSOURCE, FSMC_ENLCD_AF);
    
    //LCD 复位
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                              //推挽输出GPIO_OType_PP = 0x00
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;                           //无上拉下拉GPIO_PuPd_NOPULL = 0x00

    GPIO_InitStructure.GPIO_Pin = LTDC_REST_GPIO_PIN;
    GPIO_Init(LTDC_REST_GPIO_PORT, &GPIO_InitStructure);
    GPIO_SetBits(LTDC_REST_GPIO_PORT, LTDC_REST_GPIO_PIN);//低电平复位
    
    GPIO_InitStructure.GPIO_Pin = LTDC_BK_GPIO_PIN;
    GPIO_Init(LTDC_BK_GPIO_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(LTDC_BK_GPIO_PORT, LTDC_BK_GPIO_PIN);//高电平点亮背光
    
    //串口USART
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;                               //复用GPIO_Mode_AF = 0x02
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                              //推挽输出GPIO_OType_PP = 0x00
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;                           //无上拉下拉GPIO_PuPd_NOPULL = 0x00
    //USART1 连接控制板控制 发指令查询状态
    GPIO_InitStructure.GPIO_Pin = USART1_TX_GPIO_PIN;
    GPIO_Init(USART1_TX_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(USART1_TX_GPIO_PORT, USART1_TX_PINSOURCE, USART1_TX_AF);
    
    GPIO_InitStructure.GPIO_Pin = USART1_RX_GPIO_PIN;
    GPIO_Init(USART1_RX_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(USART1_RX_GPIO_PORT, USART1_RX_PINSOURCE, USART1_RX_AF);

    //USART2 连接PC主机
    GPIO_InitStructure.GPIO_Pin = USART2_TX_GPIO_PIN;
    GPIO_Init(USART2_TX_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(USART2_TX_GPIO_PORT, USART2_TX_PINSOURCE, USART2_TX_AF);
    
    GPIO_InitStructure.GPIO_Pin = USART2_RX_GPIO_PIN;
    GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(USART2_RX_GPIO_PORT, USART2_RX_PINSOURCE, USART2_RX_AF);
    
    //SPI
    //Flash PSI1
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;                               //复用GPIO_Mode_AF = 0x02
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                              //推挽输出GPIO_OType_PP = 0x00
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;                           //无上拉下拉GPIO_PuPd_NOPULL = 0x00
    
    GPIO_InitStructure.GPIO_Pin = SPIF_SCK_GPIO_PIN;
    GPIO_Init(SPIF_SCK_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(SPIF_SCK_GPIO_PORT, SPIF_SCK_PINSOURCE, SPIF_SCK_AF);
    
    GPIO_InitStructure.GPIO_Pin = SPIF_MOSI_GPIO_PIN;
    GPIO_Init(SPIF_MOSI_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(SPIF_MOSI_GPIO_PORT, SPIF_MOSI_PINSOURCE, SPIF_MOSI_AF);
    
    GPIO_InitStructure.GPIO_Pin = SPIF_MISO_GPIO_PIN;
    GPIO_Init(SPIF_MISO_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(SPIF_MISO_GPIO_PORT, SPIF_MISO_PINSOURCE, SPIF_MISO_AF);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//输入配置时无效
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;//输出配置时无效

    GPIO_InitStructure.GPIO_Pin = SPIF_NSS_GPIO_PIN;
    GPIO_Init(SPIF_NSS_GPIO_PORT, &GPIO_InitStructure);
    GPIO_SetBits(SPIF_NSS_GPIO_PORT, SPIF_NSS_GPIO_PIN);//低电平是片选
    //GPIO_PinAFConfig(SPIF_NSS_GPIO_PORT, SPIF_NSS_PINSOURCE, SPIF_NSS_AF);
    
    //TOUCH SPI2
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;                               //复用GPIO_Mode_AF = 0x02
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                              //推挽输出GPIO_OType_PP = 0x00
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;                           //无上拉下拉GPIO_PuPd_NOPULL = 0x00
    
    GPIO_InitStructure.GPIO_Pin = SPIT_SCK_GPIO_PIN;
    GPIO_Init(SPIT_SCK_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(SPIT_SCK_GPIO_PORT, SPIT_SCK_PINSOURCE, SPIT_SCK_AF);
    
    GPIO_InitStructure.GPIO_Pin = SPIT_MOSI_GPIO_PIN;
    GPIO_Init(SPIT_MOSI_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(SPIT_MOSI_GPIO_PORT, SPIT_MOSI_PINSOURCE, SPIT_MOSI_AF);
    
    GPIO_InitStructure.GPIO_Pin = SPIT_MISO_GPIO_PIN;
    GPIO_Init(SPIT_MISO_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(SPIT_MISO_GPIO_PORT, SPIT_MISO_PINSOURCE, SPIT_MISO_AF);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//输入配置时无效
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;//输出配置时无效

    GPIO_InitStructure.GPIO_Pin = SPIT_NSS_GPIO_PIN;
    GPIO_Init(SPIT_NSS_GPIO_PORT, &GPIO_InitStructure);
    GPIO_SetBits(SPIT_NSS_GPIO_PORT, SPIT_NSS_GPIO_PIN);//低电平是片选
    //GPIO_PinAFConfig(SPIT_NSS_GPIO_PORT, SPIT_NSS_PINSOURCE, SPIT_NSS_AF);
    
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;                               //输入GPIO_Mode_IN = 0x00
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//输入配置时无效
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;//输出配置时无效

    GPIO_InitStructure.GPIO_Pin = SPIT_BUSY_GPIO_PIN;
    GPIO_Init(SPIT_BUSY_GPIO_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = SPIT_INT_GPIO_PIN;
    GPIO_Init(SPIT_INT_GPIO_PORT, &GPIO_InitStructure);
    
    //Touch_I2C引脚配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//输入配置时无效
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;//输出配置时无效

    GPIO_InitStructure.GPIO_Pin = I2CT_SCL_GPIO_PIN;
    GPIO_Init(I2CT_SCL_GPIO_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = I2CT_SDA_GPIO_PIN;
    GPIO_Init(I2CT_SDA_GPIO_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//输入配置时无效
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;//输出配置时无效
    
    GPIO_InitStructure.GPIO_Pin = I2CT_RST_GPIO_PIN;
    GPIO_Init(I2CT_RST_GPIO_PORT, &GPIO_InitStructure);
    GPIO_SetBits(I2CT_RST_GPIO_PORT, I2CT_RST_GPIO_PIN);//置位GTP_I2C_RST，确保复位时产生下降沿和上升沿
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//FT触摸IC浮空输入，不需要推挽输出设置高电平
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//输入配置时无效
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;//输出配置时无效

    GPIO_InitStructure.GPIO_Pin = I2CT_INT_GPIO_PIN;
    GPIO_Init(I2CT_INT_GPIO_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(I2CT_INT_GPIO_PORT, I2CT_INT_GPIO_PIN);//复位GTP_I2C_INT GT触摸IC需要高电平设置I2C地址
    
    //Timer
    //PWM输出调整背光
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;                               //复用GPIO_Mode_AF = 0x02
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                              //推挽输出GPIO_OType_PP = 0x00
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;                           //无上拉下拉GPIO_PuPd_NOPULL = 0x00
    
    GPIO_InitStructure.GPIO_Pin = Tim_LCDBL_GPIO_PIN;
    GPIO_Init(Tim_LCDBL_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(Tim_LCDBL_GPIO_PORT, Tim_LCDBL_PINSOURCE, Tim_LCDBL_AF);
    
    //编码器脉冲捕获
    GPIO_InitStructure.GPIO_Pin = Tim_CODER_A_GPIO_PIN;
    GPIO_Init(Tim_CODER_A_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(Tim_CODER_A_GPIO_PORT, Tim_CODER_A_PINSOURCE, Tim_CODER_A_AF);
    
    GPIO_InitStructure.GPIO_Pin = Tim_CODER_B_GPIO_PIN;
    GPIO_Init(Tim_CODER_B_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(Tim_CODER_B_GPIO_PORT, Tim_CODER_B_PINSOURCE, Tim_CODER_B_AF);
    
    //测试按键
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;                               //输入GPIO_Mode_IN = 0x00
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//输入配置时无效
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;//输出配置时无效
    
    GPIO_InitStructure.GPIO_Pin = IO_TestKey_GPIO_PIN;
    GPIO_Init(IO_TestKey_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @brief  SPI1 Flash_CS控制函数
  * @param  
Enable:Enable是选择有效，DISABLE是失能
  * @retval None
  */
void GpioSPIFlashCSControl(FunctionalState Enable)//开漏输出,高电平失能，低电平片选
{
    if(Enable)
        GPIO_ResetBits(SPIF_NSS_GPIO_PORT, SPIF_NSS_GPIO_PIN);//低电平片选
    else
        GPIO_SetBits(SPIF_NSS_GPIO_PORT, SPIF_NSS_GPIO_PIN);//高电平失能
}

/**
  * @brief  SPI1 Touch_CS控制函数
  * @param  
Enable:Enable是选择有效，DISABLE是失能
  * @retval None
  */
void GpioSPITouchCSControl(FunctionalState Enable)//开漏输出,高电平失能，低电平片选
{
    if(Enable)
        GPIO_ResetBits(SPIT_NSS_GPIO_PORT, SPIT_NSS_GPIO_PIN);//低电平片选
    else
        GPIO_SetBits(SPIT_NSS_GPIO_PORT, SPIT_NSS_GPIO_PIN);//高电平失能
}

/**
  * @brief  LCD SSD1963复位控制函数
  * @param  
Enable:Enable是开启，DISABLE是关闭
  * @retval None
  */
void GpioLCDSSDControl(FunctionalState Enable)//推挽输出,高电平工作，低电平复位
{
    if(Enable)
        GPIO_SetBits(LTDC_REST_GPIO_PORT, LTDC_REST_GPIO_PIN);//高电平工作
    else
        GPIO_ResetBits(LTDC_REST_GPIO_PORT, LTDC_REST_GPIO_PIN);//低电平复位
}