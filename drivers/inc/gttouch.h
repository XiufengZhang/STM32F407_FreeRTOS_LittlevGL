#ifndef _GTTOUCH_H
#define _GTTOUCH_H

#ifdef __cplusplus
extern "C"
{
#endif

//#define TOUCH_GT      //电容触摸屏 GT触摸IC

#define GT9xxSlaveAddress 0x5D    //IC复位时INT引脚低电平
#define GT9xxConfigAddress 0x8047 //配置寄存器地址
#define GT9xxCoordAddress 0x814E  //读取坐标寄存器地址
#define GT9xxIDAddress 0x8140     //读取ID寄存器地址

#define GT_COUNT 0x08 //FT触摸IC坐标信息单次循环读取次数

    extern ErrorStatus GTTouchStatus; //标记GF触摸驱动IC初始化是否正常 0失败 1正常
    extern uint8_t GTTouchUpFlag;     //FTTouch触摸中断标记，在main函数中跟新触摸坐标数据
    extern uint16_t GT_CoodData[4];   //0位表示是否有按键按下，1位X坐标，2位Y坐标，3位触摸面积

    ErrorStatus GT_IIC_Initiali(void);
    void GT_IIC_ReadCoord(void);

#ifdef __cplusplus
}
#endif

#endif
