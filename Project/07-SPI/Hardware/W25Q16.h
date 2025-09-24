#ifndef __W25Q16_H_
#define  __W25Q16_H_

void W25Q16_Init(void);

void W25Q16_ReadProperty(uint8_t *manufactureID, uint8_t *typeID, uint8_t *capacityID);
void W25Q16_Read_Data(uint32_t address, uint8_t *array, uint16_t size);

void W25Q16_Sector_Erase(uint32_t address);
void W25Q16_Page_Program(uint32_t address, uint8_t *array, uint16_t size);

#endif
