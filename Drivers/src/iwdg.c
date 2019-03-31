#include "stm32f4xx.h"
#include "stm32f4xx_iwdg.h"
#include "iwdg.h"


/**
  * @brief  独立看门狗初始化
  * @param  no
  * @retval no
  */
void WatchDogInit(void)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_256);
    IWDG_SetReload(0x0FFF);
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable);
    IWDG_Enable();
}

/**
  * @brief  喂独立看门狗
  * @param  no
  * @retval no
  */
void WatchDogFeed(void)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_ReloadCounter();
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable);
}