/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2017  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.44 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to STMicroelectronics International
N.V. a Dutch company with a Swiss branch and its headquarters in Plan-
les-Ouates, Geneva, 39 Chemin du Champ des Filles, Switzerland for the
purposes of creating libraries for ARM Cortex-M-based 32-bit microcon_
troller products commercialized by Licensee only, sublicensed and dis_
tributed under the terms and conditions of the End User License Agree_
ment supplied by STMicroelectronics International N.V.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUIConf.c
Purpose     : Display controller initialization
---------------------------END-OF-HEADER------------------------------
*/

/**
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics. 
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license SLA0044,
  * the "License"; You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *                      http://www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include "stm32f4xx.h"
#include "GUI.h"
#include "sram.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Define the available number of bytes available for the GUI
//
#define GUI_NUMBYTES  0x14000 //Byte 0x14000 80KB, 0x100000 1MB, 0x200000 2MB

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
#ifdef USE_EXRAM
#if defined ( __CC_ARM )
/*
#define GUI_EXTBUFADD         SRAM_BANK_ADDRHead//外部SSD1963已经有显存，此内存作为GUI程序运行。如果是SDRAM作为显存，要减去显存地址。
static U32 aMemory[GUI_NUMBYTES / 4] __attribute__((at(GUI_EXTBUFADD)));
*/
static U32 aMemory[GUI_NUMBYTES / 4] __attribute__((section("._user_heap")));;// 32 bit aligned memory area
#elif defined ( __ICCARM__ )//需要增加#include "stm32f4xx.h"引用
/*
#pragma location = {address|register|NAME}
#pragma location = address 等价于@address
#pragma location = register 等价于@register
#pragma location = NAME 等价于@section

#define GUI_EXTBUFADD         SRAM_BANK_ADDRHead//外部SSD1963已经有显存，此内存作为GUI程序运行。如果是SDRAM作为显存，要减去显存地址。
#pragma location = GUI_EXTBUFADD
static __no_init U32 aMemory[GUI_NUMBYTES / 4];
static __no_init U32 aMemory[GUI_NUMBYTES / 4] @ GUI_EXTBUFADD;//等价于上两行
*/
static __no_init U32 aMemory[GUI_NUMBYTES / 4] @ EXRAM;// 32 bit aligned memory area
#elif defined (__GNUC__)
/*
#define GUI_EXTBUFADD         Bank1_SRAM3_ADDRHead//外部SSD1963已经有显存，此内存作为GUI程序运行。如果是SDRAM作为显存，要减去显存地址。
static U32 aMemory[GUI_NUMBYTES / 4] __attribute__((section("._user_heap.__at_GUI_EXTBUFADD")));
*/
static U32 aMemory[GUI_NUMBYTES / 4] __attribute__((section("._user_heap")));;// 32 bit aligned memory area
#endif
#else
static U32 aMemory[GUI_NUMBYTES / 4];// 32 bit aligned memory area
#endif

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   available memory for the GUI.
*/
void GUI_X_Config(void) {
  //
  // Assign memory to emWin
  //
  GUI_ALLOC_AssignMemory(aMemory, GUI_NUMBYTES);
  //
  // Set default font
  //
  GUI_SetDefaultFont(GUI_FONT_24_ASCII);
}

/*************************** End of file ****************************/
