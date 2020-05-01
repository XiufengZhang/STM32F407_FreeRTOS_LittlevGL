#ifndef __UART_H
#define __UART_H

#ifdef __cplusplus
extern "C"
{
#endif

#define USART1BUFFERNUM 30 //USART1接收发送最大字节数
#define USART2BUFFERNUM 30 //USART2接收发送最大字节数

#define SOFTWARE_VER "1.3.0" //版本号1.2.0

#define USART_END "\r\n"
#define USART_SPACE " "
#define USART_UNDERLIN "_"
#define USART_COLON ":"

#define USART_SYSTEM ":SYSTEM"
#define SYSTEM_VERSION ":VERSION"
#define SYSTEM_STATE ":STATE"

#define USART_LIGHT ":LIGHT"
#define LIGHT_STATE ":STATE"
#define LIGHT_POWER_ON ":POWER_ON"
#define LIGHT_POWER_OFF ":POWER_OFF"
#define LIGHT_INTE ":INTE"
#define LIGHT_INTE_SET ":INTE_SET"
#define LIGHT_INTE_READ ":INTE_READ"
#define LIGHT_TIME ":TIME"
#define LIGHT_TIME_READ ":TIME_READ"

#define LIGHT_TIME_READ ":TIME_READ"

#define USART_FILTER ":FILTER"
#define FILTER_STATE ":STATE"
#define FILTER_POSITION_SET ":POSITION_SET"
#define FILTER_DESCR ":DESCR"
#define FILTER_DESCR_READ ":DESCR_READ"

#define USART_SHUTER ":SHUTER"
#define SHUTER_STATE ":STATE"
#define SHUTER_MA_ON ":MA_ON"
#define SHUTER_MA_OFF ":MA_OFF"
#define SHUTER_AU_ON ":AU_ON"
#define SHUTER_AU_TIME ":AU_TIME"
#define SHUTER_TIME ":TIME"
#define SHUTER_TIME_WRITE ":TIME_WRITE"
#define SHUTER_TIME_READ ":TIME_READ"

#define USART_COMM ":COMM"
#define COMM_STATE ":STATE"
#define COMM_SET ":SET"

#define ORDER_COUNT 2 //发送指令2次未返回数据，超时

#define EXPIRE_ORDERTIME 100 //发送指令1s后超时定时器

    // typedef void(*TimerOutHandler_f)(uint8_t main_State, uint8_t minor_State);//函数指针

    typedef union //如果没有typedef就必须用union CharFloat_TypeDef来声明变量
    {
        char c[4];
        uint32_t i;
    } CharUInt_TypeDef;

    typedef union //如果没有typedef就必须用union CharFloat_TypeDef来声明变量
    {
        char c[4];
        int i;
    } CharInt_TypeDef;

    typedef union //如果没有typedef就必须用union CharFloat_TypeDef来声明变量
    {
        char c[4];
        float f;
    } CharFloat_TypeDef;

    //定义状态结构体
    typedef struct
    {
        uint8_t State_Main;
        uint8_t State_Minor;
        uint8_t State_Count;
        uint8_t SettingsMask;
    } SysComState_TypeDef;

    //定义系统状态
    typedef enum
    {
        SYSTEM_INIT = 0, //初始化状态
        SYSTEM_IDLE,
        SYSTEM_SETTINGS,
        SYSTEM_WORK,

        SYSTEM_COUNT
    } SysStatus_TypeDef;

    //定义初始化状态
    typedef enum
    {
        INIT_IDLE = 0,
        INIT_SYS_VERSION,
        INIT_SYS_STATE,
        INIT_LIGHT_STATE,
        INIT_LIGHT_INTE,
        INIT_LIGHT_TIME,
        INIT_FILTER_STATE,
        INIT_FILTER_DESCR_ONE,
        INIT_FILTER_DESCR_TWO,
        INIT_FILTER_DESCR_THR,
        INIT_FILTER_DESCR_FOUR,
        INIT_FILTER_DESCR_FIVE,
        INIT_FILTER_DESCR_SIX,
        INIT_FILTER_DESCR_SEVEN,
        INIT_FILTER_DESCR_EIGHT,
        INIT_SHUTER_STATE,
        INIT_SHUTER_TIME,
        INIT_COMM_STATE,

        INIT_COUNT
    } InitStatus_TypeDef;

    //定义设置态
    typedef enum
    {
        SETTINGS_IDLE = 0, //空闲状态
        SETTINGS_LIGHT_INTE = 0x01,
        SETTINGS_SHUTTER_TIME = 0x02,
        SETTINGS_REMOTE_STATE = 0x04,

        SETTINGS_COUNT
    } SettingsStatus_TypeDef;

    //定义工作状态
    typedef enum
    {
        WORK_IDLE = 0, //空闲状态
        WORK_LIGHT,
        WORK_LIGHT_ON,
        WORK_LIGHT_OFF,
        WORK_LIGHT_INTE,
        WORK_SHUTTER,
        WORK_SHUTTER_AMON,
        WORK_SHUTTER_AMOFF,
        WORK_SHUTTER_AUTO,
        WORK_FILTER,

        WORK_COUNT
    } WorkStatus_TypeDef;

    extern FlagStatus UART1DMABusy;  //存储测试板串口1状态，RESET 0不忙，SET 1忙
    extern FlagStatus UART2DMABusy;  //存储测试板串口2状态，RESET 0不忙，SET 1忙
    extern FlagStatus UART1ReceFlag; //标记UART1是否收到数据 0未收到数据 1收到数据
    extern FlagStatus UART2ReceFlag; //标记UART2是否收到数据 0未收到数据 1收到数据
    extern uint8_t UART1ReadDataLen; //记录USART1接收数组长度
    extern uint8_t UART2ReadDataLen; //记录USART2接收数组长度

    extern uint8_t UART_SystemErrorStatus;
    extern uint8_t UART_LightErrStatus;
    extern uint8_t UART_LightStepStatus;
    extern uint8_t UART_LightIntensity;
    extern uint32_t UART_LightTimeHour;
    extern uint8_t UART_LightTimeMinu;
    extern uint8_t UART_StepErrorStatus; //步进电机故障，0无故障 1定位初始化故障
    extern uint8_t UART_StepRUNStatus;
    extern uint8_t UART_StepPositionCurrent;
    extern uint8_t UART_StepPositionTarget;
    extern uint8_t UART_ShutErrStatus;   //快门错误状态保持，0正常 1故障
    extern uint8_t UART_ShutRunStatus;   //快门运行状态，0已停止 1正在运行 2故障
    extern uint8_t UART_ShutterAMStatus; //快门手动自动状态 0手动 1自动
    extern uint32_t UART_ShutterTime;
    extern uint8_t UART_RemoteStatus;
    extern uint8_t UART_RemoteAddr;

    extern char UART_FilterDescrip[8][8];

    extern uint8_t UART_ShutAMAU;  //快门手动自动状态 0手动 1自动，本地使用变量
    extern uint8_t UART_ShutOnOff; //快门运行停止，0停止 1运行，本地使用变量
    extern SysComState_TypeDef SystemState;

    void UART1Init(void);
    // void USART1_SendDataProce(void);
    void USART1_ReadDataProce(void);

    void GUI_InitComSend(void);
    void GUI_SettingsComSend(uint8_t settingMask);
    void GUI_WorkLightComSend(void);
    void GUI_WorkLightInteComSend(void);
    void GUI_WorkShutterComSend(void);
    void GUI_WorkFilterComSend(void);

#ifdef __cplusplus
}
#endif

#endif