#include <math.h>
#include "arm_math.h"
#include "stm32f4xx.h"
#include "dspfunction.h"

int8_t test1 = -2;
int8_t test2 = 0;

void DSPFunction(void)
{
    arm_abs_q7(&test1, &test2, 1);
}