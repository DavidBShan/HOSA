#ifndef __DS18B20_H
#define __DS18B20_H 
#include "sys.h"   
   	
//�����Ҫ�޸����ţ�ֻ���޸�����ĺ�
#define RCC_DS18B20_PORT 	    RCC_APB2Periph_GPIOB		/* GPIO�˿�ʱ�� */
#define DS18B20_GPIO_PIN      GPIO_Pin_9
#define DS18B20_GPIO_PORT     GPIOB

//IO�������ã�CRL�Ĵ�����Ӧ����0~7,CRH�Ĵ�����Ӧ����8~15��
#define DS18B20_IO_IN()  {DS18B20_GPIO_PORT->CRH&=0xFFFFFF0F;DS18B20_GPIO_PORT->CRH|=0x00000080;}
#define DS18B20_IO_OUT() {DS18B20_GPIO_PORT->CRH&=0xFFFFFF0F;DS18B20_GPIO_PORT->CRH|=0x00000030;}

#define DS18B20_OUT_0   GPIO_ResetBits(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN)//IOΪ�͵�ƽ
#define DS18B20_OUT_1   GPIO_SetBits(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN)//IOΪ�ߵ�ƽ
#define READ_DS18B20_IO GPIO_ReadInputDataBit(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN)//��ȡIO��ƽ

void DS18B20_GPIO_Init(void);
u8 DS18B20_Init(void);			//��ʼ��DS18B20
float ReadTemperature(void); //��ȡ�¶�ֵ

#endif

















