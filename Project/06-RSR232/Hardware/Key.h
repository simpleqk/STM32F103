#ifndef __KEY_H_
#define __KEY_H_

void Key_Init(void);

//return: 0-none key pressed;
//        1-key1 pressed;
//        2-key2 pressed;
//        3-both key1 and key2 pressed;
uint8_t Key_GetState(void);

#endif
