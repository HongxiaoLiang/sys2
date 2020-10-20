/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*              ����Wifi���ܵ�ͷ�ļ�               */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __WIFI_H
#define __WIFI_H

#include "uart4.h"	    //������Ҫ��ͷ�ļ�

#define RESET_IO(x)    GPIO_WriteBit(GPIOA, GPIO_Pin_12, (BitAction)x)  //PA4����WiFi�ĸ�λ

#define WiFi_printf       u4_printf           //����2���� WiFi
#define WiFi_RxCounter    Uart4_RxCounter    //����2���� WiFi
#define WiFi_RX_BUF       Uart4_RxBuff       //����2���� WiFi
#define WiFi_RXBUFF_SIZE  UART4_RXBUFF_SIZE  //����2���� WiFi

#define SSID   "huawei5i"                     //·����SSID����
#define PASS   "12345678"                 //·��������

void WiFi_ResetIO_Init(void);
char WiFi_SendCmd(char *cmd, int timeout);
char WiFi_Reset(int timeout);
char WiFi_JoinAP(int timeout);
char WiFi_Connect_Server(int timeout);
char WiFi_Smartconfig(int timeout);
char WiFi_WaitAP(int timeout);
char WiFi_Connect_IoTServer(void);


#endif
