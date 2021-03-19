#include <string.h>
#include "stm32f4xx.h"
#include "gpio.h"
#include "spi.h"
#include "timer.h"
#include "iwdg.h"
#include "spiflash.h"

ErrorStatus FlashStatus = ERROR;                          //标记串行Flash是否正常 ERROR失败 SUCCESS正常
static uint8_t FlashDataTemp[SPI_FLASH_SectorSize] = {0}; //4KB字节存储空间

uint8_t Flash_DataRefresh = 0;                           //Flash数据需要写入标识 0不需要写入 1要从新写入
static uint8_t Flash_DataArray[Flash_DATA_NUMBER] = {0}; //数据存储数组
char Flash_DeviceSN[Flash_SN_NUMBER + 1] = {"Ligh1901"}; //设备序列号 SN 4字节字符串 32位整数例如180102999
uint8_t Flash_DeviceType = 0;                            //定义设备类型 0设备是303 1设备是305
uint8_t Flash_Language = 0;                              //定义语言 0英文 1中文

/**
  * @brief  向FLASH发送 进入掉电模式 命令
  * @param  none
  * @retval none
  */
void SPI_Flash_PowerDown(void)
{
    uint8_t Temp = 0;

    GpioSPIFlashCSControl(ENABLE);
    SPI1_SendData(W25X_PowerDown);
    SPI1_ReadData(&Temp);
    GpioSPIFlashCSControl(DISABLE);
}

/**
  * @brief  向FLASH发送 唤醒 命令
  * @param  none
  * @retval none
  */
void SPI_Flash_WAKEUP(void)
{
    uint8_t Temp = 0;

    GpioSPIFlashCSControl(ENABLE);
    SPI1_SendData(W25X_ReleasePowerDown);
    SPI1_ReadData(&Temp);
    GpioSPIFlashCSControl(DISABLE);
}

/**
  * @brief  读取FLASH Device ID
  * @param  none
  * @retval FLASH Device ID
  */
// static uint8_t SPI_FLASH_ReadDeviceID(void)//返回0X17
// {
//     uint8_t Temp = 0;
//     uint8_t DeviceID = 0;

//     GpioSPIFlashCSControl(ENABLE);
//     SPI1_SendData(W25X_ReleasePowerDown);
//     SPI1_ReadData(&Temp);
//     SPI1_SendData(Dummy_Byte);
//     SPI1_ReadData(&Temp);
//     SPI1_SendData(Dummy_Byte);
//     SPI1_ReadData(&Temp);
//     SPI1_SendData(Dummy_Byte);
//     SPI1_ReadData(&Temp);
//     SPI1_SendData(Dummy_Byte);
//     SPI1_ReadData(&DeviceID);
//     GpioSPIFlashCSControl(DISABLE);
//     return DeviceID;
// }

/**
  * @brief  读取FLASH Manufacturer/Device ID
  * @param  none
  * @retval FLASH Manufacturer/Device ID
  */
// static uint16_t SPI_FLASH_ReadManufacturerID(void)//返回0XEF17
// {
//     uint8_t Temp = 0;
//     uint16_t ManufacturerDeviceID = 0;

//     GpioSPIFlashCSControl(ENABLE);
//     SPI1_SendData(W25X_ManufactDeviceID);
//     SPI1_ReadData(&Temp);
//     SPI1_SendData(Dummy_Byte);
//     SPI1_ReadData(&Temp);
//     SPI1_SendData(Dummy_Byte);
//     SPI1_ReadData(&Temp);
//     SPI1_SendData(0x00);
//     SPI1_ReadData(&Temp);
//     SPI1_SendData(Dummy_Byte);
//     SPI1_ReadData(&Temp);
//     ManufacturerDeviceID |= (uint16_t)Temp << 8;
//     SPI1_SendData(Dummy_Byte);
//     SPI1_ReadData(&Temp);
//     ManufacturerDeviceID |= (uint16_t)Temp;
//     GpioSPIFlashCSControl(DISABLE);
//     return ManufacturerDeviceID;
// }

/**
  * @brief  读取FLASH Manufacturer/Device ID
  * @param  none
  * @retval FLASH ManufacturerDevice ID
  */
static uint32_t SPI_FLASH_ReadManufacturerDeviceID(void) //返回0XEF4018
{
    uint8_t Temp = 0;
    uint32_t ManufacturerDeviceID = 0;

    GpioSPIFlashCSControl(ENABLE);
    SPI1_SendData(W25X_JedecDeviceID);
    SPI1_ReadData(&Temp);
    SPI1_SendData(Dummy_Byte);
    SPI1_ReadData(&Temp);
    ManufacturerDeviceID |= (uint32_t)Temp << 16;
    SPI1_SendData(Dummy_Byte);
    SPI1_ReadData(&Temp);
    ManufacturerDeviceID |= (uint32_t)Temp << 8;
    SPI1_SendData(Dummy_Byte);
    SPI1_ReadData(&Temp);
    ManufacturerDeviceID |= (uint32_t)Temp;
    GpioSPIFlashCSControl(DISABLE);
    return ManufacturerDeviceID;
}

/**
  * @brief  向FLASH发送 写使能 命令
  * @param  none
  * @retval none
  */
static void SPI_FLASH_WriteEnable(void)
{
    uint8_t Temp = 0;

    GpioSPIFlashCSControl(ENABLE);
    SPI1_SendData(W25X_WriteEnable); //Status Registers1的S1 WEL位
    SPI1_ReadData(&Temp);
    GpioSPIFlashCSControl(DISABLE);
}

/**
  * @brief  向FLASH发送 写失能 命令
  * @param  none
  * @retval none
  */
static void SPI_FLASH_WriteDisable(void)
{
    uint8_t Temp = 0;

    GpioSPIFlashCSControl(ENABLE);
    SPI1_SendData(W25X_WriteDisable); //Status Registers1的S1 WEL位
    SPI1_ReadData(&Temp);
    GpioSPIFlashCSControl(DISABLE);
}

/**
  * @brief  向FLASH发送 ReadStatusReg1 命令 查询写状态是否使能
  * @param  none
  * @retval SET 1是使能写模式
  */
static FlagStatus SPI_FLASH_WriteState(void)
{
    uint8_t Temp = 0;

    GpioSPIFlashCSControl(ENABLE);
    SPI1_SendData(W25X_ReadStatusReg1); //Status Registers1的S1 WEL位
    SPI1_ReadData(&Temp);
    GpioSPIFlashCSControl(DISABLE);
    if (Temp & WEL_Flag)
        return SET;
    else
        return RESET;
}

#if defined(W25Q256)
/**
  * @brief  向FLASH发送 4-Byte模式 命令
  * @param  none
  * @retval none
  */
static void SPI_FLASH_4ByteAddressEnter(void)
{
    uint8_t Temp = 0;

    GpioSPIFlashCSControl(ENABLE);
    SPI1_SendData(W25X_4ByteAddressEnter); //Status Registers3的S16 ADS位
    SPI1_ReadData(&Temp);
    GpioSPIFlashCSControl(DISABLE);
}

/**
  * @brief  向FLASH发送 取消4-Byte模式 命令
  * @param  none
  * @retval none
  */
// static void SPI_FLASH_4ByteAddressExit(void)
// {
//   uint8_t Temp = 0;

//   GpioSPIFlashCSControl(ENABLE);
//   SPI1_SendData(W25X_4ByteAddressExit); //Status Registers3的S16 ADS位
//   SPI1_ReadData(&Temp);
//   GpioSPIFlashCSControl(DISABLE);
// }

/**
  * @brief  向FLASH发送 ReadStatusReg3 命令 查询4byte address模式
  * @param  none
  * @retval SET 1是4Byte模式
  */
static FlagStatus SPI_FLASH_4ByteAddressMode(void)
{
    uint8_t Temp = 0;

    GpioSPIFlashCSControl(ENABLE);
    SPI1_SendData(W25X_ReadStatusReg3); //Status Registers3的S16 ADS位
    SPI1_ReadData(&Temp);
    GpioSPIFlashCSControl(DISABLE);
    if (Temp & ADS_Flag)
        return SET;
    else
        return RESET;
}
#endif

/**
  * @brief  等待WIP(BUSY)标志被置0，即等待到FLASH内部数据写入完毕
  * @param  none
  * @retval none
  */
static ErrorStatus SPI_FLASH_WaitForWriteEnd(void)
{
    uint8_t Temp = 0;
    uint8_t FLASH_Status = 0;
    uint16_t ErrorCount = 0;

    GpioSPIFlashCSControl(ENABLE);
    SPI1_SendData(W25X_ReadStatusReg1); //读取Status Registers1的S0 BUSY
    SPI1_ReadData(&Temp);
    do
    {
        SPI1_SendData(Dummy_Byte);
        SPI1_ReadData(&FLASH_Status);
        STM32Delay_ms(1); //延时1ms
        ErrorCount++;
        if (ErrorCount > 0x2000) //8.192s
        {
            GpioSPIFlashCSControl(DISABLE);
            return ERROR;
        }
        WatchDogFeed();
    } while (FLASH_Status & BUSY_Flag);
    GpioSPIFlashCSControl(DISABLE);
    return SUCCESS;
}

/**
  * @brief  擦除FLASH扇区
  * @param  SectorNum：要擦除的扇区0~4095
  * @retval none
  */
void SPI_FLASH_SectorErase(uint32_t SectorNum)
{
    uint8_t Temp = 0;
    uint32_t SectorAddr = 0;

    SPI_FLASH_WaitForWriteEnd(); //等待写完成
    SPI_FLASH_WriteEnable();     //发送FLASH写使能命令
    SPI_FLASH_WaitForWriteEnd(); //等待写完成
    if (SPI_FLASH_WriteState() != SET)
        SPI_FLASH_WriteEnable(); //发送FLASH写使能命令

    if (SectorNum < SPI_FLASH_SectorNumMax)
        SectorAddr = SectorNum * SPI_FLASH_SectorSize;
    else
        SectorAddr = (SPI_FLASH_SectorNumMax - 1) * SPI_FLASH_SectorSize;
    GpioSPIFlashCSControl(ENABLE);
    SPI1_SendData(W25X_SectorErase);
    SPI1_ReadData(&Temp);
#if defined(W25Q256)
    SPI1_SendData((SectorAddr & 0xFF000000) >> 24);
    SPI1_ReadData(&Temp);
#endif
    SPI1_SendData((SectorAddr & 0xFF0000) >> 16);
    SPI1_ReadData(&Temp);
    SPI1_SendData((SectorAddr & 0xFF00) >> 8);
    SPI1_ReadData(&Temp);
    SPI1_SendData(SectorAddr & 0xFF);
    SPI1_ReadData(&Temp);
    GpioSPIFlashCSControl(DISABLE);

    SPI_FLASH_WaitForWriteEnd(); //等待写完成
    SPI_FLASH_WriteDisable();    //发送FLASH写失能命令
}

/**
  * @brief  擦除FLASH块
  * @param  SectorAddr：要擦除的块0~511
  * @retval none
  */
void SPI_FLASH_BlockErase32(uint32_t Block32Num)
{
    uint8_t Temp = 0;
    uint32_t BlockAddr = 0;

    SPI_FLASH_WaitForWriteEnd(); //等待写完成
    SPI_FLASH_WriteEnable();     //发送FLASH写使能命令
    SPI_FLASH_WaitForWriteEnd(); //等待写完成
    if (SPI_FLASH_WriteState() != SET)
        SPI_FLASH_WriteEnable(); //发送FLASH写使能命令

    if (Block32Num < SPI_FLASH_Block32NumMax)
        BlockAddr = Block32Num * SPI_FLASH_BlockSize32;
    else
        BlockAddr = (SPI_FLASH_Block32NumMax - 1) * SPI_FLASH_BlockSize32;
    GpioSPIFlashCSControl(ENABLE);
    SPI1_SendData(W25X_BlockErase32);
    SPI1_ReadData(&Temp);
#if defined(W25Q256)
    SPI1_SendData((BlockAddr & 0xFF000000) >> 24);
    SPI1_ReadData(&Temp);
#endif
    SPI1_SendData((BlockAddr & 0xFF0000) >> 16);
    SPI1_ReadData(&Temp);
    SPI1_SendData((BlockAddr & 0xFF00) >> 8);
    SPI1_ReadData(&Temp);
    SPI1_SendData(BlockAddr & 0xFF);
    SPI1_ReadData(&Temp);
    GpioSPIFlashCSControl(DISABLE);

    SPI_FLASH_WaitForWriteEnd(); //等待写完成
    SPI_FLASH_WriteDisable();    //发送FLASH写失能命令
}

/**
  * @brief  擦除FLASH块
  * @param  SectorAddr：要擦除的块0~255
  * @retval none
  */
void SPI_FLASH_BlockErase64(uint32_t Block64Num)
{
    uint8_t Temp = 0;
    uint32_t BlockAddr = 0;

    SPI_FLASH_WaitForWriteEnd(); //等待写完成
    SPI_FLASH_WriteEnable();     //发送FLASH写使能命令
    SPI_FLASH_WaitForWriteEnd(); //等待写完成
    if (SPI_FLASH_WriteState() != SET)
        SPI_FLASH_WriteEnable(); //发送FLASH写使能命令

    if (Block64Num < SPI_FLASH_Block64NumMax)
        BlockAddr = Block64Num * SPI_FLASH_BlockSize64;
    else
        BlockAddr = (SPI_FLASH_Block64NumMax - 1) * SPI_FLASH_BlockSize64;
    GpioSPIFlashCSControl(ENABLE);
    SPI1_SendData(W25X_BlockErase64);
    SPI1_ReadData(&Temp);
#if defined(W25Q256)
    SPI1_SendData((BlockAddr & 0xFF000000) >> 24);
    SPI1_ReadData(&Temp);
#endif
    SPI1_SendData((BlockAddr & 0xFF0000) >> 16);
    SPI1_ReadData(&Temp);
    SPI1_SendData((BlockAddr & 0xFF00) >> 8);
    SPI1_ReadData(&Temp);
    SPI1_SendData(BlockAddr & 0xFF);
    SPI1_ReadData(&Temp);
    GpioSPIFlashCSControl(DISABLE);

    SPI_FLASH_WaitForWriteEnd(); //等待写完成
    SPI_FLASH_WriteDisable();    //发送FLASH写失能命令
}

/**
  * @brief  擦除FLASH扇区，整片擦除
  * @param  none
  * @retval none
  */
void SPI_FLASH_BulkErase(void)
{
    uint8_t Temp = 0;

    SPI_FLASH_WaitForWriteEnd(); //等待写完成
    SPI_FLASH_WriteEnable();     //发送FLASH写使能命令
    SPI_FLASH_WaitForWriteEnd(); //等待写完成
    if (SPI_FLASH_WriteState() != SET)
        SPI_FLASH_WriteEnable(); //发送FLASH写使能命令

    GpioSPIFlashCSControl(ENABLE);
    SPI1_SendData(W25X_ChipErase); //发送整块擦除指令
    SPI1_ReadData(&Temp);
    GpioSPIFlashCSControl(DISABLE);

    SPI_FLASH_WaitForWriteEnd(); //等待写完成
    SPI_FLASH_WriteDisable();    //发送FLASH写失能命令
}

/**
  * @brief 读取FLASH数据
  * @param 
    pBuffer: 存储读出数据的指针
    ReadAddr: 读取地址
    NumByteToRead: 读取数据长度
  * @retval none
  */
void SPI_FLASH_BufferRead(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{
    uint8_t Temp = 0;

    SPI_FLASH_WaitForWriteEnd(); //等待写完成

    GpioSPIFlashCSControl(ENABLE);
    SPI1_SendData(W25X_ReadData); //发送读指令
    SPI1_ReadData(&Temp);
#if defined(W25Q256)
    SPI1_SendData((ReadAddr & 0xFF000000) >> 24);
    SPI1_ReadData(&Temp);
#endif
    SPI1_SendData((ReadAddr & 0xFF0000) >> 16);
    SPI1_ReadData(&Temp);
    SPI1_SendData((ReadAddr & 0xFF00) >> 8);
    SPI1_ReadData(&Temp);
    SPI1_SendData(ReadAddr & 0xFF);
    SPI1_ReadData(&Temp);
    for (uint32_t i = 0; i < NumByteToRead; i++)
    {
        SPI1_SendData(Dummy_Byte);
        SPI1_ReadData(pBuffer); //读取一个字节
        pBuffer++;              //指向下一个字节缓冲区
    }
    GpioSPIFlashCSControl(DISABLE);
}

/**
  * @brief  对FLASH Page写入数据，调用本函数直接写入数据
  * @param 
    pBuffer: 要写入数据的指针
    WriteAddr: 写入地址
    NumByteToWrite: 写入数据长度
  * @retval none
  */
static void SPI_FLASH_PageWrite(const uint8_t *pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite)
{
    uint8_t Temp = 0;

    SPI_FLASH_WaitForWriteEnd(); //等待写完成
    SPI_FLASH_WriteEnable();     //发送FLASH写使能命令
    SPI_FLASH_WaitForWriteEnd(); //等待写完成
    if (SPI_FLASH_WriteState() != SET)
        SPI_FLASH_WriteEnable(); //发送FLASH写使能命令

    GpioSPIFlashCSControl(ENABLE);
    SPI1_SendData(W25X_PageProgram); //发送Page指令
    SPI1_ReadData(&Temp);
#if defined(W25Q256)
    SPI1_SendData((WriteAddr & 0xFF000000) >> 24);
    SPI1_ReadData(&Temp);
#endif
    SPI1_SendData((WriteAddr & 0xFF0000) >> 16);
    SPI1_ReadData(&Temp);
    SPI1_SendData((WriteAddr & 0xFF00) >> 8);
    SPI1_ReadData(&Temp);
    SPI1_SendData(WriteAddr & 0xFF);
    SPI1_ReadData(&Temp);
    for (uint32_t i = 0; i < NumByteToWrite; i++)
    {
        SPI1_SendData(*pBuffer); //发送数据
        SPI1_ReadData(&Temp);
        pBuffer++;
    }
    GpioSPIFlashCSControl(DISABLE);

    SPI_FLASH_WaitForWriteEnd(); //等待写完成
    SPI_FLASH_WriteDisable();    ////发送FLASH写失能命令
}

/**
  * @brief  对FLASH写入数据，调用本函数写入数据前需要先擦除扇区
  * @param 
    pBuffer: 要写入数据的指针
    WriteAddr: 写入地址
    NumByteToWrite: 写入数据长度
  * @retval none
  */
//写操作，靠Page Program这个指令完成，判断所写入的数据是否在一个扇区之内，判断扇区内是否有数据
void SPI_FLASH_BufferWrite(const uint8_t *pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite)
{
    uint32_t i = 0;
    uint16_t AddrNum = 0;       //本次写入地址所在Page的位置0~255
    uint16_t PageNum = 0;       //本次写入的地址所在的Page号，0~15
    uint16_t SectorNum = 0;     //本次写入的地址所在的扇区号，0~4095
    uint32_t WriteNumCapa = 0;  //本扇区可以容纳写入字节的数量
    uint32_t WriteNumThis = 0;  //本次写入的数量，按最多一个扇区16个page 4KB 4096个字节写入
    uint32_t WriteNumResi = 0;  //剩余要写入的数量
    uint32_t WriteAddrSect = 0; //本次要写入的首地址，扇区对齐首地址
    uint32_t WriteAddrThis = 0; //本次要写入的首地址，按一个扇区16个page4KB字节写入 非扇区对齐首地址
    uint32_t WriteAddrResi = 0; //剩余要写入的地址，每次最多写入4KB字节，首地址

    WriteNumResi = NumByteToWrite;
    WriteAddrResi = WriteAddr;
    while (WriteNumResi > 0)
    {
        SectorNum = WriteAddrResi / SPI_FLASH_SectorSize;
        PageNum = (WriteAddrResi % SPI_FLASH_SectorSize) / SPI_FLASH_PageSize;
        AddrNum = (WriteAddrResi % SPI_FLASH_SectorSize) % SPI_FLASH_PageSize;
        WriteNumCapa = SPI_FLASH_SectorSize - PageNum * SPI_FLASH_PageSize - AddrNum;
        WriteAddrThis = WriteAddrResi;                    //保存当前循环要写入的首地址 非扇区对齐首地址
        WriteAddrSect = SectorNum * SPI_FLASH_SectorSize; //扇区对齐首地址
        if (WriteNumCapa < WriteNumResi)
        {
            WriteNumThis = WriteNumCapa;
            WriteNumResi = WriteNumResi - WriteNumCapa;
            WriteAddrResi = WriteAddrThis + WriteNumThis; //保存下次循环要写入的首地址
        }
        else
        {
            WriteNumThis = WriteNumResi;
            WriteNumResi = 0;
            WriteAddrResi = 0;
        }
        //读取本次要写入的范围，看是否存在非0xFF的字节
        SPI_FLASH_BufferRead(FlashDataTemp, WriteAddrThis, WriteNumThis);
        for (i = 0; i < WriteNumThis; i++)
        {
            if (FlashDataTemp[i] != 0xFF)
                break;
        }
        //如果存在读取整个扇区 写入首地址和扇区地址对齐 写入数量也等于扇区字节数
        //拷贝字节 格式化扇区
        //整个扇区的写入
        if (i < WriteNumThis)
        {
            SPI_FLASH_BufferRead(FlashDataTemp, WriteAddrSect, SPI_FLASH_SectorSize); //读取整个扇区的字节
            SPI_FLASH_SectorErase(SectorNum);                                         //格式化扇区
            for (i = 0; i < WriteNumThis; i++)                                        //拷贝字节
            {
                FlashDataTemp[PageNum * SPI_FLASH_PageSize + AddrNum + i] = *pBuffer;
                pBuffer++;
            }
            WriteAddrThis = WriteAddrSect; //为了使用统一的写入for循环
            WriteNumThis = SPI_FLASH_SectorSize;
        }
        else
        {
            for (i = 0; i < WriteNumThis; i++) //拷贝字节
            {
                FlashDataTemp[i] = *pBuffer;
                pBuffer++;
            }
        }
        //如果不存在直接写入 可能存在多个page
        //while循环每次最多写入256字节，写完为止SPI_FLASH_WaitForWriteEnd()
        i = 0;
        while (WriteNumThis > 0)
        {

            if (WriteNumThis > SPI_FLASH_PageSize)
            {
                SPI_FLASH_PageWrite(&FlashDataTemp[i], WriteAddrThis, SPI_FLASH_PageSize);
                WriteNumThis -= SPI_FLASH_PageSize;
                WriteAddrThis += SPI_FLASH_PageSize;
                i = SPI_FLASH_PageSize;
            }
            else
            {
                SPI_FLASH_PageWrite(&FlashDataTemp[i], WriteAddrThis, WriteNumThis);
                WriteNumThis = 0;
                WriteAddrThis = 0;
            }
        }
        memset(FlashDataTemp, 0, sizeof(FlashDataTemp)); //数组清零
    }
}

/**
  * @brief SPI FLASH初始化
  * @param  none
  * @retval ErrorStatus初始化是否成功
  */
ErrorStatus SPI_FLASH_Init(void)
{
    SPI1_Init();
    if (SPI_FLASH_ReadManufacturerDeviceID() == sFLASH_ID)
    {
#if defined(W25Q256)
        SPI_FLASH_4ByteAddressEnter();
        SPI_FLASH_WaitForWriteEnd(); //等待写完成
        if (SPI_FLASH_4ByteAddressMode() != SET)
            SPI_FLASH_4ByteAddressEnter();
#endif
        return SUCCESS;
    }
    else
        return ERROR;
}

/**
  * @brief  Flash数据读取初始化
  * @param  none
  * @retval none
  */
void SPI_FLASH_DataRead(void)
{
    SPI_FLASH_BufferRead((uint8_t *)Flash_DataArray, Flash_STAT_ADDR, Flash_DATA_NUMBER);

    memcpy(Flash_DeviceSN, Flash_DataArray, Flash_SN_NUMBER);

    #ifdef CME_303
    memcpy(&Flash_DeviceType, Flash_DataArray + Flash_SN_NUMBER, sizeof(uint8_t));
    if (Flash_DeviceType > 1)
    {
        Flash_DeviceType = 0;
    }
    #endif

    memcpy(&Flash_Language, Flash_DataArray + Flash_SN_NUMBER + sizeof(uint8_t), sizeof(uint8_t));
    if (Flash_Language > 1)
    {
        Flash_Language = 0;
    }
}

/**
  * @brief  Flash数据写入，保存掉电数据
  * @param  none
  * @retval none
  */
void SPI_FLASH_DataWrite(void)
{
    memcpy(Flash_DataArray, Flash_DeviceSN, Flash_SN_NUMBER);
    memcpy(Flash_DataArray + Flash_SN_NUMBER, &Flash_DeviceType, sizeof(uint8_t));
    memcpy(Flash_DataArray + Flash_SN_NUMBER + sizeof(uint8_t), &Flash_Language, sizeof(uint8_t));

    SPI_FLASH_BufferWrite((uint8_t *)Flash_DataArray, Flash_STAT_ADDR, Flash_DATA_NUMBER);
}