#ifndef __LED_H_
#define __LED_H_

enum LED_NO
{
	ELED_0 = 1,
	ELED_1 = 2,
	
	ELED_ALL
};

void LED_Init(void);
void LED_On(enum LED_NO led);
void LED_Off(enum LED_NO led);

#endif
