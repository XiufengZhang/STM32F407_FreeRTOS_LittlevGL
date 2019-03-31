#include "stm32f4xx.h"
#include "stm32f4xx_spi.h"
#include "spi.h"

//spi1,spi4,spi5,spi6在APB2总线上(84MHz),SPI最高频率是42MHz;
//spi2,spi3在APB1总线上(48MHz),SPI最高频率是24MHz;
//多芯片共用一个spi总线要考虑spi频率和SPI_CPOL、SPI_CPHA、SPI_FirstBit
/**
  * @brief  SPI1 Flash初始化
  * @param  None
  * @retval None
  */
void SPI1_Init(void)
{
    SPI_InitTypeDef SPI_InitStructure;

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                      //设置SPI工作模式:设置为主SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                  //设置SPI的数据大小:SPI发送接收8位帧结构
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                         //选择了串行时钟的稳态:时钟悬空低 SPI_CPOL_Low,SPI_CPOL_High 0和3两种状态都支持
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                       //数据捕获于第1个时钟沿 SPI_CPHA_1Edge,SPI_CPHA_2Edge
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                          //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:硬件控制SS输出 SPI_NSS_Hard
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; //定义波特率预分频的值:波特率预分频值为2
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                 //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI_InitStructure.SPI_CRCPolynomial = 7;                           //CRC值计算的多项式
    SPI_Init(SPI1, &SPI_InitStructure);                                //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

    SPI_Cmd(SPI1, ENABLE); //使能SPI外设
    //SPI_SSOutputCmd(SPI1, ENABLE);//使能硬件NSS脚输出低电平
}

/**
  * @brief  SPI1 Flash发送一个字节
  * @param  None
  * @retval None
  */
ErrorStatus SPI1_SendData(uint8_t SendData)
{
    uint16_t ErrorCount = 0;

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
    {
        ErrorCount++;
        if (ErrorCount > 200)
            return ERROR;
    }
    SPI_I2S_SendData(SPI1, SendData); //通过外设SPIx发送一个数据
    return SUCCESS;
}

/**
  * @brief  SPI1 Flash读取一个字节
  * @param  None
  * @retval None
  */
ErrorStatus SPI1_ReadData(uint8_t *ReadData)
{
    uint16_t ErrorCount = 0;

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
    {
        ErrorCount++;
        if (ErrorCount > 200)
            return ERROR;
    }
    *ReadData = SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据
    return SUCCESS;
}

/**
  * @brief  SPI2 TOUCH初始化
  * @param  None
  * @retval None
  */
void SPI2_Init(void)
{
    SPI_InitTypeDef SPI_InitStructure;

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                       //设置SPI工作模式:设置为主SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                   //设置SPI的数据大小:SPI发送接收8位帧结构
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                          //选择了串行时钟的稳态:时钟悬空低
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                        //数据捕获于第1个时钟沿
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                           //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:硬件控制SS输出SPI_NSS_Hard
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; //定义波特率预分频的值:波特率预分频值为16
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                  //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI_InitStructure.SPI_CRCPolynomial = 7;                            //CRC值计算的多项式
    SPI_Init(SPI2, &SPI_InitStructure);                                 //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

    SPI_Cmd(SPI2, ENABLE); //使能SPI外设
    //SPI_SSOutputCmd(SPI2, ENABLE);//使能硬件NSS脚输出低电平
}

/**
  * @brief  SPI2 TOUCH发送一个字节
  * @param  None
  * @retval None
  */
ErrorStatus SPI2_SendData(uint8_t SendData)
{
    uint16_t ErrorCount = 0;

    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
    {
        ErrorCount++;
        if (ErrorCount > 200)
            return ERROR;
    }
    SPI_I2S_SendData(SPI2, SendData); //通过外设SPIx发送一个数据
    return SUCCESS;
}

/**
  * @brief  SPI2 TOUCH读取一个字节
  * @param  None
  * @retval None
  */
ErrorStatus SPI2_ReadData(uint8_t *ReadData)
{
    uint16_t ErrorCount = 0;

    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
    {
        ErrorCount++;
        if (ErrorCount > 200)
            return ERROR;
    }
    *ReadData = SPI_I2S_ReceiveData(SPI2); //返回通过SPIx最近接收的数据
    return SUCCESS;
}