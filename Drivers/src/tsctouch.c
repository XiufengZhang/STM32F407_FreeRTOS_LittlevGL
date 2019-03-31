#include <stdlib.h>
#include <string.h>
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "gpio.h"
#include "spi.h"
#include "timer.h"
#include "tsctouch.h"


ErrorStatus RTTouchStatus = ERROR;//标记电阻屏触摸驱动IC初始化是否正常 0失败 1正常
uint16_t TOUCH_AD_LEFT = 95;    //70低分辨率电阻屏
uint16_t TOUCH_AD_RIGHT = 1900; //1940低分辨率电阻屏
uint16_t TOUCH_AD_TOP = 140;    //125低分辨率电阻屏
uint16_t TOUCH_AD_BOTTOM = 1750;//1760低分辨率电阻屏
uint16_t TSC_CoodData[3] = {0};//0位表示是否有按键按下，1位X坐标，2位Y坐标 电阻屏校准使用

/**
  * @brief  TSC TOUCH初始化
  * @param  None
  * @retval None
  */
ErrorStatus TSC_Init(void)
{
    uint8_t TempData = 0;
    
    SPI2_Init();
    GpioSPITouchCSControl(ENABLE);
    SPI2_SendData(TSCCMD_RDX);//发送读取指令
    SPI2_ReadData(&TempData);
    SPI2_SendData(0x00);//读取第一个字节
    SPI2_ReadData(&TempData);
    SPI2_SendData(0x00);//读取第二个字节
    SPI2_ReadData(&TempData);    
    if(GPIO_ReadInputDataBit(SPIT_BUSY_GPIO_PORT, SPIT_BUSY_GPIO_PIN))//如果SPI初始化后 busy引脚还是高电平说明触摸IC初始化失败 CS片选后busy为低电平
        if(GPIO_ReadInputDataBit(SPIT_BUSY_GPIO_PORT, SPIT_BUSY_GPIO_PIN))
        {
            GpioSPITouchCSControl(DISABLE);
            return ERROR;
        }
    GpioSPITouchCSControl(DISABLE);
    return SUCCESS;
}

/**
  * @brief  TSC TOUCH读取触摸坐标
  * @param  None
  * @retval None
  */
uint16_t TSC_ReadAD(uint8_t TSCCMD)
{
    uint8_t Temp = 0;
    uint16_t TSCReadData = 0;
    
    GpioSPITouchCSControl(ENABLE);
    SPI2_SendData(TSCCMD);//发送读取指令
    SPI2_ReadData(&Temp);
    SPI2_SendData(0x00);//读取第一个字节
    SPI2_ReadData(&Temp);
    TSCReadData |= (uint16_t)Temp << 4;
    SPI2_SendData(0x00);//读取第二个字节
    SPI2_ReadData(&Temp);
    TSCReadData |= Temp >> 4;
    GpioSPITouchCSControl(DISABLE);
    
    return TSCReadData;
}

/**
  * @brief  TSC TOUCH读取滤波后坐标
  * @param  None
  * @retval None
  */
uint16_t TSC_ReadFilter(uint8_t TSCCMD)
{
    uint8_t i = 0;
    uint16_t ReadTemp = 0;
    uint16_t ReadData[TSCCMD_COUNT] = {0};
    
    if(GPIO_ReadInputDataBit(SPIT_INT_GPIO_PORT, SPIT_INT_GPIO_PIN))
    {
        return ReadTemp;
    }
    while(!GPIO_ReadInputDataBit(SPIT_INT_GPIO_PORT, SPIT_INT_GPIO_PIN) && i < TSCCMD_COUNT)
    {
        ReadTemp = TSC_ReadAD(TSCCMD);
        if(ReadTemp > 20)//有效数据大于20
        {
            ReadData[i] = ReadTemp;
            i++;
        }
        STM32Delay_us(20);
    }
    ReadTemp = 0;
    if(GPIO_ReadInputDataBit(SPIT_INT_GPIO_PORT, SPIT_INT_GPIO_PIN))
    {
        return ReadTemp;
    }
    if(i == TSCCMD_COUNT)
    {
        for(i=0; i<TSCCMD_COUNT-1; i++)
        {
            for(uint8_t j=i+1; j<TSCCMD_COUNT; j++)
            {
                if(ReadData[i] > ReadData[j])
                {
                    ReadTemp = ReadData[i];
                    ReadData[i] = ReadData[j];
                    ReadData[j] = ReadTemp;
                }
            }
        }
        ReadTemp = 0;
        for(i=TSCCMD_REMO; i<TSCCMD_COUNT-TSCCMD_REMO; i++)
        {
            ReadTemp += ReadData[i];
        }
        ReadTemp = ReadTemp/(TSCCMD_COUNT-TSCCMD_REMO*2);
        return ReadTemp;
    }
    else
        return ReadTemp;
}

/**
  * @brief  TSC触摸IC 读取触摸坐标
  * @param  None.
  * @retval None.
  */
uint16_t TSC_ReadCoord(uint8_t TSCCMD)
{
    uint16_t ReadTemp = 0;
    uint16_t ReadData[2] = {0};
    
    ReadData[0] = TSC_ReadFilter(TSCCMD);
    ReadData[1] = TSC_ReadFilter(TSCCMD);
    while(abs(ReadData[0]-ReadData[1]) > TSCCMD_MINI)
    {
        ReadData[0] = TSC_ReadFilter(TSCCMD);
        ReadData[1] = TSC_ReadFilter(TSCCMD);
    }
    if(ReadData[0] == 0 || ReadData[1] == 0)
    {
        TSC_CoodData[0] = 0;
    }
    else
    {
        ReadTemp = (ReadData[0] + ReadData[1])/2;//配置触摸屏XY轴是否对调或者正负反转问题 在GUI_TOUCH_SetOrientation函数中处理
        TSC_CoodData[0] = 1;
        if(TSCCMD == TSCCMD_RDX)
            TSC_CoodData[1] = ReadTemp;
        else
            TSC_CoodData[2] = ReadTemp;
    }
    return ReadTemp;
}