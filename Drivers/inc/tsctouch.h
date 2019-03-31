#ifndef __TSCTOUCH_H
#define __TSCTOUCH_H


#ifdef __cplusplus
 extern "C" {
#endif

//#define TOUCH_RT      //电阻触摸屏 TSC触摸IC

//#define TSCMIRROR_X
//#define TSCMIRROR_Y
#define TSCMIRROR_XY    //定义X轴和Y轴都反转

#define TOUCHLimiSmall  400
#define TOUCHLimiLarge1  1400
#define TOUCHLimiLarge2  2200

#define TSCCMD_RDX      0x90  //TSC2046测量x轴转换指令 触摸屏方向和液晶屏方向相同时0x90 配置触摸屏XY轴是否对调或者正负反转问题在GUI_TOUCH_SetOrientation函数中处理
#define TSCCMD_RDY	0xD0  //TSC2046测量y轴转换指令
#define TSCCMD_COUNT	0x0C  //TSC2046坐标信息循环读取次数
#define TSCCMD_MINI	0x32//0x1E  //TSC2046连续两次读取坐标信息的差值如果小于
#define TSCCMD_REMO	0x02//0x1E  //TSC2046连续两次读取坐标信息的差值如果小于
//单次读取数据有效是大于50，如果有触摸按下，读取数据一定大于50


extern ErrorStatus RTTouchStatus;//标记电阻屏触摸驱动IC初始化是否正常 0失败 1正常
extern uint16_t TOUCH_AD_LEFT;    //70低分辨率电阻屏
extern uint16_t TOUCH_AD_RIGHT; //1940低分辨率电阻屏
extern uint16_t TOUCH_AD_TOP;    //125低分辨率电阻屏
extern uint16_t TOUCH_AD_BOTTOM;//1760低分辨率电阻屏
extern uint16_t TSC_CoodData[3];//0位表示是否有按键按下，1位X坐标，2位Y坐标

ErrorStatus TSC_Init(void);
uint16_t TSC_ReadAD(uint8_t TSCCMD);
uint16_t TSC_ReadFilter(uint8_t TSCCMD);
uint16_t TSC_ReadCoord(uint8_t TSCCMD);

#ifdef __cplusplus
}
#endif

#endif