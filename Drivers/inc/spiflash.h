#ifndef __SPIFLASH_H
#define __SPIFLASH_H

#ifdef __cplusplus
extern "C"
{
#endif

// #define W25X16
#define W25Q16
// #define W25Q32
// #define W25Q64
// #define W25Q128
// #define W25Q256

/* Private typedef -----------------------------------------------------------*/
#if defined(W25X16)
#define sFLASH_ID 0xEF3015
#define SPI_FLASH_TotalSize 2048 //KByte 2MB
#elif defined(W25Q16)
#define sFLASH_ID 0xEF4015
#define SPI_FLASH_TotalSize 2048 //KByte 2MB
#elif defined(W25Q32)
#define sFLASH_ID 0xEF4016
#define SPI_FLASH_TotalSize 4096 //KByte 4MB
#elif defined(W25Q64)
#define sFLASH_ID 0XEF4017
#define SPI_FLASH_TotalSize 8192 //KByte 8MB
#elif defined(W25Q128)
#define sFLASH_ID 0XEF4018
#define SPI_FLASH_TotalSize 16384 //KByte 16MB
#elif defined(W25Q256)
#define sFLASH_ID 0XEF4019
#define SPI_FLASH_TotalSize 32768 //KByte 32MB
#else
#error "The flash type has not been defined"
#endif

#define SPI_FLASH_PageSize 256                                                         //Byte
#define SPI_FLASH_SectorSize 4096                                                      //Byte 4KB
#define SPI_FLASH_BlockSize32 32768                                                    //Byte 32KB
#define SPI_FLASH_BlockSize64 65536                                                    //Byte 64Kb
#define SPI_FLASH_SectorNumMax ((SPI_FLASH_TotalSize)*1024 / (SPI_FLASH_SectorSize))   //扇区从0开始
#define SPI_FLASH_Block32NumMax ((SPI_FLASH_TotalSize)*1024 / (SPI_FLASH_BlockSize32)) //块从0开始
#define SPI_FLASH_Block64NumMax ((SPI_FLASH_TotalSize)*1024 / (SPI_FLASH_BlockSize64)) //块从0开始

#define PHYSIC_START_ADDR 0                  //物理起始地址
#define LOG_PAGE_SIZE SPI_FLASH_PageSize     //spiffs 逻辑page大小
#define LOG_BLOCK_SIZE SPI_FLASH_BlockSize64 //spiffs 逻辑block大小

/* Private define ------------------------------------------------------------*/
#define W25X_WriteEnable 0x06
#define W25X_WriteDisable 0x04
#define W25X_ReadStatusReg1 0x05 //Registers1
#define W25X_ReadStatusReg2 0x35 //Registers2
#define W25X_ReadStatusReg3 0x15 //Registers3
#define W25X_WriteStatusReg 0x01
//在4-Byte Address模式下3地址/4地址操作指令通用,在3-Byte Address模式下不通用，要用独立地址
// #define W25X_ReadData 0x13        //4字节地址
// #define W25X_FastReadData 0x0C    //快速读取 只提高频率
// #define W25X_FastReadDualOut 0x3C //快速读取双倍 DI(IO0) DO(IO1)
// #define W25X_FastReadDualIO 0xBC  //快速读取双倍 DI(IO0) DO(IO1) 地址也双倍
// #define W25X_FastReadQuadOut 0x6C //快速读取四倍 DI(IO0) DO(IO1) IO2 IO3
// #define W25X_FastReadQuadIO 0xEC  //快速读取四倍 DI(IO0) DO(IO1) IO2 IO3 地址也四倍
// #define W25X_PageProgram 0x12
// #define W25X_PageProgramQuad 0x34 //快速写入四倍
#define W25X_ReadData 0x03
#define W25X_FastReadData 0x0B    //快速读取 只提高频率
#define W25X_FastReadDualOut 0x3B //快速读取双倍 DI(IO0) DO(IO1)
#define W25X_FastReadDualIO 0xBB  //快速读取双倍 DI(IO0) DO(IO1) 地址也双倍
#define W25X_FastReadQuadOut 0x6B //快速读取四倍 DI(IO0) DO(IO1) IO2 IO3
#define W25X_FastReadQuadIO 0xEB  //快速读取四倍 DI(IO0) DO(IO1) IO2 IO3 地址也四倍
#define W25X_PageProgram 0x02
#define W25X_PageProgramQuad 0x32 //快速写入四倍 DI(IO0) DO(IO1) IO2 IO3
#define W25X_SectorErase 0x20
#define W25X_BlockErase32 0x52
#define W25X_BlockErase64 0xD8
#define W25X_ChipErase 0xC7
#define W25X_PowerDown 0xB9
#define W25X_ReleasePowerDown 0xAB
#define W25X_DeviceID 0xAB
#define W25X_ManufactDeviceID 0x90
#define W25X_JedecDeviceID 0x9F
#define W25X_4ByteAddressEnter 0xB7
#define W25X_4ByteAddressExit 0xE9

#define BUSY_Flag 0x01 //Registers1 S0 BUSY Flag
#define WEL_Flag 0x02  //Registers1 S1 WEL Flag
#define ADS_Flag 0x01  //Registers3 S16 ADS Flag
#define Dummy_Byte 0xFF

#define Flash_STAT_ADDR 0x00 //Flash地址
#define Flash_SN_NUMBER 8    //SN字节长度
#define Flash_DATA_NUMBER 10 //读取存储Flash数据长度

    extern ErrorStatus FlashStatus; //标记串行Flash是否正常 ERROR失败 SUCCESS正常

    extern uint8_t Flash_DataRefresh;                //Flash数据需要写入标识 0不需要写入 1要从新写入
    extern char Flash_DeviceSN[Flash_SN_NUMBER + 1]; //设备序列号 SN 4字节字符串 32位整数例如180102999
    extern uint8_t Flash_DeviceType;                 //定义设备类型 0设备是303 1设备是305
    extern uint8_t Flash_Language;                   //定义语言 0英文 1中文

    void SPI_Flash_PowerDown(void);
    void SPI_Flash_WAKEUP(void);
    void SPI_FLASH_SectorErase(uint32_t SectorNum);
    void SPI_FLASH_BlockErase32(uint32_t Block32Num);
    void SPI_FLASH_BlockErase64(uint32_t Block64Num);
    void SPI_FLASH_BulkErase(void);

    ErrorStatus SPI_FLASH_Init(void);
    void SPI_FLASH_DataRead(void);
    void SPI_FLASH_DataWrite(void);

#ifdef __cplusplus
}
#endif

#endif