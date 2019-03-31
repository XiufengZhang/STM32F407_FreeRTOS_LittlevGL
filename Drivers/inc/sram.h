#ifndef __SRAM_H
#define __SRAM_H

#ifdef __cplusplus
extern "C"
{
#endif

#define USE_EXRAM
//SDRAM类型容量地址相关
// #define SRAM_SIZE 0x0010000 //SRAM 大小, 64K 字节
// #define SRAM_SIZE 0x0020000 //SRAM 大小, 128K 字节
#define SRAM_SIZE 0x0040000 //SRAM 大小, 256K 字节
// #define SRAM_SIZE 0x0080000 //SRAM 大小, 512K 字节
// #define SRAM_SIZE 0x0100000 //SRAM 大小, 1M 字节

//STM32F407的bank1支持4个固定大小的存储区域, 分别是64MB
//使用#if只能判断整数数值 FSMC_Bank1_NORSRAM1 0x00000000, FSMC_Bank1_NORSRAM2 0x00000002, FSMC_Bank1_NORSRAM3 0x00000004, FSMC_Bank1_NORSRAM4 0x00000006
#define SRAM_BANK_SELECT 0x00000004 //0x00000000
#if (SRAM_BANK_SELECT == 0x00000000)
#define SRAM_BANK_ADDR ((uint32_t)0x60000000) //SRAM 起始地址 FMC_NE1存储空间的起始地址
#elif (SRAM_BANK_SELECT == 0x00000002)
#define SRAM_BANK_ADDR ((uint32_t)0x64000000) //SRAM 起始地址 FMC_NE2存储空间的起始地址
#elif (SRAM_BANK_SELECT == 0x00000004)
#define SRAM_BANK_ADDR ((uint32_t)0x68000000) //SRAM 起始地址 FMC_NE3存储空间的起始地址
#elif (SRAM_BANK_SELECT == 0x00000006)
#define SRAM_BANK_ADDR ((uint32_t)0x6A000000) //SRAM 起始地址 FMC_NE4存储空间的起始地址
#endif
#define SRAM_BANK_ADDRHead SRAM_BANK_ADDR                              //定义首地址
#define SRAM_BANK_ADDREnd ((uint32_t)(SRAM_BANK_ADDR + SRAM_SIZE - 2)) //定义未地址,按照16位访问

//AHB3时钟是168MHz
#define SRAM_ADDRSET 0x01 //地址设置阶段的持续时间
#define ADDRHLD 0x00      //地址保持阶段的持续时间
#define DATASET 0x03      //数据阶段的持续时间
#define BUSTURN 0x01      //总线周转阶段的持续时间

    extern ErrorStatus SRAMStatus; // 标记SRAM是否正常 0 ERROR, 1正SUCCESS

    ErrorStatus SRAMFSMC_Init(void);
    void SRAMFSMC_WriteBuffer(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite);
    void SRAMFSMC_ReadBuffer(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumHalfwordToRead);

#ifdef __cplusplus
}
#endif

#endif