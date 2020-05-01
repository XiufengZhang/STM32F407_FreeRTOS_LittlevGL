#include <string.h>
#include "stm32f4xx.h"
#include "stm32f4xx_fsmc.h"
#include "gpio.h"
#include "timer.h"
#include "ssdlcd.h"

ErrorStatus SSDLCDStatus = ERROR; //标记SSDLCD驱动IC初始化是否正常 0失败 1正常

/**
  * @brief  SSD1963 FSMC初始化
  * @param  None
  * @retval None
  */
static void SSDFSMC_Init(uint8_t HighStatus)
{
    FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef FSMC_NORSRAMReadStructure;
    FSMC_NORSRAMTimingInitTypeDef FSMC_NORSRAMWriteStructure;

    if (HighStatus)
    {
        FSMC_NORSRAMReadStructure.FSMC_AddressSetupTime = 0x00;        //地址设置阶段的持续时间 (Address setup phase duration) 地址建立时间（ADDSET）为3个HCLK 10/168M=59.5ns
        FSMC_NORSRAMReadStructure.FSMC_AddressHoldTime = 0x00;         //地址保持阶段的持续时间 (Address-hold phase duration)  地址保持时间（ADDHLD）
        FSMC_NORSRAMReadStructure.FSMC_DataSetupTime = 0x09;           //数据阶段的持续时间 (Data-phase duration)              数据保存时间为15个读取周期
        FSMC_NORSRAMReadStructure.FSMC_BusTurnAroundDuration = 0x00;   //总线周转阶段的持续时间 (Bus turnaround phase duration)
        FSMC_NORSRAMReadStructure.FSMC_CLKDivision = 0x00;             //This parameter is not used for asynchronous NOR Flash, SRAM or ROM accesses.
        FSMC_NORSRAMReadStructure.FSMC_DataLatency = 0x00;             //It is don't care in asynchronous NOR, SRAM or ROM accesses
        FSMC_NORSRAMReadStructure.FSMC_AccessMode = FSMC_AccessMode_A; //模式A

        FSMC_NORSRAMWriteStructure.FSMC_AddressSetupTime = 0x00;        //地址设置阶段的持续时间 (Address setup phase duration) 地址建立时间（ADDSET）为3个HCLK 10/168M=59.5ns
        FSMC_NORSRAMWriteStructure.FSMC_AddressHoldTime = 0x00;         //地址保持阶段的持续时间 (Address-hold phase duration)  地址保持时间（ADDHLD）
        FSMC_NORSRAMWriteStructure.FSMC_DataSetupTime = 0x03;           //数据阶段的持续时间 (Data-phase duration)              数据保存时间为6个HCLK 6/168M=142.8ns
        FSMC_NORSRAMWriteStructure.FSMC_BusTurnAroundDuration = 0x00;   //总线周转阶段的持续时间 (Bus turnaround phase duration)
        FSMC_NORSRAMWriteStructure.FSMC_CLKDivision = 0x00;             //This parameter is not used for asynchronous NOR Flash, SRAM or ROM accesses.
        FSMC_NORSRAMWriteStructure.FSMC_DataLatency = 0x00;             //It is don't care in asynchronous NOR, SRAM or ROM accesses
        FSMC_NORSRAMWriteStructure.FSMC_AccessMode = FSMC_AccessMode_A; //模式A
    }
    else //低速是5MHz的频率
    {
        FSMC_NORSRAMReadStructure.FSMC_AddressSetupTime = 0x09;        //地址设置阶段的持续时间 (Address setup phase duration) 地址建立时间（ADDSET）为10个HCLK 10/168M=59.5ns
        FSMC_NORSRAMReadStructure.FSMC_AddressHoldTime = 0x00;         //地址保持阶段的持续时间 (Address-hold phase duration)  地址保持时间（ADDHLD）
        FSMC_NORSRAMReadStructure.FSMC_DataSetupTime = 0x3C;           //数据阶段的持续时间 (Data-phase duration)              数据保存时间为60个读取周期
        FSMC_NORSRAMReadStructure.FSMC_BusTurnAroundDuration = 0x00;   //总线周转阶段的持续时间 (Bus turnaround phase duration)
        FSMC_NORSRAMReadStructure.FSMC_CLKDivision = 0x00;             //This parameter is not used for asynchronous NOR Flash, SRAM or ROM accesses.
        FSMC_NORSRAMReadStructure.FSMC_DataLatency = 0x00;             //It is don't care in asynchronous NOR, SRAM or ROM accesses
        FSMC_NORSRAMReadStructure.FSMC_AccessMode = FSMC_AccessMode_A; //模式A

        FSMC_NORSRAMWriteStructure.FSMC_AddressSetupTime = 0x09;        //地址设置阶段的持续时间 (Address setup phase duration) 地址建立时间（ADDSET）为10个HCLK 10/168M=59.5ns
        FSMC_NORSRAMWriteStructure.FSMC_AddressHoldTime = 0x00;         //地址保持阶段的持续时间 (Address-hold phase duration)  地址保持时间（ADDHLD）
        FSMC_NORSRAMWriteStructure.FSMC_DataSetupTime = 0x18;           //数据阶段的持续时间 (Data-phase duration)              数据保存时间为24个HCLK 24/168M=142.8ns
        FSMC_NORSRAMWriteStructure.FSMC_BusTurnAroundDuration = 0x00;   //总线周转阶段的持续时间 (Bus turnaround phase duration)
        FSMC_NORSRAMWriteStructure.FSMC_CLKDivision = 0x00;             //This parameter is not used for asynchronous NOR Flash, SRAM or ROM accesses.
        FSMC_NORSRAMWriteStructure.FSMC_DataLatency = 0x00;             //It is don't care in asynchronous NOR, SRAM or ROM accesses
        FSMC_NORSRAMWriteStructure.FSMC_AccessMode = FSMC_AccessMode_A; //模式A
    }

    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;                               //使用NE4 ，也就对应BTCR[6],[7]
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;             //地址/数据复用使能位 (Address/data multiplexing enable bit) 不复用数据地址
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;                        //存储器类型 (Memory type) SRAM
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;               //存储器数据总线宽度 (Memory databus width) 16bit
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;           //同步模式 突发使能位 (Burst enable bit)
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;         //等待信号极性位 (Wait signal polarity bit) 仅当15位ASYNCWAIT置1才有效
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;                         //同步模式 环回突发模式支持 (Wrapped burst mode support)
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState; //等待时序配置 (Wait timing configuration) NWAIT 信号指示存储器中的数据是否有效，或者在同步模式下访问 Flash 时是否必须插入等待周期。
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;              //写入使能位 (Write enable bit) 存储器写使能
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;                     //同步模式 等待使能位 (Wait enable bit) 使能/禁止在同步模式下访问 Flash 时通过 NWAIT 信号插入等待周期
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;                  //扩展模式使能 (Extended mode enable) 使能后可以选择A B C D模式，否则根据外设类型自动在1或者2模式下
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;         //异步传输期间的等待信号 (Wait signal during asynchronous transfers)
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;                     //同步模式 写入突发使能 (Write burst enable)
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAMReadStructure;       //读写时序
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_NORSRAMWriteStructure;          //写时序

    //FSMC_NORSRAMDeInit(FSMC_Bank1_NORSRAM4);
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, DISABLE);
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); //初始化FSMC配置
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE); //使能BANK1 NE4
}

/**
  * @brief  SSD1963写入指令
  * @param  None
  * @retval None
  */
static void SSDLCD_WriteCommand(uint16_t SSDCommand)
{
    *(__IO uint16_t *)SSDLCD_COMMAND = SSDCommand;
}

/**
  * @brief  SSD1963写入数据
  * @param  None
  * @retval None
  */
static void SSDLCD_WriteData(uint16_t SSDData)
{
    *(__IO uint16_t *)SSDLCD_DATA = SSDData;
}

/**
  * @brief  SSD1963读取数据
  * @param  None
  * @retval None
  */
// static uint16_t SSDLCD_ReadData(void)
// {
//   uint16_t SSDTemp = 0;

//   SSDTemp = *(__IO uint16_t *)SSDLCD_DATA;
//   return SSDTemp;
// }

/**
  * @brief  SSD1963向指定寄存器写入指定长度数据
  * @param  None
  * @retval None
  */
static void SSDLCD_WriteCommandData(uint16_t SSDCommand, uint16_t *SSDData, uint16_t SSDDataCount)
{
    *(__IO uint16_t *)SSDLCD_COMMAND = SSDCommand;

    for (uint16_t i = 0; i < SSDDataCount; i++)
    {
        *(__IO uint16_t *)SSDLCD_DATA = *SSDData;
        SSDData++;
    }
}

/**
  * @brief  SSD1963向指定寄存器读取指定长度数据
  * @param  None
  * @retval None
  */
static void SSDLCD_ReadCommandData(uint16_t SSDCommand, uint16_t *SSDData, uint16_t SSDDataCount)
{
    *(__IO uint16_t *)SSDLCD_COMMAND = SSDCommand;
    STM32Delay_us(5);
    for (uint16_t i = 0; i < SSDDataCount; i++)
    {
        *SSDData = *(__IO uint16_t *)SSDLCD_DATA;
        SSDData++;
    }
}

/**
  * @brief  SSD1963复位
  * @param  None
  * @retval None
  */
static void SSDLCD_REST(void)
{
    GpioLCDSSDControl(DISABLE);
    STM32Delay_ms(100);
    GpioLCDSSDControl(ENABLE);
    STM32Delay_ms(200);
}

/**
  * @brief  SSD1963初始化
  * @param  None
  * @retval None
  */
ErrorStatus SSDLCD_Init(void)
{
    uint16_t TempData[8] = {0};
    uint16_t TempReadBuff[8] = {0};
    uint16_t PLL_Temp = 0; //存储PLL时钟频率，在设置LCD刷新频率(DCLK Frequency)时用到
    uint32_t LCDC_FPR = 0; //用于配置LCD刷新频率
    ErrorStatus SSDInitStatus = ERROR;

    SSDLCD_REST();
    SSDFSMC_Init(0); //FSMC低速配置

    for (uint8_t i = 0; i < 3; i++)
    {
        SSDLCD_ReadCommandData(SSD_read_ddb, TempData, 5); //读取产品描述信息
        if (TempData[0] == 0x01 && TempData[1] == 0x57 && TempData[2] == 0x61 && TempData[3] == 0x01 && TempData[4] == 0xFF)
        {
            SSDInitStatus = SUCCESS;
            break;
        }
    }
    if (SSDInitStatus)
    {
//设置PLL时钟频率，PLL frequency = VCO / (N + 1)，VCO = Reference input clock x (M + 1)，250MHz < VCO < 800MHz
#if (SSD_REFERENCE_CLOCK == 8)
        //8MHz晶振:
        TempData[0] = 49; //M=49,VCO=400MHz
        TempData[1] = 3;  //N=3,PLL=100MHz
        // TempData[0] = 44; //M=44,VCO=360MHz
        // TempData[1] = 2;  //N=2,PLL=120MHz
        // TempData[0] = 35; //M=35,VCO=288MHz
        // TempData[1] = 2;  //N=2,PLL=96MHz
#elif (SSD_REFERENCE_CLOCK == 10)
        //10MHz晶振:
        // TempData[0] = 49; //M=49,VCO=500MHz
        // TempData[1] = 4;  //N=3,PLL=100MHz
        // TempData[0] = 39; //M=39,VCO=400MHz
        // TempData[1] = 3;  //N=3,PLL=100MHz
        TempData[0] = 29; //M=29,VCO=300MHz
        TempData[1] = 2;  //N=2,PLL=100MHz
#else
#error "The TFT LCD DCLK has not been defined"
#endif
        TempData[2] = 0x04; //使能PLL
        PLL_Temp = SSD_REFERENCE_CLOCK * (TempData[0] + 1) / (TempData[1] + 1);
        SSDLCD_WriteCommandData(SSD_set_pll_mn, TempData, 3); //E2寄存器设置时钟频率
        STM32Delay_us(100);

        memset(TempReadBuff, 0, sizeof(TempReadBuff)); //数组清零
        SSDLCD_ReadCommandData(SSD_get_pll_mn, TempReadBuff, 3);
        if (TempReadBuff[0] != TempData[0] || TempReadBuff[1] != TempData[1] || TempReadBuff[2] != TempData[2])
        {
            SSDLCD_WriteCommandData(SSD_set_pll_mn, TempData, 3); //E2寄存器设置时钟频率
            STM32Delay_us(100);
            memset(TempReadBuff, 0, sizeof(TempReadBuff)); //数组清零
            SSDLCD_ReadCommandData(SSD_get_pll_mn, TempReadBuff, 3);
            if (TempReadBuff[0] != TempData[0] || TempReadBuff[1] != TempData[1] || TempReadBuff[2] != TempData[2])
            {
                SSDLCD_WriteCommandData(SSD_set_pll_mn, TempData, 3); //E2寄存器设置时钟频率
                STM32Delay_us(100);
            }
        }

        TempData[0] = 0x01;
        SSDLCD_WriteCommandData(SSD_set_pll, TempData, 1); //E0启动PLL
        STM32Delay_us(100);

        TempData[0] = 0x03;
        SSDLCD_WriteCommandData(SSD_set_pll, TempData, 1); //E0启动PLL 锁定PLL
        STM32Delay_ms(10);

        SSDLCD_WriteCommand(SSD_soft_reset); //除了E0-E5软复位
        SSDFSMC_Init(1);                     //FSMC高速配置
        STM32Delay_ms(10);

        TempData[0] = 0x00;                                         //从上至下 从左至右 RGB
        SSDLCD_WriteCommandData(SSD_set_address_mode, TempData, 1); //设置刷新模式

        //设置刷新频率(DCLK Frequency),DCLK Frequency来自TFTLCD datasheet(5.3MHz),PLL=100,计算出LCDC_FPR=55574
        //TempData[0] = 0x00;//DCLK Frequency = PLL x ((LCDC_FPR + 1) / 2^20)
        //TempData[1] = 0xD9;
        //TempData[2] = 0x16;
        LCDC_FPR = SSD_REFERENCE_DCLK * 1048576 / PLL_Temp - 1; //pow(2,20) = 1048576
        TempData[0] = (uint8_t)((LCDC_FPR >> 16) & 0xFF);
        TempData[1] = (uint8_t)((LCDC_FPR >> 8) & 0xFF);
        TempData[2] = (uint8_t)(LCDC_FPR & 0xFF);
        SSDLCD_WriteCommandData(SSD_set_lshift_freq, TempData, 3); //E6寄存器设置像素时钟频率
        STM32Delay_us(5);
        memset(TempReadBuff, 0, sizeof(TempReadBuff)); //数组清零
        SSDLCD_ReadCommandData(SSD_get_lshift_freq, TempReadBuff, 3);
        if (TempReadBuff[0] != TempData[0] || TempReadBuff[1] != TempData[1] || TempReadBuff[2] != TempData[2])
        {
            SSDLCD_WriteCommandData(SSD_set_lshift_freq, TempData, 3); //E2寄存器设置时钟频率
            STM32Delay_us(5);
            memset(TempReadBuff, 0, sizeof(TempReadBuff)); //数组清零
            SSDLCD_ReadCommandData(SSD_get_lshift_freq, TempReadBuff, 3);
            if (TempReadBuff[0] != TempData[0] || TempReadBuff[1] != TempData[1] || TempReadBuff[2] != TempData[2])
            {
                SSDLCD_WriteCommandData(SSD_set_lshift_freq, TempData, 3); //E2寄存器设置时钟频率
                STM32Delay_us(5);
            }
        }

        TempData[0] = 0x20;                          //24位
        TempData[1] = 0x00;                          //TFT模式
        TempData[2] = (SSD_HOR_RESOLUTION - 1) >> 8; //水平像素
        TempData[3] = (SSD_HOR_RESOLUTION - 1) & 0x00FF;
        TempData[4] = (SSD_VER_RESOLUTION - 1) >> 8; //垂直像素
        TempData[5] = (SSD_VER_RESOLUTION - 1) & 0x00FF;
        TempData[6] = 0x00;                                     //0x2D;//输入RGB转换
        SSDLCD_WriteCommandData(SSD_set_lcd_mode, TempData, 7); //设置LCD模式

        TempData[0] = SSD_HT >> 8; //水平总像素
        TempData[1] = SSD_HT & 0x00FF;
        TempData[2] = SSD_HPS >> 8; //水平同步信号开始到行有效数据开始之间的相素时钟
        TempData[3] = SSD_HPS & 0x00FF;
        TempData[4] = SSD_HPW;      //水平同步信号宽度
        TempData[5] = SSD_LPS >> 8; //水平同步信号起始位置
        TempData[6] = SSD_LPS & 0x00FF;
        TempData[7] = 0x00;
        SSDLCD_WriteCommandData(SSD_set_hori_period, TempData, 8); //设置LCD水平驱动参数

        TempData[0] = SSD_VT >> 8; //垂直总像素
        TempData[1] = SSD_VT & 0x00FF;
        TempData[2] = SSD_VPS >> 8;     //垂直同步信号开始到行有效数据开始之间的相素时钟
        TempData[3] = SSD_VPS & 0x00FF; //(SSD_VER_FRONT_PORCH+1)&0x00FF;
        TempData[4] = SSD_VPW;          //SSD_VER_FRONT_PORCH-1;//垂直同步信号宽度
        TempData[5] = SSD_FPS >> 8;     //垂直同步信号起始位置
        TempData[6] = SSD_FPS & 0x00FF;
        SSDLCD_WriteCommandData(SSD_set_vert_period, TempData, 7); //设置LCD垂直驱动参数

        TempData[0] = 0x03;                                                 //RGB565
        SSDLCD_WriteCommandData(SSD_set_pixel_data_interface, TempData, 1); //设置LCD像素点格式
        memset(TempReadBuff, 0, sizeof(TempReadBuff));                      //数组清零
        SSDLCD_ReadCommandData(SSD_get_pixel_data_interface, TempReadBuff, 1);
        if (TempReadBuff[0] != TempData[0])
        {
            SSDLCD_WriteCommandData(SSD_set_pixel_data_interface, TempData, 1); //设置LCD像素点格式
        }

        SSDLCD_WriteCommand(SSD_set_display_on); //开启显示

        TempData[0] = 0x00;
        SSDLCD_WriteCommandData(SSD_set_dbc_conf, TempData, 1); //设置LCD自动白平衡

        //GPIO
        TempData[0] = 0x01;
        TempData[1] = 0x01;
        SSDLCD_WriteCommandData(SSD_set_gpio_conf, TempData, 2); //GPIO设置 0x00B8
        TempData[0] = 0x01;
        SSDLCD_WriteCommandData(SSD_set_gpio_value, TempData, 1); //GPIO0输出高电平

        //PWM
    }
    return SSDInitStatus;
}

/**
  * @brief  用固定颜色清屏函数.
  * @param
    Backlight: 控制背光0-255对应0~100%光
  * @retval None
  */
void SSDLCD_BL(uint16_t Backlight)
{
    uint16_t TempWriteData[7] = {0};
    uint16_t TempReadData[7] = {0};

    TempWriteData[0] = 0x05;      //PWM signal frequency = PLL clock / (256 * PWMF[7:0]) / 256 60Hz
    TempWriteData[1] = Backlight; //Set the PWM duty cycle PWM duty cycle = PWM[7:0] / 256
    TempWriteData[2] = 0x01;      //PWM controlled by host PWM enable
    TempWriteData[3] = 0x00;      //
    TempWriteData[4] = 0x00;      //
    TempWriteData[5] = 0x00;      //
    SSDLCD_ReadCommandData(SSD_get_pwm_conf, TempReadData, 6);
    do
    {
        SSDLCD_WriteCommandData(SSD_set_pwm_conf, TempWriteData, 6); //GPIO设置 0x00B8
        STM32Delay_ms(10);
        SSDLCD_ReadCommandData(SSD_get_pwm_conf, TempReadData, 6);
    } while (SSDLCDStatus && (TempWriteData[0] != TempReadData[0] || TempWriteData[1] != TempReadData[1] || TempWriteData[2] != TempReadData[2] || 
    TempWriteData[3] != TempReadData[3] || TempWriteData[4] != TempReadData[4] || TempWriteData[5] != TempReadData[5]));
}

/**
  * @brief  用固定颜色清屏函数.
  * @param  Color: 颜色控制参数RGB565
  * @retval None
  */
void SSDLCD_Clear(uint16_t Color)
{
    uint16_t TempData[4] = {0};

    TempData[0] = 0x00;
    TempData[1] = 0x00;
    TempData[2] = (SSD_HOR_RESOLUTION - 1) >> 8;
    TempData[3] = (SSD_HOR_RESOLUTION - 1) & 0x00FF;
    SSDLCD_WriteCommandData(SSD_set_column_address, TempData, 4); //设置LCD显示区域X轴的坐标范围

    TempData[0] = 0x00;
    TempData[1] = 0x00;
    TempData[2] = (SSD_VER_RESOLUTION - 1) >> 8;
    TempData[3] = (SSD_VER_RESOLUTION - 1) & 0x00FF;
    SSDLCD_WriteCommandData(SSD_set_page_address, TempData, 4); //设置LCD显示区域Y轴的坐标范围

    SSDLCD_WriteCommand(SSD_write_memory_start); //设置LCD显示区域X轴的坐标范围
    for (uint16_t i = 0; i < SSD_VER_RESOLUTION; i++)
        for (uint16_t j = 0; j < SSD_HOR_RESOLUTION; j++)
            SSDLCD_WriteData(Color);
}

/**
  * @brief  用固定颜色充填区域函数.
  * @param  x1
  * @param  y1
  * @param  x2
  * @param  y2
  * @retval None
  */
void SSDLCD_Area_Set(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    uint16_t TempData[4] = {0};

    TempData[0] = ((x1 - 1) >> 8) & 0x00FF;
    TempData[1] = (x1 - 1) & 0x00FF;
    TempData[2] = ((x2 - 1) >> 8) & 0x00FF;
    TempData[3] = (x2 - 1) & 0x00FF;
    SSDLCD_WriteCommandData(SSD_set_column_address, TempData, 4); //设置LCD显示区域X轴的坐标范围

    TempData[0] = ((y1 - 1) >> 8) & 0x00FF;
    TempData[1] = (y1 - 1) & 0x00FF;
    TempData[2] = ((y2 - 1) >> 8) & 0x00FF;
    TempData[3] = (y2 - 1) & 0x00FF;
    SSDLCD_WriteCommandData(SSD_set_page_address, TempData, 4); //设置LCD显示区域Y轴的坐标范围

    SSDLCD_WriteCommand(SSD_write_memory_start); //设置LCD显示区域X轴的坐标范围
}

/**
  * @brief  用固定颜色充填区域函数.
  * @param  Color: 颜色控制参数RGB565
  * @retval None
  */
void SSDLCD_Area_Colur(uint16_t Color)
{
    SSDLCD_WriteData(Color);
}

/**
  * @brief  用固定颜色充填区域函数.
  * @param  x1
  * @param  y1
  * @param  x2
  * @param  y2
  * @param  Color: 颜色控制参数RGB565
  * @retval None
  */
void SSDLCD_Area_Fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t Color)
{
    uint16_t TempData[4] = {0};

    TempData[0] = ((x1 - 1) >> 8) & 0x00FF;
    TempData[1] = (x1 - 1) & 0x00FF;
    TempData[2] = ((x2 - 1) >> 8) & 0x00FF;
    TempData[3] = (x2 - 1) & 0x00FF;
    SSDLCD_WriteCommandData(SSD_set_column_address, TempData, 4); //设置LCD显示区域X轴的坐标范围

    TempData[0] = ((y1 - 1) >> 8) & 0x00FF;
    TempData[1] = (y1 - 1) & 0x00FF;
    TempData[2] = ((y2 - 1) >> 8) & 0x00FF;
    TempData[3] = (y2 - 1) & 0x00FF;
    SSDLCD_WriteCommandData(SSD_set_page_address, TempData, 4); //设置LCD显示区域Y轴的坐标范围

    SSDLCD_WriteCommand(SSD_write_memory_start); //设置LCD显示区域X轴的坐标范围
    for (uint16_t i = 0; i < (x2 - x1); i++)
        for (uint16_t j = 0; j < (y2 - y1); j++)
            SSDLCD_WriteData(Color);
}