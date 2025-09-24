#ifndef __AD_DMA_H_
#define __AD_DMA_H_

//sample IO using AD function and auto transmit the AD result to memory by DMA

void AD_DMA_Init(uint16_t *adValue, uint16_t size, uint8_t autoflag);
uint16_t AD_DMA_GetValue(void); //used only if the autoflag=0 in function AD_DMA_Init

#endif
