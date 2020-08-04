/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.c
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "timer.h"
#include "spiflash.h"
#include "sram.h"
#include "ssdlcd.h"
#include "fttouch.h"
#include "gttouch.h"
#include "uart.h"
#include "iwdg.h"
#include "lvgl/lvgl.h"
// #include "GUI.h"
// #include "GUI_Display.h"

// ErrorStatus temp = ERROR;
// GUI_PID_STATE TouchState;
// uint8_t FlashTest[256] = {0};
// uint32_t FlashTest1 = 0;
// uint32_t FlashTest2 = 0;

/** @addtogroup Template_Project
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/


/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    /*!< At this stage the microcontroller clock setting is already configured, 
    this is done through SystemInit() function which is called from startup
    files (startup_stm32f40_41xxx.s/startup_stm32f427_437xx.s/
    startup_stm32f429_439xx.s/startup_stm32f401xx.s/startup_stm32f411xe.s or
    startup_stm32f446xx.s)
    before to branch to application main.
    To reconfigure the default setting of SystemInit() function, 
    refer to system_stm32f4xx.c file */
    /* Add your application code here
    
    core_cm4.h 内核设备访问层，stm32f4xx.h中断号 地址定义等，system_stm32f4xx.c system_stm32f4xx.h初始化函数系统定义。
    
    C/C++ Compiler -> Preprocessor 定义STM32F40_41xxx USE_STDPERIPH_DRIVER，也可以在stm32f4xx.h文件中定义。
    定义USE_STDPERIPH_DRIVER是引用"stm32f4xx_conf.h"外设配置文件
    定义STM32F40_41xxx是确定设备类型 在system_stm32f4xx.c中查找可用定义
    HSE_VALUE 在stm32f4xx.h 文件中定义，默认是25MHz根据实际情况更改，系统时钟频率是按照定义值计算的，不能测量实际值。
    PLL_M、PLL_N、PLL_P、PLL_Q在system_stm32f4xx.c中定义
    
    汇编语言启动system_stm32f4xx.c文件中的SystemInit函数，再调用SetSysClock函数，再到main函数。SetSysClock函数启动HSE，
    根据HSE_VALUE定义设置系统时钟，如果外部高速晶振实际频率倍频后大于168MHz，不能进入main函数，出现不能调试现象。
    
    DSP预定义说明
    __FPU_PRESENT 用来确定处理器是否带 FPU 功能，stm32f4xx.h 头文件中已经定义。
    __FPU_USED 用来确定是否开启 FPU 功能，设置 Floating Point Hardware 为 Use Single Precision一样的效果。在core_cm4.h文件中当__FPU_PRESENT为1是自动赋值
    __ARMVFP__(IAR) __VFP_FP__(GCC) __TARGET_FPU_VFP(MDK) 是否开启FPU，为1时在core_cm4.h文件中执行__FPU_USED __FPU_PRESENT
    ARM_MATH_CM4这个就非常重要，必须要配置进去，否则在编译之后，会默认使用math.h的库函数，而不会用到硬件的FPU的。
    ARM_MATH_MATRIX_CHECK是库函数的参数检查开关，这里添加后就是打开。
    ARM_MATH_ROUNDING这个是库函数在运算是是否开启四舍五入的功能，这里添加，可以根据自己的需要进行配置。
    __ICCARM__、__CC_ARM、__GNUC__是不同编译器的编译配置宏定义，__ICCARM__是IAR环境,__CC_ARM是代表MDK开发环境，__GNUC__是gcc编译环境。
    __IAR_SYSTEMS_ASM__ IAR汇编程序标识
    
    stm32f4xx_dma2d.c文件中void DMA2D_Init(DMA2D_InitTypeDef* DMA2D_InitStruct)函数，
    DMA2D->OCOLR |= ((outgreen) | (outred) | (DMA2D_InitStruct->DMA2D_OutputBlue) | (outalpha));
    改为DMA2D->OCOLR = ((outgreen) | (outred) | (DMA2D_InitStruct->DMA2D_OutputBlue) | (outalpha));
    */

    // CLKHSEStatus = RccInit(); //标记外部晶振启动是否正常 ERROR失败 SUCCESS正常 已移到各个模块内部初始化
    // SRAMStatus = SRAMFSMC_Init();//已移到system_stm32f4xx.c SystemInit_ExtMemCtl中初始化
    // GpioInit(); //已移到各个模块内部初始化
    TimCyclic_Init(); //ms中断 emwin心跳
    STM32Delay_Init();
    //    Tim5Init();//编码器
    FlashStatus = SPI_FLASH_Init(); //标记串行Flash是否正常 ERROR失败 SUCCESS正常
    if (FlashStatus)
      SPI_FLASH_DataRead(); //读取Flash或者EEPROM数据
    UART1Init();

    Tim4Init(); //PWM输出调整背光
    lv_init();
    lv_port_disp_init();
    // GUI_Init(); //初始化emWin GUI
    // GUI_Delay(10);
    // Display_Initial(); //GUI初始化设置 皮肤
    FTFLCD_BL(100);

    lv_obj_t * scr = lv_obj_create(NULL, NULL);
    lv_scr_load(scr); /*Load the screen*/

    lv_obj_t * btn = lv_btn_create(scr, NULL);     /*lv_scr_act() Add a button the current screen*/
    lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
    lv_obj_set_size(btn, 100, 50);                          /*Set its size*/

    lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, NULL);/*Assign a callback to the button*/
    lv_obj_t * label = lv_label_create(btn, NULL);          /*Add a label to the button*/
    lv_label_set_text(label, "Button");                     /*Set the labels text*/

    //    GUIDEMO_Main();
    //    GUI_SetColor(GUI_RED);
    //    GUI_FillRect(0, 0, 199, 33);
    //    GUI_SetColor(GUI_GREEN);
    //    GUI_FillRect(0, 34, 199, 66);
    //    GUI_SetColor(GUI_BLUE);
    //    GUI_FillRect(0, 67, 199, 99);
    //    GUI_SetBkColor(GUI_WHITE);
    //    GUI_FillCircle(100, 50, 35);
    //    GUI_FillRect(10, 10, 40, 90);
    //    GUI_FillRect(160, 10, 190, 90);
    //    GUI_DrawCircle(400, 240, 100);
    //    GUI_SetColor(GUI_BLUE);
    //    GUI_FillRect(0, 0, 799, 479);
    //    GUI_SetBkColor(GUI_WHITE);
    //    GUI_Clear();
    //    GUI_SetColor(GUI_BLACK);
    //    GUI_DispStringHCenterAt("Alphablending", 45, 41);
    //    GUI_SetColor((0x40uL << 24) | GUI_RED);
    //    GUI_FillRect(0, 0, 49, 49);
    //    GUI_SetColor((0x80uL << 24) | GUI_GREEN);
    //    GUI_FillRect(20, 20, 69, 69);
    //    GUI_SetColor((0xC0uL << 24) | GUI_BLUE);
    //    GUI_FillRect(40, 40, 89, 89);
    //    FTFLCD_BL(0);
    //显示自检画面

    // Display_Welcome(); //显示欢迎画面
    WatchDogInit();

    /* Infinite loop */
    while (1)
    {
      WatchDogFeed();
      if (Flash_DataRefresh)
      {
        Flash_DataRefresh = 0;
        SPI_FLASH_DataWrite();
      }
      if (GTTouchUpFlag == 1)
      {
        GTTouchUpFlag = 0;
        GT_IIC_ReadCoord();
      }
      if (FTTouchUpFlag == 1)
      {
        FTTouchUpFlag = 0;
        FT_IIC_ReadCoord();
      }
      if (UART1ReceFlag)
      {
        UART1ReceFlag = RESET;
        USART1_ReadDataProce();
      }

      // FlashTest1 = GUI_ALLOC_GetNumFreeBytes();
      // FlashTest2 = GUI_ALLOC_GetNumUsedBytes();

      if (Timer10msFlag)
      {
        Timer10msFlag = 0;
        TimerProcess(); //定时器过程
        lv_tick_inc(10);
      }
      if (Timer200msFlag)
      {
        Timer200msFlag = 0;
        lv_task_handler();
        // GUI_Exec(); //GUI_Delay(5);//GUI延时 类似GUI_Exec重绘无效部分 WM_InvalidateWindow
      }
      if (Timer1sFlag)
      {
        Timer1sFlag = 0;
      }
    }
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
