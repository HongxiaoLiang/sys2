/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*             ʵ�ִ���2���ܵ�ͷ�ļ�               */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __UART4_H
#define __UART4_H

#include "stdio.h"      //������Ҫ��ͷ�ļ�
#include "stdarg.h"		//������Ҫ��ͷ�ļ� 
#include "string.h"     //������Ҫ��ͷ�ļ�

#define UART4_RX_ENABLE     1      //�Ƿ������չ���  1������  0���ر�
#define UART4_TXBUFF_SIZE   1024   //���崮��2 ���ͻ�������С 1024�ֽ�

#if  UART4_RX_ENABLE                          //���ʹ�ܽ��չ���
#define UART4_RXBUFF_SIZE   1024              //���崮��2 ���ջ�������С 1024�ֽ�
extern char Uart4_RxCompleted ;               //�ⲿ�����������ļ����Ե��øñ���
extern unsigned int Uart4_RxCounter;          //�ⲿ�����������ļ����Ե��øñ���
extern char Uart4_RxBuff[UART4_RXBUFF_SIZE]; //�ⲿ�����������ļ����Ե��øñ���
#endif

void Uart4_Init(unsigned int);       
void u4_printf(char*,...) ;          
void u4_TxData(unsigned char *data);

#endif


