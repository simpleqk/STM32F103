#ifndef __TIMER_H_
#define __TIMER_H_

//generate interupt by timer

typedef void (*PFTIMER)(uint32_t arg);

void Timer_Init(PFTIMER pf, uint32_t argpf);
void Timer_SetPrescaler(uint16_t value);
uint16_t Timer_GetCount(void);

#endif
