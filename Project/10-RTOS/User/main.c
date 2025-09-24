#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "UDMA.h"
#include "Serial.h"
//rtos header
#include "FreeRTOS.h"
#include "Task.h"
#include "event_groups.h"

#define STACK_SIZE 128 //128*4=512bytes
#define EVENT_BITS 1   //0x0001, 0 position bit

void vTaskFunc1(void * pvParameters)
{
	LED_Init();
	LED_On(ELED_1);

	while(1)
	{
		LED_On(ELED_0);
		//Delay_s(1);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		LED_Off(ELED_0);
		//Delay_s(1);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

void vTaskFunc2(void * pvParameters)
{	
	uint8_t i;
	EventGroupHandle_t pxEvent = (EventGroupHandle_t)pvParameters;
	
	OLED_Init();
		
	OLED_Fill(0xff);
	//Delay_s(1);
	vTaskDelay(1000 / portTICK_PERIOD_MS);

	OLED_Clear();
	//Delay_s(1);
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	
	OLED_DisplayCN(2,0);
	//Delay_s(2);
	vTaskDelay(2000 / portTICK_PERIOD_MS);
	OLED_Clear();	

	OLED_DisplayStr(1,0, "FreeRTOS Mode", EOLED_FONT_6);

	//notify the OLED have initialized
	xEventGroupSetBits(pxEvent, EVENT_BITS);
	
	while(1)
	{
		OLED_DisplayStr(3,0, "Value: ", EOLED_FONT_6);	
		OLED_DisplayNum(3, 7*6, i++, EOLED_FONT_6);
		
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

//void vTaskFunc4(void * pvParameters)
//{
//	uint8_t i = 0;
//	uint8_t sbyte = 0x78;
////	uint8_t rbyte = 0x00;
//	uint8_t sarray[] = "Welcome STM Serial!";
//	uint8_t rarray[10] = {0};
//	uint8_t rlen = 0;
//	EventGroupHandle_t pxEvent = (EventGroupHandle_t)pvParameters;
//	EventBits_t bitsRet = 0;
//	TickType_t xTicksToWait = 200 / portTICK_PERIOD_MS;
//	
//	Serial_Init(9600, 1);
//	//printf("printf function test, %d", sbyte);
//	//Delay_s(1);
//	vTaskDelay(1000 / portTICK_PERIOD_MS);

//	//wait OLED initialization finish
//	while(1)
//	{
//		bitsRet = xEventGroupWaitBits(pxEvent, EVENT_BITS, pdFALSE, pdTRUE, xTicksToWait);
//		if (bitsRet == EVENT_BITS)
//		{
//			break;
//		}
//	}
//	
//	while(1)
//	{
//		//send
//		OLED_DisplayStr(3, 0,   "Send: ", EOLED_FONT_6);
//		OLED_DisplayStr(3, 6*6, "                    ", EOLED_FONT_6);
//		
//		//Serial_SendByte(sbyte);
//		OLED_DisplayNum(3, 6*6, sbyte, EOLED_FONT_6);
//		
//		//Delay_s(1);
//		vTaskDelay(1000 / portTICK_PERIOD_MS);
//		
//		Serial_SendArray(sarray, sizeof(sarray)-1); //ignore string tail '\0'
//		OLED_DisplayStr(3, 6*6, (const char*)sarray, EOLED_FONT_6);
//		
//		//Delay_s(1);
//		vTaskDelay(1000 / portTICK_PERIOD_MS);
//		
//		//printf("你好STM!");
//		//Delay_s(1);
//		vTaskDelay(1000 / portTICK_PERIOD_MS);
//		
//		rlen = sizeof(rarray);
//		if(0 == Serial_ReceiveArray(rarray, &rlen))
//		{
//			OLED_DisplayStr(3, 0,    "ReceiveA: ", EOLED_FONT_6);
//			OLED_DisplayStr(3, 10*6, "                    ", EOLED_FONT_6);
//			
//			//printf("STM receive array data (len=%d): ", rlen);
//			for (i=0; i<rlen; i++)
//			{
//				//printf("%c",  rarray[i]);
//				OLED_DisplayChar(3, (10+1+i)*6, rarray[i], EOLED_FONT_6);
//			}
//			//printf("\n");
//			
//			//Delay_s(1);
//			vTaskDelay(1000 / portTICK_PERIOD_MS);
//		}
//	}
//}

void vTaskFunc3(void * pvParameters)
{
	uint8_t i = 0;
	uint8_t sarray[] = "Welcome STM Serial!";
	uint8_t rarray[10] = {0};
	uint8_t rlen = 0;
	EventGroupHandle_t pxEvent = (EventGroupHandle_t)pvParameters;
	EventBits_t bitsRet = 0;
	TickType_t xTicksToWait = 200 / portTICK_PERIOD_MS;
	
	Serial_Init(9600, 1);

	//wait OLED initialization finish
	while(1)
	{
		bitsRet = xEventGroupWaitBits(pxEvent, EVENT_BITS, pdFALSE, pdTRUE, xTicksToWait);
		if (bitsRet == EVENT_BITS)
		{
			break;
		}
	}
		
	while(1)
	{
		OLED_DisplayStr(5, 0,   "UASRT Send: ", EOLED_FONT_6);
		OLED_DisplayNum(5, 12*6, i++, EOLED_FONT_6);
		Serial_SendByte('A');
		//Serial_SendArray(sarray, sizeof(sarray)-1); //ignore string tail '\0'
		
		rlen = sizeof(rarray);
		if(0 == Serial_ReceiveArray(rarray, &rlen))
		{
			OLED_DisplayStr(7, 0, "Received: ", EOLED_FONT_6);
			OLED_DisplayStr(7, 10*6, (char*)rarray, EOLED_FONT_6);
		}
		else
		{
			OLED_DisplayStr(7, 0, "                    ", EOLED_FONT_6);
		}
		
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

//#define DTEST_AD_DMA 0

//uint8_t A[9] = {0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9};
//uint8_t B[9] = {0};

//void DisplayArray8b(uint8_t row, uint8_t col, const uint8_t *array, uint16_t size)
//{
//	uint16_t i;
//	
//	for (i=0; i<size; i++)
//	{
//		OLED_DisplayNum(row, col+i*(6+2)*2, array[i], EOLED_FONT_6);
//	}
//}


void StartTasks()
{
	BaseType_t xReturn;
    TaskHandle_t xHandle = NULL;
	EventGroupHandle_t xEvent = NULL;

	xEvent = xEventGroupCreate();
		
	//https://www.freertos.org/Documentation/02-Kernel/04-API-references/01-Task-creation/01-xTaskCreate
	xReturn = xTaskCreate(vTaskFunc1, "Task1", STACK_SIZE, NULL, tskIDLE_PRIORITY, &xHandle);
	if( pdPASS == xReturn )
	{
//		vTaskDelete(xHandle);
	}
	
	xReturn = xTaskCreate(vTaskFunc2, "Task2", STACK_SIZE, xEvent, tskIDLE_PRIORITY+1, NULL);
	
	xReturn = xTaskCreate(vTaskFunc3, "Task3", STACK_SIZE, xEvent, tskIDLE_PRIORITY+2, NULL);
	
	vTaskStartScheduler();
	
	vEventGroupDelete(xEvent);
}

int main()
{
	StartTasks();
	
	while(1)
	{
		//Delay_ms(200);
		vTaskDelay(200 / portTICK_PERIOD_MS);
	};
	
}
