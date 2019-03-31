#include <stdlib.h>
#include <string.h>
#include "misc.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "gpio.h"
#include "timer.h"
#include "gttouch.h"
#include "GUI.h"

ErrorStatus GTTouchStatus = ERROR;//标记GF触摸驱动IC初始化是否正常 0失败 1正常
uint8_t GTTouchUpFlag = 0;//FTTouch触摸中断标记，在main函数中跟新触摸坐标数据
uint16_t GT_CoodData[4] = {0};//0位表示是否有按键按下，1位X坐标，2位Y坐标，3位触摸面积

// 5寸屏GT9157驱动配置
uint8_t CTP_CFG_GT9157[] = {
    0x00,0x20,0x03,0xE0,0x01,0x01,0x3C,0x00,0x01,0x08,//触点数量0x804C寄存器 0x804D 00：上升沿触发
    0x28,0x0C,0x50,0x32,0x03,0x05,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x17,0x19,0x1E,0x14,0x8B,0x2B,0x0D,
    0x33,0x35,0x0C,0x08,0x00,0x00,0x00,0x9A,0x03,0x11,
    0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,
    0x00,0x20,0x58,0x94,0xC5,0x02,0x00,0x00,0x00,0x04,
    0xB0,0x23,0x00,0x93,0x2B,0x00,0x7B,0x35,0x00,0x69,
    0x41,0x00,0x5B,0x4F,0x00,0x5B,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0E,0x10,
    0x12,0x14,0x16,0x18,0x1A,0xFF,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0F,
    0x10,0x12,0x13,0x16,0x18,0x1C,0x1D,0x1E,0x1F,0x20,
    0x21,0x22,0x24,0x26,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
    0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0x48,0x01
};

// 7寸屏GT911驱动配置
uint8_t CTP_CFG_GT911[] = {
    0x00,0x20,0x03,0xE0,0x01,0x01,0x3D,0x00,0x01,0x48,//触点数量0x804C寄存器 0x804D 00：上升沿触发
    0x28,0x0D,0x50,0x32,0x03,0x05,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x18,0x1A,0x1E,0x14,0x8A,0x2A,0x0C,
    0x30,0x38,0x31,0x0D,0x00,0x00,0x02,0xB9,0x03,0x2D,
    0x00,0x00,0x00,0x00,0x00,0x03,0x64,0x32,0x00,0x00,
    0x00,0x1D,0x41,0x94,0xC5,0x02,0x07,0x00,0x00,0x04,
    0xA5,0x1F,0x00,0x94,0x25,0x00,0x88,0x2B,0x00,0x7D,
    0x33,0x00,0x74,0x3C,0x00,0x74,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x18,0x16,0x14,0x12,0x10,0x0E,0x0C,0x0A,
    0x08,0x06,0x04,0x02,0xFF,0xFF,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x24,0x22,0x21,0x20,0x1F,0x1E,0x1D,0x1C,
    0x18,0x16,0x13,0x12,0x10,0x0F,0x0A,0x08,0x06,0x04,
    0x02,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x11,0x01
};

/**
  * @brief  TG触摸IC中断输入使能.
  * @param  None.
  * @retval None.
  */
static void  GT_IRQEnable(FunctionalState IRQENABLE)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    if(IRQENABLE)
    {
        SYSCFG_EXTILineConfig(I2CT_INT_EXTI_PORTSOURCE, I2CT_INT_EXTI_PINSOURCE);//连接中断源
        EXTI_ClearITPendingBit(I2CT_INT_EXTI_LINE);//清除中断标识
        EXTI_InitStructure.EXTI_Line = I2CT_INT_EXTI_LINE;
        EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
        EXTI_InitStructure.EXTI_LineCmd = ENABLE;
        EXTI_Init(&EXTI_InitStructure);
        
        NVIC_InitStructure.NVIC_IRQChannel = I2CT_INT_EXTI_IRQ;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
    }
    else
    {
        //GPIO_ResetBits(I2CT_RST_GPIO_PORT, I2CT_RST_GPIO_PIN);//复位GTP_I2C_RST
        
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
static void  GT_ResetChip(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    GPIO_ResetBits(I2CT_RST_GPIO_PORT, I2CT_RST_GPIO_PIN);//复位GTP_I2C_RST
    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Pin = I2CT_INT_GPIO_PIN;
    GPIO_Init(I2CT_INT_GPIO_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(I2CT_INT_GPIO_PORT, I2CT_INT_GPIO_PIN);//复位GTP_I2C_INT
    
    STM32Delay_ms(5);//延时1ms
    GPIO_SetBits(I2CT_RST_GPIO_PORT, I2CT_RST_GPIO_PIN);//置位GTP_I2C_RST
    
    STM32Delay_ms(10);//延时10ms
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
static void GT_IIC_Delay(void)
{
    volatile uint8_t i;
    
    for (i = 0; ; i++)//STM32F103 5~12之间，12是200K频率,5是400K频率;STM32F429 30~50之间，30是400K频率
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
static void GT_IIC_SDA_OUT(FunctionalState Enable)
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
static void GT_IIC_Start(void)
{
    GT_IIC_SDA_OUT(ENABLE);
    GPIO_SetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    GPIO_SetBits(I2CT_SDA_GPIO_PORT, I2CT_SDA_GPIO_PIN);
    GT_IIC_Delay();
    GPIO_ResetBits(I2CT_SDA_GPIO_PORT, I2CT_SDA_GPIO_PIN);
    GT_IIC_Delay();
    GPIO_ResetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    GT_IIC_Delay();
}

/**
  * @brief  TG触摸IC I2C产生停止信号.
  * @param  None.
  * @retval None.
  */
static void GT_IIC_Stop(void)
{
    GT_IIC_SDA_OUT(ENABLE);
    GPIO_SetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    GPIO_ResetBits(I2CT_SDA_GPIO_PORT, I2CT_SDA_GPIO_PIN);
    GT_IIC_Delay();
    GPIO_SetBits(I2CT_SDA_GPIO_PORT, I2CT_SDA_GPIO_PIN);
    GT_IIC_Delay();
}

/**
  * @brief  TG触摸IC I2C产生应答信号.
  * @param  None.
  * @retval None.
  */
static void GT_IIC_Ack(void)
{
    GPIO_ResetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    GT_IIC_SDA_OUT(ENABLE);
    GPIO_ResetBits(I2CT_SDA_GPIO_PORT, I2CT_SDA_GPIO_PIN);
    GT_IIC_Delay();
    GPIO_SetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    GT_IIC_Delay();
    GPIO_ResetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    GT_IIC_Delay();
}

/**
  * @brief  TG触摸IC I2C不产生应答信号.
  * @param  None.
  * @retval None.
  */
static void GT_IIC_NAck(void)
{
    GPIO_ResetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    GT_IIC_SDA_OUT(ENABLE);
    GPIO_SetBits(I2CT_SDA_GPIO_PORT, I2CT_SDA_GPIO_PIN);
    GT_IIC_Delay();
    GPIO_SetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    GT_IIC_Delay();
    GPIO_ResetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    GPIO_ResetBits(I2CT_SDA_GPIO_PORT, I2CT_SDA_GPIO_PIN);
    GT_IIC_Delay();
}

/**
  * @brief  TG触摸IC I2C等待应答信号.
  * @param  None.
  * @retval ErrorStatus返回应答状态.
  */
static ErrorStatus GT_IIC_Wait_Ack(void)
{
    uint16_t GTErrTime=0;
    
    GPIO_ResetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    GT_IIC_SDA_OUT(DISABLE);
    GT_IIC_Delay();
    GPIO_SetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    GT_IIC_Delay();
    while(GPIO_ReadInputDataBit(I2CT_SDA_GPIO_PORT, I2CT_SDA_GPIO_PIN))
    {
        GTErrTime++;
        if(GTErrTime>300)
        {
            GT_IIC_Stop();
            return ERROR;
        }
    }
    GPIO_ResetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    GT_IIC_Delay();
    return SUCCESS;
}

/**
  * @brief  TG触摸IC I2C发送一个字节.
  * @param  data要发送的数据.
  * @retval None.
  */
static void GT_IIC_Send_Byte(uint8_t data)
{
    uint8_t i;
    
    GPIO_ResetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    GT_IIC_SDA_OUT(ENABLE);
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
        GT_IIC_Delay();
        GPIO_SetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
        GT_IIC_Delay();
        GPIO_ResetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
        GT_IIC_Delay();
    }
}

/**
  * @brief  TG触摸IC I2C接收一个字节.
  * @param  None.
  * @retval uint8_t要接收的数据.
  */
static uint8_t GT_IIC_Read_Byte(void)
{
    uint8_t i,receive=0;
    
    GPIO_ResetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
    GT_IIC_SDA_OUT(DISABLE);
    for(i=0;i<8;i++)
    {
        receive<<=1;
        GT_IIC_Delay();
        GPIO_SetBits(I2CT_SCL_GPIO_PORT, I2CT_SCL_GPIO_PIN);
        GT_IIC_Delay();
        if(GPIO_ReadInputDataBit(I2CT_SDA_GPIO_PORT, I2CT_SDA_GPIO_PIN))
            receive|=0x01;
        GT_IIC_Delay();
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
static ErrorStatus GT_IIC_WriteData(uint8_t SlaveAddress, uint16_t RegisterAddress, const uint8_t *pWriteDataBuffer, uint8_t WriteDataCount)
{
    uint16_t Temp;
    uint16_t i;
    
    Temp = SlaveAddress<<1;
    GT_IIC_Start();
    GT_IIC_Send_Byte(Temp);
    if(!GT_IIC_Wait_Ack())
    {
        GT_IIC_Stop();
        return ERROR;
    }
    GT_IIC_Send_Byte((uint8_t)(RegisterAddress>>8));
    if(!GT_IIC_Wait_Ack())
    {
        GT_IIC_Stop();
        return ERROR;
    }
    GT_IIC_Send_Byte((uint8_t)RegisterAddress);
    if(!GT_IIC_Wait_Ack())
    {
        GT_IIC_Stop();
        return ERROR;
    }
    for(i=0; i<WriteDataCount; i++)
    {
        GT_IIC_Send_Byte(*pWriteDataBuffer);
        pWriteDataBuffer++;
        if(!GT_IIC_Wait_Ack())
        {
            GT_IIC_Stop();
            return ERROR;
        }
    }
    GT_IIC_Stop();
    return SUCCESS;
}

/**
  * @brief  TG触摸IC 读取多个字节
  * @param  SlaveAddress I2C从设备7位地址
  * @param  RegisterAddress I2C从设备寄存器地址
  * @param  pReadDataBuffer 读取数组指针
  * @retval ErrorStatus 是否写入成功状态
  */
static ErrorStatus GT_IIC_ReadData(uint8_t SlaveAddress, uint16_t RegisterAddress, uint8_t *pReadDataBuffer, uint8_t ReadDataCount)
{
    uint16_t Temp;
    uint16_t i;
    
    Temp = SlaveAddress<<1;
    GT_IIC_Start();
    GT_IIC_Send_Byte(Temp);
    if(!GT_IIC_Wait_Ack())
    {
        GT_IIC_Stop();
        return ERROR;
    }
    GT_IIC_Send_Byte((uint8_t)(RegisterAddress>>8));
    if(!GT_IIC_Wait_Ack())
    {
        GT_IIC_Stop();
        return ERROR;
    }
    GT_IIC_Send_Byte((uint8_t)RegisterAddress);
    if(!GT_IIC_Wait_Ack())
    {
        GT_IIC_Stop();
        return ERROR;
    }
    GT_IIC_Start();
    Temp |= 0x01;
    GT_IIC_Send_Byte(Temp);
    if(!GT_IIC_Wait_Ack())
    {
        GT_IIC_Stop();
        return ERROR;
    }
    for(i=0; i<ReadDataCount; i++)
    {
        *pReadDataBuffer = GT_IIC_Read_Byte();
        pReadDataBuffer++;
        if(i<(ReadDataCount-1))
        {
            GT_IIC_Ack();
        }
        else
        {
            GT_IIC_NAck();
        }
    }
    GT_IIC_Stop();
    return SUCCESS;
}

/**
  * @brief  TG触摸IC 初始化配置
  * @param  None.
  * @retval None.
  */
ErrorStatus GT_IIC_Initiali(void)
{
    uint8_t TempVersion[4] = {0};
    
    GT_IRQEnable(DISABLE);
    GT_ResetChip();
    
    if(!GT_IIC_ReadData(GT9xxSlaveAddress, GT9xxIDAddress, TempVersion, 4))//读取版本信息
        if(!GT_IIC_ReadData(GT9xxSlaveAddress, GT9xxIDAddress, TempVersion, 4))//读取版本信息
            return ERROR;
    
    if(TempVersion[0] == '9' && TempVersion[1] == '1' && TempVersion[2] == '1')
    {
        if(!GT_IIC_WriteData(GT9xxSlaveAddress, GT9xxConfigAddress, CTP_CFG_GT911, sizeof(CTP_CFG_GT911)/sizeof(CTP_CFG_GT911[0])))
            if(!GT_IIC_WriteData(GT9xxSlaveAddress, GT9xxConfigAddress, CTP_CFG_GT911, sizeof(CTP_CFG_GT911)/sizeof(CTP_CFG_GT911[0])))
                return ERROR;
        STM32Delay_ms(50);
        GT_IRQEnable(ENABLE);
        return SUCCESS;
    }
    else if(TempVersion[0] == '9' && TempVersion[1] == '1' && TempVersion[2] == '5' && TempVersion[3] == '7')
    {
        if(!GT_IIC_WriteData(GT9xxSlaveAddress, GT9xxConfigAddress, CTP_CFG_GT9157, sizeof(CTP_CFG_GT9157)/sizeof(CTP_CFG_GT9157[0])))
            if(!GT_IIC_WriteData(GT9xxSlaveAddress, GT9xxConfigAddress, CTP_CFG_GT9157, sizeof(CTP_CFG_GT9157)/sizeof(CTP_CFG_GT9157[0])))
                return ERROR;
        STM32Delay_ms(50);
        GT_IRQEnable(ENABLE);
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
void  GT_IIC_ReadCoord(void)
{
//    uint8_t i = 0;
//    uint8_t Temp[8] = {0};
//    uint8_t TempWrite = 0;
//    int CoodTemp[2] = {0};
//    int CoodData[2][GT_COUNT] = {{0},{0}};
//    
//    GT_IIC_ReadData(GT9xxSlaveAddress, GT9xxCoordAddress, Temp, 8);//读取坐标信息
//    GT_IIC_WriteData(GT9xxSlaveAddress, GT9xxCoordAddress, &TempWrite, 1);
//    while((Temp[0]&0x81) == 0x81 && i < GT_COUNT)
//    {
//        CoodTemp[0] = (uint16_t)Temp[2] | (uint16_t)Temp[3]<<8;
//        CoodTemp[1] = (uint16_t)Temp[4] | (uint16_t)Temp[5]<<8;
//        if(CoodTemp[0] > 0 && CoodTemp[1] >0)
//        {
//            CoodData[0][i] = CoodTemp[0];
//            CoodData[1][i] = CoodTemp[1];
//            i++;
//        }
//        STM32Delay_us(10);
//        GT_IIC_ReadData(GT9xxSlaveAddress, GT9xxCoordAddress, Temp, 8);//读取坐标信息
//        GT_IIC_WriteData(GT9xxSlaveAddress, GT9xxCoordAddress, &TempWrite, 1);
//    }
//    
//    if(i == GT_COUNT)
//    {
//        for(i=0; i<GT_COUNT-1; i++)
//        {
//            for(uint8_t j=i+1; j<GT_COUNT; j++)
//            {
//                if(CoodData[0][i] > CoodData[0][j])
//                {
//                    CoodTemp[0] = CoodData[0][i];
//                    CoodData[0][i] = CoodData[0][j];
//                    CoodData[0][j] = CoodTemp[0];
//                }
//                if(CoodData[1][i] > CoodData[1][j])
//                {
//                    CoodTemp[1] = CoodData[1][i];
//                    CoodData[1][i] = CoodData[1][j];
//                    CoodData[1][j] = CoodTemp[1];
//                }
//            }
//        }
//        memset(CoodTemp, 0, sizeof(CoodTemp));
//        for(i=1; i<GT_COUNT-1; i++)
//        {
//            CoodTemp[0] += CoodData[0][i];
//            CoodTemp[1] += CoodData[1][i];
//        }
//        CoodTemp[0] = CoodTemp[0]/(GT_COUNT-2);
//        CoodTemp[1] = CoodTemp[1]/(GT_COUNT-2);
//        GT_CoodData[0] = 1;
//        GT_CoodData[1] = (uint16_t)CoodTemp[0];
//        GT_CoodData[2] = (uint16_t)CoodTemp[1];
//    }
//    else
//    {
//        memset(GT_CoodData, 0, sizeof(GT_CoodData));
//        CoodTemp[0] = -1;
//        CoodTemp[1] = -1;
//    }
//    GUI_TOUCH_StoreState(CoodTemp[0], CoodTemp[1]);//使用GUI_TOUCH_StoreStateEx函数，ICONVIEW小工具不能接收释放消息
    
    uint8_t Temp[8] = {0};
    uint8_t TempWrite = 0;
    int CoodTemp[2] = {0};
    
    GT_IIC_ReadData(GT9xxSlaveAddress, GT9xxCoordAddress, Temp, 8);//读取坐标信息
    if((Temp[0]&0x80) == 0x00)
        return;
    else
        GT_IIC_WriteData(GT9xxSlaveAddress, GT9xxCoordAddress, &TempWrite, 1);
    if((Temp[0]&0x81) == 0x81)
    {
        GT_CoodData[0] = 1;
        GT_CoodData[1] = (uint16_t)Temp[2] | (uint16_t)Temp[3]<<8;
        GT_CoodData[2] = (uint16_t)Temp[4] | (uint16_t)Temp[5]<<8;
        GT_CoodData[3] = (uint16_t)Temp[6] | (uint16_t)Temp[7]<<8;
        CoodTemp[0] = GT_CoodData[1];
        CoodTemp[1] = GT_CoodData[2];
    }
    else
    {
        memset(GT_CoodData, 0, sizeof(GT_CoodData));
        CoodTemp[0] = -1;
        CoodTemp[1] = -1;
    }
    GUI_TOUCH_StoreState(CoodTemp[0], CoodTemp[1]);//使用GUI_TOUCH_StoreStateEx函数，ICONVIEW小工具不能接收释放消息
}