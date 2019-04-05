#include <stdlib.h>
#include <string.h>
#include "misc.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "gpio.h"
#include "timer.h"
#include "fttouch.h"
// #include "GUI.h"

ErrorStatus FTTouchStatus = ERROR;//标记TF触摸驱动IC初始化是否正常 0失败 1正常
uint8_t FTTouchUpFlag = 0;//FTTouch触摸中断标记，在main函数中跟新触摸坐标数据
uint16_t FT_CoodData[4] = {0};//0位表示是否有按键按下，1位X坐标，2位Y坐标，3位按下状态，按下 抬起 保持

/**
  * @brief  TG触摸IC中断输入使能.
  * @param  None.
  * @retval None.
  */
static void  FT_IRQEnable(FunctionalState IRQENABLE)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    if(IRQENABLE)
    {
        SYSCFG_EXTILineConfig(I2CT_INT_EXTI_PORTSOURCE, I2CT_INT_EXTI_PINSOURCE);//连接中断源
        EXTI_ClearITPendingBit(I2CT_INT_EXTI_LINE);//清除中断标识
        EXTI_InitStructure.EXTI_Line = I2CT_INT_EXTI_LINE;
        EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Falling;//上升沿下降沿触发EXTI_Trigger_Falling EXTI_Trigger_Rising EXTI_Trigger_Rising_Falling
        EXTI_InitStructure.EXTI_LineCmd = ENABLE;
        EXTI_Init(&EXTI_InitStructure);
        
        NVIC_InitStructure.NVIC_IRQChannel = I2CT_INT_EXTI_IRQ;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
    }
    else
    {
        //GPIO_ResetBits(I2CT_RST_GPIO_PORT, I2CT_RST_GPIO_PIN);//复位FTP_I2C_RST
        
        EXTI_ClearITPendingBit(I2CT_INT_EXTI_LINE);//清除中断标识
        EXTI_InitStructure.EXTI_Line = I2CT_INT_EXTI_LINE;
        EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
        EXTI_InitStructure.EXTI_LineCmd = DISABLE;
        EXTI_Init(&EXTI_InitStructure);
    }
}

/**
  * @brief  TG触摸IC复位.
  * @param  None.
  * @retval None.
  */
static void FT_ResetChip(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    GPIO_ResetBits(I2CT_RST_GPIO_PORT, I2CT_RST_GPIO_PIN);//复位FTP_I2C_RST
    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Pin = I2CT_INT_GPIO_PIN;
    GPIO_Init(I2CT_INT_GPIO_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(I2CT_INT_GPIO_PORT, I2CT_INT_GPIO_PIN);//复位FTP_I2C_INT
    
    STM32Delay_ms(10);//延时10ms
    GPIO_SetBits(I2CT_RST_GPIO_PORT, I2CT_RST_GPIO_PIN);//置位FTP_I2C_RST
    
    STM32Delay_ms(300);//延时300ms
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = I2CT_INT_GPIO_PIN;
    GPIO_Init(I2CT_INT_GPIO_PORT, &GPIO_InitStructure);//浮空输入
}

/**
  * @brief  TG触摸IC I2C模拟通讯延时.
  * @param  None.
  * @retval None.
  */
static void FT_IIC_Delay(void)
{
    volatile uint8_t i;
    
    for (i = 0; ; i++)//;//STM32F103 5~12之间，12是200K频率,5是400K频率;STM32F429 30~50之间，30是400K频率
    {
        if(i > 35)
            break;
    }
}

/**
  * @brief  TG触摸IC I2C_SDA输出使能.
  * @param  None.
  * @retval None.
  */
static void FT_IIC_SDA_OUT(FunctionalState Enable)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    if(Enable)
    {
        if((I2CT_SDA_GPIO_PORT->MODER & I2CT_SDA_MODER) == 0x0000)
        {
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
            GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
            GPIO_InitStructure.GPIO_Pin = I2CT_SDA_GPIO_PIN;
            if(GPIO_ReadInputDataBit(I2CT_SDA_GPIO_PORT, I2CT_SDA_GPIO_PIN))
            {
                GPIO_Init(I2CT_SDA_GPIO_PORT, &GPIO_InitStructure);
                GPIO_SetBits(I2CT_SDA_GPIO_PORT, I2CT_SDA_GPIO_PIN);
            }
            else
            {
                GPIO_Init(I2CT_SDA_GPIO_PORT, &GPIO_InitStructure);
                GPIO_ResetBits(I2CT_SDA_GPIO_PORT, I2CT_SDA_GPIO_PIN);
            }
        }
    }
    else
    {
        if((I2CT_SDA_GPIO_PORT->MODER & I2CT_SDA_MODER) != 0x0000)
        {
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
            GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
            GPIO_InitStructure.GPIO_Pin = I2CT_SDA_GPIO_PIN;
            GPIO_Init(I2CT_SDA_GPIO_PORT, &GPIO_InitStructure);
        }
    }
}

/**
  * @brief  TG触摸IC I2C产生起始信号.
  * @param  None.
  * @retval None.
  */
static void FT_IIC_Start(void)
{
    FT_IIC_SDA_OUT(ENABLE);
    GPIO_SetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    GPIO_SetBits(I2CT_SDA_GPIO_PORT, I2CT_SDA_GPIO_PIN);
    FT_IIC_Delay();
    GPIO_ResetBits(I2CT_SDA_GPIO_PORT, I2CT_SDA_GPIO_PIN);
    FT_IIC_Delay();
    GPIO_ResetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    FT_IIC_Delay();
}

/**
  * @brief  TG触摸IC I2C产生停止信号.
  * @param  None.
  * @retval None.
  */
static void FT_IIC_Stop(void)
{
    FT_IIC_SDA_OUT(ENABLE);
    GPIO_SetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    GPIO_ResetBits(I2CT_SDA_GPIO_PORT, I2CT_SDA_GPIO_PIN);
    FT_IIC_Delay();
    GPIO_SetBits(I2CT_SDA_GPIO_PORT, I2CT_SDA_GPIO_PIN);
    FT_IIC_Delay();
}

/**
  * @brief  TG触摸IC I2C产生应答信号.
  * @param  None.
  * @retval None.
  */
static void FT_IIC_Ack(void)
{
    GPIO_ResetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    FT_IIC_SDA_OUT(ENABLE);
    GPIO_ResetBits(I2CT_SDA_GPIO_PORT, I2CT_SDA_GPIO_PIN);
    FT_IIC_Delay();
    GPIO_SetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    FT_IIC_Delay();
    GPIO_ResetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    FT_IIC_Delay();
}

/**
  * @brief  TG触摸IC I2C不产生应答信号.
  * @param  None.
  * @retval None.
  */
static void FT_IIC_NAck(void)
{
    GPIO_ResetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    FT_IIC_SDA_OUT(ENABLE);
    GPIO_SetBits(I2CT_SDA_GPIO_PORT, I2CT_SDA_GPIO_PIN);
    FT_IIC_Delay();
    GPIO_SetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    FT_IIC_Delay();
    GPIO_ResetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    GPIO_ResetBits(I2CT_SDA_GPIO_PORT, I2CT_SDA_GPIO_PIN);
    FT_IIC_Delay();
}

/**
  * @brief  TG触摸IC I2C等待应答信号.
  * @param  None.
  * @retval ErrorStatus返回应答状态.
  */
static ErrorStatus FT_IIC_Wait_Ack(void)
{
    uint16_t FTErrTime=0;
    
    GPIO_ResetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    FT_IIC_SDA_OUT(DISABLE);
    FT_IIC_Delay();
    GPIO_SetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    FT_IIC_Delay();
    while(GPIO_ReadInputDataBit(I2CT_SDA_GPIO_PORT, I2CT_SDA_GPIO_PIN))
    {
        FTErrTime++;
        if(FTErrTime>300)
        {
            FT_IIC_Stop();
            return ERROR;
        }
    }
    GPIO_ResetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    FT_IIC_Delay();
    return SUCCESS;
}

/**
  * @brief  TG触摸IC I2C发送一个字节.
  * @param  data要发送的数据.
  * @retval None.
  */
static void FT_IIC_Send_Byte(uint8_t data)
{
    uint8_t i;
    
    GPIO_ResetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    FT_IIC_SDA_OUT(ENABLE);
    //GT_IIC_Delay();
    for(i=0;i<8;i++)
    {
        if((data&0x80) == 0x80)
        {
            GPIO_SetBits(I2CT_SDA_GPIO_PORT, I2CT_SDA_GPIO_PIN);
        }
        else
        {
            GPIO_ResetBits(I2CT_SDA_GPIO_PORT, I2CT_SDA_GPIO_PIN);
        }
        data<<=1;
        FT_IIC_Delay();
        GPIO_SetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
        FT_IIC_Delay();
        GPIO_ResetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
        FT_IIC_Delay();
    }
}

/**
  * @brief  TG触摸IC I2C接收一个字节.
  * @param  None.
  * @retval uint8_t要接收的数据.
  */
static uint8_t FT_IIC_Read_Byte(void)
{
    uint8_t i,receive=0;
    
    GPIO_ResetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    FT_IIC_SDA_OUT(DISABLE);
    for(i=0;i<8;i++)
    {
        receive<<=1;
        FT_IIC_Delay();
        GPIO_SetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
        FT_IIC_Delay();
        if(GPIO_ReadInputDataBit(I2CT_SDA_GPIO_PORT, I2CT_SDA_GPIO_PIN))
            receive|=0x01;
        FT_IIC_Delay();
        GPIO_ResetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    }
    return receive;
}

/**
  * @brief  TG触摸IC 写入多个字节
  * @param  SlaveAddress I2C从设备7位地址
  * @param  RegisterAddress I2C从设备寄存器地址
  * @param  pWriteDataBuffer 写入数组指针
  * @retval ErrorStatus 是否写入成功状态
  */
static ErrorStatus FT_IIC_WriteData(uint8_t SlaveAddress, uint8_t RegisterAddress, const uint8_t *pWriteDataBuffer, uint8_t WriteDataCount)
{
    uint16_t Temp;
    uint16_t i;
    
    Temp = SlaveAddress<<1;
    FT_IIC_Start();
    FT_IIC_Send_Byte(Temp);
    if(!FT_IIC_Wait_Ack())
    {
        FT_IIC_Stop();
        return ERROR;
    }
    FT_IIC_Send_Byte((uint8_t)RegisterAddress);
    if(!FT_IIC_Wait_Ack())
    {
        FT_IIC_Stop();
        return ERROR;
    }
    for(i=0; i<WriteDataCount; i++)
    {
        FT_IIC_Send_Byte(*pWriteDataBuffer);
        pWriteDataBuffer++;
        if(!FT_IIC_Wait_Ack())
        {
            FT_IIC_Stop();
            return ERROR;
        }
    }
    FT_IIC_Stop();
    return SUCCESS;
}

/**
  * @brief  TG触摸IC 读取多个字节
  * @param  SlaveAddress I2C从设备7位地址
  * @param  RegisterAddress I2C从设备寄存器地址
  * @param  pReadDataBuffer 读取数组指针
  * @retval ErrorStatus 是否写入成功状态
  */
static ErrorStatus FT_IIC_ReadData(uint8_t SlaveAddress, uint8_t RegisterAddress, uint8_t *pReadDataBuffer, uint8_t ReadDataCount)
{
    uint16_t Temp;
    uint16_t i;
    
    Temp = SlaveAddress<<1;
    FT_IIC_Start();
    FT_IIC_Send_Byte(Temp);
    if(!FT_IIC_Wait_Ack())
    {
        FT_IIC_Stop();
        return ERROR;
    }
    FT_IIC_Send_Byte((uint8_t)RegisterAddress);
    if(!FT_IIC_Wait_Ack())
    {
        FT_IIC_Stop();
        return ERROR;
    }
    FT_IIC_Start();
    Temp |= 0x01;
    FT_IIC_Send_Byte(Temp);
    if(!FT_IIC_Wait_Ack())
    {
        FT_IIC_Stop();
        return ERROR;
    }
    for(i=0; i<ReadDataCount; i++)
    {
        *pReadDataBuffer = FT_IIC_Read_Byte();
        pReadDataBuffer++;
        if(i<(ReadDataCount-1))
        {
            FT_IIC_Ack();
        }
        else
        {
            FT_IIC_NAck();
        }
    }
    FT_IIC_Stop();
    return SUCCESS;
}

/**
  * @brief  TG触摸IC 初始化配置
  * @param  None.
  * @retval None.
  */
ErrorStatus FT_IIC_Initiali(void)
{
    uint8_t TempVersion[2] = {0};
    
    FT_IRQEnable(DISABLE);
    FT_ResetChip();
    
    TempVersion[0] = 0;
    if(!FT_IIC_WriteData(FTSlaveAddress, FTDevideMode, TempVersion, 1))//0正常模式，1信息模式，4测试模式
        if(!FT_IIC_WriteData(FTSlaveAddress, FTDevideMode, TempVersion, 1))
            return ERROR;
    TempVersion[0] = 21;
    if(!FT_IIC_WriteData(FTSlaveAddress, FTTouchingThreshold, TempVersion, 1))
        if(!FT_IIC_WriteData(FTSlaveAddress, FTTouchingThreshold, TempVersion, 1))
            return ERROR;
    /*
    TempVersion[0] = 60;
    if(!FT_IIC_WriteData(FTSlaveAddress, FTWaterThreshold, TempVersion, 1))
        if(!FT_IIC_WriteData(FTSlaveAddress, FTWaterThreshold, TempVersion, 1))
            return ERROR;
    TempVersion[0] = 12;
    if(!FT_IIC_WriteData(FTSlaveAddress, FTActivePeriod, TempVersion, 1))
        if(!FT_IIC_WriteData(FTSlaveAddress, FTActivePeriod, TempVersion, 1))
            return ERROR;
    TempVersion[0] = 40;
    if(!FT_IIC_WriteData(FTSlaveAddress, FTMonitorPeriod, TempVersion, 1))//Monitor周期
        if(!FT_IIC_WriteData(FTSlaveAddress, FTMonitorPeriod, TempVersion, 1))
            return ERROR;
    */
    TempVersion[0] = 1;//采用中断读取方式
    //TempVersion[0] = 0;//采用循环读取方式
    if(!FT_IIC_WriteData(FTSlaveAddress, FTInterrupt, TempVersion, 1))
        if(!FT_IIC_WriteData(FTSlaveAddress, FTInterrupt, TempVersion, 1))
            return ERROR;
    
    FT_IIC_ReadData(FTSlaveAddress, FTLibraryVersion, TempVersion, 2);
    if(TempVersion[0] == 0x30 && TempVersion[1] == 0x03)//FT5306
    {
        FT_IRQEnable(ENABLE);
        return SUCCESS;
    }
    else if(TempVersion[0] == 0x30 && TempVersion[1] == 0xF8)//FT5216
    {
        FT_IRQEnable(ENABLE);
        return SUCCESS;
    }
    else
        return ERROR;
}

/**
  * @brief  TG触摸IC 读取触摸坐标
  * @param  None.
  * @retval None.
  */
void  FT_IIC_ReadCoord(void)
{
    uint8_t Temp[5] = {0};
    int CoodTemp[4] = {0};
    
    FT_IIC_ReadData(FTSlaveAddress, FTTouchNumber, Temp, 5);//读取坐标信息
    if((Temp[0]&0x0F) > 0x00)
    {
        CoodTemp[0] = 1;
        CoodTemp[1] = (uint16_t)(Temp[1]&0x0F)<<8 | (uint16_t)Temp[2];
        CoodTemp[2] = (uint16_t)(Temp[3]&0x0F)<<8 | (uint16_t)Temp[4];
        CoodTemp[3] = (uint16_t)(Temp[1]&0xC0)>>6;
        
    }
    else
    {
//        CoodTemp[0] = 0;,CoodTemp[3] = 0;
        CoodTemp[1] = -1;
        CoodTemp[2] = -1;
    }
    
    if(CoodTemp[0] != FT_CoodData[0])
    {
    //   GUI_TOUCH_StoreState(CoodTemp[1], CoodTemp[2]);
    }
    else
    {
    //   if(abs(CoodTemp[1] - FT_CoodData[1]) > FT_PIXEL_ERROR || abs(CoodTemp[2] - FT_CoodData[2]) > FT_PIXEL_ERROR)
        // GUI_TOUCH_StoreState(CoodTemp[1], CoodTemp[2]);
    }
    
    memcpy(FT_CoodData, CoodTemp, sizeof(FT_CoodData));
    
//    GUI_PID_STATE State;//使用GUI_TOUCH_StoreStateEx函数，ICONVIEW小工具不能接收释放消息
//    State.Pressed = FT_CoodData[0];
//    State.x = FT_CoodData[1];
//    State.y = FT_CoodData[2];
//    GUI_TOUCH_StoreStateEx(&State);
}