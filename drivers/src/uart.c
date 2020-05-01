#include <stdio.h>
#include <string.h>
#include "misc.h"
#include "stm32f4xx.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_usart.h"
#include "timer.h"
// #include "GUI.h"
// #include "GUI_Display.h"
#include "uart.h"

FlagStatus UART1DMABusy = RESET;                  //存储测试板串口1状态，RESET 0不忙，SET 1忙
FlagStatus UART2DMABusy = RESET;                  //存储测试板串口2状态，RESET 0不忙，SET 1忙
FlagStatus UART1ReceFlag = RESET;                 //标记UART1是否收到数据 RESET 0未收到数据 SET 1收到数据
FlagStatus UART2ReceFlag = RESET;                 //标记UART2是否收到数据 RESET 0未收到数据 SET 1收到数据
uint8_t UART1ReadDataLen = 0;                     //记录USART1接收数组长度
uint8_t UART2ReadDataLen = 0;                     //记录USART2接收数组长度
static char UART1SendData[USART1BUFFERNUM] = {0}; //‘\r\n’(0x13 0x10) 结尾
static char UART1ReceData[USART1BUFFERNUM] = {0}; //‘\r\n’(0x13 0x10) 结尾
// static char UART2SendData[USART2BUFFERNUM]={0};//‘\r\n’(0x13 0x10) 结尾
// static char UART2ReceData[USART2BUFFERNUM]={0};//‘\r\n’(0x13 0x10) 结尾

uint8_t UART_SystemErrorStatus = 0;
uint8_t UART_LightErrStatus = 0;
uint8_t UART_LightStepStatus = 0;
uint8_t UART_LightIntensity = 0;
uint32_t UART_LightTimeHour = 0;
uint8_t UART_LightTimeMinu = 0;
uint8_t UART_StepErrorStatus = 0; //步进电机故障，0无故障 1定位初始化故障
uint8_t UART_StepRUNStatus = 0;
uint8_t UART_StepPositionCurrent = 0;
uint8_t UART_StepPositionTarget = 0;
uint8_t UART_ShutErrStatus = 0;   //快门错误状态保持，0正常 1故障
uint8_t UART_ShutRunStatus = 0;   //快门运行状态，0已停止 1正在运行 2故障
uint8_t UART_ShutterAMStatus = 0; //快门手动自动状态 0手动 1自动
uint32_t UART_ShutterTime = 0;
uint8_t UART_RemoteStatus = 0;
uint8_t UART_RemoteAddr = 0;

char UART_FilterDescrip[8][8] = {};

uint8_t UART_ShutAMAU = 0;                                                                                 //快门手动自动状态 0手动 1自动，本地使用变量
uint8_t UART_ShutOnOff = 0;                                                                                //快门运行停止，0停止 1运行，本地使用变量
SysComState_TypeDef SystemState = {.State_Main = SYSTEM_INIT, .State_Minor = INIT_IDLE, .State_Count = 0}; //标识系统当前状态，根据状态不同串口接收数据处理不同。

/**
  * @brief  USART1DMA中断初始化
  * @param  None
  * @retval None
  */
static void USART1DMA_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream5_IRQn; //DMA2通道5 USART1_RX
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn; //DMA2通道7 USART1_TX
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  USART2DMA中断初始化
  * @param  None
  * @retval None
  */
// static void USART2DMA_NVIC_Config(void)
// {
//     NVIC_InitTypeDef NVIC_InitStructure;

//     NVIC_InitStructure.NVIC_IRQChannel=DMA1_Stream5_IRQn;//DMA1通道5 USART2_RX
//     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
//     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//     NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
//     NVIC_Init(&NVIC_InitStructure);

//     NVIC_InitStructure.NVIC_IRQChannel=DMA1_Stream6_IRQn;//DMA1通道6 USART2_TX
//     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
//     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//     NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
//     NVIC_Init(&NVIC_InitStructure);
// }

/**
  * @brief  USART1中断初始化
  * @param  None
  * @retval None
  */
static void USART1_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; //USART1
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  USART2中断初始化
  * @param  None
  * @retval None
  */
// static void USART2_NVIC_Config(void)
// {
//     NVIC_InitTypeDef NVIC_InitStructure;

//     NVIC_InitStructure.NVIC_IRQChannel=USART2_IRQn;//USART1
//     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
//     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//     NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
//     NVIC_Init(&NVIC_InitStructure);
// }

/**
  * @brief  USART1初始化
  * @param  no
  * @retval no
  */
void UART1Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    USART_InitTypeDef USART_InitStruct;

    //USART1-TX
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;                    //DMA外设寄存器地址 ->成员选择运算符优先，USART1->TDR是具体数值，&取地址运算符后变为地址
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)UART1SendData;                     //DMA内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                              //DMA数据传输方向从内存到外设
    DMA_InitStructure.DMA_BufferSize = sizeof(UART1SendData) / sizeof(UART1SendData[0]); //DMA通道缓存大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                     //DNA外设地址自动增加失能
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                              //DNA内存地址自动增加使能
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;              //DMA外设传输字节宽度
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;                      //DMA内存传输字节宽度
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                                      //工作在循环缓存模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                                //DMA通道优先级
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                               //DMA的FIFO模式禁止，直接模式
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream7, &DMA_InitStructure);    //根据以上参数初始化DMA2
    DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE); //Transfer complete interrupt mask
    DMA_ITConfig(DMA2_Stream7, DMA_IT_TE, ENABLE); //Transfer error interrupt mask
    DMA_Cmd(DMA2_Stream7, DISABLE);                //每次发送数据时软件开启

    //USART1-RX
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;                    //DMA外设USART1-RX基地址USART1->DR
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)UART1ReceData;                     //DMA内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                              //数据传输方向，从外设到内存
    DMA_InitStructure.DMA_BufferSize = sizeof(UART1ReceData) / sizeof(UART1ReceData[0]); //DMA通道的DMA缓存的大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                     //DNA外设地址自动增加失能
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                              //DNA内存地址自动增加使能
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;              //DMA外设传输字节宽度
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;                      //DMA内存传输字节宽度
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                                      //工作在循环缓存模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                                //DMA通道优先级
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                               //DMA的FIFO模式禁止，直接模式
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream5, &DMA_InitStructure);    //根据以上参数初始化DMA1
    DMA_ITConfig(DMA2_Stream5, DMA_IT_TE, ENABLE); //Transfer error interrupt mask
    DMA_Cmd(DMA2_Stream5, ENABLE);

    USART1DMA_NVIC_Config();

    USART_InitStruct.USART_BaudRate = 19200;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No; //USART_Parity_Even
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStruct);

    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); //接收空闲中断
    USART_ITConfig(USART1, USART_IT_PE, ENABLE);   //奇偶校验错误中断
    USART_ITConfig(USART1, USART_IT_ERR, ENABLE);  //包含了FE NE ORE
    USART1_NVIC_Config();

    USART_DMACmd(USART1, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);
    USART_Cmd(USART1, ENABLE);
}

/**
  * @brief  串口1数据处理
  * @param  None.
  * @retval None.
  */
static void USART1_SendDataProce(void)
{
    // if (strlen(UART1SendData) && UART1DMABusy == RESET)
    {
        UART1DMABusy = SET;
        DMA_SetCurrDataCounter(DMA2_Stream7, strlen(UART1SendData)); //保证DMA传输不错位，失能才能写入
        DMA_Cmd(DMA2_Stream7, ENABLE);                               //每次发送数据时软件开启
    }
}

/**
  * @brief  读取控制器版本号
  * @param  None.
  * @retval None.
  */
static void USART_SystemVersionRead(void)
{
    if (UART1DMABusy == RESET)
    {
        memset(UART1SendData, 0, sizeof(UART1SendData)); //发送数组清零
        sprintf(UART1SendData, "%s%s\r\n", USART_SYSTEM, SYSTEM_VERSION);
        USART1_SendDataProce();
    }
}

/**
  * @brief  读取控制器系统状态
  * @param  None.
  * @retval None.
  */
static void USART_SystemStateRead(void)
{
    if (UART1DMABusy == RESET)
    {
        memset(UART1SendData, 0, sizeof(UART1SendData)); //发送数组清零
        sprintf(UART1SendData, "%s%s\r\n", USART_SYSTEM, SYSTEM_STATE);
        USART1_SendDataProce();
    }
}

/**
  * @brief  读取控制器光源状态
  * @param  None.
  * @retval None.
  */
static void USART_LightStateRead(void)
{
    if (UART1DMABusy == RESET)
    {
        memset(UART1SendData, 0, sizeof(UART1SendData)); //发送数组清零
        sprintf(UART1SendData, "%s%s\r\n", USART_LIGHT, LIGHT_STATE);
        USART1_SendDataProce();
    }
}

/**
  * @brief  控制器光源开
  * @param  None.
  * @retval None.
  */
static void USART_LightPowerOn(void)
{
    if (UART1DMABusy == RESET)
    {
        memset(UART1SendData, 0, sizeof(UART1SendData)); //发送数组清零
        sprintf(UART1SendData, "%s%s\r\n", USART_LIGHT, LIGHT_POWER_ON);
        USART1_SendDataProce();
    }
}

/**
  * @brief  控制器光源关
  * @param  None.
  * @retval None.
  */
static void USART_LightPowerOff(void)
{
    if (UART1DMABusy == RESET)
    {
        memset(UART1SendData, 0, sizeof(UART1SendData)); //发送数组清零
        sprintf(UART1SendData, "%s%s\r\n", USART_LIGHT, LIGHT_POWER_OFF);
        USART1_SendDataProce();
    }
}

/**
  * @brief  读取控制器光源强度设定值
  * @param  None.
  * @retval None.
  */
static void USART_LightInteRead(void)
{
    if (UART1DMABusy == RESET)
    {
        memset(UART1SendData, 0, sizeof(UART1SendData)); //发送数组清零
        sprintf(UART1SendData, "%s%s\r\n", USART_LIGHT, LIGHT_INTE_READ);
        USART1_SendDataProce();
    }
}

/**
  * @brief  控制器光源强度设置
  * @param  None.
  * @retval None.
  */
static void USART_LightInteSet(void)
{
    if (UART1DMABusy == RESET)
    {
        memset(UART1SendData, 0, sizeof(UART1SendData)); //发送数组清零
        sprintf(UART1SendData, "%s%s %hu\r\n", USART_LIGHT, LIGHT_INTE_SET, UART_LightIntensity);
        USART1_SendDataProce();
    }
}

/**
  * @brief  读取控制器光源已使用时间
  * @param  None.
  * @retval None.
  */
static void USART_LightTimeRead(void)
{
    if (UART1DMABusy == RESET)
    {
        memset(UART1SendData, 0, sizeof(UART1SendData)); //发送数组清零
        sprintf(UART1SendData, "%s%s\r\n", USART_LIGHT, LIGHT_TIME_READ);
        USART1_SendDataProce();
    }
}

/**
  * @brief  读取滤光片状态
  * @param  None.
  * @retval None.
  */
static void USART_FilterStateRead(void)
{
    if (UART1DMABusy == RESET)
    {
        memset(UART1SendData, 0, sizeof(UART1SendData)); //发送数组清零
        sprintf(UART1SendData, "%s%s\r\n", USART_FILTER, FILTER_STATE);
        USART1_SendDataProce();
    }
}

/**
  * @brief  读取指定滤光片描述
  * @param  FilterNumber    滤光片序号1~8.
  * @retval None.
  */
static void USART_FilterDescriptionRead(uint8_t stepPosition)
{
    if (UART1DMABusy == RESET)
    {
        memset(UART1SendData, 0, sizeof(UART1SendData)); //发送数组清零
        sprintf(UART1SendData, "%s%s %hu\r\n", USART_FILTER, FILTER_DESCR_READ, stepPosition);
        USART1_SendDataProce();
    }
}

/**
  * @brief  指定滤光片设置
  * @param  FilterNumber    滤光片序号1~8.
  * @retval None.
  */
static void USART_FilterPositionSet(uint8_t stepPosition)
{
    if (UART1DMABusy == RESET)
    {
        memset(UART1SendData, 0, sizeof(UART1SendData)); //发送数组清零
        sprintf(UART1SendData, "%s%s %hu\r\n", USART_FILTER, FILTER_POSITION_SET, stepPosition);
        USART1_SendDataProce();
    }
}

/**
  * @brief  读取快门状态
  * @param  None.
  * @retval None.
  */
static void USART_ShuterStateRead(void)
{
    if (UART1DMABusy == RESET)
    {
        memset(UART1SendData, 0, sizeof(UART1SendData)); //发送数组清零
        sprintf(UART1SendData, "%s%s\r\n", USART_SHUTER, SHUTER_STATE);
        USART1_SendDataProce();
    }
}

/**
  * @brief  快门手动关
  * @param  None.
  * @retval None.
  */
static void USART_ShuterAMOff(void)
{
    if (UART1DMABusy == RESET)
    {
        memset(UART1SendData, 0, sizeof(UART1SendData)); //发送数组清零
        sprintf(UART1SendData, "%s%s\r\n", USART_SHUTER, SHUTER_MA_OFF);
        USART1_SendDataProce();
    }
}

/**
  * @brief  快门手动开
  * @param  None.
  * @retval None.
  */
static void USART_ShuterAMOn(void)
{
    if (UART1DMABusy == RESET)
    {
        memset(UART1SendData, 0, sizeof(UART1SendData)); //发送数组清零
        sprintf(UART1SendData, "%s%s\r\n", USART_SHUTER, SHUTER_MA_ON);
        USART1_SendDataProce();
    }
}

/**
  * @brief  快门自动开
  * @param  None.
  * @retval None.
  */
static void USART_ShuterAUOn(void)
{
    if (UART1DMABusy == RESET)
    {
        memset(UART1SendData, 0, sizeof(UART1SendData)); //发送数组清零
        sprintf(UART1SendData, "%s%s\r\n", USART_SHUTER, SHUTER_AU_ON);
        USART1_SendDataProce();
    }
}

/**
  * @brief  读取快门间隔时间
  * @param  None.
  * @retval None.
  */
static void USART_ShuterTimeRead(void)
{
    if (UART1DMABusy == RESET)
    {
        memset(UART1SendData, 0, sizeof(UART1SendData)); //发送数组清零
        sprintf(UART1SendData, "%s%s\r\n", USART_SHUTER, SHUTER_TIME_READ);
        USART1_SendDataProce();
    }
}

/**
  * @brief  快门间隔时间设置
  * @param  None.
  * @retval None.
  */
static void USART_ShuterTimeSet(void)
{
    if (UART1DMABusy == RESET)
    {
        memset(UART1SendData, 0, sizeof(UART1SendData)); //发送数组清零
        sprintf(UART1SendData, "%s%s %lu\r\n", USART_SHUTER, SHUTER_TIME_WRITE, UART_ShutterTime);
        USART1_SendDataProce();
    }
}

/**
  * @brief  读取远程通信状态
  * @param  None.
  * @retval None.
  */
static void USART_CommunicationStateRead(void)
{
    if (UART1DMABusy == RESET)
    {
        memset(UART1SendData, 0, sizeof(UART1SendData)); //发送数组清零
        sprintf(UART1SendData, "%s%s\r\n", USART_COMM, COMM_STATE);
        USART1_SendDataProce();
    }
}

/**
  * @brief  远程通信状态设置
  * @param  None.
  * @retval None.
  */
static void USART_CommunicationStateSet(void)
{
    if (UART1DMABusy == RESET)
    {
        memset(UART1SendData, 0, sizeof(UART1SendData)); //发送数组清零
        sprintf(UART1SendData, "%s%s %hu_%hu\r\n", USART_COMM, COMM_SET, UART_RemoteStatus, UART_RemoteAddr);
        USART1_SendDataProce();
    }
}

/**
  * @brief  系统发送指令
  * @param  pSysState 结构体指针 保存当前发送指令的状态.
  * @retval None.
  */
static void USART_ComSend(SysComState_TypeDef *pSysState)
{
    if (pSysState != NULL)
    {
        switch (pSysState->State_Main)
        {
        case SYSTEM_INIT:
            switch (pSysState->State_Minor)
            {
            case INIT_SYS_VERSION:
                USART_SystemVersionRead();
                break;
            case INIT_SYS_STATE:
                USART_SystemStateRead();
                break;
            case INIT_LIGHT_STATE:
                USART_LightStateRead();
                break;
            case INIT_LIGHT_INTE:
                USART_LightInteRead();
                break;
            case INIT_LIGHT_TIME:
                USART_LightTimeRead();
                break;
            case INIT_FILTER_STATE:
                USART_FilterStateRead();
                break;
            case INIT_FILTER_DESCR_ONE:
                USART_FilterDescriptionRead(1);
                break;
            case INIT_FILTER_DESCR_TWO:
                USART_FilterDescriptionRead(2);
                break;
            case INIT_FILTER_DESCR_THR:
                USART_FilterDescriptionRead(3);
                break;
            case INIT_FILTER_DESCR_FOUR:
                USART_FilterDescriptionRead(4);
                break;
            case INIT_FILTER_DESCR_FIVE:
                USART_FilterDescriptionRead(5);
                break;
            case INIT_FILTER_DESCR_SIX:
                USART_FilterDescriptionRead(6);
                break;
            case INIT_FILTER_DESCR_SEVEN:
                USART_FilterDescriptionRead(7);
                break;
            case INIT_FILTER_DESCR_EIGHT:
                USART_FilterDescriptionRead(8);
                break;
            case INIT_SHUTER_STATE:
                USART_ShuterStateRead();
                break;
            case INIT_SHUTER_TIME:
                USART_ShuterTimeRead();
                break;
            case INIT_COMM_STATE:
                USART_CommunicationStateRead();
                break;
            }
            break;
        case SYSTEM_SETTINGS:
            switch (pSysState->State_Minor)
            {
            case SETTINGS_LIGHT_INTE:
                USART_LightInteSet();
                break;
            case SETTINGS_SHUTTER_TIME:
                USART_ShuterTimeSet();
                break;
            case SETTINGS_REMOTE_STATE:
                USART_CommunicationStateSet();
                break;
            }
            break;
        case SYSTEM_WORK:
            switch (pSysState->State_Minor)
            {
            case WORK_LIGHT_ON:
                USART_LightPowerOn();
                break;
            case WORK_LIGHT_OFF:
                USART_LightPowerOff();
                break;
            case WORK_LIGHT_INTE:
                USART_LightInteSet();
                break;
            case WORK_SHUTTER_AMON:
                USART_ShuterAMOn();
                break;
            case WORK_SHUTTER_AMOFF:
                USART_ShuterAMOff();
                break;
            case WORK_SHUTTER_AUTO:
                USART_ShuterAUOn();
                break;
            case WORK_FILTER:
                USART_FilterPositionSet(UART_StepPositionTarget);
                break;
            }
            break;
        }
    }
}

/**
  * @brief  定时器0 串口发送消息超时处理函数
  * @param
    TimerId 定时器ID
    arg     传入参数指针
  * @retval None.
  */
static void USART_ComSendTimer(uint8_t TimerId, void *arg)
{
    SysComState_TypeDef *pSysState = NULL;

    if (arg != NULL)
    {
        pSysState = arg;
        pSysState->State_Count++;
        switch (pSysState->State_Main)
        {
        case SYSTEM_INIT:
            if (pSysState->State_Count >= ORDER_COUNT) //发送指令是否超过次数
            {
                if (pSysState->State_Minor < INIT_COUNT - 1)
                {
                    pSysState->State_Minor++;
                    pSysState->State_Count = 0;
                    USART_ComSend(pSysState);
                }
                else
                {
                    pSysState->State_Minor = INIT_IDLE;
                    pSysState->State_Count = 0;
                    DelTimer(TimerId);
                    // HomeWMRefresh();
                    // WelcomeToHome();
                }
            }
            else
            {
                USART_ComSend(pSysState);
            }
            break;
        case SYSTEM_SETTINGS:
            if (pSysState->State_Count >= ORDER_COUNT) //发送指令是否超过次数
            {
                if (pSysState->SettingsMask & SETTINGS_LIGHT_INTE)
                {
                    pSysState->SettingsMask &= ~SETTINGS_LIGHT_INTE;
                    pSysState->State_Minor = SETTINGS_LIGHT_INTE;
                    pSysState->State_Count = 0;
                    USART_ComSend(pSysState);
                }
                else if (pSysState->SettingsMask & SETTINGS_SHUTTER_TIME)
                {
                    pSysState->SettingsMask &= ~SETTINGS_SHUTTER_TIME;
                    pSysState->State_Minor = SETTINGS_SHUTTER_TIME;
                    pSysState->State_Count = 0;
                    USART_ComSend(pSysState);
                }
                else if (pSysState->SettingsMask & SETTINGS_REMOTE_STATE)
                {
                    pSysState->SettingsMask &= ~SETTINGS_REMOTE_STATE;
                    pSysState->State_Minor = SETTINGS_REMOTE_STATE;
                    pSysState->State_Count = 0;
                    USART_ComSend(pSysState);
                }
                else
                {
                    pSysState->State_Main = SYSTEM_IDLE;
                    pSysState->State_Minor = 0;
                    pSysState->State_Count = 0;
                    DelTimer(TimerId);
                    // HomeWMRefresh();
                }
            }
            else
            {
                USART_ComSend(pSysState);
            }
            break;
        case SYSTEM_WORK:
            if (pSysState->State_Count >= ORDER_COUNT) //发送指令是否超过次数
            {
                pSysState->State_Main = SYSTEM_IDLE;
                pSysState->State_Minor = 0;
                pSysState->State_Count = 0;
                DelTimer(TimerId);
                // HomeWMRefresh();
            }
            else
            {
                USART_ComSend(pSysState);
            }
            break;
        default:
            DelTimer(TimerId);
            // HomeWMRefresh();
            break;
        }
    }
    else
    {
        DelTimer(TimerId);
    }
}

/**
  * @brief  系统首次发送指令
  * @param  pSysState 结构体指针 保存当前发送指令的状态.
  * @retval None.
  */
static void USART_ComSendFirst(SysComState_TypeDef *pSysState)
{
    if (pSysState != NULL)
    {
        switch (pSysState->State_Main)
        {
        case SYSTEM_INIT:
            pSysState->State_Minor = INIT_SYS_VERSION;
            pSysState->State_Count = 0;
            USART_ComSend(pSysState);
            AddTimer(0, EXPIRE_ORDERTIME, USART_ComSendTimer, (void *)pSysState); //增加超时定时器
            break;
        case SYSTEM_SETTINGS:
            if (pSysState->SettingsMask & SETTINGS_LIGHT_INTE)
            {
                pSysState->SettingsMask &= ~SETTINGS_LIGHT_INTE;
                pSysState->State_Minor = SETTINGS_LIGHT_INTE;
            }
            else if (pSysState->SettingsMask & SETTINGS_SHUTTER_TIME)
            {
                pSysState->SettingsMask &= ~SETTINGS_SHUTTER_TIME;
                pSysState->State_Minor = SETTINGS_SHUTTER_TIME;
                pSysState->State_Count = 0;
            }
            else if (pSysState->SettingsMask & SETTINGS_REMOTE_STATE)
            {
                pSysState->SettingsMask &= ~SETTINGS_REMOTE_STATE;
                pSysState->State_Minor = SETTINGS_REMOTE_STATE;
            }
            pSysState->State_Count = 0;
            USART_ComSend(pSysState);
            AddTimer(0, EXPIRE_ORDERTIME, USART_ComSendTimer, (void *)pSysState); //增加超时定时器
            break;
        case SYSTEM_WORK:
            switch (pSysState->State_Minor)
            {
            case WORK_LIGHT:
                if (UART_LightStepStatus == 0)
                {
                    pSysState->State_Minor = WORK_LIGHT_ON;
                }
                else
                {
                    pSysState->State_Minor = WORK_LIGHT_OFF;
                }
                pSysState->State_Count = 0;
                USART_ComSend(pSysState);
                AddTimer(0, EXPIRE_ORDERTIME, USART_ComSendTimer, (void *)pSysState); //增加超时定时器
                break;
            case WORK_LIGHT_INTE:
            case WORK_FILTER:
                pSysState->State_Count = 0;
                USART_ComSend(pSysState);
                AddTimer(0, EXPIRE_ORDERTIME, USART_ComSendTimer, (void *)pSysState); //增加超时定时器
                break;
            case WORK_SHUTTER:
                if (UART_ShutAMAU)
                {
                    if (UART_ShutOnOff)
                    {
                        pSysState->State_Minor = WORK_SHUTTER_AMOFF;
                    }
                    else
                    {
                        pSysState->State_Minor = WORK_SHUTTER_AUTO;
                    }
                }
                else
                {
                    if (UART_ShutOnOff)
                    {
                        pSysState->State_Minor = WORK_SHUTTER_AMOFF;
                    }
                    else
                    {
                        pSysState->State_Minor = WORK_SHUTTER_AMON;
                    }
                }
                UART_ShutOnOff = !UART_ShutOnOff;
                pSysState->State_Count = 0;
                USART_ComSend(pSysState);
                AddTimer(0, EXPIRE_ORDERTIME, USART_ComSendTimer, (void *)pSysState); //增加超时定时器
                break;
            }
            break;
        default:
            DelTimer(0);
            break;
        }
    }
}

/**
  * @brief  系统循环发送指令
  * @param  pSysState 结构体指针 保存当前发送指令的状态.
  * @retval None.
  */
static void USART_ComSendCyclic(SysComState_TypeDef *pSysState)
{
    if (pSysState != NULL)
    {
        switch (pSysState->State_Main)
        {
        case SYSTEM_INIT:
            if (pSysState->State_Minor < INIT_COUNT - 1)
            {
                pSysState->State_Minor++;
                pSysState->State_Count = 0;
                USART_ComSend(pSysState);
                AddTimer(0, EXPIRE_ORDERTIME, USART_ComSendTimer, (void *)pSysState); //增加超时定时器
            }
            else
            {
                pSysState->State_Main = SYSTEM_IDLE;
                pSysState->State_Minor = 0;
                pSysState->State_Count = 0;
                DelTimer(0);
                // HomeWMRefresh();
                // WelcomeToHome();
            }
            break;
        case SYSTEM_SETTINGS:
            if (pSysState->SettingsMask & SETTINGS_LIGHT_INTE)
            {
                pSysState->SettingsMask &= ~SETTINGS_LIGHT_INTE;
                pSysState->State_Minor = SETTINGS_LIGHT_INTE;
                pSysState->State_Count = 0;
                USART_ComSend(pSysState);
                AddTimer(0, EXPIRE_ORDERTIME, USART_ComSendTimer, (void *)pSysState); //增加超时定时器
            }
            else if (pSysState->SettingsMask & SETTINGS_SHUTTER_TIME)
            {
                pSysState->SettingsMask &= ~SETTINGS_SHUTTER_TIME;
                pSysState->State_Minor = SETTINGS_SHUTTER_TIME;
                pSysState->State_Count = 0;
                USART_ComSend(pSysState);
                AddTimer(0, EXPIRE_ORDERTIME, USART_ComSendTimer, (void *)pSysState); //增加超时定时器
            }
            else if (pSysState->SettingsMask & SETTINGS_REMOTE_STATE)
            {
                pSysState->SettingsMask &= ~SETTINGS_REMOTE_STATE;
                pSysState->State_Minor = SETTINGS_REMOTE_STATE;
                pSysState->State_Count = 0;
                USART_ComSend(pSysState);
                AddTimer(0, EXPIRE_ORDERTIME, USART_ComSendTimer, (void *)pSysState); //增加超时定时器
            }
            else
            {
                pSysState->State_Main = SYSTEM_IDLE;
                pSysState->State_Minor = 0;
                pSysState->State_Count = 0;
                DelTimer(0);
                // HomeWMRefresh();
            }
            break;
        case SYSTEM_WORK:
            pSysState->State_Main = SYSTEM_IDLE;
            pSysState->State_Minor = 0;
            pSysState->State_Count = 0;
            DelTimer(0);
            // HomeWMRefresh();
            break;
        default:
            DelTimer(0);
            // HomeWMRefresh();
            break;
        }
    }
}

/**
  * @brief  系统初始化后主动发起通讯
  * @param  None.
  * @retval None.
  */
void GUI_InitComSend(void)
{
    SystemState.State_Main = SYSTEM_INIT;
    USART_ComSendFirst(&SystemState);
}

/**
  * @brief  系统设置确认后发送指令
  * @param  settingMask 标识需要设置的参数.
  * @retval None.
  */
void GUI_SettingsComSend(uint8_t settingMask)
{
    if (SystemState.State_Main == SYSTEM_INIT && SystemState.State_Minor == INIT_IDLE)
    {
        USART_ComSendFirst(&SystemState);
    }
    else if (SystemState.State_Main == SYSTEM_IDLE)
    {
        SystemState.State_Main = SYSTEM_SETTINGS;
        SystemState.SettingsMask = settingMask;
        USART_ComSendFirst(&SystemState);
    }
}

/**
  * @brief  光源发送指令
  * @param  None.
  * @retval None.
  */
void GUI_WorkLightComSend(void)
{
    if (SystemState.State_Main == SYSTEM_INIT && SystemState.State_Minor == INIT_IDLE)
    {
        USART_ComSendFirst(&SystemState);
    }
    else if (SystemState.State_Main == SYSTEM_IDLE)
    {
        SystemState.State_Main = SYSTEM_WORK;
        SystemState.State_Minor = WORK_LIGHT;
        USART_ComSendFirst(&SystemState);
    }
}

/**
  * @brief  光源光照强度发送指令
  * @param  None.
  * @retval None.
  */
void GUI_WorkLightInteComSend(void)
{
    if (SystemState.State_Main == SYSTEM_INIT && SystemState.State_Minor == INIT_IDLE)
    {
        USART_ComSendFirst(&SystemState);
    }
    else if (SystemState.State_Main == SYSTEM_IDLE)
    {
        SystemState.State_Main = SYSTEM_WORK;
        SystemState.State_Minor = WORK_LIGHT_INTE;
        USART_ComSendFirst(&SystemState);
    }
}

/**
  * @brief  快门发送指令
  * @param  None.
  * @retval None.
  */
void GUI_WorkShutterComSend(void)
{
    if (SystemState.State_Main == SYSTEM_INIT && SystemState.State_Minor == INIT_IDLE)
    {
        USART_ComSendFirst(&SystemState);
    }
    else if (SystemState.State_Main == SYSTEM_IDLE)
    {
        SystemState.State_Main = SYSTEM_WORK;
        SystemState.State_Minor = WORK_SHUTTER;
        USART_ComSendFirst(&SystemState);
    }
}

/**
  * @brief  滤光片发送指令
  * @param  None.
  * @retval None.
  */
void GUI_WorkFilterComSend(void)
{
    if (SystemState.State_Main == SYSTEM_INIT && SystemState.State_Minor == INIT_IDLE)
    {
        USART_ComSendFirst(&SystemState);
    }
    else if (SystemState.State_Main == SYSTEM_IDLE)
    {
        SystemState.State_Main = SYSTEM_WORK;
        SystemState.State_Minor = WORK_FILTER;
        USART_ComSendFirst(&SystemState);
    }
}

/**
  * @brief  USART1 数据处理
  * @param  None.
  * @retval None.
  */
void USART1_ReadDataProce(void)
{
    if (UART1ReadDataLen > 4 && UART1ReceData[UART1ReadDataLen - 2] == 0x0D && UART1ReceData[UART1ReadDataLen - 1] == 0x0A) //‘\r\n’(0x13 0x10) 结尾
    {
        if (memcmp(UART1ReceData, USART_SYSTEM, strlen(USART_SYSTEM)) == 0)
        {
            if (memcmp(UART1ReceData + strlen(USART_SYSTEM), SYSTEM_VERSION, strlen(SYSTEM_VERSION)) == 0)
            {
                //保存控制板版本号
                USART_ComSendCyclic(&SystemState);
            }
            else if (memcmp(UART1ReceData + strlen(USART_SYSTEM), SYSTEM_STATE, strlen(SYSTEM_STATE)) == 0)
            {
                sscanf(UART1ReceData + strlen(USART_SYSTEM) + strlen(SYSTEM_STATE) + strlen(USART_SPACE), "%hhu", &UART_SystemErrorStatus);
                USART_ComSendCyclic(&SystemState);
            }
        }
        else if (memcmp(UART1ReceData, USART_LIGHT, strlen(USART_LIGHT)) == 0)
        {
            if (memcmp(UART1ReceData + strlen(USART_LIGHT), LIGHT_STATE, strlen(LIGHT_STATE)) == 0)
            {
                sscanf(UART1ReceData + strlen(USART_LIGHT) + strlen(LIGHT_STATE) + strlen(USART_SPACE), "%hhu_%hhu", &UART_LightErrStatus, &UART_LightStepStatus);
                USART_ComSendCyclic(&SystemState);
            }
            else if (memcmp(UART1ReceData + strlen(USART_LIGHT), LIGHT_INTE, strlen(LIGHT_INTE)) == 0)
            {
                sscanf(UART1ReceData + strlen(USART_LIGHT) + strlen(LIGHT_INTE) + strlen(USART_SPACE), "%hhu", &UART_LightIntensity);
                USART_ComSendCyclic(&SystemState);
            }
            else if (memcmp(UART1ReceData + strlen(USART_LIGHT), LIGHT_TIME, strlen(LIGHT_TIME)) == 0)
            {
                sscanf(UART1ReceData + strlen(USART_LIGHT) + strlen(LIGHT_TIME) + strlen(USART_SPACE), "%luh%hhum", &UART_LightTimeHour, &UART_LightTimeMinu);
                USART_ComSendCyclic(&SystemState);
            }
        }
        else if (memcmp(UART1ReceData, USART_FILTER, strlen(USART_FILTER)) == 0)
        {
            if (memcmp(UART1ReceData + strlen(USART_FILTER), FILTER_STATE, strlen(FILTER_STATE)) == 0)
            {
                sscanf(UART1ReceData + strlen(USART_FILTER) + strlen(FILTER_STATE) + strlen(USART_SPACE), "%hhu_%hhu_%hhu_%hhu", &UART_StepErrorStatus, &UART_StepRUNStatus, &UART_StepPositionCurrent, &UART_StepPositionTarget);
                USART_ComSendCyclic(&SystemState);
            }
            else if (memcmp(UART1ReceData + strlen(USART_FILTER), FILTER_DESCR, strlen(FILTER_DESCR)) == 0)
            {
                uint8_t tempNumber = 0;
                char tempDescription[8] = {0};

                sscanf(UART1ReceData + strlen(USART_FILTER) + strlen(FILTER_DESCR) + strlen(USART_SPACE), "%hhu_%7s", &tempNumber, tempDescription);
                if (tempNumber > 0)
                {
                    memcpy(UART_FilterDescrip[tempNumber - 1], tempDescription, strlen(tempDescription));
                }
                USART_ComSendCyclic(&SystemState);
            }
        }
        else if (memcmp(UART1ReceData, USART_SHUTER, strlen(USART_SHUTER)) == 0)
        {
            if (memcmp(UART1ReceData + strlen(USART_SHUTER), SHUTER_STATE, strlen(SHUTER_STATE)) == 0)
            {
                sscanf(UART1ReceData + strlen(USART_SHUTER) + strlen(SHUTER_STATE) + strlen(USART_SPACE), "%hhu_%hhu_%hhu", &UART_ShutErrStatus, &UART_ShutRunStatus, &UART_ShutterAMStatus);
                USART_ComSendCyclic(&SystemState);
            }
            else if (memcmp(UART1ReceData + strlen(USART_SHUTER), SHUTER_TIME, strlen(SHUTER_TIME)) == 0)
            {
                sscanf(UART1ReceData + strlen(USART_SHUTER) + strlen(SHUTER_TIME) + strlen(USART_SPACE), "%lus", &UART_ShutterTime);
                USART_ComSendCyclic(&SystemState);
            }
        }
        else if (memcmp(UART1ReceData, USART_COMM, strlen(USART_COMM)) == 0)
        {
            if (memcmp(UART1ReceData + strlen(USART_COMM), SHUTER_STATE, strlen(SHUTER_STATE)) == 0)
            {
                sscanf(UART1ReceData + strlen(USART_COMM) + strlen(SHUTER_STATE) + strlen(USART_SPACE), "%hhu_%hhu", &UART_RemoteStatus, &UART_RemoteAddr);
                USART_ComSendCyclic(&SystemState);
            }
        }

        /* switch(SystemState.State_Main)
        {
        case SYSTEM_INIT:
            switch(SystemState.State_Minor)
            {
            case INIT_IDLE:
            case INIT_FILTER_DESCR:
                if(memcmp(UART1ReceData, USART_FILTER, strlen(USART_FILTER)) == 0)
                {
                    if(memcmp(UART1ReceData + strlen(USART_FILTER), FILTER_STATE, strlen(FILTER_STATE)) == 0)
                    {
                        uint8_t tempErrorState = 0;
                        uint8_t tempPositionCurrent = 0;
                        uint8_t tempPositionTarget = 0;
                        sscanf(UART1ReceData + strlen(USART_FILTER) + strlen(FILTER_STATE) + strlen(USART_SPACE), "%lu", &tempErrorState);
                        sscanf(UART1ReceData + strlen(USART_FILTER) + strlen(FILTER_STATE) + strlen(USART_SPACE) + sizeof(uint8_t) + sizeof(uint8_t), "%lu", &tempPositionCurrent);
                        sscanf(UART1ReceData + strlen(USART_FILTER) + strlen(FILTER_STATE) + strlen(USART_SPACE) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t), "%lu", &tempPositionTarget);
                        if(tempErrorState || (tempPositionCurrent == tempPositionTarget))//定位初始化故障,或者当前位置等于目标位置
                        {
                            //更改界面，停止界面定时器，emWin回调

                            USART_SystemVersionRead();//发送读取指令
                            SystemState.State_Minor++;
                            SystemState.State_Count = 1;
                            AddTimer(0, EXPIRE_ORDERTIME, Timer_TimerOutFuction, (void*) &SystemState);//增加超时定时器
                        }
                    }
                }
                break;
            case INIT_SYS_VERSION:
            case INIT_SYS_STATE:
                if(memcmp(UART1ReceData, USART_SYSTEM, strlen(USART_SYSTEM)) == 0)
                {
                    if(memcmp(UART1ReceData + strlen(USART_SYSTEM), SYSTEM_VERSION, strlen(SYSTEM_VERSION)) == 0)
                    {
                        //保存控制板版本号
                        USART_SystemStateRead();//发送读取指令
                        SystemState.State_Minor++;
                        SystemState.State_Count = 1;
                        AddTimer(0, EXPIRE_ORDERTIME, Timer_TimerOutFuction, (void*) &SystemState);//增加超时定时器
                    }
                    else if(memcmp(UART1ReceData + strlen(USART_SYSTEM), SYSTEM_STATE, strlen(SYSTEM_STATE)) == 0)
                    {
                        sscanf(UART1ReceData + strlen(USART_SYSTEM) + strlen(SYSTEM_STATE) + strlen(USART_SPACE), "%lu", &UART_SystemErrorStatus);
                        USART_LightStateRead();//发送读取指令
                        SystemState.State_Minor++;
                        SystemState.State_Count = 1;
                        AddTimer(0, EXPIRE_ORDERTIME, Timer_TimerOutFuction, (void*) &SystemState);//增加超时定时器
                    }
                }
                break;
            case INIT_LIGHT_STATE:
            case INIT_LIGHT_INTE:
            case INIT_LIGHT_TIME:
                break;
            case INIT_SHUTER_STATE:
            case INIT_SHUTER_TIME:
                break;
            case INIT_COMM_STATE:
                break;
            }
            break;
        } */
    }
    memset(UART1ReceData, 0, sizeof(UART1ReceData)); //接收数组清零
    // USART_Cmd(USART1, ENABLE);
}