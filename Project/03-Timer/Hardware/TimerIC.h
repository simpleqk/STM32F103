#ifndef __TIMERIC_H_
#define __TIMERIC_H_

//calc pulse peroid by timer input capture mode

void Timer_IC_Init(void);
uint32_t Timer_IC_GetFreq(void);
uint32_t Timer_IC_GetDuty(void);

#endif
