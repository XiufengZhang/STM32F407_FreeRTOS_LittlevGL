#ifndef __GUI_DISPLAY_H__
#define __GUI_DISPLAY_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define ID_TIMER_WelcomeToHome (GUI_ID_USER + 0x01)

#define ID_BUTTON_Menu (GUI_ID_USER + 0x20)
#define ID_BUTTON_Shutter (GUI_ID_USER + 0x21)
#define ID_BUTTON_Lamp (GUI_ID_USER + 0x22)
#define ID_BUTTON_Filter1 (GUI_ID_USER + 0x23)
#define ID_BUTTON_Filter2 (GUI_ID_USER + 0x24)
#define ID_BUTTON_Filter3 (GUI_ID_USER + 0x25)
#define ID_BUTTON_Filter4 (GUI_ID_USER + 0x26)
#define ID_BUTTON_Filter5 (GUI_ID_USER + 0x27)
#define ID_BUTTON_Filter6 (GUI_ID_USER + 0x28)
#define ID_BUTTON_Filter7 (GUI_ID_USER + 0x29)
#define ID_BUTTON_Filter8 (GUI_ID_USER + 0x2A)
#define ID_BUTTON_Back (GUI_ID_USER + 0x2B)
#define ID_BUTTON_Enter (GUI_ID_USER + 0x2C)

#define ID_SCROLLBAR_lightSet (GUI_ID_USER + 0x40)

#define ID_RADIO_Language (GUI_ID_USER + 0x60)
#define ID_RADIO_Device (GUI_ID_USER + 0x61)
#define ID_RADIO_Shutter (GUI_ID_USER + 0x62)
#define ID_RADIO_UART (GUI_ID_USER + 0x63)

#define ID_DROPDOWN_Hours (GUI_ID_USER + 0x80)
#define ID_DROPDOWN_Minutes (GUI_ID_USER + 0x81)
#define ID_DROPDOWN_Address (GUI_ID_USER + 0x82)

#define ID_MULTIPAGE_Settings (GUI_ID_USER + 0xA0)

    //定义WM_MESSAGE数据值
    typedef enum
    {
        DataV_PAINT = 0, //刷新界面
        DataV_SWITCH,    //切换界面
        DataV_BACK,      //返回界面
        DataV_ENTER,     //确定界面

        DataV_COUNT
    } MessageDataV_TypeDef;

    extern GUI_BITMAP bmHome_Logo_100x77;
    extern GUI_BITMAP bmHome_Logo_200x154;

    extern GUI_CONST_STORAGE GUI_FONT GUI_FontSongType24;
    extern GUI_CONST_STORAGE GUI_FONT GUI_FontSongType32;

    void Display_Initial(void);
    void Display_Welcome(void);
    void Display_Frame(void);
    void WelcomeToHome(void);
    void HomeWMRefresh(void);

#ifdef __cplusplus
}
#endif

#endif