#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"
#include "gpio.h"
#include "timer.h"
#include "spiflash.h"
#include "ssdlcd.h"
#include "uart.h"
#include "GUI.h"
//#include "GUI_AUX.h"
#include "GUI_Display.h"

#include "WM.h"
#include "BUTTON.h"
#include "CHECKBOX.h"
#include "DIALOG.h"
#include "DROPDOWN.h"
#include "EDIT.h"
#include "FRAMEWIN.h"
#include "ICONVIEW.h"
#include "MULTIPAGE.h"
#include "RADIO.h"
#include "SLIDER.h"
#include "TEXT.h"

static WM_HWIN WM_Current; //当前窗口
static WM_HWIN _hWelcome;  //欢迎窗口
static WM_HWIN _hHome;     //主窗口
static WM_HWIN _hSettings; //参数设置窗口

// static WM_HWIN _hDateTime;    //日期时间设置窗口
// static WM_HWIN _hInformation; //产品信息窗口
// static WM_HWIN _hLanguage;    //语言设置窗口
// static WM_HWIN _hHelp;        //帮助窗口

int GUI_SettingMask = 0; //标识哪些位需要进行设置

void Display_Welcome(void);
void Display_Frame(void);
void Display_Settings(void);

/**
  * @brief  串口收到信息刷新窗口
  * @param  None
  * @retval None
  */
void HomeWMRefresh(void)
{
    WM_MESSAGE tempMsg;

    if (WM_Current == _hHome)
    {
        tempMsg.MsgId = WM_USER;
        tempMsg.hWin = _hHome;
        tempMsg.Data.v = DataV_PAINT;

        WM_SendMessage(tempMsg.hWin, &tempMsg);
        // WM_SendMessageNoPara(_hHome, WM_PAINT);
    }
}

/**
  * @brief  切换界面从欢迎界面至主界面
  * @param  None
  * @retval None
  */
void WelcomeToHome(void)
{
    WM_MESSAGE tempMsg;

    if (WM_Current == _hWelcome)
    {
        tempMsg.MsgId = WM_USER;
        tempMsg.hWin = _hWelcome;
        tempMsg.Data.v = DataV_SWITCH;

        WM_SendMessage(tempMsg.hWin, &tempMsg);
    }
}

/**
  * @brief  _hWelcome回调函数
  * @param  None
  * @retval None
  */
static void _cbBkWelcome(WM_MESSAGE *pMsg)
{
    WM_HWIN hItem;
    int WMSizeX;
    // int WMSizeY;
    int NCode;
    // int Id;
    static WM_HTIMER _hTimerWelcomeToHome; //欢迎窗口跳转至主窗口

    switch (pMsg->MsgId)
    {
    case WM_CREATE:
        hItem = pMsg->hWin;
        _hTimerWelcomeToHome = WM_CreateTimer(hItem, ID_TIMER_WelcomeToHome, 45000, 0); //增加定时器，35s后没有读取控制板数据进入主界面
        break;
    case WM_DELETE:
        // WM_DeleteTimer(_hTimerWelcomeToHome); //删除GUI定时器 窗口删除时会自动删除定时器
        break;
    case WM_PAINT:
        hItem = pMsg->hWin;
        WMSizeX = WM_GetWindowSizeX(hItem);
        // WMSizeY = WM_GetWindowSizeY(hItem);
        GUI_SetBkColor(GUI_BLACK);
        GUI_Clear();
        GUI_DrawBitmap(&bmHome_Logo_200x154, 300, 130);
        // GUI_SetColor(GUI_BLACK);
        // GUI_FillRect(0, 0, WMSizeX - 1, WMSizeY - 1);
        GUI_SetColor(GUI_BLUE);
        GUI_SetTextMode(GUI_TEXTMODE_TRANS); //设置字体显示模式，非透明GUI_TEXTMODE_NORMAL 透明GUI_TEXTMODE_TRANS
        // GUI_SetTextStyle(GUI_TS_UNDERLINE);
        // GUI_SetTextStyle(GUI_TS_NORMAL);
        // GUI_SetTextAlign(GUI_TA_HCENTER); //居中对齐显示
        switch (Flash_Language)
        {
        case 1: //中文
            GUI_SetFont(&GUI_FontSongType32);
            GUI_DispStringHCenterAt("北京纽比特科技有限公司", WMSizeX / 2, 300); //WMSizeY / 2 - GUI_GetFontSizeY()
            break;
        default:
            GUI_SetFont(GUI_FONT_32B_ASCII);
            GUI_DispStringHCenterAt("Beijing NBT Technology Co.,Ltd", WMSizeX / 2, 300);
            break;
        }
        break;
    case WM_NOTIFY_PARENT:
        // Id = WM_GetId(pMsg->hWinSrc);
        // NCode = pMsg->Data.v;
        // switch (Id)
        // {
        // }
        break;
    case WM_TIMER:
        hItem = pMsg->hWin;
        NCode = pMsg->Data.v;
        if (NCode == _hTimerWelcomeToHome)
        {
            GUI_InitComSend();
            WM_DeleteWindow(hItem); //删除窗口
            Display_Frame();        //建立主窗口
        }
        break;
    // case WM_PID_STATE_CHANGED:
    //     hItem = pMsg->hWin;
    //     WM_DeleteWindow(hItem); //删除窗口
    //     Display_Frame();        //建立主窗口
    //     break;
    case WM_USER:
        hItem = pMsg->hWin;
        NCode = pMsg->Data.v;
        if (NCode == DataV_SWITCH)
        {
            WM_DeleteWindow(hItem); //删除窗口
            Display_Frame();        //建立主窗口
        }
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

/**
  * @brief  Home主窗口的回调函数
  * @param  None
  * @retval None
  */
static void _cbBkHome(WM_MESSAGE *pMsg)
{
    WM_HWIN hItem;
    int WMSizeX;
    //  int WMSizeY;
    int NCode;
    int Id;
    static BUTTON_Handle hButton_Menu, hButton_Shutter, hButton_Lamp;
    static BUTTON_Handle hButton_Filter[8];
    static SCROLLBAR_Handle hScrollbar_lightSet;
    static int tempLightIntensity;

    switch (pMsg->MsgId)
    {
    case WM_CREATE:
        tempLightIntensity = UART_LightIntensity;
        hItem = pMsg->hWin;
        hButton_Menu = BUTTON_CreateEx(20, 425, 160, 40, hItem, WM_CF_SHOW, 0, ID_BUTTON_Menu);
        hButton_Shutter = BUTTON_CreateEx(420, 425, 160, 40, hItem, WM_CF_SHOW, 0, ID_BUTTON_Shutter);
        hButton_Lamp = BUTTON_CreateEx(620, 425, 160, 40, hItem, WM_CF_SHOW, 0, ID_BUTTON_Lamp);
        // emWin5.28及其以上版本，XXXX_SetBkColor()函数是不起作用的，因为此函数是针对于旧版本修改控件背景色。要通过修改皮肤色的方法修改控件背景色。
        // BUTTON_SetBkColor(hButton_Menu, BUTTON_CI_UNPRESSED, GUI_RED);
        for (uint8_t i = 0; i < 8; i++)
        {
            if (Flash_DeviceType)
                hButton_Filter[i] = BUTTON_CreateEx(20 + 100 * i, 130, 60, 40, hItem, WM_CF_SHOW, 0, ID_BUTTON_Filter1 + i);
            else
                hButton_Filter[i] = BUTTON_CreateEx(20 + 100 * i, 130, 60, 40, hItem, WM_CF_HIDE, 0, ID_BUTTON_Filter1 + i);

            BUTTON_SetFont(hButton_Filter[i], GUI_FONT_24_ASCII);
            char tempchar[2] = {"1"};
            tempchar[0] = 0x31 + i;
            BUTTON_SetText(hButton_Filter[i], tempchar);
        }
        // BUTTON_SetTextAlign(hButton_Menu, GUI_TA_HCENTER | GUI_TA_VCENTER);    //文字居中对齐
        // BUTTON_SetTextAlign(hButton_Shutter, GUI_TA_HCENTER | GUI_TA_VCENTER); //文字居中对齐
        // BUTTON_SetTextAlign(hButton_Lamp, GUI_TA_HCENTER | GUI_TA_VCENTER);    //文字居中对齐
        switch (Flash_Language)
        {
        case 1:
            BUTTON_SetFont(hButton_Menu, &GUI_FontSongType24);
            BUTTON_SetFont(hButton_Shutter, &GUI_FontSongType24);
            BUTTON_SetFont(hButton_Lamp, &GUI_FontSongType24);
            BUTTON_SetText(hButton_Menu, "菜单");
            BUTTON_SetText(hButton_Shutter, "快门");
            BUTTON_SetText(hButton_Lamp, "光源");
            break;
        default:
            BUTTON_SetFont(hButton_Menu, GUI_FONT_24_ASCII);
            BUTTON_SetFont(hButton_Shutter, GUI_FONT_24_ASCII);
            BUTTON_SetFont(hButton_Lamp, GUI_FONT_24_ASCII);
            BUTTON_SetText(hButton_Menu, "Menu");
            BUTTON_SetText(hButton_Shutter, "Shutter");
            BUTTON_SetText(hButton_Lamp, "Lamp");
            break;
        }
        // BUTTON_SetBitmapEx(hButton_Menu, BUTTON_BI_UNPRESSED, &bmHome_Lamp, 0, 0);                               //设置显示指定按钮时要使用的位图
        hScrollbar_lightSet = SCROLLBAR_CreateEx(50, 300, 700, 50, hItem, WM_CF_SHOW, 0, ID_SCROLLBAR_lightSet); //180~580
        SCROLLBAR_SetNumItems(hScrollbar_lightSet, 110);                                                         //设置110，最大值是100，可能是滚动条按钮占用了10个Items
        SCROLLBAR_SetValue(hScrollbar_lightSet, tempLightIntensity);
        break;
    case WM_DELETE:
        break;
    case WM_PAINT:
        hItem = pMsg->hWin;
        WMSizeX = WM_GetWindowSizeX(hItem);
        // WMSizeY = WM_GetWindowSizeY(hItem);
        GUI_SetBkColor(GUI_BLACK);
        GUI_Clear();

        switch (Flash_Language)
        {
        case 1: //中文
            BUTTON_SetFont(hButton_Menu, &GUI_FontSongType24);
            BUTTON_SetFont(hButton_Shutter, &GUI_FontSongType24);
            BUTTON_SetFont(hButton_Lamp, &GUI_FontSongType24);
            BUTTON_SetText(hButton_Menu, "菜单");
            BUTTON_SetText(hButton_Shutter, "快门");
            BUTTON_SetText(hButton_Lamp, "光源");
            break;
        default:
            BUTTON_SetFont(hButton_Menu, GUI_FONT_24_ASCII);
            BUTTON_SetFont(hButton_Shutter, GUI_FONT_24_ASCII);
            BUTTON_SetFont(hButton_Lamp, GUI_FONT_24_ASCII);
            BUTTON_SetText(hButton_Menu, "Menu");
            BUTTON_SetText(hButton_Shutter, "Shutter");
            BUTTON_SetText(hButton_Lamp, "Lamp");
            break;
        }

        GUI_SetColor(GUI_WHITE);
        GUI_SetTextMode(GUI_TEXTMODE_TRANS); //设置字体显示模式，非透明GUI_TEXTMODE_NORMAL 透明GUI_TEXTMODE_TRANS
        // GUI_SetTextStyle(GUI_TS_UNDERLINE);
        // GUI_SetTextStyle(GUI_TS_NORMAL);
        // GUI_SetTextAlign(GUI_TA_HCENTER); //居中对齐显示
        GUI_SetFont(GUI_FONT_24_ASCII);
        GUI_DispStringAt("SN: ", 20, 20);
        GUI_DispString(Flash_DeviceSN);
        switch (Flash_Language)
        {
        case 1: //中文
            GUI_SetFont(&GUI_FontSongType32);
            if (SystemState.State_Main == SYSTEM_INIT)
            {
                if (SystemState.State_Minor == INIT_IDLE)
                {
                    GUI_DispStringAtCEOL("设备初始化失败", 200, 20);
                }
                else
                {
                    GUI_DispStringAtCEOL("设备正在初始化...", 200, 20);
                }
            }
            else if (SystemState.State_Main == SYSTEM_WORK)
            {
                if (SystemState.State_Minor == WORK_LIGHT_ON)
                {
                    GUI_DispStringAtCEOL("正在打开光源...", 200, 20);
                }
                else if (SystemState.State_Minor == WORK_LIGHT_OFF)
                {
                    GUI_DispStringAtCEOL("正在关闭光源...", 200, 20);
                }
                else if (SystemState.State_Minor == WORK_FILTER)
                {
                    GUI_DispStringAtCEOL("正在切换滤光片...", 200, 20);
                }
            }
            else if (SystemState.State_Main == SYSTEM_IDLE)
            {
                if (UART_SystemErrorStatus)
                {
                    GUI_DispStringAtCEOL("设备故障: 故障码0x", 200, 20);
                    GUI_DispHex(UART_SystemErrorStatus, sizeof(UART_SystemErrorStatus) * 2);
                }
            }
            break;
        default:
            GUI_SetFont(GUI_FONT_32_ASCII);
            if (SystemState.State_Main == SYSTEM_INIT)
            {
                if (SystemState.State_Minor == INIT_IDLE)
                {
                    GUI_DispStringAtCEOL("The device initialization failed", 200, 20);
                }
                else
                {
                    GUI_DispStringAtCEOL("The device is initializing...", 200, 20);
                }
            }
            else if (SystemState.State_Main == SYSTEM_WORK)
            {
                if (SystemState.State_Minor == WORK_LIGHT_ON)
                {
                    GUI_DispStringAtCEOL("The light is turning on...", 200, 20);
                }
                else if (SystemState.State_Minor == WORK_LIGHT_OFF)
                {
                    GUI_DispStringAtCEOL("The light is being turned off....", 200, 20);
                }
                else if (SystemState.State_Minor == WORK_FILTER)
                {
                    GUI_DispStringAtCEOL("Switching filters...", 200, 20);
                }
            }
            else if (SystemState.State_Main == SYSTEM_IDLE)
            {
                if (UART_SystemErrorStatus)
                {
                    GUI_DispStringAtCEOL("Trouble code:0x", 200, 20);
                    GUI_DispHex(UART_SystemErrorStatus, sizeof(UART_SystemErrorStatus) * 2);
                }
            }
            break;
        }

        if (Flash_DeviceType)
        {
            for (uint8_t i = 0; i < 8; i++)
            {
                WM_ShowWindow(hButton_Filter[i]); //显示按钮
            }
            switch (Flash_Language)
            {
            case 1: //中文
                GUI_SetFont(&GUI_FontSongType24);
                GUI_DispStringAtCEOL("滤光片: ", 20, 90);
                break;
            default:
                GUI_SetFont(GUI_FONT_24_ASCII);
                GUI_DispStringAtCEOL("Filter: ", 20, 90);
                break;
            }
            GUI_SetFont(GUI_FONT_24_ASCII);
            GUI_SetColor(GUI_ORANGE);
            GUI_DispDecMin(UART_StepPositionCurrent);
            GUI_DispString(":");
            GUI_DispString(UART_FilterDescrip[UART_StepPositionCurrent]);
        }
        else
        {
            for (uint8_t i = 0; i < 8; i++)
            {
                WM_HideWindow(hButton_Filter[i]); //隐藏按钮
            }
        }

        switch (Flash_Language)
        {
        case 1: //中文
            GUI_SetFont(&GUI_FontSongType24);
            GUI_DispStringAtCEOL("光照强度: ", 20, 230);
            break;
        default:
            GUI_SetFont(GUI_FONT_24_ASCII);
            GUI_DispStringAtCEOL("Light Intensity: ", 20, 230);
            break;
        }
        GUI_SetFont(GUI_FONT_24_ASCII);
        GUI_SetColor(GUI_ORANGE);
        GUI_DispDecMin(UART_LightIntensity);
        GUI_DispString("%");
        GUI_SetColor(GUI_WHITE);
        GUI_DispStringHCenterAt("0", 75, 265);
        GUI_DispStringHCenterAt("100", 725, 265);

        switch (Flash_Language)
        {
        case 1: //中文
            GUI_SetFont(&GUI_FontSongType24);
            GUI_DispStringAtCEOL("已使用: ", 70, 390);
            break;
        default:
            GUI_SetFont(GUI_FONT_24_ASCII);
            GUI_DispStringAtCEOL("Has been used: ", 70, 390);
            break;
        }
        GUI_DispDecMin(UART_LightTimeHour);
        GUI_DispString("h");
        GUI_DispDecMin(UART_LightTimeMinu);
        GUI_DispString("m");
        if (UART_LightErrStatus)
        {
            GUI_DispStringHCenterAt("ERROR", 500, 390);
        }
        else
        {
            if (UART_LightStepStatus == 4)
            {
                GUI_DispStringHCenterAt("ON", 500, 390);
            }
            else
            {
                GUI_DispStringHCenterAt("OFF", 500, 390);
            }
        }
        if (UART_ShutErrStatus == 1)
        {
            GUI_DispStringHCenterAt("ERROR", 700, 390);
        }
        else
        {
            if (UART_ShutOnOff == 1)
            {
                GUI_DispStringHCenterAt("ON", 700, 390);
            }
            else
            {
                GUI_DispStringHCenterAt("ON", 700, 390);
            }
        }

        GUI_DrawBitmap(&bmHome_Logo_100x77, WMSizeX - 110, 10);
        break;
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id)
        {
        case ID_BUTTON_Menu:
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                Display_Settings();
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
            // USER START (Optionally insert additional code for further Ids)
            // USER END
        case ID_BUTTON_Shutter:
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                GUI_WorkShutterComSend();
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
            // USER START (Optionally insert additional code for further Ids)
            // USER END
        case ID_BUTTON_Lamp:
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                GUI_WorkLightComSend();
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
            // USER START (Optionally insert additional code for further Ids)
            // USER END
        case ID_BUTTON_Filter1:
        case ID_BUTTON_Filter2:
        case ID_BUTTON_Filter3:
        case ID_BUTTON_Filter4:
        case ID_BUTTON_Filter5:
        case ID_BUTTON_Filter6:
        case ID_BUTTON_Filter7:
        case ID_BUTTON_Filter8:
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                if (Id >= ID_BUTTON_Filter1)
                {
                    int tempPosition = Id - ID_BUTTON_Filter1 + 1;
                    // if (UART_StepRUNStatus == 0 && tempPosition != UART_StepPositionCurrent)
                    {
                        UART_StepPositionTarget = tempPosition;
                        GUI_WorkFilterComSend();
                    }
                }
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
            // USER START (Optionally insert additional code for further Ids)
            // USER END
        case ID_SCROLLBAR_lightSet:
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_VALUE_CHANGED:
                // USER START (Optionally insert code for reacting on notification message)
                tempLightIntensity = SCROLLBAR_GetValue(pMsg->hWinSrc);
                if (tempLightIntensity != UART_LightIntensity)
                {
                    UART_LightIntensity = tempLightIntensity;
                    GUI_WorkLightInteComSend();
                }
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
            // USER START (Optionally insert additional code for further Ids)
            // USER END
        }
        break;
    // case WM_PID_STATE_CHANGED:
    //     hItem = pMsg->hWin;
    //     break;
    case WM_USER:
        hItem = pMsg->hWin;
        NCode = pMsg->Data.v;
        if (NCode == DataV_PAINT)
        {
            WM_InvalidateWindow(hItem); //使窗口的指定矩形区域无效。
        }
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

/**
  * @brief  系统设置窗口的回调函数
  * @param  None
  * @retval None
  */
static void _cbBkSystemSettings(WM_MESSAGE *pMsg)
{
    WM_HWIN hItem;
    // int WMSizeX;
    // int WMSizeY;
    int NCode;
    int Id;
    static RADIO_Handle hRadio_Language, hRadio_Device;
    static int tempLanguage, tempDevice;

    switch (pMsg->MsgId)
    {
    case WM_CREATE:
        tempLanguage = Flash_Language;
        tempDevice = Flash_DeviceType;
        hItem = pMsg->hWin;
        hRadio_Language = RADIO_CreateEx(20, 70, 250, 100, hItem, WM_CF_SHOW, 0, ID_RADIO_Language, 2, 50);
        // hRadio_Device = RADIO_CreateEx(350, 70, 250, 100, hItem, WM_CF_SHOW, 0, ID_RADIO_Device, 2, 50);
        RADIO_SetTextColor(hRadio_Language, GUI_WHITE);
        // RADIO_SetTextColor(hRadio_Device, GUI_WHITE);
        switch (Flash_Language)
        {
        case 1:
            RADIO_SetFont(hRadio_Language, &GUI_FontSongType24);
            RADIO_SetText(hRadio_Language, "英文", 0);
            RADIO_SetText(hRadio_Language, "中文", 1);
            RADIO_SetValue(hRadio_Language, 1);

            // RADIO_SetFont(hRadio_Device, &GUI_FontSongType24);
            // RADIO_SetText(hRadio_Device, "设备型号303", 0);
            // RADIO_SetText(hRadio_Device, "设备型号305", 1);
            break;
        default:
            RADIO_SetFont(hRadio_Language, GUI_FONT_24_ASCII);
            RADIO_SetText(hRadio_Language, "English", 0);
            RADIO_SetText(hRadio_Language, "Chinese", 1);

            // RADIO_SetFont(hRadio_Device, GUI_FONT_24_ASCII);
            // RADIO_SetText(hRadio_Device, "Device typ 303", 0);
            // RADIO_SetText(hRadio_Device, "Device typ 305", 1);
            break;
        }
        RADIO_SetValue(hRadio_Language, tempLanguage);
        // RADIO_SetValue(hRadio_Device, tempDevice);
        break;
    case WM_DELETE:
        break;
    case WM_PAINT:
        hItem = pMsg->hWin;
        // WMSizeX = WM_GetWindowSizeX(hItem);
        // WMSizeY = WM_GetWindowSizeY(hItem);
        GUI_SetBkColor(GUI_BLACK);
        GUI_Clear();
        GUI_SetColor(GUI_WHITE);
        GUI_SetTextMode(GUI_TEXTMODE_TRANS); //设置字体显示模式，非透明GUI_TEXTMODE_NORMAL 透明GUI_TEXTMODE_TRANS
        // GUI_SetTextStyle(GUI_TS_UNDERLINE);
        // GUI_SetTextStyle(GUI_TS_NORMAL);
        // GUI_SetTextAlign(GUI_TA_HCENTER); //居中对齐显示
        switch (Flash_Language)
        {
        case 1: //中文
            GUI_SetFont(&GUI_FontSongType24);
            GUI_DispStringAtCEOL("请选择语言:", 20, 20);
            GUI_DispStringAtCEOL("请选择设备型号:", 350, 20);
            break;
        default:
            GUI_SetFont(GUI_FONT_24_ASCII);
            GUI_DispStringAtCEOL("Please select language:", 20, 20);
            GUI_DispStringAtCEOL("Please select device typ:", 350, 20);
            break;
        }
        break;
    case WM_NOTIFY_PARENT:
        hItem = pMsg->hWin;
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id)
        {
        case ID_RADIO_Language:
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_VALUE_CHANGED:
                // USER START (Optionally insert code for reacting on notification message)
                tempLanguage = RADIO_GetValue(pMsg->hWinSrc);
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
        case ID_RADIO_Device:
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_VALUE_CHANGED:
                // USER START (Optionally insert code for reacting on notification message)
                tempDevice = RADIO_GetValue(pMsg->hWinSrc);
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
            // USER START (Optionally insert additional code for further Ids)
            // USER END
        }
        break;
    // case WM_PID_STATE_CHANGED:
    //     hItem = pMsg->hWin;
    //     break;
    case WM_USER:
        hItem = pMsg->hWin;
        NCode = pMsg->Data.v;
        if (NCode == DataV_ENTER)
        {
            if (tempLanguage != Flash_Language || tempDevice != Flash_DeviceType)
            {
                Flash_Language = tempLanguage;
                Flash_DeviceType = tempDevice;
                Flash_DataRefresh = 1;
            }
        }
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

/**
  * @brief  快门设置窗口的回调函数
  * @param  None
  * @retval None
  */
static void _cbBkShutterSettings(WM_MESSAGE *pMsg)
{
    WM_HWIN hItem;
    // int WMSizeX;
    // int WMSizeY;
    int NCode;
    int Id;
    static RADIO_Handle hRadio_Shutter;
    static DROPDOWN_Handle hDropdown_Hours, hDropdown_Minutes;
    static int tempShutterAMStatus, tempShutterTime, tempShutterTimeHours, tempShutterTimeMinutes;
    char BufTemp[5] = {0};

    switch (pMsg->MsgId)
    {
    case WM_CREATE:
        tempShutterAMStatus = UART_ShutAMAU;
        tempShutterTime = UART_ShutterTime;
        tempShutterTimeHours = UART_ShutterTime / 60;
        tempShutterTimeMinutes = UART_ShutterTime % 60;
        hItem = pMsg->hWin;
        hRadio_Shutter = RADIO_CreateEx(20, 70, 250, 100, hItem, WM_CF_SHOW, 0, ID_RADIO_Shutter, 2, 50);
        // RADIO_SetFocusColor(hRadio_Language, GUI_LIGHTRED);
        RADIO_SetTextColor(hRadio_Shutter, GUI_WHITE);
        switch (Flash_Language)
        {
        case 1:
            RADIO_SetFont(hRadio_Shutter, &GUI_FontSongType24);
            RADIO_SetText(hRadio_Shutter, "快门手动", 0);
            RADIO_SetText(hRadio_Shutter, "快门自动", 1);
            break;
        default:
            RADIO_SetFont(hRadio_Shutter, GUI_FONT_24_ASCII);
            RADIO_SetText(hRadio_Shutter, "Shutter manual", 0);
            RADIO_SetText(hRadio_Shutter, "shutter automatic", 1);
            break;
        }
        RADIO_SetValue(hRadio_Shutter, tempShutterAMStatus);
        hDropdown_Hours = DROPDOWN_CreateEx(350, 60, 100, 250, hItem, WM_CF_SHOW, DROPDOWN_CF_AUTOSCROLLBAR, ID_DROPDOWN_Hours);
        hDropdown_Minutes = DROPDOWN_CreateEx(500, 60, 100, 250, hItem, WM_CF_SHOW, DROPDOWN_CF_AUTOSCROLLBAR, ID_DROPDOWN_Minutes);
        DROPDOWN_SetItemSpacing(hDropdown_Hours, 10);                              //设置下拉列表中各项目之间的间距
        DROPDOWN_SetItemSpacing(hDropdown_Minutes, 10);                            //设置下拉列表中各项目之间的间距
        DROPDOWN_SetScrollbarWidth(hDropdown_Hours, 40);                           //设置滚动条的宽度
        DROPDOWN_SetScrollbarWidth(hDropdown_Minutes, 40);                         //设置滚动条的宽度
        DROPDOWN_SetTextAlign(hDropdown_Hours, GUI_TA_HCENTER | GUI_TA_VCENTER);   //设置用于显示关闭状态下拉文本的对齐方式
        DROPDOWN_SetTextAlign(hDropdown_Minutes, GUI_TA_HCENTER | GUI_TA_VCENTER); //设置用于显示关闭状态下拉文本的对齐方式
        DROPDOWN_SetTextHeight(hDropdown_Hours, 28);                               //设置用于显示关闭状态下拉文本的矩形的高度
        DROPDOWN_SetTextHeight(hDropdown_Minutes, 28);                             //设置用于显示关闭状态下拉文本的矩形的高度
        DROPDOWN_SetFont(hDropdown_Hours, GUI_FONT_24_ASCII);
        DROPDOWN_SetFont(hDropdown_Minutes, GUI_FONT_24_ASCII);
        for (uint8_t i = 0; i < 60; i++)
        {
            memset(BufTemp, 0, sizeof(BufTemp));
            sprintf(BufTemp, "%d", i);
            DROPDOWN_AddString(hDropdown_Hours, BufTemp);   //增加字符串
            DROPDOWN_AddString(hDropdown_Minutes, BufTemp); //增加字符串
        }
        DROPDOWN_SetSel(hDropdown_Hours, tempShutterTimeHours);
        DROPDOWN_SetSel(hDropdown_Minutes, tempShutterTimeMinutes);
        break;
    case WM_DELETE:
        break;
    case WM_PAINT:
        hItem = pMsg->hWin;
        // WMSizeX = WM_GetWindowSizeX(hItem);
        // WMSizeY = WM_GetWindowSizeY(hItem);
        GUI_SetBkColor(GUI_BLACK);
        GUI_Clear();
        GUI_SetColor(GUI_WHITE);
        GUI_SetTextMode(GUI_TEXTMODE_TRANS); //设置字体显示模式，非透明GUI_TEXTMODE_NORMAL 透明GUI_TEXTMODE_TRANS
        // GUI_SetTextStyle(GUI_TS_UNDERLINE);
        // GUI_SetTextStyle(GUI_TS_NORMAL);
        // GUI_SetTextAlign(GUI_TA_HCENTER); //居中对齐显示
        switch (Flash_Language)
        {
        case 1: //中文
            GUI_SetFont(&GUI_FontSongType24);
            GUI_DispStringAtCEOL("请选择快门控制方式:", 20, 20);
            GUI_DispStringAtCEOL("请选择快门间隔时间:", 350, 20); //最小单位间隔1s
            break;
        default:
            GUI_SetFont(GUI_FONT_24_ASCII);
            GUI_DispStringAtCEOL("Please select shutter:", 20, 20);
            GUI_DispStringAtCEOL("Please select Timer:", 350, 20);
            break;
        }
        GUI_SetFont(GUI_FONT_24_ASCII);
        GUI_DispStringAt("h", 455, 75);
        GUI_DispStringAt("m", 605, 75);
        break;
    case WM_NOTIFY_PARENT:
        hItem = pMsg->hWin;
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id)
        {
        case ID_RADIO_Shutter:
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_VALUE_CHANGED:
                // USER START (Optionally insert code for reacting on notification message)
                tempShutterAMStatus = RADIO_GetValue(pMsg->hWinSrc);
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
        case ID_DROPDOWN_Hours:
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_SEL_CHANGED:
                // USER START (Optionally insert code for reacting on notification message)
                tempShutterTimeHours = DROPDOWN_GetSel(pMsg->hWinSrc);
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
        case ID_DROPDOWN_Minutes:
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_SEL_CHANGED:
                // USER START (Optionally insert code for reacting on notification message)
                tempShutterTimeMinutes = DROPDOWN_GetSel(pMsg->hWinSrc);
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
            // USER START (Optionally insert additional code for further Ids)
            // USER END
        }
        break;
    // case WM_PID_STATE_CHANGED:
    //   hItem = pMsg->hWin;
    //   break;
    case WM_USER:
        hItem = pMsg->hWin;
        NCode = pMsg->Data.v;
        if (NCode == DataV_ENTER)
        {
            tempShutterTime = tempShutterTimeHours + tempShutterTimeMinutes;
            if (tempShutterTime != UART_ShutterTime)
            {
                UART_ShutterTime = tempShutterTime;
                GUI_SettingMask |= SETTINGS_SHUTTER_TIME;
            }

            UART_ShutAMAU = tempShutterAMStatus;
        }
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

/**
  * @brief  远程设置窗口的回调函数
  * @param  None
  * @retval None
  */
static void _cbBkRemoteSettings(WM_MESSAGE *pMsg)
{
    WM_HWIN hItem;
    // int WMSizeX;
    // int WMSizeY;
    int NCode;
    int Id;
    static RADIO_Handle hRadio_UART;
    static DROPDOWN_Handle hDropdown_Address;
    static int tempRemoteStatus, tempRemoteAddress;
    char BufTemp[5] = {0};

    switch (pMsg->MsgId)
    {
    case WM_CREATE:
        tempRemoteStatus = UART_RemoteStatus;
        tempRemoteAddress = UART_RemoteAddr;
        hItem = pMsg->hWin;
        hRadio_UART = RADIO_CreateEx(20, 70, 250, 100, hItem, WM_CF_SHOW, 0, ID_RADIO_UART, 2, 50);
        // RADIO_SetFocusColor(hRadio_Language, GUI_LIGHTRED);
        RADIO_SetTextColor(hRadio_UART, GUI_WHITE);
        switch (Flash_Language)
        {
        case 1:
            RADIO_SetFont(hRadio_UART, &GUI_FontSongType24);
            RADIO_SetText(hRadio_UART, "设备本地控制", 0);
            RADIO_SetText(hRadio_UART, "设备远程控制", 1);
            break;
        default:
            RADIO_SetFont(hRadio_UART, GUI_FONT_24_ASCII);
            RADIO_SetText(hRadio_UART, "Device local control", 0);
            RADIO_SetText(hRadio_UART, "Device remote control", 1);
            break;
        }
        RADIO_SetValue(hRadio_UART, tempRemoteStatus);
        hDropdown_Address = DROPDOWN_CreateEx(350, 60, 100, 250, hItem, WM_CF_SHOW, DROPDOWN_CF_AUTOSCROLLBAR, ID_DROPDOWN_Address);
        DROPDOWN_SetItemSpacing(hDropdown_Address, 10);                            //设置下拉列表中各项目之间的间距
        DROPDOWN_SetScrollbarWidth(hDropdown_Address, 40);                         //设置滚动条的宽度
        DROPDOWN_SetTextAlign(hDropdown_Address, GUI_TA_HCENTER | GUI_TA_VCENTER); //设置用于显示关闭状态下拉文本的对齐方式
        DROPDOWN_SetTextHeight(hDropdown_Address, 28);                             //设置用于显示关闭状态下拉文本的矩形的高度
        DROPDOWN_SetFont(hDropdown_Address, GUI_FONT_24_ASCII);
        for (uint8_t i = 0; i < 60; i++)
        {
            memset(BufTemp, 0, sizeof(BufTemp));
            sprintf(BufTemp, "%d", i);
            DROPDOWN_AddString(hDropdown_Address, BufTemp); //增加字符串
        }
        DROPDOWN_SetSel(hDropdown_Address, tempRemoteAddress);
        break;
    case WM_DELETE:
        break;
    case WM_PAINT:
        hItem = pMsg->hWin;
        // WMSizeX = WM_GetWindowSizeX(hItem);
        // WMSizeY = WM_GetWindowSizeY(hItem);
        GUI_SetBkColor(GUI_BLACK);
        GUI_Clear();
        GUI_SetColor(GUI_WHITE);
        GUI_SetTextMode(GUI_TEXTMODE_TRANS); //设置字体显示模式，非透明GUI_TEXTMODE_NORMAL 透明GUI_TEXTMODE_TRANS
        // GUI_SetTextStyle(GUI_TS_UNDERLINE);
        // GUI_SetTextStyle(GUI_TS_NORMAL);
        // GUI_SetTextAlign(GUI_TA_HCENTER); //居中对齐显示
        switch (Flash_Language)
        {
        case 1: //中文
            GUI_SetFont(&GUI_FontSongType24);
            GUI_DispStringAtCEOL("请选择设备控制方式:", 20, 20);
            GUI_DispStringAtCEOL("请选择设备地址:", 350, 20);
            break;
        default:
            GUI_SetFont(GUI_FONT_24_ASCII);
            GUI_DispStringAtCEOL("Please select control mode:", 20, 20);
            GUI_DispStringAtCEOL("Please select remote Address:", 350, 20);
            break;
        }
        break;
    case WM_NOTIFY_PARENT:
        hItem = pMsg->hWin;
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id)
        {
        case ID_RADIO_UART:
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_VALUE_CHANGED:
                // USER START (Optionally insert code for reacting on notification message)
                tempRemoteStatus = RADIO_GetValue(pMsg->hWinSrc);
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
        case ID_DROPDOWN_Address:
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_SEL_CHANGED:
                // USER START (Optionally insert code for reacting on notification message)
                tempRemoteAddress = DROPDOWN_GetSel(pMsg->hWinSrc);
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
            // USER START (Optionally insert additional code for further Ids)
            // USER END
        }
        break;
    // case WM_PID_STATE_CHANGED:
    //     hItem = pMsg->hWin;
    //     break;
    case WM_USER:
        hItem = pMsg->hWin;
        NCode = pMsg->Data.v;
        if (NCode == DataV_ENTER)
        {
            if (tempRemoteStatus != UART_RemoteStatus || tempRemoteAddress != UART_RemoteAddr)
            {
                UART_RemoteStatus = tempRemoteStatus;
                UART_RemoteAddr = tempRemoteAddress;
                GUI_SettingMask |= SETTINGS_REMOTE_STATE;
            }
        }
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

/**
  * @brief  故障报警窗口的回调函数
  * @param  None
  * @retval None
  */
static void _cbBkAlarmSettings(WM_MESSAGE *pMsg)
{
    // WM_HWIN hItem;
    // int WMSizeX;
    // int WMSizeY;
    // int NCode;
    // int Id;

    switch (pMsg->MsgId)
    {
    case WM_CREATE:
        break;
    case WM_DELETE:
        break;
    case WM_PAINT:
        // hItem = pMsg->hWin;
        // WMSizeX = WM_GetWindowSizeX(hItem);
        // WMSizeY = WM_GetWindowSizeY(hItem);
        GUI_SetBkColor(GUI_BLACK);
        GUI_Clear();
        GUI_SetColor(GUI_RED);               //GUI_WHITE
        GUI_SetTextMode(GUI_TEXTMODE_TRANS); //设置字体显示模式，非透明GUI_TEXTMODE_NORMAL 透明GUI_TEXTMODE_TRANS
        // GUI_SetTextStyle(GUI_TS_UNDERLINE);
        // GUI_SetTextStyle(GUI_TS_NORMAL);
        // GUI_SetTextAlign(GUI_TA_HCENTER); //居中对齐显示
        switch (Flash_Language)
        {
        case 1: //中文
            GUI_SetFont(&GUI_FontSongType24);
            if (UART_SystemErrorStatus)
            {
                GUI_DispStringAtCEOL("故障码:0x", 20, 20);
                GUI_DispHex(UART_SystemErrorStatus, sizeof(UART_SystemErrorStatus) * 2);
                GUI_DispNextLine();
                if (UART_SystemErrorStatus & 0x01)
                {
                    GUI_DispCEOL();
                    GUI_DispString("滤光片定位故障.");
                }
                if (UART_SystemErrorStatus & 0x02)
                {
                    GUI_DispCEOL();
                    GUI_DispString("快门开关故障.");
                }
                if (UART_SystemErrorStatus & 0x04)
                {
                    GUI_DispCEOL();
                    GUI_DispString("光源继电器故障.");
                }
                if (UART_SystemErrorStatus & 0x01)
                {
                    GUI_DispCEOL();
                    GUI_DispString("风扇启停故障.");
                }
                if (UART_SystemErrorStatus & 0x01)
                {
                    GUI_DispCEOL();
                    GUI_DispString("设备机箱门未关闭.");
                }
                if (UART_SystemErrorStatus & 0x01)
                {
                    GUI_DispCEOL();
                    GUI_DispString("设备超温过高.");
                }
            }
            else
            {
                GUI_DispStringAtCEOL("设备正常,未出现任何故障.", 20, 20);
            }
            break;
        default:
            GUI_SetFont(GUI_FONT_24_ASCII);
            if (UART_SystemErrorStatus)
            {
                GUI_DispStringAtCEOL("Trouble code:0x", 20, 20);
                GUI_DispHex(UART_SystemErrorStatus, sizeof(UART_SystemErrorStatus) * 2);
                GUI_DispNextLine();
                if (UART_SystemErrorStatus & 0x01)
                {
                    GUI_DispCEOL();
                    GUI_DispString("The initial position of the filter fails.");
                }
                if (UART_SystemErrorStatus & 0x02)
                {
                    GUI_DispCEOL();
                    GUI_DispString("The shutter switch failure.");
                }
                if (UART_SystemErrorStatus & 0x04)
                {
                    GUI_DispCEOL();
                    GUI_DispString("The relay for the light failure.");
                }
                if (UART_SystemErrorStatus & 0x01)
                {
                    GUI_DispCEOL();
                    GUI_DispString("The fan start or stop fault.");
                }
                if (UART_SystemErrorStatus & 0x01)
                {
                    GUI_DispCEOL();
                    GUI_DispString("The equipment door is not closed.");
                }
                if (UART_SystemErrorStatus & 0x01)
                {
                    GUI_DispCEOL();
                    GUI_DispString("The equipmen temperature is too high.");
                }
            }
            else
            {
                GUI_DispStringAtCEOL("The equipment is normal, without any fault.", 20, 20);
            }
            break;
        }
        break;
    case WM_NOTIFY_PARENT:
        // hItem = pMsg->hWin;
        // Id = WM_GetId(pMsg->hWinSrc);
        // NCode = pMsg->Data.v;
        // switch (Id)
        // {
        // }
        break;
    // case WM_PID_STATE_CHANGED:
    //     hItem = pMsg->hWin;
    //     break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

/**
  * @brief  设置窗口的回调函数
  * @param  None
  * @retval None
  */
static void _cbBkSettings(WM_MESSAGE *pMsg)
{
    WM_HWIN hItem;
    int WMSizeX;
    // int WMSizeY;
    int NCode;
    int Id;
    static WM_HWIN _HSystemSettings, _HShutterSettings, _HRemoteSettings, _HAlarmSettings; //, _HFlilterSettings
    static BUTTON_Handle hButton_Back, hButton_Enter;
    static MULTIPAGE_Handle hMultipage_Settings;

    switch (pMsg->MsgId)
    {
    case WM_CREATE:
        GUI_SettingMask = 0;
        hItem = pMsg->hWin;
        hMultipage_Settings = MULTIPAGE_CreateEx(20, 20, 660, 380, hItem, WM_CF_SHOW, 0, ID_MULTIPAGE_Settings);
        _HSystemSettings = WM_CreateWindow(0, 0, 660, 380, WM_CF_SHOW | WM_CF_MEMDEV, _cbBkSystemSettings, 0);   //系统设置窗口
        _HShutterSettings = WM_CreateWindow(0, 0, 660, 380, WM_CF_SHOW | WM_CF_MEMDEV, _cbBkShutterSettings, 0); //快门设置窗口
        _HRemoteSettings = WM_CreateWindow(0, 0, 660, 380, WM_CF_SHOW | WM_CF_MEMDEV, _cbBkRemoteSettings, 0);   //通讯设置窗口
        _HAlarmSettings = WM_CreateWindow(0, 0, 660, 380, WM_CF_SHOW | WM_CF_MEMDEV, _cbBkAlarmSettings, 0);     //故障报警窗口
        hButton_Back = BUTTON_CreateEx(420, 425, 160, 40, hItem, WM_CF_SHOW, 0, ID_BUTTON_Back);
        hButton_Enter = BUTTON_CreateEx(620, 425, 160, 40, hItem, WM_CF_SHOW, 0, ID_BUTTON_Enter);
        // BUTTON_SetTextAlign(hButton13, GUI_TA_HCENTER | GUI_TA_VCENTER); //文字居中对齐
        // BUTTON_SetTextAlign(hButton14, GUI_TA_HCENTER | GUI_TA_VCENTER); //文字居中对齐
        switch (Flash_Language)
        {
        case 1:
            MULTIPAGE_SetFont(hMultipage_Settings, &GUI_FontSongType24);
            MULTIPAGE_AddEmptyPage(hMultipage_Settings, _HSystemSettings, "系统设置");
            MULTIPAGE_AddEmptyPage(hMultipage_Settings, _HShutterSettings, "快门设置");
            MULTIPAGE_AddEmptyPage(hMultipage_Settings, _HRemoteSettings, "通讯设置");
            MULTIPAGE_AddEmptyPage(hMultipage_Settings, _HAlarmSettings, "故障报警");
            BUTTON_SetFont(hButton_Back, &GUI_FontSongType24);
            BUTTON_SetFont(hButton_Enter, &GUI_FontSongType24);
            BUTTON_SetText(hButton_Back, "返回");
            BUTTON_SetText(hButton_Enter, "确定");
            break;
        default:
            MULTIPAGE_SetFont(hMultipage_Settings, GUI_FONT_24_ASCII);
            MULTIPAGE_AddEmptyPage(hMultipage_Settings, _HSystemSettings, "System");
            MULTIPAGE_AddEmptyPage(hMultipage_Settings, _HShutterSettings, "Shutter");
            MULTIPAGE_AddEmptyPage(hMultipage_Settings, _HRemoteSettings, "Remote");
            MULTIPAGE_AddEmptyPage(hMultipage_Settings, _HAlarmSettings, "Alarm");
            BUTTON_SetFont(hButton_Back, GUI_FONT_24_ASCII);
            BUTTON_SetFont(hButton_Enter, GUI_FONT_24_ASCII);
            BUTTON_SetText(hButton_Back, "Back");
            BUTTON_SetText(hButton_Enter, "Enter");
            break;
        }
        MULTIPAGE_SelectPage(hMultipage_Settings, 0);
        break;
    case WM_DELETE:
        break;
    case WM_PAINT:
        hItem = pMsg->hWin;
        WMSizeX = WM_GetWindowSizeX(hItem);
        // WMSizeY = WM_GetWindowSizeY(hItem);
        GUI_SetBkColor(GUI_BLACK);
        GUI_Clear();

        // switch (Flash_Language)
        // {
        // case 1:
        //     BUTTON_SetFont(hButton_Back, &GUI_FontSongType25);
        //     BUTTON_SetFont(hButton_Enter, &GUI_FontSongType25);
        //     BUTTON_SetText(hButton_Back, "返回");
        //     BUTTON_SetText(hButton_Enter, "确定");
        //     MULTIPAGE_SetFont(hMultipage_Settings, &GUI_FontSongType25);
        //     MULTIPAGE_SetText(hMultipage_Settings, "系统设置", 0);
        //     MULTIPAGE_SetText(hMultipage_Settings, "快门设置", 1);
        //     MULTIPAGE_SetText(hMultipage_Settings, "通讯设置", 2);
        //     MULTIPAGE_SetText(hMultipage_Settings, "故障报警", 3);
        //     break;
        // default:
        //     BUTTON_SetFont(hButton_Back, GUI_FONT_24_ASCII);
        //     BUTTON_SetFont(hButton_Enter, GUI_FONT_24_ASCII);
        //     BUTTON_SetText(hButton_Back, "Back");
        //     BUTTON_SetText(hButton_Enter, "Enter");
        //     MULTIPAGE_SetFont(hMultipage_Settings, GUI_FONT_24_ASCII);
        //     MULTIPAGE_SetText(hMultipage_Settings, "System", 0);
        //     MULTIPAGE_SetText(hMultipage_Settings, "Shutter", 1);
        //     MULTIPAGE_SetText(hMultipage_Settings, "Remote", 2);
        //     MULTIPAGE_SetText(hMultipage_Settings, "Alarm", 3);
        //     break;
        // }

        GUI_SetColor(GUI_WHITE);
        GUI_SetTextMode(GUI_TEXTMODE_TRANS); //设置字体显示模式，非透明GUI_TEXTMODE_NORMAL 透明GUI_TEXTMODE_TRANS
        // GUI_SetTextStyle(GUI_TS_UNDERLINE);
        // GUI_SetTextStyle(GUI_TS_NORMAL);
        // GUI_SetTextAlign(GUI_TA_HCENTER); //居中对齐显示
        // switch (Flash_Language)
        // {
        // case 1: //中文
        //     GUI_SetFont(&GUI_FontSongType25);
        //     break;
        // default:
        //     GUI_SetFont(GUI_FONT_24_ASCII);
        //     break;
        // }

        GUI_DrawBitmap(&bmHome_Logo_100x77, WMSizeX - 110, 10);
        break;
    case WM_NOTIFY_PARENT:
        hItem = pMsg->hWin;
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id)
        {
        case ID_BUTTON_Back:
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                WM_DeleteWindow(hItem); //删除窗口
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
        case ID_BUTTON_Enter:
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                {
                    WM_MESSAGE tempMsg;
                    tempMsg.MsgId = WM_USER;
                    tempMsg.Data.v = DataV_ENTER;

                    tempMsg.hWin = _HSystemSettings;
                    WM_SendMessage(tempMsg.hWin, &tempMsg);
                    tempMsg.hWin = _HShutterSettings;
                    WM_SendMessage(tempMsg.hWin, &tempMsg);
                    tempMsg.hWin = _HRemoteSettings;
                    WM_SendMessage(tempMsg.hWin, &tempMsg);
                    tempMsg.hWin = _HAlarmSettings;
                    WM_SendMessage(tempMsg.hWin, &tempMsg);

                    if (GUI_SettingMask != 0)
                    {
                        GUI_SettingsComSend(GUI_SettingMask);
                    }
                    WM_DeleteWindow(hItem); //删除窗口
                }
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
            // USER START (Optionally insert additional code for further Ids)
            // USER END
        }
        break;
    // case WM_PID_STATE_CHANGED:
    //     hItem = pMsg->hWin;
    //     break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

/**
  * @brief  GUI初始化设置 皮肤
  * @param  None
  * @retval None
*/
void Display_Initial(void)
{
    // GUI_EnableAlpha(1);                         //使能Alpha混合0禁用，1使能
    GUI_UC_SetEncodeUTF8();                     //为了显示中文
    WIDGET_SetDefaultEffect(&WIDGET_Effect_3D); //设置小工具默认效果设置为3D效果

    BUTTON_SKINFLEX_PROPS Props;
    // BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);       //按钮
    BUTTON_GetSkinFlexProps(&Props, BUTTON_SKINFLEX_PI_ENABLED);
    Props.aColorUpper[0] = GUI_LIGHTBLUE;
    Props.aColorUpper[1] = GUI_BLUE;
    Props.aColorLower[0] = GUI_BLUE;
    Props.aColorLower[1] = GUI_LIGHTBLUE;
    BUTTON_SetSkinFlexProps(&Props, BUTTON_SKINFLEX_PI_ENABLED);
    BUTTON_GetSkinFlexProps(&Props, BUTTON_SKINFLEX_PI_FOCUSED);
    Props.aColorUpper[0] = GUI_LIGHTBLUE;
    Props.aColorUpper[1] = GUI_BLUE;
    Props.aColorLower[0] = GUI_BLUE;
    Props.aColorLower[1] = GUI_LIGHTBLUE;
    BUTTON_SetSkinFlexProps(&Props, BUTTON_SKINFLEX_PI_FOCUSED);
    BUTTON_GetSkinFlexProps(&Props, BUTTON_SKINFLEX_PI_PRESSED);
    Props.aColorUpper[0] = GUI_LIGHTBLUE;
    Props.aColorUpper[1] = GUI_LIGHTBLUE;
    Props.aColorLower[0] = GUI_LIGHTBLUE;
    Props.aColorLower[1] = GUI_LIGHTBLUE;
    BUTTON_SetSkinFlexProps(&Props, BUTTON_SKINFLEX_PI_PRESSED);
    // BUTTON_SetDefaultBkColor(GUI_BLUE, BUTTON_CI_PRESSED);
    // BUTTON_SetDefaultBkColor(GUI_LIGHTBLUE, BUTTON_CI_UNPRESSED);
    // CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);   //复选框
    // DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);   //下拉列表
    // FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);   //框架窗口
    // HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);       //标题
    // MENU_SetDefaultSkin(MENU_SKIN_FLEX);           //菜单
    // MULTIPAGE_SetDefaultSkin(MULTIPAGE_SKIN_FLEX); //多页
    // PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);     //进度条
    // RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);         //单选按钮
    // SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX); //滚动条
    // SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);       //滑块
    // SPINBOX_SetDefaultSkin(SPINBOX_SKIN_FLEX);     //Spinning box
}

/**
  * @brief  GUI欢迎界面初始化
  * @param  None
  * @retval None
  */
void Display_Welcome(void)
{
    _hWelcome = WM_CreateWindow(0, 0, SSD_HOR_RESOLUTION, SSD_VER_RESOLUTION, WM_CF_SHOW | WM_CF_MEMDEV, _cbBkWelcome, 0); //
    WM_Current = _hWelcome;                                                                                                //在WM_CreateWindow后赋值，调用WM_CreateWindow会赋值_hWelcome
                                                                                                                           //  GUI_Exec();
}

/**
  * @brief  GUI主界面初始化
  * @param  None
  * @retval None
  */
void Display_Frame(void)
{
    // _hToFrame = WM_CreateWindow(0, 0, SSD_HOR_RESOLUTION, ToStatusHeight, WM_CF_SHOW | WM_CF_MEMDEV, _cbBkToFrame, 0);                                   //建立框架界面
    // _hBoFrame = WM_CreateWindow(0, SSD_VER_RESOLUTION - BoStatusHeight, SSD_HOR_RESOLUTION, BoStatusHeight, WM_CF_SHOW | WM_CF_MEMDEV, _cbBkBoFrame, 0); //建立框架界面
    _hHome = WM_CreateWindow(0, 0, SSD_HOR_RESOLUTION, SSD_VER_RESOLUTION, WM_CF_SHOW | WM_CF_MEMDEV, _cbBkHome, 0); //
    WM_Current = _hHome;
    // GUI_Exec();
}

/**
  * @brief  GUI设置界面初始化
  * @param  None
  * @retval None
  */
void Display_Settings(void)
{
    _hSettings = WM_CreateWindow(0, 0, SSD_HOR_RESOLUTION, SSD_VER_RESOLUTION, WM_CF_SHOW | WM_CF_MEMDEV, _cbBkSettings, 0); //
    WM_Current = _hSettings;
    // GUI_Exec();
}

/*
GUI_MEMDEV_Handle hMem0,hMem1;

hMem0 = GUI_MEMDEV_Create();
hMem1 = GUI_MEMDEV_Create();

GUI_MEMDEV_Clear(hMem0);
GUI_MEMDEV_Select(hMem0);

GUI_MEMDEV_Clear(hMem1);
GUI_MEMDEV_Select(hMem1);

//GUI_MEMDEV_Select(0);
//GUI_SelectLCD();
GUI_MEMDEV_CopyToLCD(hMem0);

GUI_MEMDEV_FadeInDevices(hMem0, hMem1, 1000);



WM_DisableMemdev()禁止使用存储设备来重绘窗口。
WM_EnableMemdev()启用使用存储设备来重绘窗口。

WM_CreateTimer()创建定时器，其功能是经过指定周期后，向指定窗口发送消息。该定时器与指定窗口相关联。
WM_DeleteTimer()
WM_RestartTimer()

WM_BringToTop()//放置顶层
WM_HideWindow()//隐藏窗口
WM_ShowWindow()//显示窗口
WM_ResizeWindow()//缩放窗口


WIDGET_SetDefaultEffect

MULTIPAGE_SetTabHeight()设置选项卡的高度，MULTIPAGE_SetTabWidth()设置选项卡宽度，MULTIPAGE_SetFont()设置选项卡字体。
*/