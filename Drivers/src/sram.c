#include <string.h>
#include "stm32f4xx.h"
#include "stm32f4xx_fsmc.h"
#include "timer.h"
#include "sram.h"

//使用NOR/SRAM的 Bank1.sector3,地址位HADDR[27,26]=10
//对IS61LV3216/IS62WV3216,地址线范围为A0~A14
//对IS61LV6416/IS62WV6416,地址线范围为A0~A15   IS61LV3216与IS61LV6416 pin兼容
//对IS61LV12816/IS62WV12816,地址线范围为A0~A16
//对IS61LV25616/IS62WV25616,地址线范围为A0~A17
//对IS61LV51216/IS62WV51216,地址线范围为A0~A18 IS61LV25616与IS61LV51216 pin兼容
ErrorStatus SRAMStatus = ERROR; // 标记SRAM是否正常 0 ERROR, 1正SUCCESS

/**
  * @brief  SRAM FSMC测试是否初始化成功，只能测试是否能读写，不能测试是否超出范围 SRAM超出范围出现假读写成功 SDRAM超出范围硬件故障
  * @param  None
  * @retval ErrorStatus测试SRAM读写是否成功
  */
static ErrorStatus SRAMFSMC_WriteReadTest(void)
{
  volatile uint16_t Temp = 0;

  *(uint16_t *)(SRAM_BANK_ADDRHead) = 0x5050;
  Temp = *(uint16_t *)(SRAM_BANK_ADDRHead);
  if (Temp != 0x5050)
    return ERROR;

  return SUCCESS;
}

/**
  * @brief  SRAM FSMC初始化
  * @param  None
  * @retval None
  */
ErrorStatus SRAMFSMC_Init(void)
{
  FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef FSMC_NORSRAMReadWriteStructure;

  FSMC_NORSRAMReadWriteStructure.FSMC_AddressSetupTime = SRAM_ADDRSET; //地址设置阶段的持续时间 (Address setup phase duration) 地址建立时间（ADDRSET）为2个HCLK 2/168M=11.9ns
  FSMC_NORSRAMReadWriteStructure.FSMC_AddressHoldTime = ADDRHLD;       //地址保持阶段的持续时间 (Address-hold phase duration)  地址保持时间（ADDRHLD）
  FSMC_NORSRAMReadWriteStructure.FSMC_DataSetupTime = DATASET;         //数据阶段的持续时间 (Data-phase duration)              数据建立时间（DATASET）为4个HCLK 4/168M=23.8ns 多2个HCLK
  FSMC_NORSRAMReadWriteStructure.FSMC_BusTurnAroundDuration = BUSTURN; //总线周转阶段的持续时间 (Bus turnaround phase duration)
  FSMC_NORSRAMReadWriteStructure.FSMC_CLKDivision = 0x00;              //This parameter is not used for asynchronous NOR Flash, SRAM or ROM accesses.
  FSMC_NORSRAMReadWriteStructure.FSMC_DataLatency = 0x00;              //It is don't care in asynchronous NOR, SRAM or ROM accesses
  FSMC_NORSRAMReadWriteStructure.FSMC_AccessMode = FSMC_AccessMode_A;  //模式A

  FSMC_NORSRAMInitStructure.FSMC_Bank = SRAM_BANK_SELECT;                                  //使用NE3 ，也就对应BTCR[4],[5]。
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;             //地址/数据复用使能位 (Address/data multiplexing enable bit) 不复用数据地址
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;                        //存储器类型 (Memory type) SRAM
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;               //存储器数据总线宽度 (Memory databus width) 16bit
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;           //同步模式 突发使能位 (Burst enable bit)
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;         //等待信号极性位 (Wait signal polarity bit) 仅当15位ASYNCWAIT置1才有效
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;                         //同步模式 环回突发模式支持 (Wrapped burst mode support)
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState; //等待时序配置 (Wait timing configuration) NWAIT 信号指示存储器中的数据是否有效，或者在同步模式下访问 Flash 时是否必须插入等待周期。
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;              //写入使能位 (Write enable bit) 存储器写使能
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;                     //同步模式 等待使能位 (Wait enable bit) 使能/禁止在同步模式下访问 Flash 时通过 NWAIT 信号插入等待周期
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;                 //扩展模式使能 (Extended mode enable)
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;         //异步传输期间的等待信号 (Wait signal during asynchronous transfers)
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;                     //同步模式 写入突发使能 (Write burst enable)
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAMReadWriteStructure;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_NORSRAMReadWriteStructure; //读写同样时序

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); //初始化FSMC配置
  FSMC_NORSRAMCmd(SRAM_BANK_SELECT, ENABLE);    // 使能BANK3

  return SRAMFSMC_WriteReadTest();
}

/**
  * @brief  SRAM FSMC在指定地址开始,连续写入n个字节.其实地址是0,函数内部自动增加了基地址Bank1_SRAM3_ADDRHead
  * @param  
pBuffer:字节指针
WriteAddr:要写入的地址
NumHalfwordToWrite要写入的字节数
  * @retval None
  */
void SRAMFSMC_WriteBuffer(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t NumHalfwordToWrite)
{
  for (; NumHalfwordToWrite != 0; NumHalfwordToWrite--)
  {
    *(uint8_t *)(SRAM_BANK_ADDRHead + WriteAddr) = *pBuffer;
    WriteAddr++;
    pBuffer++;
  }
}

/**
  * @brief  SRAM FSMC在指定地址开始,连续读出n个字节.其实地址是0,函数内部自动增加了基地址Bank1_SRAM3_ADDRHead
  * @param  
pBuffer:字节指针
WriteAddr:要读出的起始地址
NumHalfwordToWrite要读取的字节数
  * @retval None
  */
void SRAMFSMC_ReadBuffer(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumHalfwordToRead)
{
  for (; NumHalfwordToRead != 0; NumHalfwordToRead--)
  {
    *pBuffer++ = *(uint8_t *)(SRAM_BANK_ADDRHead + ReadAddr);
    ReadAddr++;
  }
}