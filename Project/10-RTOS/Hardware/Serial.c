#include "stm32f10x.h"                  // Device header
#include "Serial.h"                     // std library header, printf
#if (DUSE_RTOS)
//for rtos environment
#include "FreeRTOS.h"
#include "Task.h"
#include "Queue.h"

static QueueHandle_t g_xTxQueue;
#endif//DUSE_RTOS

//IO default Mapping
//USART1_TX    PA9
//USART1_RX    PA10
//
//USART2_TX    PA2
//USART2_RX    PA3
//
//USART3_TX    PB10
//USART3_RX    PB11

#define DUSE_USART1 1

static uint8_t  g_rxITFlag;
static uint8_t  g_rxCache[64] = {0}; //data frame format maybe is TLV format
static uint8_t *g_prrxCache = g_rxCache;
static uint8_t *g_pwrxCache = g_rxCache;

#if (DUSE_USART1)
#define USARTx                  USART1
#define USARTx_RCC              RCC_APB2Periph_USART1
#define USARTx_Tx_IO            GPIO_Pin_9
#define USARTx_Rx_IO            GPIO_Pin_10
#define USARTx_NVIC_IRQChannel  USART1_IRQn
#else
#define USARTx                  USART2
#define USARTx_RCC              RCC_APB1Periph_USART2
#define USARTx_Tx_IO            GPIO_Pin_2
#define USARTx_Rx_IO            GPIO_Pin_3
#define USARTx_NVIC_IRQChannel  USART2_IRQn
#endif//DUSE_USART1

void Serial_Init(uint32_t baudrate, uint8_t enableRxIT)
{
	g_rxITFlag = enableRxIT;
	
	//Enables or disables the High Speed APB (APB2) peripheral clock
#if (DUSE_USART1)
	RCC_APB2PeriphClockCmd(USARTx_RCC, ENABLE);
#else
	RCC_APB1PeriphClockCmd(USARTx_RCC, ENABLE);
#endif//DUSE_USART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//Initializes the GPIOx peripheral
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = USARTx_Tx_IO; //Tx
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = USARTx_Rx_IO; //Rx
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//Initializes the USARTx peripheral
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //USART_WordLength_8b / USART_WordLength_9b
	USART_InitStructure.USART_Parity = USART_Parity_No; //USART_Parity_No / USART_Parity_Even / USART_Parity_Odd
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //USART_StopBits_1 / USART_StopBits_0_5 / USART_StopBits_2 / USART_StopBits_1_5
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; //USART_Mode_Rx / USART_Mode_Tx
	USART_Init(USARTx, &USART_InitStructure);
	
	if (0 != g_rxITFlag)
	{
		//Enables or disables the specified USART interrupts, for Rx
		USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
#if (DUSE_RTOS)
		//create queue for sending data
		g_xTxQueue = xQueueCreate(64, sizeof(uint8_t));
#endif//DUSE_RTOS
		
		//Configures the priority grouping 
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		
		//Initializes the NVIC peripheral
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel = USARTx_NVIC_IRQChannel;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		NVIC_Init(&NVIC_InitStructure);
	}
	
	USART_Cmd(USARTx, ENABLE);
}

//---------------------------------------------------------

#if (DUSE_RTOS)
void Serial_SendByte(uint8_t data)
{
	BaseType_t xResult = pdTRUE;
	TickType_t xTimeout = pdMS_TO_TICKS(10);  // 10ms timeout
	
	//the xQueueSend function is thread-safe
	//must use xQueueSendFromISR function in interrupt function
	xResult = xQueueSend(g_xTxQueue, &data, xTimeout);
	
	if (pdTRUE == xResult)
	{
		//Enables or disables the specified USART interrupts, for Tx
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	}
}

#else

void Serial_SendByte(uint8_t data)
{
	uint32_t timeout = 10000;
	
	USART_SendData(USARTx, (uint16_t)data);
	
	//while( SET != USART_GetFlagStatus(USARTx, USART_FLAG_TXE) ); //USART_FLAG_TXE / USART_FLAG_TC
	while( (SET != USART_GetFlagStatus(USART1, USART_FLAG_TXE)) && timeout--);
}
#endif//DUSE_RTOS

#if (DUSE_RTOS)
void Serial_SendArray(const uint8_t *array, uint16_t len)
{
	uint16_t i;
	const uint8_t *p = array;
	BaseType_t xResult = pdTRUE;
	TickType_t xTimeout = pdMS_TO_TICKS(10);  // 10ms timeout
	
	//the xQueueSend function is thread-safe
	//must use xQueueSendFromISR function in interrupt function
	for (i=0; i<len; i++)
	{
		xResult = xQueueSend(g_xTxQueue, p, xTimeout);	
		if (pdTRUE == xResult)
		{
			p++;
		}
		else
		{
			break;
		}
	}

	if (pdTRUE == xResult)
	{
		//Enables or disables the specified USART interrupts, for Tx
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	}
}

#else

void Serial_SendArray(const uint8_t *array, uint16_t len)
{
	uint16_t i;
	
	for (i=0; i<len; i++)
	{
		Serial_SendByte(array[i]);
	}
}
#endif//DUSE_RTOS

#if (DUSE_MICROLIB)
//redirect std library functions fputc that printf called to serial
//need enable  "[Target] -> [Use MicroLIB]" option
int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);
	return ch;
}
#endif//DUSE_MICROLIB

//---------------------------------------------------------

int8_t Serial_ReceiveByte(uint8_t *byte)
{
	if (0 == g_rxITFlag)
	{
		//loop query received data
		if (SET == USART_GetFlagStatus(USARTx, USART_FLAG_RXNE))
		{
			*byte = (uint8_t) USART_ReceiveData(USARTx);
			return 0;
		}
		else
		{
			return -1;
		}
	}
	
	else
	{
		//read cache
		if (g_pwrxCache != g_prrxCache)
		{
			*byte = *g_prrxCache++;
			
			if (g_prrxCache > g_rxCache + sizeof(g_rxCache))
			{
				g_prrxCache = g_rxCache;
			}
			
			return 0;
		}
		else
		{
			return -1;
		}
	}
}

int8_t Serial_ReceiveArray(uint8_t *byte, uint8_t *len)
{
	uint8_t i = 0;
	
	if (0 == g_rxITFlag)
	{
		//loop query received data
		if (SET == USART_GetFlagStatus(USARTx, USART_FLAG_RXNE))
		{
			*byte = (uint8_t) USART_ReceiveData(USARTx);
			*len  = 1;
			return 0;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		//read cache
		if (g_pwrxCache != g_prrxCache)
		{
			if (g_pwrxCache > g_prrxCache)
			{
#if (DUSE_MICROLIB)
				//for test
				printf("Serial_ReceiveArray: [begin state] pr=%0xd < pw=%0xd\n", (uint32_t)g_prrxCache, (uint32_t)g_pwrxCache);
#endif//DUSE_MICROLIB
				for (i=0; (i<*len) && (g_prrxCache!=g_pwrxCache); i++)
				{
					*byte++ = *g_prrxCache++;
				}
				*len = i;
				
#if (DUSE_MICROLIB)
				printf("Serial_ReceiveArray: [end state] len=%d, pr=%0xd, pw=%0xd\n", *len, (uint32_t)g_prrxCache, (uint32_t)g_pwrxCache);
#endif//DUSE_MICROLIB
			}
			else
			{
#if (DUSE_MICROLIB)
				//for test
				printf("Serial_ReceiveArray: [begin state] pr=%0xd > pw=%0xd\n", (uint32_t)g_prrxCache, (uint32_t)g_pwrxCache);
#endif//DUSE_MICROLIB

				i = 0;
				while( (g_prrxCache!=g_rxCache+sizeof(g_rxCache)) && (i<*len) )
				{
					*byte++ = *g_prrxCache++;
					i++;
				}
				
				if (g_prrxCache == g_rxCache+sizeof(g_rxCache))
				{
					*byte++ = *g_prrxCache;
					i++;
					
					g_prrxCache = g_rxCache;
				}
				
				while( (g_prrxCache!=g_pwrxCache) && (i<*len) )
				{
					*byte++ = *g_prrxCache++;
					i++;
				}
				*len = i;
				
#if (DUSE_MICROLIB)
				//for test
				printf("Serial_ReceiveArray: [end state] len=%d, pr=%0xd, pw=%0xd, rxCahche range[%0xd ~ %0xd]\n", 
					*len, (uint32_t)g_prrxCache, (uint32_t)g_pwrxCache, 
					(uint32_t)g_rxCache, (uint32_t)(g_rxCache+sizeof(g_rxCache)));
#endif//DUSE_MICROLIB
			}
			
			return 0;
		}
		else
		{
			return -1;
		}
	}	
}

#if (DUSE_USART1)
void USART1_IRQHandler(void)
#else
void USART2_IRQHandler(void)
#endif//DUSE_USART1	
{
#if (DUSE_RTOS)	
	uint8_t data;
	BaseType_t xReturn = pdFALSE;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
#endif//DUSE_RTOS
	
	//receive data
	if (SET == USART_GetITStatus(USARTx, USART_IT_RXNE))
	{
		if (g_pwrxCache > g_rxCache + sizeof(g_rxCache))
		{
			g_pwrxCache = g_rxCache;
		}
		*g_pwrxCache++ = (uint8_t) USART_ReceiveData(USARTx);
		
		USART_ClearITPendingBit(USARTx, USART_IT_RXNE);
	}
	
#if (DUSE_RTOS)
	if (SET == USART_GetITStatus(USARTx, USART_IT_TXE))
	{
		//try get data from queue
		xReturn = xQueueReceiveFromISR(g_xTxQueue, &data, &xHigherPriorityTaskWoken);
		if (pdTRUE == xReturn)
		{
			USART_SendData(USARTx, data);
        }
		else
		{
			//no data to send, disable Tx interrupt temporary
			//not use USART_ClearITPendingBit because the TXE flag cann't be cleared 
            USART_ITConfig(USARTx, USART_IT_TXE, DISABLE);
        }
	}
#endif//DUSE_RTOS
}
