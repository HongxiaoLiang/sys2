  /*
*********************************************************************************************************
*	                                  
*	ģ������ : mmma7361���ٶȴ�����ģ��  ����ļ�û�ж�Ӧ��  
*
*	��    �� : V2.0
*	˵    �� : �ܽŽӿڼ��ܽŶ��� ,����û��ʹ��0g-detect
*			x,y,z,3��Ľӿ���adcdouble.c�еĽӿ����Ӧ��ģ����ֲʱ��ע��
*********************************************************************************************************/
#ifndef __MMA7361_H
#define	__MMA7361_H


#include "stm32f10x.h"
#include "adcdouble.h"
#include"delay.h"

#define   SleepGPIO   GPIOC	 //˯��ģʽ�ܽ�
#define   SleepPin    GPIO_Pin_5
#define   SelftestGPIO   GPIOC//���Ҽ��ܽ�
#define  SelftestPin    GPIO_Pin_4
#define   gSelectGPIO   GPIOC //������ѡ��ܽ�
#define   gSelectPin    GPIO_Pin_0
#define   PortCLK   RCC_APB2Periph_GPIOC
void  MMA7361Init(void); //�������ֻдһ����ʼ��������Ϊ��X,Y,Z��ֵ��������������

#endif /* __ADC_H */


