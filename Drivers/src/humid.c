#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "crc.h"
#include "gpio.h"
#include "timer.h"
#include "humid.h"


ErrorStatus HUMIDStatus = ERROR;// 标记温湿度IC是否通讯正常 0通讯失败 1正常
uint16_t HUMID_Temp = 0;
uint16_t HUMID_Humi = 0;
float HUMID_TempConvert = 0;
float HUMID_HumiConvert = 0;



/**
  * @brief I2C输出使能.
  * @param  Enable IIC_SDA是否输出使能
  * @retval 无
  */
void IIC_SDA_OUT(FunctionalState Enable)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    if(Enable)
    {
        if((I2CH_SDA_GPIO_PORT->MODER & I2CH_SDA_MODER) == 0x0000)
        {
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
            GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
            GPIO_InitStructure.GPIO_Pin = I2CH_SDA_GPIO_PIN;
            if(GPIO_ReadInputDataBit(I2CH_SDA_GPIO_PORT, I2CH_SDA_GPIO_PIN))
            {
                GPIO_Init(I2CH_SDA_GPIO_PORT, &GPIO_InitStructure);
                GPIO_SetBits(I2CH_SDA_GPIO_PORT, I2CH_SDA_GPIO_PIN);
            }
            else
            {
                GPIO_Init(I2CH_SDA_GPIO_PORT, &GPIO_InitStructure);
                GPIO_ResetBits(I2CH_SDA_GPIO_PORT, I2CH_SDA_GPIO_PIN);
            }
        }
    }
    else
    {
        if((I2CH_SDA_GPIO_PORT->MODER & I2CH_SDA_MODER) != 0x0000)
        {
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
            GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
            GPIO_InitStructure.GPIO_Pin = I2CH_SDA_GPIO_PIN;
            GPIO_Init(I2CH_SDA_GPIO_PORT, &GPIO_InitStructure);
        }
    }
}

/**
  * @brief  2C模拟通讯延时.
  * @param  None.
  * @retval None.
  */
static void IIC_Delay(void)
{
    uint8_t i;
    
    for (i = 0; i < 35; i++);//STM32F103 5~12之间，12是200K频率,5是400K频率;STM32F429 30~50之间，30是400K频率
}


/**
  * @brief  //I2C产生起始信号
  * @param  None.
  * @retval None.
  */
void IIC_Start(void)
{
    IIC_SDA_OUT(ENABLE);
    GPIO_SetBits(I2CH_SCL_GPIO_PORT, I2CH_SCL_GPIO_PIN);
    GPIO_SetBits(I2CH_SDA_GPIO_PORT, I2CH_SDA_GPIO_PIN);
    IIC_Delay();
    GPIO_ResetBits(I2CH_SDA_GPIO_PORT, I2CH_SDA_GPIO_PIN);
    IIC_Delay();
    GPIO_ResetBits(I2CH_SCL_GPIO_PORT, I2CH_SCL_GPIO_PIN);
    IIC_Delay();
}

/**
  * @brief  //I2C产生停止信号
  * @param  None.
  * @retval None.
  */
void IIC_Stop(void)
{
    IIC_SDA_OUT(ENABLE);
    GPIO_SetBits(I2CH_SCL_GPIO_PORT, I2CH_SCL_GPIO_PIN);
    GPIO_ResetBits(I2CH_SDA_GPIO_PORT, I2CH_SDA_GPIO_PIN);
    IIC_Delay();
    GPIO_SetBits(I2CH_SDA_GPIO_PORT, I2CH_SDA_GPIO_PIN);
    IIC_Delay();				   	
}

/**
  * @brief  //I2C等待应答信号
  * @param  None.
  * @retval None.
  */
ErrorStatus IIC_Wait_Ack(void)
{
    uint8_t ucErrTime=0;
    
    GPIO_ResetBits(I2CH_SCL_GPIO_PORT, I2CH_SCL_GPIO_PIN);
    IIC_SDA_OUT(DISABLE);
    IIC_Delay();
    GPIO_SetBits(I2CH_SCL_GPIO_PORT, I2CH_SCL_GPIO_PIN);
    IIC_Delay();
    while(GPIO_ReadInputDataBit(I2CH_SDA_GPIO_PORT, I2CH_SDA_GPIO_PIN))
    {
        ucErrTime++;
        if(ucErrTime>30)
        {
            IIC_Stop();
            return ERROR;
        }
    }
    GPIO_ResetBits(I2CH_SCL_GPIO_PORT, I2CH_SCL_GPIO_PIN);
    //IIC_SDA_OUT(DISABLE);
    IIC_Delay();
    return SUCCESS;
}

/**
  * @brief  //I2C产生应答信号
  * @param  None.
  * @retval None.
  */
void IIC_Ack(void)
{
    GPIO_ResetBits(I2CH_SCL_GPIO_PORT, I2CH_SCL_GPIO_PIN);
    IIC_SDA_OUT(ENABLE);
    GPIO_ResetBits(I2CH_SDA_GPIO_PORT, I2CH_SDA_GPIO_PIN);
    IIC_Delay();
    GPIO_SetBits(I2CH_SCL_GPIO_PORT, I2CH_SCL_GPIO_PIN);
    IIC_Delay();
    GPIO_ResetBits(I2CH_SCL_GPIO_PORT, I2CH_SCL_GPIO_PIN);
    IIC_Delay();
}

/**
  * @brief  //I2C不产生应答信号
  * @param  None.
  * @retval None.
  */
void IIC_NAck(void)
{
    GPIO_ResetBits(I2CH_SCL_GPIO_PORT, I2CH_SCL_GPIO_PIN);
    IIC_SDA_OUT(ENABLE);
    GPIO_SetBits(I2CH_SDA_GPIO_PORT, I2CH_SDA_GPIO_PIN);
    IIC_Delay();
    GPIO_SetBits(I2CH_SCL_GPIO_PORT, I2CH_SCL_GPIO_PIN);
    IIC_Delay();
    GPIO_ResetBits(I2CH_SCL_GPIO_PORT, I2CH_SCL_GPIO_PIN);
    GPIO_ResetBits(I2CH_SDA_GPIO_PORT, I2CH_SDA_GPIO_PIN);
    IIC_Delay();
}

/**
  * @brief  //I2C发送一个字节
  * @param  None.
  * @retval None.
  */
void IIC_Send_Byte(uint8_t data)
{
    uint8_t i;
    
    GPIO_ResetBits(I2CH_SCL_GPIO_PORT, I2CH_SCL_GPIO_PIN);
    IIC_SDA_OUT(ENABLE);
    //IIC_Delay();
    for(i=0;i<8;i++)
    {
        if((data&0x80) == 0x80)
        {
            GPIO_SetBits(I2CH_SDA_GPIO_PORT, I2CH_SDA_GPIO_PIN);
        }
        else
        {
            GPIO_ResetBits(I2CH_SDA_GPIO_PORT, I2CH_SDA_GPIO_PIN);
        }
        data<<=1;
        IIC_Delay();
        GPIO_SetBits(I2CH_SCL_GPIO_PORT, I2CH_SCL_GPIO_PIN);
        IIC_Delay();
        GPIO_ResetBits(I2CH_SCL_GPIO_PORT, I2CH_SCL_GPIO_PIN);
        IIC_Delay();
    }
}

/**
  * @brief  //I2C读取一个字节
  * @param  None.
  * @retval None.
  */
uint8_t IIC_Read_Byte(void)
{
    uint8_t i,receive=0;
    
    GPIO_ResetBits(I2CH_SCL_GPIO_PORT, I2CH_SCL_GPIO_PIN);
    IIC_SDA_OUT(DISABLE);
    //IIC_Delay();
    for(i=0;i<8;i++)
    {
        IIC_Delay();
        GPIO_SetBits(I2CH_SCL_GPIO_PORT, I2CH_SCL_GPIO_PIN);
        receive<<=1;
        if(GPIO_ReadInputDataBit(I2CH_SDA_GPIO_PORT, I2CH_SDA_GPIO_PIN))
            receive|=0x01;
        IIC_Delay();
        GPIO_ResetBits(I2CH_SCL_GPIO_PORT, I2CH_SCL_GPIO_PIN);
        IIC_Delay();
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

/**
  * @brief  温湿度IC初始化
  * @param  None.
  * @retval Resolution.0x00 12 bits 14 bits,0x01 8 bits 12 bits,0x80 10 bits 13 bits, 0x81温湿度转换都是11位
  */
ErrorStatus HUMID_Init(uint8_t Resolution)
{
    uint8_t Temp = 0;
    uint8_t TempData = 0;
    
    Temp = HUMID_Address<<1;
    IIC_Start();
    IIC_Send_Byte(Temp);
    if(!IIC_Wait_Ack())
        return ERROR;
    IIC_Send_Byte(HUMIDUserRReg_Address);
    if(!IIC_Wait_Ack())
        return ERROR;
    
    Temp |= 0x01;//转为读状态
    IIC_Start();
    IIC_Send_Byte(Temp);
    if(!IIC_Wait_Ack())
        return ERROR;
    TempData = IIC_Read_Byte();
    IIC_NAck();
    
    Temp &= 0xFE;//转为写状态
    IIC_Start();
    IIC_Send_Byte(Temp);
    if(!IIC_Wait_Ack())
        return ERROR;
    IIC_Send_Byte(HUMIDUserWReg_Address);
    if(!IIC_Wait_Ack())
        return ERROR;
    IIC_Send_Byte((TempData & 0x7E) | (Resolution & 0x81));
    if(!IIC_Wait_Ack())
        return ERROR;
    IIC_Stop();
    return SUCCESS;
}

/**
  * @brief  读取温度数据
  * @param  None.
  * @retval None.
  */
ErrorStatus HUMID_ReadTemp(void)
{
    uint8_t i = 0;
    uint8_t Temp = 0;
    uint8_t TempData[3] = {0};
    
    Temp = HUMID_Address<<1;
    IIC_Start();
    IIC_Send_Byte(Temp);
    if(!IIC_Wait_Ack())
        return ERROR;
    IIC_Send_Byte(HUMIDTempReg_Address);
    if(!IIC_Wait_Ack())
        return ERROR;
    
    Temp |= 0x01;//转为读状态
    i = 0;//初始化计数
    do{
        STM32Delay_ms(1);
        i++;
        if(i<80)//正常转换过程小于50ms
        {
            IIC_Start();
            IIC_Send_Byte(Temp);
        }
        else
            return ERROR;
    }while(!IIC_Wait_Ack());
    for(i=0; i<3; i++)
    {
        TempData[i] = IIC_Read_Byte();
        if(i<2)
            IIC_Ack();
        else
            IIC_NAck();
    }
    IIC_Stop();
    //CRC 转换
    if((TempData[1]&0x02) == 0)
    {
        if(GetCRC8(TempData, 2) == TempData[2])
        {
            HUMID_Temp = (uint16_t)TempData[0]<<8 | (TempData[1]&0xFC);
            HUMID_TempConvert = 175.72*HUMID_Temp/0xFFFF-46.85;
        }
    }
    return SUCCESS;
}

/**
  * @brief  读取湿度数据
  * @param  None.
  * @retval None.
  */
ErrorStatus HUMID_ReadHumi(void)
{
    uint8_t i = 0;
    uint8_t Temp = 0;
    uint8_t TempData[3] = {0};
    
    Temp = HUMID_Address<<1;
    IIC_Start();
    IIC_Send_Byte(Temp);
    if(!IIC_Wait_Ack())
        return ERROR;
    IIC_Send_Byte(HUMIDHumiReg_Address);
    if(!IIC_Wait_Ack())
        return ERROR;
    
    Temp |= 0x01;//转为读状态
    i = 0;//初始化计数
    do{
        STM32Delay_ms(1);
        i++;
        if(i<80)//正常转换过程小于50ms
        {
            IIC_Start();
            IIC_Send_Byte(Temp);
        }
        else
            return ERROR;
    }while(!IIC_Wait_Ack());
    for(i=0; i<3; i++)
    {
        TempData[i] = IIC_Read_Byte();
        if(i<2)
            IIC_Ack();
        else
            IIC_NAck();
    }
    IIC_Stop();
    //CRC 转换
    if(TempData[1]&0x02)//1位是湿度
    {
        if(GetCRC8(TempData, 2) == TempData[2])
        {
            HUMID_Humi = (uint16_t)TempData[0]<<8 | (TempData[1]&0xFC);
            HUMID_HumiConvert = 125.0*HUMID_Humi/0xFFFF-6;
        }
    }
    return SUCCESS;
}