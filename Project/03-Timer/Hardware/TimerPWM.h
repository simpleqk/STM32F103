#ifndef __TIMER_PWM_H_
#define __TIMER_PWM_H_

//generate PWM pulse by timer output compare mode

void Timer_PWM_Init(void);
void Timer_PWM_Update_DutyCycle(uint16_t value);
void Timer_PWM_SetPrescaler(uint16_t value);

#endif
