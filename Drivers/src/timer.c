#include "misc.h"
#include "stm32f4xx.h"
#include "stm32f4xx_tim.h"
#include "gpio.h"
//#include "iwdg.h"
#include "timer.h"
// #include "GUI.h"

uint32_t Tick_Num = 0;
uint8_t Timer1msFlag = 0;
uint8_t Timer10msFlag = 0;
uint8_t Timer200msFlag = 0;
uint8_t Timer500msFlag = 0;
uint8_t Timer1sFlag = 0;
uint8_t Timer1MinFlag = 0;
volatile uint8_t STMDelayFlag = 0;
static Timer_t timerArray[MAX_TIMER] = {0};

/******************************************************************************
* Function name : Tim14Init
* Input param 	: None
* Return 		: None
* 定时器溢出时间计算: (分频值/Fcpu)*计数值 Fcpu 84M
*******************************************************************************/
void TimCyclic_Init(void) //间隔时间循环中断
{
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

    NVIC_InitStructure.NVIC_IRQChannel = TIM8_TRG_COM_TIM14_IRQn; //10ms中断定时器
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_TimeBaseInitStruct.TIM_Prescaler = 0xDF; //84MHz时钟频率，224 0xE0 (n-1)分频
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = 0x0177; //224预分频 0x0EA6 3750计数 10ms中断，0x0177 375计数 1ms中断
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM14, &TIM_TimeBaseInitStruct);
    TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM14, ENABLE);
}

/**
  * @brief  TimCyclic_Proc 中断循环处理
  * @param  None
  * @retval None
  */
 void TimCyclic_Process(void)
{
    static uint16_t Tick_timer = 0;

    Timer1msFlag = 1;
    // OS_TimeMS++;
    Tick_timer++;
    
    if(Tick_timer%10 == 0)
    {
      Tick_Num++;
      Timer10msFlag = 1;
    }
    if(Tick_timer%200 == 0)
    {
      Timer200msFlag = 1;
    }
    if(Tick_timer%500 == 0)
    {
      Timer500msFlag = 1;
    }
    if(Tick_timer%1000 == 0)
    {
      Timer1sFlag = 1;
    }
    if(Tick_timer>=60000)
    {
      Timer1MinFlag = 1;
      Tick_timer=0;
    }
}

/**
  * @brief  STM32Delay函数初始化，使能中断
  * @param  None
  * @retval None
  */
void STM32Delay_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn; //延时定时器
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**************************************************
* Function name : STM32Delay_us
* Description 	:延时 晶振频率: 84M
* Input param 	:
us:1-65535us
* Return        : None
**************************************************/
void STM32Delay_us(uint16_t us)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

    STMDelayFlag = 1;
    TIM_TimeBaseInitStruct.TIM_Prescaler = 0x53; //84 0x54 (n-1)预分频
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = us;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);
    TIM_ClearFlag(TIM6, TIM_IT_Update);
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM6, ENABLE);
    while (STMDelayFlag > 0)
        ;
}

/**************************************************
* Function name : STM32Delay_ms
* Description 	:延时 晶振频率: 84M
* Input param 	:
ms:1-32767ms//65535
* Return        : None
**************************************************/
void STM32Delay_ms(uint16_t ms)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

    STMDelayFlag = 1;
    TIM_TimeBaseInitStruct.TIM_Prescaler = 0xA40F; //42000 0xA410 (n-1)预分频
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = ms * 2;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);
    TIM_ClearFlag(TIM6, TIM_IT_Update);
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE); //设置中断
    TIM_Cmd(TIM6, ENABLE);                     //使能定时器
    while (STMDelayFlag > 0)
        ;
    //{
    //WatchDogFeed();
    //}
}

/******************************************************************************
* Function name : Get_Tick
* Description 	: 获取系统Tick值
* Input param 	: None
* Return        : 返回定时器计数周期值
* See also      : None
*******************************************************************************/
uint32_t Get_Tick(void)
{
    uint32_t tmp;
    tmp = Tick_Num;
    return tmp;
}

/******************************************************************************
* Function name : Get_TickGap
* Description 	: 获取定时器Tick间隔值
* Input param 	:
Tick            :记录的原始数值
* Return        : 返回定时器计数间隔值
* See also      : None
*******************************************************************************/
uint32_t Get_TickGap(uint32_t Tick)
{
    uint32_t curruntimer = 0;

    if (Tick_Num >= Tick)
    {
        curruntimer = Tick_Num - Tick;
    }
    else //ticks值溢出
    {
        curruntimer = 0xFFFFFFFF - Tick + Tick_Num + 0x01;
    }
    return curruntimer;
}

/**************************************************
* Function name : AddTimer
* Description 	:增加定时器
* Input param 	:
TimerID         :ID号0-3
timeOut         :超时处理时间 以定时器中断周期为基准时间 成倍增加
handler         :定时器时间到执行函数
*arg            :定时器执行函数的参数
* Return        : None
**************************************************/
void AddTimer(uint8_t TimerID, uint32_t timeOut, TimerEventHandler_f handler, void *arg)
{
    if (TimerID > (MAX_TIMER - 1))
    {
        return;
    }
    timerArray[TimerID].timeOut = timeOut;
    timerArray[TimerID].ticks = Get_Tick();
    timerArray[TimerID].handler = handler;
    timerArray[TimerID].arg = arg;
}

/**************************************************
* Function name : DelTimer
* Description 	:删除定时器
* Input param 	:
TimerID         :ID号0-3
* Return        : None
**************************************************/
void DelTimer(uint8_t TimerID)
{
    timerArray[TimerID].timeOut = 0;
    timerArray[TimerID].ticks = 0;
    timerArray[TimerID].handler = 0;
    timerArray[TimerID].arg = 0;
}

/**************************************************
* Function name : IsTimer
* Description 	:判断定时器是否非空
* Input param 	:
TimerID         :ID号0-3
* Return        : None
**************************************************/
uint8_t IsTimer(uint8_t TimerID)
{
    if (timerArray[TimerID].handler == 0)
    {
        return 0;
    }
    return 1;
}

/**************************************************
* Function name : TimerProcess
* Description 	:在main函数中循环判断定时器是否延时时间到
* Input param 	:
* Return        : None
**************************************************/
void TimerProcess(void)
{
    uint8_t i;

    for (i = 0; i < MAX_TIMER; i++)
    {
        if (timerArray[i].handler == 0)
            continue;
        else
        {
            if (Get_TickGap(timerArray[i].ticks) >= timerArray[i].timeOut)
            {
                timerArray[i].ticks = Get_Tick(); //定时时间到，更新定时器
                timerArray[i].handler(i, timerArray[i].arg);
            }
        }
    }
}

/**
  * @brief  Tim2初始化 84Mz //过零捕获单脉冲触发
  * @param  None
  * @retval None
  */
void Tim2Init(void) //TIM2_CH1
{
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; //单脉冲触发
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_TimeBaseInitStructure.TIM_Prescaler = 0x0347;               //TIMx时钟频率除数的预分频值1200(n-1)0x04AF,70KHz频率;预分频值840(n-1)0x0347,100KHz频率;预分频值21(n-1)0x0014,40MHz频率
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
    TIM_TimeBaseInitStructure.TIM_Period = 0x01F4;                  //自动重装载寄存器周期的值500 0x01F4 5ms一次中断 过零检测10ms一次，要控制在10ms之内
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //TIM输出比较触发模式--TIM_OCMode_PWM1,计数器小于捕获比较寄存器值时输出高电平 TIM_OCMode_PWM2,计数器小于捕获比较寄存器值时输出低电平
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0x0000;                   //设置了待装入捕获比较寄存器的值 延时脉冲宽度至少200uS 1个计数10us
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //TIM输出比较触发模式--TIM_OCMode_PWM1,计数器小于捕获比较寄存器值时输出高电平 TIM_OCMode_PWM2,计数器小于捕获比较寄存器值时输出低电平
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0x0000;                   //设置了待装入捕获比较寄存器的值 延时脉冲宽度至少200uS 1个计数10us
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);

    TIM_SelectOnePulseMode(TIM2, TIM_OPMode_Single); //选择单脉冲模式
    TIM_CtrlPWMOutputs(TIM2, ENABLE);

    //TIM_SelectInputTrigger(TIM2, TIM_TS_TI1FP1);
    //TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Reset);//TIM_SlaveMode_Trigger

    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); //定时器跟新中断，目的是定时器停止后复位Compare值，不复位的话会输出高电平。
    //TIM_Cmd(TIM2, ENABLE);
}

/**
  * @brief  Tim8初始化 168Mz //过零捕获单脉冲触发
  * @param  None
  * @retval None
  */
void Tim8Init(void) //TIM8_CH1 TIM8_CH2 TIM8_CH3
{
    //    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    //
    //    NVIC_InitStructure.NVIC_IRQChannel=TIM8_UP_TIM13_IRQn;//单脉冲触发
    //    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
    //    NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;
    //    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    //    NVIC_Init(&NVIC_InitStructure);

    TIM_TimeBaseInitStructure.TIM_Prescaler = 0x068F;               //TIMx时钟频率除数的预分频值1680(n-1)0x068F,100KHz频率
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
    TIM_TimeBaseInitStructure.TIM_Period = 0x012C;                  //PWM2模式减去OC中的延时值计算触发时间	//自动重装载寄存器周期的值500 0x01F4 5ms;100 0x0064 1ms;200 0x00C8 2ms;300 0x012C 3ms;400 0x190 4ms 过零检测10ms一次，要控制在10ms之内，1000个计数是10ms
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //TIM输出比较触发模式--TIM_OCMode_PWM1,计数器小于捕获比较寄存器值时输出高电平 TIM_OCMode_PWM2,计数器小于捕获比较寄存器值时输出低电平
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OCInitStructure.TIM_Pulse = 0x03E8;                   //PWM2模式要大于装载寄存器值100		//设置了待装入捕获比较寄存器的值 延时脉冲宽度至少200uS 1个计数10us
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    TIM_OC2Init(TIM8, &TIM_OCInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //TIM输出比较触发模式--TIM_OCMode_PWM1,计数器小于捕获比较寄存器值时输出高电平 TIM_OCMode_PWM2,计数器小于捕获比较寄存器值时输出低电平
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OCInitStructure.TIM_Pulse = 0x03E8;                   //PWM2模式要大于装载寄存器值100		//设置了待装入捕获比较寄存器的值 延时脉冲宽度至少200uS 1个计数10us
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    TIM_OC3Init(TIM8, &TIM_OCInitStructure);

    TIM_SelectOnePulseMode(TIM8, TIM_OPMode_Single); //选择单脉冲模式
    TIM_CtrlPWMOutputs(TIM8, ENABLE);

    //TIM_SelectInputTrigger(TIM8, TIM_TS_TI1FP1);
    //TIM_SelectSlaveMode(TIM8, TIM_SlaveMode_Reset);//TIM_SlaveMode_Trigger

    //TIM_ClearFlag(TIM8, TIM_FLAG_Update);
    //TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
    //TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);//定时器跟新中断，目的是定时器停止后复位Compare值，不复位的话会输出高电平。
    //TIM_Cmd(TIM8, ENABLE);
}

/**
  * @brief  Tim8单脉冲触发控制
  * @param  None
  * @retval None
  */
void TimTriac(uint8_t TriacNumb)
{
    if (TriacNumb == 1)
    {
        TIM_SetCompare2(TIM8, 0x0003); //延时脉冲宽度 1个计数10us 3
        TIM_SetCompare3(TIM8, 0x03E8); //PWM2模式要大于装载寄存器值100
    }
    else if (TriacNumb == 2)
    {
        TIM_SetCompare2(TIM8, 0x03E8); //PWM2模式要大于装载寄存器值100
        TIM_SetCompare3(TIM8, 0x0003); //延时脉冲宽度 1个计数10us 3
    }
    TIM_Cmd(TIM8, ENABLE); //因为是单脉冲模式，所以每次都要使能
}

/**
  * @brief  Tim4初始化 84Mz PWM输出调整背光
  * @param  None
  * @retval None
  */
void Tim4Init(void) //TIM4_CH3
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    TIM_TimeBaseInitStructure.TIM_Prescaler = 0x1067;               //TIMx时钟频率除数的预分频值42000 x1068 (n-1),20KHz频率
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
    TIM_TimeBaseInitStructure.TIM_Period = 0x00C8;                  //自动重装载寄存器周期的值200 0x00C8 100Hz跟新频率
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //TIM输出比较触发模式--TIM_OCMode_PWM1,计数器小于捕获比较寄存器值时输出高电平 TIM_OCMode_PWM2,计数器小于捕获比较寄存器值时输出低电平
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0x0000;                   //初始化关闭				//设置了待装入捕获比较寄存器值150 0x0096,75%占空比
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性
    TIM_OC3Init(TIM4, &TIM_OCInitStructure);

    //TIM_SelectOnePulseMode(TIM4, TIM_OPMode_Single);  //选择单脉冲模式
    TIM_CtrlPWMOutputs(TIM4, ENABLE);
    TIM_Cmd(TIM4, ENABLE);
}

/**
  * @brief  TFT LCD背光控制函数.
  * @param  BLFunction: 控制背光使能参数.
  *   This parameter can be: ENABLE or DISABLE.
  *   TFTLCDBackLight 背光强度0~100
  * @retval None
  */
void FTFLCD_BL(uint8_t TFTLCDBackLight)
{
    if (TFTLCDBackLight == 0)
        TIM_SetCompare3(TIM4, 0);
    else if (TFTLCDBackLight > 100)
        TIM_SetCompare3(TIM4, 200);
    else
        TIM_SetCompare3(TIM4, 100 + TFTLCDBackLight);
    //TIM_Cmd(TIM4, ENABLE);//非单脉冲模式，在初始化时已经使能
}

/**
  * @brief  Tim5初始化 84Mz 编码器脉冲捕获
  * @param  None
  * @retval None
  */
void Tim5Init(void) //TIM5_CH1 TIM5_CH2
{
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn; //流量计脉冲捕获
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_TimeBaseInitStructure.TIM_Prescaler = 0x20CF;               //TIMx时钟频率除数的预分频值 72MHz时1200(n-1)0x04AF 60KHz频率, 84MHz时1400(n-1)0x0577 60KHz频率，84MHz时8400(n-1)0x20CF 10KHz频率
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
    TIM_TimeBaseInitStructure.TIM_Period = 0x2710;                  //自动重装载寄存器周期的值,60KHz频率60000 1S,10KHz频率10000 1S
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStructure);

    TIM_ClearFlag(TIM5, TIM_FLAG_Update);
    TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);

    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV2; //2分频，每2个事件触发一次捕获
    TIM_ICInitStructure.TIM_ICFilter = 0xF;               //数字滤波，采样频率fSAMPLING=fDTS/32， N=8，连续8次采样
    TIM_ICInit(TIM5, &TIM_ICInitStructure);

    TIM_ClearFlag(TIM5, TIM_FLAG_CC2);
    TIM_ClearFlag(TIM5, TIM_FLAG_CC2OF); //重复中断标识
    TIM_ClearITPendingBit(TIM5, TIM_IT_CC2);
    TIM_ITConfig(TIM5, TIM_IT_CC2, ENABLE);
    TIM_Cmd(TIM5, ENABLE);
}