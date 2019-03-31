#ifndef __RTC_H
#define __RTC_H


#ifdef __cplusplus
 extern "C" {
#endif

#define ASYNCHPREDIV    0x7F            //异步分频因子
#define SYNCHPREDIV     0xFF            //同步分频因子

#define RTCYear		17		//RTC初始化默认年份 只包含十位个位
#define RTCMonth	3		//RTC初始化默认月份
#define RTCDay		3		//RTC初始化默认日
#define RTCWeek		5		//RTC初始化默认星期
#define RTCHour		17		//RTC初始化默认小时
#define RTCMin		00		//RTC初始化默认分钟
#define RTCSec		00		//RTC初始化默认秒

//日历结构体
typedef struct//如果没有typedef就必须用struct Calendar_TypeDef来声明变量
{
    uint8_t hour;
    uint8_t min;
    uint8_t sec;			
    //公历日月年周
    uint8_t w_year;//uint16_t//万年历日期年份不包含千位百位,RTC设置时只设置年份的个位和十位
    uint8_t w_month;
    uint8_t w_date;
    uint8_t week;		 
}Calendar_TypeDef;

//C中定义一个结构体类型要用typedef:
//typedef struct [Student]//(如果没有typedef就必须用struct Student来声明变量)
//{
//int a;
//}Stu;//Stu数据类型名，这里的Stu实际上就是struct Student的别名。Stu==struct Student
//也可以不写Student

//C++中可以与C中定义完全一样，也可以如下
//struct Student//Student是结构体名 typedef不使用
//{
//    ...
//}SU;//SU是结构体类型Student的一个变量


//放在GUI文件中extern uint8_t System_RTCFormat;//记录时间格式只参与GUI界面显示，不参与RTC设置，0是12小时格式，1是24小时格式
//放在GUI文件中uint8_t System_RTCFormat = 1;//记录时间格式只参与GUI界面显示，不参与RTC设置，0是12小时格式，1是24小时格式

extern ErrorStatus RTCStatus;// 标记RTC是否初始化成功 0通讯失败 1正常
extern uint8_t RTCUpTimeFlag;//RTC定时唤醒中断标记，在main函数中跟新RTC时间数据，并在GUI中跟新显示
extern uint8_t RTCDateHead;//万年历日期年份千位百位,RTC设置时只设置年份的个位和十位
extern Calendar_TypeDef RTC_Calendar;	//日历结构体

ErrorStatus RTC_DayTimerInit(void);//初始化RTC,返回0,失败;1,成功;
ErrorStatus RTC_DateTimeSet(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec);//设置时间	
ErrorStatus RTC_AlarmSet(uint8_t hour, uint8_t min, uint8_t sec);
FlagStatus RTC_RangeYear(uint16_t syear, uint8_t smon, uint8_t sday);
FlagStatus RTC_RangeTime(uint8_t hour,uint8_t min,uint8_t sec);
FlagStatus RTC_Leap_Year(uint16_t year);
uint8_t RTC_DayInMonth(uint16_t syear, uint8_t smon);
uint32_t RTC_SecInYear(Calendar_TypeDef calendar);
uint8_t RTC_Get_Week(uint16_t year, uint8_t month, uint8_t day);
uint16_t RTC_BetweenDays(uint16_t syear1, uint8_t smon1, uint8_t sday1, uint16_t syear2, uint8_t smon2, uint8_t sday2);
void RTC_DateTimeGet(void);//更新时间

#ifdef __cplusplus
}
#endif
		 
#endif