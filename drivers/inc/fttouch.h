#ifndef _FTTOUCH_H
#define _FTTOUCH_H

#ifdef __cplusplus
extern "C"
{
#endif

#define TOUCH_FT //电容触摸屏 FT触摸IC

#define FTSlaveAddress 0x38      //I2C 7位从地址
#define FTDevideMode 0x00        //模式设置地址 0正常模式，1信息模式，4测试模式
#define FTTouchingThreshold 0x80 //触摸门槛设置地址 默认值是25改为22
#define FTWaterThreshold 0x83    //表面有水触摸门槛设置地址 默认1改为60
#define FTActivePeriod 0x88      //激活周期设置地址 默认值是6改为12
#define FTMonitorPeriod 0x89     //监视周期设置地址 默认值是20改为40
#define FTInterrupt 0xA4         //中断方式设置地址
#define FTLibraryVersion 0xA1    //版本号读取地址
#define FTTouchNumber 0x02       //触摸点数量读取地址

#define FT_COUNT 0x04 //FT触摸IC坐标信息单次循环读取次数

#define FT_PIXEL_ERROR 0x02 //FT触摸IC坐标误差，大于此误差更新emwin

    extern ErrorStatus FTTouchStatus; //标记TF触摸驱动IC初始化是否正常 0失败 1正常
    extern uint8_t FTTouchUpFlag;     //FTTouch触摸中断标记，在main函数中跟新触摸坐标数据
    extern uint16_t FT_CoodData[4];   //0位表示是否有按键按下，1位X坐标，2位Y坐标，3位按下状态，按下 抬起 保持

    ErrorStatus FT_IIC_Initiali(void);
    void FT_IIC_ReadCoord(void);

#ifdef __cplusplus
}
#endif

#endif