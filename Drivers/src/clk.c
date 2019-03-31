#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_rtc.h"
#include "stm32f4xx_pwr.h"
#include "clk.h"

ErrorStatus CLKHSEStatus = ERROR; //标记外部晶振启动是否正常 ERROR失败 SUCCESS正常

/**
  * @brief  时钟 外设初始化
  * @param  None.
  * @retval None.
  */
ErrorStatus RccInit(void) //主时钟168MHz APB2 84MHz APB1 48MHz
{
    // RCC_ClocksTypeDef TempRCC_Clocks;
    // RCC_GetClocksFreq(&TempRCC_Clocks);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); //RTC BKPSRAM 放置在RTC文件处理
    PWR_BackupAccessCmd(ENABLE);
    if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x5050) //从指定的后备寄存器中读出数据:读出了与写入的指定数据不相符
    {
        RCC_BackupResetCmd(ENABLE);  //复位备份区域
        RCC_BackupResetCmd(DISABLE); //复位备份区域
        RCC_LSEConfig(RCC_LSE_ON);
    }

    //RCC_AHB1PeriphClockCmd GPIO CRC BKPSRAM DMA ETH USB
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);
#if defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F469_479xx)
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOJ, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOK, ENABLE);
#endif
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
#if defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F469_479xx)
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2D, ENABLE);
#endif
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC_Tx, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC_PTP, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_OTG_HS, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_OTG_HS_ULPI, ENABLE);

    //RCC_AHB2PeriphClockCmd 数字摄像头(DCMI) 加密处理器 (CRYP) 散列处理器 (HASH) 随机数发生器 (RNG)
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI, ENABLE);
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_CRYP, ENABLE);
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_HASH, ENABLE);
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, ENABLE);

    //RCC_AHB3PeriphClockCmd FSMC/FMC
#if defined(STM32F40_41xxx)
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
#elif defined(STM32F427_437xx) || defined(STM32F429_439xx)
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE);
#elif defined(STM32F412xG) || defined(STM32F413_423xx)
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_QSPI, ENABLE);
#elif defined(STM32F446xx) || defined(STM32F469_479xx)
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE);
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_QSPI, ENABLE);
#endif

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_EXTIT, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI4, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI5, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI6, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, ENABLE);
#if defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F446xx) || defined(STM32F469_479xx) || defined(STM32F413_423xx)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SAI1, ENABLE);
#endif
#if defined(STM32F446xx)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SAI2, ENABLE);
#endif
#if defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F469_479xx)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_LTDC, ENABLE);
#endif
#if defined(STM32F469_479xx)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_DSI, ENABLE);
#endif
#if defined(STM32F412xG)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_DFSDM1, ENABLE);
#endif
#if defined(STM32F413_423xx)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_DFSDM1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_DFSDM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_UART9, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_UART10, ENABLE);
#endif

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //PWM输出调整背光
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //延时中断
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE); //1ms中断
#if defined(STM32F410xx) || defined(STM32F413_423xx)
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_LPTIM1, ENABLE);
#endif
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE); //WWDG
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
#if defined(STM32F446xx)
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPDIF, ENABLE);
#endif
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART7, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART8, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    //I2C1 I2C2 I2C3 FMPI2C1 CAN1 CAN2 CEC

    if (RCC_GetFlagStatus(RCC_FLAG_HSERDY))
        return SUCCESS;
    else
        return ERROR;
}