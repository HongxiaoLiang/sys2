 #include"mma7361.h"
 
 void MMA7361_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHBPeriphClockCmd(PortCLK, ENABLE);

  GPIO_InitStructure.GPIO_Pin = SleepPin | SelftestPin| gSelectPin ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(SleepGPIO, &GPIO_InitStructure);				// PC1,����ʱ������������
}


 void  MMA7361Init(void)
 {
   MMA7361_GPIO_Config();
 GPIO_SetBits( SleepGPIO, SleepPin  );  //������˯��ģʽ 
    //GPIO_ResetBits( SleepGPIO, SleepPin  );  //����˯��ģʽ 
   GPIO_SetBits( SelftestGPIO, SelftestPin );
//   GPIO_SetBits(  gSelectGPIO, gSelectPin);    //  1  6g ������ ��,������ǳ�����	 
   GPIO_ResetBits(  gSelectGPIO, gSelectPin);	 // 0  1.5g ������	��	,������Գɹ���
//	delay_ms(1);//���Ҽ��ʱ�䣬��֪�Ƿ���ȷ
//    GPIO_ResetBits( SelftestGPIO, SelftestPin );��һ�䵽����ľ�������ã�

 }
