#ifndef __UDMA_H_
#define __UDMA_H_

//user customized DMA function

void UDMA_Init(uint32_t srcAddr, uint32_t dstAddr, uint16_t size);
void UDMA_TransmitData(void);

#endif
