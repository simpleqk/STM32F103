#include "stm32f10x.h"                  // Device header
#include "USPI.h"

#define DW25Q16_CMD_DUMMY 0xFF
#define DW25Q16_CMD_PAGE_PROGRAM     0x02
#define DW25Q16_CMD_READ_DATA        0x03
#define DW25Q16_CMD_WRITE_ENABLE     0x06
#define DW25Q16_CMD_WRITE_DISABLE    0x04
#define DW25Q16_CMD_READ_STATUS_REGISTER_1  0x05
#define DW25Q16_CMD_READ_STATUS_REGISTER_2  0x35
#define DW25Q16_CMD_SECTOR_ERASE     0x20
#define DW25Q16_CMD_READ_JEDECID     0x9F
#define DW25Q16_CMD_RELEASE_POWER    0xAB


void W25Q16_Init(void)
{
	USPI_Init();
	
	//release power down
//	USPI_Start();
//	USPI_SwapByte(DW25Q16_CMD_RELEASE_POWER);
//	USPI_Stop();
}

void W25Q16_ReadProperty(uint8_t *manufactureID, uint8_t *typeID, uint8_t *capacityID)
{
	USPI_Start();
	USPI_SwapByte(DW25Q16_CMD_READ_JEDECID); //send instrunction code
	
	//swap for querying result data
	//239(0xEF), 64(0x40), 21(0x15)
	*manufactureID = USPI_SwapByte(DW25Q16_CMD_DUMMY); //0xFF, can be any other value
	*typeID = USPI_SwapByte(DW25Q16_CMD_DUMMY);
	*capacityID = USPI_SwapByte(DW25Q16_CMD_DUMMY);
	
	USPI_Stop();
}

void W25Q16_Write_Enable(void)
{
	USPI_Start();
	
	USPI_SwapByte(DW25Q16_CMD_WRITE_ENABLE);
	
	USPI_Stop();
}

void W25Q16_Wait_Busy(void)
{
	uint32_t timeout = 100000;
	
	USPI_Start();
	
	USPI_SwapByte(DW25Q16_CMD_READ_STATUS_REGISTER_1);
	
	while( (0x01 == (USPI_SwapByte(DW25Q16_CMD_DUMMY) & 0x01)) && (timeout--));
//	while( 0x01 == (USPI_SwapByte(DW25Q16_CMD_DUMMY) & 0x01));
	
	USPI_Stop();
}

void W25Q16_Page_Program(uint32_t address, uint8_t *array, uint16_t size)
{
	uint16_t i;
	
	W25Q16_Write_Enable();
	
	USPI_Start();
	USPI_SwapByte(DW25Q16_CMD_PAGE_PROGRAM); //page size is 256 bytes
	
	//address is 24bit, write new data begin the address, only support bit 1->0, not support 0->1
	USPI_SwapByte(address>>16);
	USPI_SwapByte(address>>8);
	USPI_SwapByte(address);
	
	for (i=0; i<size; i++)
	{
		USPI_SwapByte(array[i]);
	}
	
	USPI_Stop();
	
	W25Q16_Wait_Busy(); //BUSY and WEL bits will be cleared 0 when erase cycle finished
}

void W25Q16_Sector_Erase(uint32_t address)
{
	W25Q16_Write_Enable();
	
	USPI_Start();
	USPI_SwapByte(DW25Q16_CMD_SECTOR_ERASE); //sector size is 4Kb
	
	//address is 24bit
	USPI_SwapByte(address>>16);
	USPI_SwapByte(address>>8);
	USPI_SwapByte(address);
	
	USPI_Stop();
	
	W25Q16_Wait_Busy(); //BUSY and WEL bits will be cleared 0 when erase cycle finished
}

void W25Q16_Read_Data(uint32_t address, uint8_t *array, uint16_t size)
{
	uint16_t i;

	USPI_Start();
	USPI_SwapByte(DW25Q16_CMD_READ_DATA);
	
	//address is 24bit
	USPI_SwapByte(address>>16);
	USPI_SwapByte(address>>8);
	USPI_SwapByte(address);
	
	//read retrieved data
	for (i=0; i<size; i++)
	{
		array[i] = USPI_SwapByte(DW25Q16_CMD_DUMMY);
	}
	
	USPI_Stop();
}
