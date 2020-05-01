# STM32F407_Optical

## 工程说明
MCU型号：STM32F407ZG  
外挂SARM 型号：S61WV12816DBLL-10TL

光功率计GUI板

硬件：测试版与量产版有2处不同
1. SARM  测试版有S61WV12816DBLL-10TL
2. flash 测试版W25Q32；量产版W25Q16

软件编译前
1. spiflash.h文件选择#define W25Q16
2. sram.h文件注释掉USE_EXRAM
3. system_stm32f4xx.c文件注释掉DATA_IN_ExtSRAM


***
## 修改记录

**v1.0.0**

1. 新建工程

**v1.0.1**

1. 测试
2. 测试