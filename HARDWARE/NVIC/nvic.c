#include "stm32f10x_it.h"
#include "usart.h"
#include "string.h"
#include "nvic.h"

//////////////
/*����ACC��GYR�Ȳ�ͬ�����ݶ�Ӧ���գ����ں��ڼӹ�ʹ��*/
///////////
void USART2_IRQHandler(void)
{
	static uint8_t rebuf[13]={0},i=0;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		rebuf[i++]=USART_ReceiveData(USART2);
		if(rebuf[0]!=0x5a)//�ж�֡ͷ
			i=0;
	  if((i==2)&&(rebuf[1]!=0x5a))//�ж�֡ͷ
			i=0;
		if(i>4)//��i����ֵ=5ʱ�������ֽڽ�����ϣ����ݳ����ֽڽ������
		{
			if(i==rebuf[3]+5)
			{
	       memcpy(RX_BUF,rebuf,i);
				stata=1;
				i=0;
			}
		}
		USART_ClearFlag(USART2,USART_FLAG_RXNE);//���жϱ�־
	}	
}
