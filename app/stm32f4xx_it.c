/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
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
#include <string.h>
#include "misc.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_usart.h"
#include "main.h"
#include "gpio.h"
#include "timer.h"
#include "fttouch.h"
#include "gttouch.h"
#include "tsctouch.h"
#include "uart.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  __disable_irq();//禁止所有中断
  NVIC_SystemReset();//软件复位MCU
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  __disable_irq();//禁止所有中断
    NVIC_SystemReset();//软件复位MCU
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
#if defined(__GNUC__)
void __attribute__ ((weak)) SVC_Handler(void)
#elif defined(__ICCARM__)
__weak void SVC_Handler(void)
#else
void SVC_Handler(void)
#endif
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
#if defined(__GNUC__)
void __attribute__ ((weak)) PendSV_Handler(void)
#elif defined(__ICCARM__)
__weak void PendSV_Handler(void)
#else
void PendSV_Handler(void)
#endif
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
#if defined(__GNUC__)
void __attribute__ ((weak)) SysTick_Handler(void)
#elif defined(__ICCARM__)
__weak void SysTick_Handler(void)
#else
void SysTick_Handler(void)
#endif
{
}

/**
  * @brief  This function handles Window WatchDog.
  * @param  None
  * @retval None
  */
void WWDG_IRQHandler(void)
{
}

/**
  * @brief  This function handles EXTI Line0.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void)
{
}

/**
  * @brief  This function handles EXTI Line1.
  * @param  None
  * @retval None
  */
void EXTI1_IRQHandler(void)
{
}

/**
  * @brief  This function handles EXTI Line2.
  * @param  None
  * @retval None
  */
void EXTI2_IRQHandler(void)
{
}

/**
  * @brief  This function handles EXTI Line3.
  * @param  None
  * @retval None
  */
void EXTI3_IRQHandler(void)
{
}

/**
  * @brief  This function handles EXTI Line4.
  * @param  None
  * @retval None
  */
void EXTI4_IRQHandler(void)
{
}

/**
  * @brief  This function handles External Line[9:5]s.
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void)
{
  if (EXTI_GetITStatus(I2CT_INT_EXTI_LINE))//触摸IC中断
  {
    EXTI_ClearITPendingBit(I2CT_INT_EXTI_LINE);
#if defined(TOUCH_GT)
    //    GT_IIC_ReadCoord();
    GTTouchUpFlag = 1;
#elif defined(TOUCH_FT)
    //    FT_IIC_ReadCoord();
    FTTouchUpFlag = 1;
#endif
  }
}

/**
  * @brief  This function handles External Line[15:10]s.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
}

/**
  * @brief  ADC溢出中断
  * @param  None
  * @retval None
  */
void ADC_IRQnHandler()
{
}

/**
  * @brief  ADC DMA
  * @param  None
  * @retval None
  */
void DMA2_Stream0_IRQHandler()
{
}

/**
  * @brief  USART1RX DMA
  * @param  None
  * @retval None
  */
void DMA2_Stream5_IRQHandler()
{
    if(DMA_GetITStatus(DMA2_Stream5, DMA_IT_TEIF5) || DMA_GetITStatus(DMA2_Stream5, DMA_IT_DMEIF5))// || DMA_GetITStatus(DMA2_Stream5, DMA_IT_FEIF5)
    {
        DMA_Cmd(DMA2_Stream5, DISABLE);
        DMA_SetCurrDataCounter(DMA2_Stream5, USART1BUFFERNUM);//保证DMA传输不错位，失能才能写入
        DMA_Cmd(DMA2_Stream5, ENABLE);
        DMA_ClearITPendingBit(DMA2_Stream5, DMA_IT_TEIF5);//清除传输错误标识
        DMA_ClearITPendingBit(DMA2_Stream5, DMA_IT_DMEIF5);
        DMA_ClearITPendingBit(DMA2_Stream5, DMA_IT_FEIF5);//清除FIFO错误标识
    }
}

/**
  * @brief  USART1TX DMA
  * @param  None
  * @retval None
  */
void DMA2_Stream7_IRQHandler()
{
    if(DMA_GetITStatus(DMA2_Stream7, DMA_IT_TCIF7))
    {
        DMA_Cmd(DMA2_Stream7, DISABLE);//发送数据时软件开启
        UART1DMABusy = RESET;
        DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TCIF7);
    }
    else if(DMA_GetITStatus(DMA2_Stream7, DMA_IT_TEIF7) || DMA_GetITStatus(DMA2_Stream7, DMA_IT_DMEIF7))// || DMA_GetITStatus(DMA2_Stream7, DMA_IT_FEIF7)
    {
        DMA_Cmd(DMA2_Stream7, DISABLE);//发送数据时软件开启
        UART1DMABusy = RESET;
        DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TEIF7);//清除传输错误标识
        DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_DMEIF7);
        DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_FEIF7);//清除FIFO错误标识
    }
}

/**
  * @brief  USART1
  * @param  None
  * @retval None
  */
void USART1_IRQHandler()
{
    if(USART_GetITStatus(USART1,USART_IT_IDLE))
    {
        UART1ReadDataLen=USART1BUFFERNUM-DMA_GetCurrDataCounter(DMA2_Stream5);//获取RX数据长度
        DMA_Cmd(DMA2_Stream5, DISABLE);//关闭RX
        DMA_SetCurrDataCounter(DMA2_Stream5, USART1BUFFERNUM);//保证DMA传输不错位，失能才能写入
        DMA_Cmd(DMA2_Stream5, ENABLE);//使能RX
        UART1ReceFlag = SET;
        USART_ReceiveData(USART1);//清除IDLE
        // USART_Cmd(USART1, DISABLE);//应当是UART1ReadDataLen大于0才失能串口
    }
    else if(USART_GetITStatus(USART1,USART_IT_PE) || USART_GetITStatus(USART1,USART_IT_ERR))
    {
        DMA_ClearFlag(DMA2_Stream5, DMA_FLAG_TCIF5);
        DMA_ClearFlag(DMA2_Stream5, DMA_FLAG_HTIF5);
        DMA_ClearFlag(DMA2_Stream5, DMA_FLAG_TEIF5);
        DMA_ClearFlag(DMA2_Stream5, DMA_FLAG_DMEIF5);
        DMA_ClearFlag(DMA2_Stream5, DMA_FLAG_FEIF5);
        DMA_Cmd(DMA2_Stream5, DISABLE);//关闭RX
        DMA_SetCurrDataCounter(DMA2_Stream5, USART1BUFFERNUM);//保证DMA传输不错位，失能才能写入
        DMA_Cmd(DMA2_Stream5, ENABLE);//使能RX
        USART_ReceiveData(USART1);//清除 PE和EER(FE NE ORE)
    }
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void TIM4_IRQHandler()
{
}

/**
  * @brief  编码器脉冲捕获
  * @param  None
  * @retval None
  */
void TIM5_IRQHandler()
{
}

/**
  * @brief  TIM6中断 定时器延时
  * @param  None
  * @retval None
  */
void TIM6_DAC_IRQHandler()
{
  if(TIM_GetITStatus(TIM6, TIM_IT_Update))
  {
    TIM_Cmd(TIM6, DISABLE);//失能定时器
    STMDelayFlag = 0;
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
//    TIM_ClearITPendingBit(TIM6, TIM_IT_CC1);
  }
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void TIM8_BRK_TIM12_IRQHandler()
{
}

/**
  * @brief  TIM14中断 间隔时间循环中断
  * @param  None
  * @retval None
  */
void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM14, TIM_IT_Update))
  {
    TimCyclic_Process();
    TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
  }
}

/**
  * @brief  This function handles RTC重复唤醒中断 global interrupt request.
  * @param  None
  * @retval None
  */
void RTC_WKUP_IRQHandler(void)
{
}

/**
  * @brief  This function handles RTC闹钟中断 global interrupt request.
  * @param  None
  * @retval None
  */
void RTC_Alarm_IRQHandler(void)
{
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
