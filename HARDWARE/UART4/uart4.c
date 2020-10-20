/*-------------------------------------------------*/
/*            ��γ����STM32ϵ�п�����              */
/*-------------------------------------------------*/
/*                                                 */
/*             ʵ�ִ���2���ܵ�Դ�ļ�               */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "uart4.h"     //������Ҫ��ͷ�ļ�
#include "timer4.h"     //������Ҫ��ͷ�ļ�

#if  UART4_RX_ENABLE                   //���ʹ�ܽ��չ���
char Uart4_RxCompleted = 0;            //����һ������ 0����ʾ����δ��� 1����ʾ������� 
unsigned int Uart4_RxCounter = 0;      //����һ����������¼����2�ܹ������˶����ֽڵ�����
char Uart4_RxBuff[UART4_RXBUFF_SIZE]; //����һ�����飬���ڱ��洮��2���յ�������   	
#endif

/*-------------------------------------------------*/
/*����������ʼ������2���͹���                      */
/*��  ����bound��������                            */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void Uart4_Init(unsigned int bound)
{  	 	
	GPIO_InitTypeDef  GPIO_InitStrue;
	USART_InitTypeDef USART_InitStrue;
	NVIC_InitTypeDef NVIC_InitStrue;
//ʱ��ʹ��	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
//GPIO10ʹ��TX	
	GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;//�����������
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_10;						//10��
	GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOC,&GPIO_InitStrue);					 //�����趨������ʼ��GPIOA_9
//GPIO11ʹ��RX
	GPIO_InitStrue.GPIO_Mode=GPIO_Mode_IN_FLOATING;//�����������
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_11;						//11��
	GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOC,&GPIO_InitStrue);					 //�����趨������ʼ��GPIOA_10
//USART��ʼ��
	USART_InitStrue.USART_BaudRate = bound;                                    //����������
	USART_InitStrue.USART_WordLength = USART_WordLength_8b;                    //8������λ
	USART_InitStrue.USART_StopBits = USART_StopBits_1;                         //1��ֹͣλ
	USART_InitStrue.USART_Parity = USART_Parity_No;                            //����żУ��λ
	USART_InitStrue.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
// UART4_RX_ENABLE               												   //���ʹ�ܽ���ģʽ
	USART_InitStrue.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	               //�շ�ģʽ                                                                              //�����ʹ�ܽ���ģʽ
	  //USART_InitStrue.USART_Mode = USART_Mode_Tx ;	                           //ֻ��ģʽ
//#endif        
    USART_Init(UART4, &USART_InitStrue);                                      //���ô���2	

#if UART4_RX_ENABLE  	         					        //���ʹ�ܽ���ģʽ
	USART_ClearFlag(UART4, USART_FLAG_RXNE);	            //������ձ�־λ
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);          //���������ж�
   NVIC_InitStrue.NVIC_IRQChannel = UART4_IRQn;       //���ô���2�ж�
	NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority=3; //��ռ���ȼ�0
	NVIC_InitStrue.NVIC_IRQChannelSubPriority =3;		//�����ȼ�0
	NVIC_InitStrue.NVIC_IRQChannelCmd = ENABLE;			//�ж�ͨ��ʹ��
	NVIC_Init(&NVIC_InitStrue);	                        //���ô���2�ж�
#endif  

	USART_Cmd(UART4, ENABLE);                              //ʹ�ܴ���2
	
}


//void UART4_IRQHandler(void)
//{
//	u8 UART4_in;
//	if(USART_GetITStatus(UART4,USART_IT_RXNE))
//	{
//			UART4_in=USART_ReceiveData(UART4);
//			USART_SendData(UART4, UART4_in);//�򴮿�3��������
//				while(USART_GetFlagStatus(UART4,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
//	}
//}

/*-------------------------------------------------*/
/*������������2 printf����                         */
/*��  ����char* fmt,...  ��ʽ������ַ����Ͳ���    */
/*����ֵ����                                       */
/*-------------------------------------------------*/

__align(8) char UART4_TxBuff[UART4_TXBUFF_SIZE];  

void u4_printf(char* fmt,...) 
{  
	unsigned int i,length;
	
	va_list ap;
	va_start(ap,fmt);
	vsprintf(UART4_TxBuff,fmt,ap);
	va_end(ap);	
	
	length=strlen((const char*)UART4_TxBuff);		
	while((UART4->SR&0X40)==0);
	for(i = 0;i < length;i ++)
	{			
		UART4->DR = UART4_TxBuff[i];
		while((UART4->SR&0X40)==0);	
	}	
}

/*-------------------------------------------------*/
/*������������2���ͻ������е�����                  */
/*��  ����data������                               */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void u4_TxData(unsigned char *data)
{
	int	i;	
	while((UART4->SR&0X40)==0);
	for(i = 1;i <= (data[0]*256+data[1]);i ++){			
		UART4->DR = data[i+1];
		while((UART4->SR&0X40)==0);	
	}
}
