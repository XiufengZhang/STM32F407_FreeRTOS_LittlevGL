#include "misc.h"
#include "stm32f4xx.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_pwr.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_rtc.h"
#include "stm32f4xx_syscfg.h"
#include "timer.h"
#include "rtc.h"


ErrorStatus RTCStatus = ERROR;// 标记RTC是否初始化成功 0通讯失败 1正常
uint8_t RTCUpTimeFlag = 0;//RTC定时唤醒中断标记，在main函数中跟新RTC时间数据，并在GUI中跟新显示
uint8_t RTCDateHead = 20;//万年历日期年份千位百位,RTC设置时只设置年份的个位和十位
uint8_t mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};//平年的月份日期表
Calendar_TypeDef RTC_Calendar;//时钟结构体


/**
  * @brief  实时时钟判断是否是闰年
  * @param  year判断年份
  * @retval 是否是闰年
  */
FlagStatus RTC_Leap_Year(uint16_t year)
{
    if(year%400==0)
        return SET;//如果以00结尾,还要能被400整除
    else if(year%4==0 && year%100!=0)
        return SET;//必须能被4整除,但不能被100整除
    else
        return RESET;
}

/**
  * @brief  判断输入日期是否在1970~2099年为合法年份内
  * @param
  * @retval 是否正确
*/
FlagStatus RTC_RangeYear(uint16_t syear, uint8_t smon, uint8_t sday)
{
    if(syear>=1970 && syear<=2099)
    {
        if(smon>=1 && smon<=12)
        {
            if(RTC_Leap_Year(syear) && smon==2)
            {
                if(sday>=1 && sday<=mon_table[smon-1]+1)
                    return SET;
                else
                    return RESET;
            }
            else
            {
                if(sday>=1 && sday<=mon_table[smon-1])
                    return SET;
                else
                    return RESET;
            }
        }
        else
            return RESET;
    }
    else
        return RESET;
}

/**
  * @brief  判断输入时间是否在合法数据内
  * @param
  * @retval 是否正确
  */
FlagStatus RTC_RangeTime(uint8_t hour,uint8_t min,uint8_t sec)
{
    if(hour<=23 && min<=59 && sec<=59)
        return SET;
    else
        return RESET;
}

/**
  * @brief  根据给定的年月，求出该月有多少天
  * @param
  * @retval 第几天
  */
uint8_t RTC_DayInMonth(uint16_t syear, uint8_t smon)
{
    uint8_t DataTemp = 0;

    if(RTC_Leap_Year(syear) && smon==2)
        DataTemp = mon_table[smon-1] +1;
    else
        DataTemp = mon_table[smon-1];
    return DataTemp;
}

/**
  * @brief  根据给定的日期，求出它在该年的第几天
  * @param
  * @retval 第几天
  */
uint16_t RTC_DayInYear(uint16_t syear, uint8_t smon, uint8_t sday)
{
    uint16_t DataTemp = 0;

    if(RTC_Leap_Year(syear) && smon>2)
        DataTemp += 1;
    for(uint8_t i=0; i < smon - 1; i++)
    {
        DataTemp += mon_table[i];
    }
    DataTemp += sday;
    return DataTemp;
}

/**
  * @brief  当前时间的秒数，在1970~2099年为合法年份内，时间也要合法
  * @param
  * @retval 秒数
  */
uint32_t RTC_SecInYear(Calendar_TypeDef calendar)
{
    uint32_t seccount = 0;
    uint32_t t = 0;
    
    if(RTC_RangeYear(calendar.w_year, calendar.w_month, calendar.w_date) && RTC_RangeTime(calendar.hour, calendar.min, calendar.sec))
    {
        for(t=1970; t<calendar.w_year; t++)//把所有年份的秒钟相加
        {
            if(RTC_Leap_Year(t))
                seccount += 31622400;//闰年的秒钟数
            else seccount += 31536000;//平年的秒钟数
        }
        for(t=0; t<calendar.w_month-1; t++)//把前面月份的秒钟数相加
        {
            seccount += (uint32_t)mon_table[t] * 86400;//月份秒钟数相加
            if(RTC_Leap_Year(calendar.w_year) && t == 1)
                seccount += 86400;//闰年2月份增加一天的秒钟数	   
        }
        seccount += (uint32_t)(calendar.w_date - 1) * 86400;//把前面日期的秒钟数相加 
        seccount += (uint32_t)calendar.hour * 3600;//小时秒钟数
        seccount += (uint32_t)calendar.min * 60;//分钟秒钟数
        seccount += calendar.sec;//最后的秒钟加上去
    }
    else
        seccount = 0;
    
    return seccount;
}

/**
  * @brief  获取两个日期的间隔天数，在1970~2099年为合法年份内，如果日期1大于日期2返回0
  * @param
  * @retval 几天
  */
uint16_t RTC_BetweenDays(uint16_t syear1, uint8_t smon1, uint8_t sday1, uint16_t syear2, uint8_t smon2, uint8_t sday2)
{
    uint16_t DataTemp = 0;

    if(RTC_RangeYear(syear1, smon1, sday1) && RTC_RangeYear(syear2, smon2, sday2))
    {
        if(syear1 == syear2 && smon1 == smon2)
        {
            if(sday1 < sday2)
                DataTemp = sday2-sday1;
            else
                DataTemp = 0;
        }
        else if(syear1 == syear2)
        {
            if(smon1 < smon2)
                DataTemp = RTC_DayInYear(syear2, smon2, sday2)-RTC_DayInYear(syear1, smon1, sday1);
            else
                DataTemp = 0;
        }
        else if(syear1 < syear2)
        {
            if(RTC_Leap_Year(syear1))
                DataTemp = 366-RTC_DayInYear(syear1, smon1, sday1);
            else
                DataTemp = 365-RTC_DayInYear(syear1, smon1, sday1);
            DataTemp += RTC_DayInYear(syear2, smon2, sday2);
            for(uint16_t i=syear1+1; i<syear2; i++)
            {
                if(RTC_Leap_Year(i))
                    DataTemp += 366;
                else
                    DataTemp += 365;
            }
        }
        else
            DataTemp = 0;
    }
    else
        DataTemp = 0;
    return DataTemp;
}

/**
  * @brief  输入公历日期得到星期，在1970~2099年为合法年份内。把一月和二月看成是上一年的十三月和十四月，例：如果是2004-1-10则换算成：2003-13-10来代入公式计算
  * @param  年月日
  * @retval 星期号 1~7是周一至周日
  */
uint8_t RTC_Get_Week(uint16_t year, uint8_t month, uint8_t day)//基姆拉尔森计算公式
{
    uint8_t Week = 0;

    if(RTC_RangeYear(year, month, day))
    {
        if(month<3)
            Week = (day + 2 * (month + 12) + 3 * (month + 13) / 5 + (year - 1) + (year - 1) / 4 - (year - 1) / 100 + (year - 1) / 400) % 7+1;
        else
            Week = (day + 2 * month + 3 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400) % 7+1;
    }
    return Week;
}

/**
  * @brief  实时时钟日期时间设置
  * @param  //1970~2099年为合法年份
  * @retval ErrorStatus
  */
ErrorStatus RTC_DateTimeSet(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{
    uint8_t Week = 0;
    RTC_TimeTypeDef RTC_TimeStructure;
    RTC_DateTypeDef RTC_DateStructure;

    if(RTC_RangeYear(syear, smon, sday) && RTC_RangeTime(hour, min, sec))
    {
        Week = RTC_Get_Week(syear, smon, sday);
        if(Week)
        {
            RTCDateHead = syear/100;//存储年份的百位和千位
            //设置时间
            RTC_TimeStructure.RTC_Hours = hour;
            RTC_TimeStructure.RTC_Minutes = min;
            RTC_TimeStructure.RTC_Seconds = sec;
            RTC_TimeStructure.RTC_H12 = RTC_H12_AM;
            if(!RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure))
                return ERROR;
            //设置日期
            RTC_DateStructure.RTC_WeekDay = Week;
            RTC_DateStructure.RTC_Month = smon;
            RTC_DateStructure.RTC_Date = sday;
            RTC_DateStructure.RTC_Year = syear%100;
            if(!RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure))
                return ERROR;
            return SUCCESS;
        }
        else
            return ERROR;
    }
    else
        return ERROR;
}

/**
  * @brief  实时时钟中断初始化
  * @param  None
  * @retval None
  */
void RTCWake_NVIC_Config(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    EXTI_ClearITPendingBit(EXTI_Line22);//EXTI线22连接到RTC唤醒事件
    EXTI_InitStructure.EXTI_Line = EXTI_Line22;//EXTI线22连接到RTC唤醒事件
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;//RTC周期唤醒中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;////先占优先级2位,从优先级2位
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能该通道中断
    NVIC_Init(&NVIC_InitStructure);//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

/**
  * @brief  实时时钟初始化
  * @param  None
  * @retval ErrorStatus
  */
ErrorStatus RTC_DayTimerInit(void)
{
    //检查是不是第一次配置时钟
    uint16_t temp=0;
    RTC_InitTypeDef RTC_InitStructure;
    RTC_TimeTypeDef RTC_TimeStructure;
    RTC_DateTypeDef RTC_DateStructure;

    //PWR_BackupAccessCmd(ENABLE);//使能后备寄存器访问 在clk文件中
    if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x5050)//从指定的后备寄存器中读出数据:读出了与写入的指定数据不相乎
    {
        //RCC_BackupResetCmd(ENABLE);	//复位备份区域
        //RCC_LSEConfig(RCC_LSE_ON); //在clk文件中 缩短等待时间
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET && temp<350)	//检查指定的RCC标志位设置与否,等待低速晶振就绪
        {
            temp++;
            STM32Delay_ms(10);
        }
        if(temp>=350)
            return ERROR;//初始化时钟失败,晶振有问题
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟
        RCC_RTCCLKCmd(ENABLE);	//使能RTC时钟
        //RTC_WaitForSynchro();//等待RTC寄存器同步
        RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
        RTC_InitStructure.RTC_AsynchPrediv = ASYNCHPREDIV;
        RTC_InitStructure.RTC_SynchPrediv = SYNCHPREDIV;
        if(RTC_Init(&RTC_InitStructure))//包含解锁，包含进入初始化模式
        {
            //设置时间
            RTC_TimeStructure.RTC_Hours = RTCHour;
            RTC_TimeStructure.RTC_Minutes = RTCMin;
            RTC_TimeStructure.RTC_Seconds = RTCSec;
            RTC_TimeStructure.RTC_H12 = RTC_H12_AM;
            if(!RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure))
                return ERROR;
            //设置日期
            RTC_DateStructure.RTC_WeekDay = RTCWeek;
            RTC_DateStructure.RTC_Month = RTCMonth;
            RTC_DateStructure.RTC_Date = RTCDay;
            RTC_DateStructure.RTC_Year = RTCYear;
            if(!RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure))
                return ERROR;
            if(!RTC_WakeUpCmd(DISABLE))
                return ERROR;
            while(!RTC_GetFlagStatus(RTC_FLAG_WUTWF));//允许更新唤醒定时器配置
            RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
            RTC_SetWakeUpCounter(0);//(WUT+1) 个 ck_wut 周期唤醒，即写入0是1s周期唤醒，写入1是2s周期唤醒
            RTC_ClearITPendingBit(RTC_IT_WUT); //清除唤醒中断标识
            RTC_ITConfig(RTC_IT_WUT, ENABLE);//使能RTC周期唤醒中断
            RTC_WakeUpCmd(ENABLE);//使能RTC周期唤醒
            //RTCWake_NVIC_Config();//RCT中断分组设置
        }
        else
            return ERROR;
        RTC_WriteBackupRegister(RTC_BKP_DR0, 0x5050);	//向指定的后备寄存器中写入用户程序数据
    }
    RTC_WaitForSynchro();//等待RTC寄存器同步
    RTCWake_NVIC_Config();//RCT中断分组设置
    RTC_ClearITPendingBit(RTC_IT_WUT); //清除唤醒中断标识 清除中断标识才能再次触发
    RTC_DateTimeGet();//更新时间
    return SUCCESS;
}

/**
  * @brief  实时时钟中断初始化
  * @param  None
  * @retval None
  */
void RTCAlar_NVIC_Config(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    EXTI_InitStructure.EXTI_Line = EXTI_Line17;//EXTI线17连接到RTC闹钟事件
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;//RTC闹钟中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;////先占优先级2位,从优先级2位
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能该通道中断
    NVIC_Init(&NVIC_InitStructure);//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

/**
  * @brief  实时时钟设置闹钟
  * @param  //闹钟时间
  * @retval ErrorStatus
  */
ErrorStatus RTC_AlarmSet(uint8_t hour, uint8_t min, uint8_t sec)
{
    RTC_AlarmTypeDef RTC_AlarmTypeInitStructure;
    RTC_TimeTypeDef RTC_TimeTypeInitStructure;

    if(RTC_RangeTime(hour, min, sec))
    {
        if(!RTC_AlarmCmd(RTC_Alarm_A,DISABLE))//关闭闹钟A
            return ERROR;
        RTC_TimeTypeInitStructure.RTC_Hours = hour;//时
	RTC_TimeTypeInitStructure.RTC_Minutes = min;//分
	RTC_TimeTypeInitStructure.RTC_Seconds = sec;//秒
	RTC_TimeTypeInitStructure.RTC_H12 = RTC_H12_AM;

        RTC_AlarmTypeInitStructure.RTC_AlarmTime = RTC_TimeTypeInitStructure;//指定闹钟时间
        RTC_AlarmTypeInitStructure.RTC_AlarmMask = RTC_AlarmMask_None;//日期或者星期、小时、分钟、秒都有效
        RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;//选择天还是星期
        RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDay = 1;//1号闹钟
        RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmTypeInitStructure);

        RTC_ClearITPendingBit(RTC_IT_ALRA);//清除闹钟中断标识
        EXTI_ClearITPendingBit(EXTI_Line17);//EXTI线17连接到RTC闹钟事件

	RTC_ITConfig(RTC_IT_ALRA, ENABLE);//RTC闹钟中断
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);//RTC闹钟
        return SUCCESS;
    }
    else
        return ERROR;
}

/**
  * @brief  实时时钟获取时间
  * @param  //1970~2099年为合法年份 //以1970年1月1日为基准
  * @retval ErrorStatus
  */
void RTC_DateTimeGet(void)
{
    RTC_TimeTypeDef RTC_TimeStructure;
    RTC_DateTypeDef RTC_DateStructure;

    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
    RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
    RTC_Calendar.w_year = RTC_DateStructure.RTC_Year;//+RTCDateHead*100
    RTC_Calendar.w_month = RTC_DateStructure.RTC_Month;
    RTC_Calendar.w_date = RTC_DateStructure.RTC_Date;
    RTC_Calendar.week = RTC_DateStructure.RTC_WeekDay;

    RTC_Calendar.hour = RTC_TimeStructure.RTC_Hours;
    RTC_Calendar.min = RTC_TimeStructure.RTC_Minutes;
    RTC_Calendar.sec = RTC_TimeStructure.RTC_Seconds;
}