#ifndef __TIMER_ENCODER_H_
#define __TIMER_ENCODER_H_

//Timer is automatically driven by two input pulses with a 90-degree phase difference

void Timer_Encoder_Init(void);
uint16_t Timer_Encoder_GetCount(uint8_t bClear);

#endif
