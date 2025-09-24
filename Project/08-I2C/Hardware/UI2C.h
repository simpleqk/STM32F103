#ifndef __UI2C_H_
#define __UI2C_H_

void UI2C_Init(void);
void UI2C_Write(uint8_t slave_address, uint8_t data_address, uint8_t *data, uint8_t len);
void UI2C_Read(uint8_t slave_address, uint8_t data_address, uint8_t *data, int len);

#endif
