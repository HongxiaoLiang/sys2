#include "rs485.h"
#include "delay.h"
#include "crc16.h"
#include "led.h"
#include "usart.h"     //������Ҫ��ͷ�ļ�

/*******************************************************************************
* �� �� ��         : RS485_Init
* ��������		   : UART5��ʼ������
* ��    ��         : bound:������
* ��    ��         : ��
*******************************************************************************/  

u8 UART5_RX_BUF[64];                   //���ջ��壬���64�ֽ�
u8 UART5_RX_CNT=0;                       //�����ֽڼ�����
u8 flagFrame=0;                         //֡������ɱ�־�������յ�һ֡������




void RS485_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD,ENABLE); //ʹ��GPIOA\Gʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);//ʹ��USART2ʱ��
	
	/*  ����GPIO��ģʽ��IO�� */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12;	//TX-485	//�������PA2
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;		  //�����������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	
	GPIO_Init(GPIOC,&GPIO_InitStructure);		/* ��ʼ����������IO */
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;	//RX-485	   //��������PA3
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;	    //ģ������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;	//CS-485
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	   //�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	
	//USART2 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(UART5, &USART_InitStructure); //��ʼ����5
	
	USART_Cmd(UART5, ENABLE);  //ʹ�ܴ��� 5
	
	USART_ClearFlag(UART5, USART_FLAG_TC);
		
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//���������ж�

	//Usart2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
	
	RS485_TX_EN=0;				//Ĭ��Ϊ����ģʽ	
}

	

//  1ms��ʱ

void TIM5_Init()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);//ʹ��TIM4ʱ��
	
	TIM_TimeBaseInitStructure.TIM_Period=1000;   //�Զ�װ��ֵ 
	TIM_TimeBaseInitStructure.TIM_Prescaler=72-1; //��Ƶϵ��
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //�������ϼ���ģʽ
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE); //������ʱ���ж�
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;//��ʱ���ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	
	
	TIM_Cmd(TIM5,ENABLE); //ʹ�ܶ�ʱ��	
}


//���㷢�͵����ݳ��ȣ����ҽ����ݷŵ�*buf������                     
u8 UartRead(u8 *buf, u8 len)  
{
	 u8 i;
	if(len>UART5_RX_CNT)  //ָ����ȡ���ȴ���ʵ�ʽ��յ������ݳ���ʱ
	{
		len=UART5_RX_CNT; //��ȡ��������Ϊʵ�ʽ��յ������ݳ���
	}
	for(i=0;i<len;i++)  //�������յ������ݵ�����ָ����
	{
		*buf=UART5_RX_BUF[i];  //�����ݸ��Ƶ�buf��
		buf++;
	}
	UART5_RX_CNT=0;              //���ռ���������
	return len;                   //����ʵ�ʶ�ȡ����
}


u8 rs485_UartWrite(u8 *buf ,u8 len) 										//����
{
	u8 i=0; 
   GPIO_SetBits(GPIOD,GPIO_Pin_7);                                            //����ģʽ
   delay_ms(3);                                                               //3MS��ʱ
    for(i=0;i<=len;i++)
    {
	USART_SendData(UART5,buf[i]);	                                      //ͨ��USARTx���跢�͵�������
	while(USART_GetFlagStatus(UART5,USART_FLAG_TXE)==RESET);             //���ָ����USART��־λ������񣬷������ݿ�λ��־
    }
    GPIO_ResetBits(GPIOD,GPIO_Pin_7);                                         //����Ϊ����ģʽ

}




//���ּĴ�����ֵ
extern int altitude;
extern u8 iaq;
extern u16 temperature1,oiltemper1,humidity1,speed1,ic1;
extern u32 gas1,pressure1,longtitude1,latitude1;
unsigned int regGroup[40];  //Modbus�Ĵ����飬��ַΪ0x00~0x04

//unsigned int regGroup[5]={456,257,789,65500};  //Modbus�Ĵ����飬��ַΪ0x00~0x04


	unsigned char i=0,cnt;
	unsigned int crc;
	unsigned char crch,crcl;
	static u8 len;
	static u8 buf[32];

//  float regGroup[20];  //Modbus�Ĵ����飬��ַΪ0x00~0x04
//���������������������֡�Ľ��գ����ȹ��ܺ�����������ѭ���е���
void UartDriver()
{
	//���ּĴ�����ֵ
	
	regGroup[0] = oiltemper1;
	regGroup[1] = temperature1;
	regGroup[2] = iaq;
	regGroup[3] = gas1/65536;
	regGroup[4] = gas1;
	regGroup[5] = pressure1/65536;
	regGroup[6] = pressure1;	
	regGroup[7] = altitude;
	regGroup[8] = humidity1;
	regGroup[9] = longtitude1/65536;
	regGroup[10] = longtitude1;
	regGroup[11] = latitude1/65536;
	regGroup[12] = latitude1;
	regGroup[13] = speed1;
	regGroup[14] = ic1;
	
	
	regGroup[15] = t1*10000;
	regGroup[16] = t2*10000;
	regGroup[17] = t3*100;
	regGroup[18] = t4*100;
	regGroup[19] = t5*100;
	regGroup[20] = t6*100;
	regGroup[21] = t7*100;	
	regGroup[22] = t8*100;
	regGroup[23] = t9*100;
	regGroup[24] = t10*100;
	regGroup[25] = t11*100;
	regGroup[26] = f1*100;
	regGroup[27] = f2*100;
	regGroup[28] = f3*100;
	regGroup[29] = f4*100;	
	
	

	if(flagFrame)            //֡������ɱ�־�������յ�һ֡������
	{
		flagFrame=0;           //֡������ɱ�־����
		len = UartRead(buf,sizeof(buf));   //�����յ������������������
		if(buf[0]==0x01)                   //�жϵ�ַ�ǲ���0x01
		{
			crc=GetCRC16(buf,len-2);       //����CRCУ��ֵ����ȥCRCУ��ֵ
			crch=crc>>8;    				//crc��λ
			crcl=crc&0xFF;					//crc��λ
			if((buf[len-2]==crch)&&(buf[len-1]==crcl))  //�ж�CRCУ���Ƿ���ȷ
			{
				switch (buf[1])  //��������ִ�в���
				{
					case 0x03:     //������
						if((buf[2]==0x00)&&(buf[3]<=0x05))  //�Ĵ�����ַ֧��0x0000~0x0005
						{
							
							if(buf[3]<=0x04) 
							{
								i=buf[3];//��ȡ�Ĵ�����ַ
								cnt=buf[5];  //��ȡ����ȡ�ļĴ�������
								buf[2]=cnt*2;  //��ȡ���ݵ��ֽ�����Ϊ�Ĵ���*2����modbus����ļĴ���Ϊ16λ
								len=3;							
								while(cnt--)
								{
									buf[len++] = regGroup[i]/256;
//									buf[len++]=0x00;				//�Ĵ������ֽڲ�0
									buf[len++]=regGroup[i++];		//���ֽ�
							}
							
						}
							break;
					}
						else  //�Ĵ�����ַ����֧��ʱ�����ش�����
						{   
							buf[1]=0x83;  //���������λ��1
							buf[2]=0x02;  //�����쳣��Ϊ02-��Ч��ַ
							len=3;
							break;
						}
					case 0x06:           //д�뵥���Ĵ���
						if((buf[2]==0x00)&&(buf[3]<=0x05))   //�Ĵ�����ַ֧��0x0000-0x0005
						{
							if(buf[3]<=0x04)
							{
								i=buf[3];				//��ȡ�Ĵ�����ַ
								regGroup[i]=buf[5];		//����Ĵ�������
//								led3=0;
							}
							len -=2;                 //����-2�����¼���CRC������ԭ֡
							break;
						}
						else  
						{							//�Ĵ�����ַ����֧�֣����ش�����
							buf[1]=0x86;           //���������λ��1
							buf[2]=0x02;           //�����쳣��Ϊ02-��Ч��ַ
							len=3;
							break;
					}
					default:    //������֧�ֵĹ�����
						    buf[1]=0x80;     //���������λ��1
							buf[2]=0x01;     //�����쳣��Ϊ01����Ч����
							len=3;
							break;
				}
			    crc=GetCRC16(buf,len);           //����CRCУ��ֵ
				buf[len++]=crc>>8;           //CRC���ֽ�
				buf[len++]=crc&0xff;        //CRC���ֽ�
				rs485_UartWrite(buf,len);  //������Ӧ֡
			}
		}
	}
}


				
void UartRxMonitor(u8 ms) //���ڽ��ռ��
{
	static u8 UART5_RX_BKP=0;  //����USART2_RC_BKP��ʱ�洢ʫ�䳤����ʵ�ʳ��ȱȽ�
	static u8 idletmr=0;        //������ʱ��
	if(UART5_RX_CNT>0)//���ռ�����������ʱ��������߿���ʱ��
	{
		if(UART5_RX_BKP!=UART5_RX_CNT) //���ռ������ı䣬���ս��յ�����ʱ��������м�ʱ
		{
			UART5_RX_BKP=UART5_RX_CNT;  //��ֵ��������ʵ�ʳ��ȸ�USART2_RX_BKP
			idletmr=0;                    //�����ʱ������
		}
		else                              ////���ռ�����δ�ı䣬�����߿���ʱ���ۼƿ���ʱ��
		{
			//�����һ֡�������֮ǰ�г���3.5���ֽ�ʱ���ͣ�٣������豸��ˢ�µ�ǰ����Ϣ���ٶ���һ���ֽ���һ���µ�����֡�Ŀ�ʼ
			if(idletmr<5)                  //����ʱ��С��1msʱ�������ۼ�
			{
				idletmr +=ms;
				if(idletmr>=5)             //����ʱ��ﵽ1msʱ�����ж�Ϊ1֡�������
				{
					flagFrame=1;//����������־��֡������ϱ�־
				}
			}
		}
	}
	else
	{
		UART5_RX_BKP=0;
	}
}
/*******************************************************************************
* �� �� ��         : UART5_IRQHandler
* ��������		   : UART5�жϺ���
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/ 
void UART5_IRQHandler(void)
{
	u8 res;	                                    //�������ݻ������
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)//���յ�����
	{	 	
		res =USART_ReceiveData(UART5);//;��ȡ���յ�������USART2->DR
		if(UART5_RX_CNT<sizeof(UART5_RX_BUF))    //һ��ֻ�ܽ���64���ֽڣ���Ϊ�趨�����Ը��󣬵��˷�ʱ��
		{
			UART5_RX_BUF[UART5_RX_CNT]=res;  //��¼���յ���ֵ
			UART5_RX_CNT++;        //ʹ����������1 
		}
	}
}
							
						
							
			
	
	
			


