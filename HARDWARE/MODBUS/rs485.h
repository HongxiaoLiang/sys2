#ifndef _rs485_H
#define _rs485_H

#include "system.h"

extern u8 UART5_RX_BUF[64];  //���ջ��棬���64�ֽ�
extern u8 UART5_RX_CNT;


//ģʽ����
#define RS485_TX_EN		PDout(7)	//485ģʽ����.0,����;1,����.
														 
void RS485_Init(u32 bound);
void TIM2_Init(void);
void UartRxMonitor(u8 ms); //���ڽ��ռ��
void UartDriver(void); //������������void UartRead(u8 *buf, u8 len); //���ڽ�������
u8 rs485_UartWrite(u8 *buf2 ,u8 len2);  //���ڷ�������
u8 UartRead(u8 *buf, u8 len) ;
extern float t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11; // 11��ʱ������ֵ���� 
extern float f1,f2,f3,f4;

#endif

