#ifndef __CLK_H
#define __CLK_H

#ifdef __cplusplus
extern "C"
{
#endif

    extern ErrorStatus CLKHSEStatus; //标记外部晶振启动是否正常 0失败 1正常

    ErrorStatus RccInit(void);

#ifdef __cplusplus
}
#endif

#endif