#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "gpio.h"
#include "i2c.h"


//I2C输出使能
void IIC_SDA_OUT(FunctionalState Enable)
{
    if(Enable)
    {
        if((SDA_GPIO->DDR & SDA_Pin) == 0x00)
        {
            if(GPIO_ReadInputPin(SDA_GPIO, SDA_Pin))
                GPIO_Init(SDA_GPIO, SDA_Pin, GPIO_MODE_OUT_OD_HIZ_FAST);
            else
                GPIO_Init(SDA_GPIO, SDA_Pin, GPIO_MODE_OUT_OD_LOW_FAST);
        }
    }
    else
    {
        if(SDA_GPIO->DDR & SDA_Pin)
        {
            GPIO_Init(SDA_GPIO, SDA_Pin, GPIO_MODE_IN_FL_NO_IT);
        }
    }
}

//I2C产生起始信号
void IIC_Start(void)
{
    IIC_SDA_OUT(ENABLE);
    GPIO_WriteHigh(SCL_GPIO, SCL_Pin);
    GPIO_WriteHigh(SDA_GPIO, SDA_Pin);
    //STM8S_Delay_us(2);
    GPIO_WriteLow(SDA_GPIO, SDA_Pin);
    //STM8S_Delay_us(1);
    GPIO_WriteLow(SCL_GPIO, SCL_Pin);
}

//I2C产生停止信号
void IIC_Stop(void)
{
    IIC_SDA_OUT(ENABLE);
    GPIO_WriteHigh(SCL_GPIO, SCL_Pin);
    GPIO_WriteLow(SDA_GPIO, SDA_Pin);
    //STM8S_Delay_us(1);
    GPIO_WriteHigh(SDA_GPIO, SDA_Pin);
    //STM8S_Delay_us(2);				   	
}

//I2C等待应答信号
ErrorStatus IIC_Wait_Ack(void)
{
    uint8_t ucErrTime=0;
    
    GPIO_WriteLow(SCL_GPIO, SCL_Pin);
    IIC_SDA_OUT(DISABLE);
    //STM8S_Delay_us(1);
    GPIO_WriteHigh(SCL_GPIO, SCL_Pin);
    while(GPIO_ReadInputPin(SDA_GPIO, SDA_Pin))
    {
        ucErrTime++;
        if(ucErrTime>30)
        {
            IIC_Stop();
            return ERROR;
        }
    }
    //STM8S_Delay_us(1);
    GPIO_WriteLow(SCL_GPIO, SCL_Pin);
    //IIC_SDA_OUT(DISABLE);
    //STM8S_Delay_us(1);
    return SUCCESS;
}

//I2C产生应答信号
void IIC_Ack(void)
{
    GPIO_WriteLow(SCL_GPIO, SCL_Pin);
    IIC_SDA_OUT(ENABLE);
    GPIO_WriteLow(SDA_GPIO, SDA_Pin);
    //STM8S_Delay_us(1);
    GPIO_WriteHigh(SCL_GPIO, SCL_Pin);
    //STM8S_Delay_us(2);
    GPIO_WriteLow(SCL_GPIO, SCL_Pin);
    //STM8S_Delay_us(1);
}

//I2C不产生应答信号		    
void IIC_NAck(void)
{
    GPIO_WriteLow(SCL_GPIO, SCL_Pin);
    IIC_SDA_OUT(ENABLE);
    GPIO_WriteHigh(SDA_GPIO, SDA_Pin);
    //STM8S_Delay_us(1);
    GPIO_WriteHigh(SCL_GPIO, SCL_Pin);
    //STM8S_Delay_us(2);
    GPIO_WriteLow(SCL_GPIO, SCL_Pin);
    GPIO_WriteLow(SDA_GPIO, SDA_Pin);
    //STM8S_Delay_us(1);
}

//I2C发送一个字节
void IIC_Send_Byte(uint8_t data)
{
    uint8_t i;
    
    GPIO_WriteLow(SCL_GPIO, SCL_Pin);
    IIC_SDA_OUT(ENABLE);
    //STM8S_Delay_us(1);
    for(i=0;i<8;i++)
    {
        if((data&0x80) == 0x80)
        {
            GPIO_WriteHigh(SDA_GPIO, SDA_Pin);
        }
        else
        {
            GPIO_WriteLow(SDA_GPIO, SDA_Pin);
        }
        data<<=1;
        //STM8S_Delay_us(1);
        GPIO_WriteHigh(SCL_GPIO, SCL_Pin);
        //STM8S_Delay_us(1);
        GPIO_WriteLow(SCL_GPIO, SCL_Pin);
        //STM8S_Delay_us(1);
    }
}

//I2C接收一个字节
uint8_t IIC_Read_Byte(void)
{
    uint8_t i,j,receive=0;
    
    GPIO_WriteLow(SCL_GPIO, SCL_Pin);
    IIC_SDA_OUT(DISABLE);
    j=115;//95
        while(j--);//延时跳过IQS33延时
    //STM8S_Delay_us(1);
    for(i=0;i<8;i++)
    {
        //STM8S_Delay_us(1);
        GPIO_WriteHigh(SCL_GPIO, SCL_Pin);
        receive<<=1;
        if(GPIO_ReadInputPin(SDA_GPIO, SDA_Pin))
            receive|=0x01;
        //STM8S_Delay_us(2);
        GPIO_WriteLow(SCL_GPIO, SCL_Pin);
        //STM8S_Delay_us(1);
        
    }
    return receive;
}

/**
  * @brief 向I2C从设备写入一个字节.
  * @param  SlaveAddress I2C从设备7位地址
  * @param  RegisterAddress I2C从设备寄存器地址
  * @retval ErrorStatus 是否写入成功状态
  */
ErrorStatus I2CWriteByte(uint8_t SlaveAddress, uint8_t RegisterAddress, uint8_t *pWriteData)
{
    uint16_t Temp;
    
    Temp = SlaveAddress<<1;
    IIC_Start();
    IIC_Send_Byte(Temp);
    if(!IIC_Wait_Ack())
    {
        return ERROR;
    }
    IIC_Send_Byte(RegisterAddress);
    if(!IIC_Wait_Ack())
    {
        return ERROR;
    }
    IIC_Send_Byte(*pWriteData);
    if(!IIC_Wait_Ack())
    {
        return ERROR;
    }
    IIC_Stop();
    return SUCCESS;
}

/**
  * @brief 向I2C从设备写入一个字节
  * @param  SlaveAddress I2C从设备7位地址
  * @param  RegisterAddress I2C从设备寄存器地址
  * @retval ErrorStatus 是否写入成功状态
  */
ErrorStatus I2CWriteData(uint8_t SlaveAddress, uint8_t RegisterAddress, uint8_t *pWriteDataBuffer, uint8_t WriteDataCount)
{
    uint16_t Temp;
    uint16_t i;
    
    Temp = SlaveAddress<<1;
    IIC_Start();
    IIC_Send_Byte(Temp);
    if(!IIC_Wait_Ack())
    {
        return ERROR;
    }
    IIC_Send_Byte(RegisterAddress);
    for(i=0; i<WriteDataCount; i++)
    {        
        if(!IIC_Wait_Ack())
        {
            return ERROR;
        }
        IIC_Send_Byte(*pWriteDataBuffer);
        pWriteDataBuffer++;
    }
    if(!IIC_Wait_Ack())
    {
        return ERROR;
    }
    IIC_Stop();
    return SUCCESS;
}

/**
  * @brief 从I2C从设备读入一个字节
  * @param  SlaveAddress I2C从设备7位地址
  * @param  RegisterAddress I2C从设备寄存器地址
  * @retval ErrorStatus 是否写入成功状态
  */
ErrorStatus I2CReadByte(uint8_t SlaveAddress, uint8_t RegisterAddress, uint8_t *pReadData)
{
    uint16_t Temp;
    
    //disableInterrupts();
    Temp = SlaveAddress<<1;
    IIC_Start();
    IIC_Send_Byte(Temp);
    if(!IIC_Wait_Ack())
    {
        return ERROR;
    }
    IIC_Send_Byte(RegisterAddress);
    if(!IIC_Wait_Ack())
    {
        return ERROR;
    }
    IIC_Start();
    Temp |= 0x01;
    IIC_Send_Byte(Temp);
    if(!IIC_Wait_Ack())
    {
        return ERROR;
    }
    *pReadData = IIC_Read_Byte();
    IIC_NAck();
    IIC_Stop();
    return SUCCESS;
}

/**
  * @brief 从I2C从设备连续读入字节
  * @param  SlaveAddress I2C从设备7位地址
  * @param  RegisterAddress I2C从设备寄存器地址
  * @retval ErrorStatus 是否写入成功状态
  */
ErrorStatus I2CReadData(uint8_t SlaveAddress, uint8_t RegisterAddress, uint8_t *pReadDataBuffer, uint8_t ReadDataCount)
{
    uint16_t Temp;
    uint16_t i;
    
    Temp = SlaveAddress<<1;
    IIC_Start();
    IIC_Send_Byte(Temp);
    if(!IIC_Wait_Ack())
    {
        return ERROR;
    }
    IIC_Send_Byte(RegisterAddress);
    if(!IIC_Wait_Ack())
    {
        return ERROR;
    }
    IIC_Start();
    Temp |= 0x01;
    IIC_Send_Byte(Temp);
    if(!IIC_Wait_Ack())
    {
        return ERROR;
    }
    for(i=0; i<ReadDataCount; i++)
    {
        *pReadDataBuffer = IIC_Read_Byte();
        pReadDataBuffer++;
        if(i<(ReadDataCount-1))
        {
            IIC_Ack();
        }
        else
        {
            IIC_NAck();
        }
    }
    IIC_Stop();
    return SUCCESS;
}