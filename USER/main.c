#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h" 
#include "max31865.h"
#include "string.h"
#include "math.h"
#include "gps.h"
#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "main.h"       //������Ҫ��ͷ�ļ�
#include "delay.h"      //������Ҫ��ͷ�ļ�
#include "usart.h"     //������Ҫ��ͷ�ļ�
#include "uart4.h"     //������Ҫ��ͷ�ļ�
#include "timer1.h"     //������Ҫ��ͷ�ļ�
#include "timer2.h"     //������Ҫ��ͷ�ļ�
#include "timer3.h"     //������Ҫ��ͷ�ļ�
#include "timer4.h"     //������Ҫ��ͷ�ļ�
#include "wifi.h"	    //������Ҫ��ͷ�ļ�
#include "mqtt.h"       //������Ҫ��ͷ�ļ�
#include "stdio.h"
#include "system.h"
#include "rs485.h"
#include "crc16.h"

#include "cmath"
#include "adcdouble.h"
#include "mma7361.h"
#define PI	3.141592653	/* pi to machine precision, defined in math.h */
#define TWOPI	(2.0*PI) 

/************************************************
 ALIENTEK ս��STM32F103������ʵ��18
�ڲ��¶ȴ����� ʵ�� 
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/





u8 USART1_TX_BUF[USART3_MAX_RECV_LEN]; 					//����1,���ͻ�����
nmea_msg gpsx; 											//GPS��Ϣ
__align(4) u8 dtbuf[50];   								      //��ӡ������
const u8*fixmode_tbl[4]={"Fail","Fail"," 2D "," 3D "};	//fix mode�ַ��� 

	 char temp1[502];
   char temp2[502];
// ADC1ת���ĵ�ѹֵͨ��MDA��ʽ����flash
   extern __IO u16 ADC_ConvertedValue[1];//
   const u16 K = 2000;//������
    __IO u16 ADC_ConvertedValueLocal[1];
	 float X[2*K];	
	 float datax[K];
	 float fre[K];//�ֲ�����
	 float spec[K];//�ֲ�����
	 float t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11; // 11��ʱ������ֵ���� 
   float f1,f2,f3,f4;
   int fps = 0;
	 int NFFT;
	 typedef unsigned char boolean_T;
	 typedef float real_T;
	 real_T rtNaN;
	 
////////////////////////////////////////////////////////////

double longtitude,latitude,humidity,temperature,oiltemper,speed,ic2;
float ic;
int pressure,gas,altitude;
u8 iaq,altitude1;

u16 oiltemper1,temperature1,humidity1,speed1,adcx1,ic1;
u32 gas1,latitude1,longtitude1,pressure1;

////////////////////////////////////////////////////////////
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_X;
  
  /* 4����ռ���ȼ���4����Ӧ���ȼ� */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  /*��ռ���ȼ��ɴ���жϼ���͵��ж�*/
	/*��Ӧ���ȼ����ȼ�ִ��*/
	NVIC_X.NVIC_IRQChannel = USART2_IRQn;//�ж�����
  NVIC_X.NVIC_IRQChannelPreemptionPriority = 3;//��ռ���ȼ�
  NVIC_X.NVIC_IRQChannelSubPriority = 3;//��Ӧ���ȼ�
  NVIC_X.NVIC_IRQChannelCmd = ENABLE;//ʹ���ж���Ӧ
  NVIC_Init(&NVIC_X);
}


void send_Instruction(void)
{
	uint8_t send_data[4]={0};
	send_data[0]=0xa5;
	send_data[1]=0x55;
	send_data[2]=0x3F;
	send_data[3]=0x39;
	USART_Send_bytes(send_data,4);//����
	
	delay_ms(100);
	
	send_data[0]=0xa5;
	send_data[1]=0x56;
	send_data[2]=0x02;
	send_data[3]=0xfd;
	USART_Send_bytes(send_data,4);//�����Զ����ָ��
	delay_ms(100);
}


//��ʾGPS��λ��Ϣ 
void Gps_Msg_Show(void)
{

 	float tp;		   
	POINT_COLOR=BLUE;  	 
	tp=gpsx.longitude;	   
	sprintf((char *)dtbuf,"Longitude:%.5f %1c   ",tp/=100000,gpsx.ewhemi);	//�õ������ַ���
	longtitude =(tp/=100000) ;
	longtitude1=gpsx.longitude;
 	LCD_ShowString(30,120,200,16,16,dtbuf);	 	   
	tp=gpsx.latitude;	   
	sprintf((char *)dtbuf,"Latitude:%.5f %1c   ",tp/=100000,gpsx.nshemi);	//�õ�γ���ַ���
	latitude=(tp/=100000);
	latitude1=gpsx.latitude;
 	LCD_ShowString(30,140,200,16,16,dtbuf);	 	 
	tp=gpsx.altitude;	   
 	sprintf((char *)dtbuf,"Altitude:%.1fm     ",tp/=10);	    			//�õ��߶��ַ���
 	LCD_ShowString(30,160,200,16,16,dtbuf);	 			   
	tp=gpsx.speed;	
  speed1=	gpsx.speed;
 	sprintf((char *)dtbuf,"Speed:%.3fkm/h     ",tp/=1000);		    		//�õ��ٶ��ַ���	
	speed=tp/=1000;
 	LCD_ShowString(30,180,200,16,16,dtbuf);	 				    
	if(gpsx.fixmode<=3)														//��λ״̬
	{  
		sprintf((char *)dtbuf,"Fix Mode:%s",fixmode_tbl[gpsx.fixmode]);	
	  LCD_ShowString(30,200,200,16,16,dtbuf);			   
	}	 	   
	sprintf((char *)dtbuf,"GPS+BD Valid satellite:%02d",gpsx.posslnum);	 		//���ڶ�λ��GPS������
 	LCD_ShowString(30,220,200,16,16,dtbuf);	    
	sprintf((char *)dtbuf,"GPS Visible satellite:%02d",gpsx.svnum%100);	 		//�ɼ�GPS������
 	LCD_ShowString(30,240,200,16,16,dtbuf);
	
	sprintf((char *)dtbuf,"BD Visible satellite:%02d",gpsx.beidou_svnum%100);	 		//�ɼ�����������
 	LCD_ShowString(30,260,200,16,16,dtbuf);
	
	sprintf((char *)dtbuf,"UTC Date:%04d/%02d/%02d   ",gpsx.utc.year,gpsx.utc.month,gpsx.utc.date);	//��ʾUTC����
	LCD_ShowString(30,280,200,16,16,dtbuf);		    
	sprintf((char *)dtbuf,"UTC Time:%02d:%02d:%02d   ",gpsx.utc.hour,gpsx.utc.min,gpsx.utc.sec);	//��ʾUTCʱ��
  LCD_ShowString(30,300,200,16,16,dtbuf);		
  	
}





	//��ѧ����
	float mean(float data[],int number)//��ֵ
	{
		float sum = 0.0;
		float avr = 0.0;
		for(int i=0;i<number;++i)
		{
			sum += data[i];
		}
	  avr = sum*1.0 / (number*1.0);
	  return avr;
	}
	
	
	float rms(float data[],int number)//������ֵ
	{
	 int i = 0;
	 float sum = 0;
	while(i < number)
	{
		sum += data[i] * data[i];
		i++;
	}
		return sqrt(sum/number);
	}
	
	
	float std(float data[],int number)
{
    float sum = 0.0, mean, standardDeviation = 0.0;
 
    int i;
 
    for(i=0; i<number; ++i)
        sum += data[i];
	
    mean = sum/number;
	
    for(i=0; i<number; ++i)
        standardDeviation += (data[i] - mean)*(data[i] - mean);
 
    return sqrt(standardDeviation/(number-1));
}



boolean_T rtIsNaN(real_T value)
{

#if defined(_MSC_VER) && (_MSC_VER <= 1200)

  return _isnan(value)? TRUE:FALSE;

#else

  return (value!=value)? 1U:0U;

#endif

}

float nanmean(const float varargin_1[K])     //��������
{
  float y;
  int c;
  int k;
  y = 0.0;
  c = 0;

  for (k = 0; k < K; k++) {
    if (!rtIsNaN(varargin_1[k])) {
      y += varargin_1[k];
      c++;
    }
  }

  if (c == 0) {
    y = rtNaN;
  } else {
    y /= (float)c;
  }

  return y;
}



float kurtosis(float data[K])   //�̶�
{
  float s2;
  int n;
  int k;
  float b_x0;
  s2 = 0.0;
  n = 0;
  for (k = 0; k < K; k++) {
    if (!rtIsNaN(data[k])) {
      s2 += data[k];
      n++;
    }
  }

  if (n == 0) {
    s2 = rtNaN;
  } else {
    s2 /= (float)n;
  }

  for (k = 0; k < K; k++) {
    b_x0 = data[k] - s2;
    b_x0 *= b_x0;
    fre[k] = b_x0;
  }

  s2 = nanmean(fre);
  for (n = 0; n < K; n++) {
    fre[n] *= fre[n];
  }

  return nanmean(fre) / (s2 * s2);
}


real_T rtInf;
real_T rtMinusInf;
boolean_T rtIsInf(real_T value)
{
  return ((value==rtInf || value==rtMinusInf) ? 1U : 0U);
}


static float rt_powd_snf(float u0, float u1)    //��������
{
  float y;
  float d0;
  float d1;
  if (rtIsNaN(u0) || rtIsNaN(u1)) {
    y = rtNaN;
  } else {
    d0 = fabs(u0);
    d1 = fabs(u1);
    if (rtIsInf(u1)) {
      if (d0 == 1.0) {
        y = 1.0;
      } else if (d0 > 1.0) {
        if (u1 > 0.0) {
          y = rtInf;
        } else {
          y = 0.0;
        }
      } else if (u1 > 0.0) {
        y = 0.0;
      } else {
        y = rtInf;
      }
    } else if (d1 == 0.0) {
      y = 1.0;
    } else if (d1 == 1.0) {
      if (u1 > 0.0) {
        y = u0;
      } else {
        y = 1.0 / u0;
      }
    } else if (u1 == 2.0) {
      y = u0 * u0;
    } else if ((u1 == 0.5) && (u0 >= 0.0)) {
      y = sqrt(u0);
    } else if ((u0 < 0.0) && (u1 > floor(u1))) {
      y = rtNaN;
    } else {
      y = pow(u0, u1);
    }
  }

  return y;
}


float skew(float data[K])   //����ֵ���㣨ƫ�ȣ�
{
  float m1;
  int n;
  int k;
  float x0;
  m1 = 0.0;
  n = 0;
  for (k = 0; k < K; k++) {
    if (!rtIsNaN(data[k])) {
      m1 += data[k];
      n++;
    }
  }

  if (n == 0) {
    m1 = rtNaN;
  } else {
    m1 /= (float)n;
  }

  for (k = 0; k < K; k++) {
    x0 = data[k] - m1;
    fre[k] = x0 * x0;
    spec[k] = x0;
  }

  m1 = nanmean(fre);
  for (n = 0; n < K; n++) {
    fre[n] *= spec[n];
  }

  return nanmean(fre) / rt_powd_snf(m1, 1.5);
}



//FFT�任����
	void four1(float data[], int nn, int isign)
{
    int n, mmax, m, j, istep, i;
    float wtemp, wr, wpr, wpi, wi, theta;
    float tempr, tempi;
    
    n = nn << 1;
    j = 1;
    for (i = 1; i < n; i += 2) {
	if (j > i) 
		{
	    tempr = data[j];     data[j] = data[i];     data[i] = tempr;
	    tempr = data[j+1]; data[j+1] = data[i+1]; data[i+1] = tempr;
	  }
	m = n >> 1;
	while (m >= 2 && j > m) {
	    j -= m;
	    m >>= 1;
	}
	j += m;
    }
    mmax = 2;
    while (n > mmax) {
	istep = 2*mmax;
	theta = TWOPI/(isign*mmax);
	wtemp = sin(0.5*theta);
	wpr = -2.0*wtemp*wtemp;
	wpi = sin(theta);
	wr = 1.0;
	wi = 0.0;
	for (m = 1; m < mmax; m += 2) {
	    for (i = m; i <= n; i += istep) {
		j =i + mmax;
		tempr = wr*data[j]   - wi*data[j+1];
		tempi = wr*data[j+1] + wi*data[j];
		data[j]   = data[i]   - tempr;
		data[j+1] = data[i+1] - tempi;
		data[i] += tempr;
		data[i+1] += tempi;
	    }
	    wr = (wtemp = wr)*wpr - wi*wpi + wr;
	    wi = wi*wpr + wtemp*wpi + wi;
	}
	mmax = istep;
    }
}




int main(void)
{	


	
	//�ֳ�����
		RS485_Init(9600);
		TIM5_Init(); 
	
	u16 i,rxlen;
	u16 lenx;
	u8 key=0XFF;
	u8 upload=0;	
	u16 adcx,ic;
	float temp;
//	u16 temp5;
	double temp3;
	short temp4;
	uint8_t data_buf[50]={0},count=0;

  float Temperature ,Humidity;
  uint32_t Gas;
  uint32_t Pressure;
  uint16_t IAQ;
	int16_t Altitude=0;
  uint8_t IAQ_accuracy;
	uint16_t temp1=0;
  int16_t temp2=0;
	
	//delay_init(72);//72M 
	extern int pressure;
	
	
//   SystemInit();
	 SystemInit();
   ADC1_Init();
   MMA7361Init();
	 float sum;
	 float high,low;
	 float plus;
	 float plus1;
	 float	plus2;
//	 
	 
	 
	
	
	
	
	
	delay_init();                   //��ʱ���ܳ�ʼ��              
	Uart4_Init(115200);
	TIM4_Init(300,7200);            //TIM4��ʼ������ʱʱ�� 300*7200*1000/72000000 = 30ms
  LED_Init();	                    //LED��ʼ��
	KEY_Init();                     //������ʼ��

	Usart_Int2(9600);
	NVIC_Configuration();//�����ж����ȼ�����
	send_Instruction();//��ģ�鷢��ָ��
	GPIO_SetBits(GPIOB,GPIO_Pin_5);
 
  MAX31865_Init();
	MAX31865_Cfg();  
  
	delay_init();	    	 //��ʱ������ʼ��	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
 //	usmart_dev.init(72);		//��ʼ��USMART		  
	usart3_init(38400);		//��ʼ������3
	POINT_COLOR=RED;
//	LCD_ShowString(30,20,200,16,16,"JIE");	  

//	LCD_ShowString(30,60,200,16,16,"ENVIR TEST");
//	LCD_ShowString(30,80,200,16,16,"KEY0:Upload NMEA Data SW");   

	if(SkyTra_Cfg_Rate(5)!=0)	//���ö�λ��Ϣ�����ٶ�Ϊ5Hz,˳���ж�GPSģ���Ƿ���λ. 
	{
//   	LCD_ShowString(30,120,200,16,16,"SkyTraF8-BD Setting...");
		do
		{
			usart3_init(9600);			//��ʼ������3������Ϊ9600
	  	SkyTra_Cfg_Prt(3);			//��������ģ��Ĳ�����Ϊ38400
			usart3_init(38400);			//��ʼ������3������Ϊ38400
      key=SkyTra_Cfg_Tp(100000);	//������Ϊ100ms
		}while(SkyTra_Cfg_Rate(5)!=0&&key!=0);//����SkyTraF8-BD�ĸ�������Ϊ5Hz
//	  LCD_ShowString(30,120,200,16,16,"SkyTraF8-BD Set Done!!");
		delay_ms(500);
//		LCD_Fill(30,120,30+200,120+16,WHITE);//�����ʾ 
	}
	
	
	WiFi_ResetIO_Init();            //��ʼ��WiFi�ĸ�λIO
  MQTT_Buff_Init();               //��ʼ������,����,�������ݵ� ������ �Լ���״̬����
	AliIoT_Parameter_Init();	    //��ʼ�����Ӱ�����IoTƽ̨MQTT�������Ĳ���	





	
	while(1) 
	{	
///////////////////////////////////////////////////////////////////////////////////////////	

		
			 ADC_ConvertedValueLocal[0] = ADC_ConvertedValue[0]; // ��ȡת����ADֵ
		   datax[fps] =  (ADC_ConvertedValueLocal[0]*3.3/4096-1.65)/0.86;
//    	 printf("datax[fps] = %.2f %d  \r\n", datax[fps],fps);
			 fps++;


     if(fps >= K)
		 { 
			NFFT = (int)pow(2.0, ceil(log((double)K)/log(2.0)));
      delay_ms(100);		 
//			 
			//���ֵ
      high = datax[0];
			low = datax[0];
			sum = 0.0;
      for (i = 0; i < K; i++) 
			 {
         if (high < datax[i]) 
				  {
			    high = datax[i];
          }
					if(low > datax[i])
					{
					low = datax[i];
					}
					sum += fabs(datax[i]);
			 }	
     
	      t7 = sum / (float)K;

	
			 
//����ֵ���㣹����15������ֵ��11��ʱ��ģ�4��Ƶ���
			 t1 = mean(datax,fps); //��ֵ
//			 printf("t1: %.2f",t1);
			 t2 = rms(datax,fps); //������ֵ
			 t3 = std(datax,fps); //��׼��ó����ǳ���N-1
	     t4 = kurtosis(datax); //�Ͷ�
			 t5 = skew(datax);      //ƫ��        
			 t6 = high-low;//���ֵ
			 t8=high/t2;  //��������
			 t9=high/t7;  //����ָ��
			 t10=t2/t7;             //����ָ��
			 t11=t3/t1; 
//			 printf("fps = %d   ", K);
//			 printf("NFFT = %d", NFFT);
//			 printf("\r\n ʱ������ֵ:t1:%.2f t2:%.2f t3:%.2f t4:%.2f t5:%.2f t6:%.2f t7:%.2f t8:%.2f t9:%.2f t10:%.2f t11:%.2f \r\n",t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11);
//			 printf("\r\n ʱ������ֵ:t5:%.2f\r\n",t5);
			 fps = 0; 
       plus = 0.00;
			 plus1 = 0.00;
			 plus2 = 0.00;
			 	

//	/* generate a ramp with 10 numbers */
//	     printf("Nx = %d\r\n", K);
//////	x = (double *) malloc(K * sizeof(double));
	    for(i=0; i<K; i++)
	      {
		     datax[i] = datax[i] - t1;
      	}
	/* calculate NFFT as the next higher power of 2 >= Nx */
        NFFT = (int)pow(2.0, ceil(log((double)K)/log(2.0)));
//      	printf("NFFT = %d\r\n", NFFT);


     //fre����
      for (i = 0; i < (NFFT/2+1); i++) 
	      { 
          fre[i] = (K/2.0) * ((1/(NFFT/2.0)) * (double)i);
//					printf("fre[%d]:%f \r\n",i,fre[i]);
        }
  

	/* allocate memory for NFFT complex numbers (note the +1) */
//	X = (double) malloc((2*NFFT+1) * sizeof(double));

	/* Storing x(n) in a complex array to make it work with four1. 
	This is needed even though x(n) is purely real in this case. */
    	for(i=0; i<K; i++)
	      {
	      	X[2*i+1] = datax[i];
	      	X[2*i+2] = 0.0;
      	}
//	/* pad the remainder of the array with zeros (0 + 0 j) */
	    for(i=K; i<NFFT; i++)
	      {
		      X[2*i+1] = 0.0;
	       	X[2*i+2] = 0.0;
	      }

////	printf("\nInput complex sequence (padded to next highest power of 2):\r\n");
////	for(i=0; i<NFFT; i++)
////	{
////		printf("x[%d] = (%.2f + j %.2f)\r\n", i, X[2*i+1], X[2*i+2]);
////	}

	/* calculate FFT */
	    four1(X, NFFT, 1);
	
//     	printf("\nFFT:\r\n");
	    for(i=0; i<NFFT; i++)
	      {
         if(i < K)
          {
	    		spec[i] = 2*(sqrt((X[2*i+1]/K)*(X[2*i+1]/K)+(X[2*i+2]/K)*(X[2*i+2]/K)));
			    plus += spec[i] * fre[i]; 
			    plus1 += fre[i] * fre[i] *spec[i];
//			    printf("spec[%d] : %.2f\r\n", i, spec[i]);
		      }
//	   	printf("X[%d] = (%.2f + j %.2f)\r\n", i, X[2*i+1]/K,-X[2*i+2]/K);
	      }

	    f1=mean(spec,K);//��ֵƵ��
	    f2=plus/(mean(spec,K)*K);//����Ƶ��
			f3=sqrt(plus1/(mean(spec,K)*K));//������Ƶ��	
			for(i=0; i<K; i++)
					plus2 += (fre[i]-f2)* (fre[i]-f2) *spec[i];
			f4=sqrt(plus2/(mean(spec,K)*K)); //��׼��Ƶ��
////			printf("Ƶ������ֵ��f1: %.2f f2: %.2f f3: %.2f f4: %.2f \r\n \r\n ", f1,f2,f3,f4);
	//--------------------------------------//			

/*--------------------------------------------------------------------*/
	  //�ֳ�����
		UartDriver();  //��������	
		delay_ms(10);
		/*   Connect_flag=1ͬ����������������,���ǿ��Է������ݺͽ���������    */
		/*--------------------------------------------------------------------*/
		if(Connect_flag==1){     
			/*-------------------------------------------------------------*/
			/*                     �����ͻ���������                      */
			/*-------------------------------------------------------------*/
				if(MQTT_TxDataOutPtr != MQTT_TxDataInPtr){                //if�����Ļ���˵�����ͻ�������������
				//3������ɽ���if
				//��1�֣�0x10 ���ӱ���
				//��2�֣�0x82 ���ı��ģ���ConnectPack_flag��λ����ʾ���ӱ��ĳɹ�
				//��3�֣�SubcribePack_flag��λ��˵�����ӺͶ��ľ��ɹ����������Ŀɷ�
				if((MQTT_TxDataOutPtr[2]==0x10)||((MQTT_TxDataOutPtr[2]==0x82)&&(ConnectPack_flag==1))||(SubcribePack_flag==1)){    
					printf("��������:0x%x\r\n",MQTT_TxDataOutPtr[2]);  //������ʾ��Ϣ
					MQTT_TxData(MQTT_TxDataOutPtr);                       //��������
					MQTT_TxDataOutPtr += BUFF_UNIT;                       //ָ������
					if(MQTT_TxDataOutPtr==MQTT_TxDataEndPtr)              //���ָ�뵽������β����
						MQTT_TxDataOutPtr = MQTT_TxDataBuf[0];            //ָ���λ����������ͷ
				} 				
			}//�����ͻ��������ݵ�else if��֧��β
			
			/*-------------------------------------------------------------*/
			/*                     ������ջ���������                      */
			/*-------------------------------------------------------------*/
			if(MQTT_RxDataOutPtr != MQTT_RxDataInPtr){  //if�����Ļ���˵�����ջ�������������														
				printf("���յ�����:");
				/*-----------------------------------------------------*/
				/*                    ����CONNACK����                  */
				/*-----------------------------------------------------*/				
				//if�жϣ������һ���ֽ���0x20����ʾ�յ�����CONNACK����
				//��������Ҫ�жϵ�4���ֽڣ�����CONNECT�����Ƿ�ɹ�
				if(MQTT_RxDataOutPtr[2]==0x20){             			
				    switch(MQTT_RxDataOutPtr[5]){					
						case 0x00 : printf("CONNECT���ĳɹ�\r\n");                            //���������Ϣ	
								    ConnectPack_flag = 1;                                        //CONNECT���ĳɹ������ı��Ŀɷ�
									break;                                                       //������֧case 0x00                                              
						case 0x01 : printf("�����Ѿܾ�����֧�ֵ�Э��汾��׼������\r\n");     //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������
									break;                                                       //������֧case 0x01   
						case 0x02 : printf("�����Ѿܾ������ϸ�Ŀͻ��˱�ʶ����׼������\r\n"); //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������
									break;                                                       //������֧case 0x02 
						case 0x03 : printf("�����Ѿܾ�������˲����ã�׼������\r\n");         //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������
									break;                                                       //������֧case 0x03
						case 0x04 : printf("�����Ѿܾ�����Ч���û��������룬׼������\r\n");   //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������						
									break;                                                       //������֧case 0x04
						case 0x05 : printf("�����Ѿܾ���δ��Ȩ��׼������\r\n");               //���������Ϣ
									Connect_flag = 0;                                            //Connect_flag���㣬��������						
									break;                                                       //������֧case 0x05 		
						default   : printf("�����Ѿܾ���δ֪״̬��׼������\r\n");             //���������Ϣ 
									Connect_flag = 0;                                            //Connect_flag���㣬��������					
									break;                                                       //������֧case default 								
					}				
				}			
				//if�жϣ���һ���ֽ���0x90����ʾ�յ�����SUBACK����
				//��������Ҫ�ж϶��Ļظ��������ǲ��ǳɹ�
				else if(MQTT_RxDataOutPtr[2]==0x90){ 
						switch(MQTT_RxDataOutPtr[6]){					
						case 0x00 :
						case 0x01 : printf("���ĳɹ�\r\n");            //���������Ϣ
							        SubcribePack_flag = 1;                //SubcribePack_flag��1����ʾ���ı��ĳɹ����������Ŀɷ���
									Ping_flag = 0;                        //Ping_flag����
   								    TIM3_ENABLE_30S();                    //����30s��PING��ʱ��
									TIM2_ENABLE_30S();                    //����30s���ϴ����ݵĶ�ʱ��
						          TempHumi_State();                     //�ȷ�һ������
									break;                                //������֧                                             
						default   : printf("����ʧ�ܣ�׼������\r\n");  //���������Ϣ 
									Connect_flag = 0;                     //Connect_flag���㣬��������
									break;                                //������֧ 								
					}					
				}
				//if�жϣ���һ���ֽ���0xD0����ʾ�յ�����PINGRESP����
				else if(MQTT_RxDataOutPtr[2]==0xD0){ 
					printf("PING���Ļظ�\r\n"); 		  //���������Ϣ 
					if(Ping_flag==1){                     //���Ping_flag=1����ʾ��һ�η���
						 Ping_flag = 0;    				  //Ҫ���Ping_flag��־
					}else if(Ping_flag>1){ 				  //���Ping_flag>1����ʾ�Ƕ�η����ˣ�������2s����Ŀ��ٷ���
						Ping_flag = 0;     				  //Ҫ���Ping_flag��־
						TIM3_ENABLE_30S(); 				  //PING��ʱ���ػ�30s��ʱ��
					}				
				}	
				//if�жϣ������һ���ֽ���0x30����ʾ�յ����Ƿ�������������������
				//����Ҫ��ȡ��������
				else if((MQTT_RxDataOutPtr[2]==0x30)){ 
					printf("�������ȼ�0����\r\n"); 		   //���������Ϣ 
					MQTT_DealPushdata_Qs0(MQTT_RxDataOutPtr);  //����ȼ�0��������
				}				
								
				MQTT_RxDataOutPtr += BUFF_UNIT;                     //ָ������
				if(MQTT_RxDataOutPtr==MQTT_RxDataEndPtr)            //���ָ�뵽������β����
					MQTT_RxDataOutPtr = MQTT_RxDataBuf[0];          //ָ���λ����������ͷ                        
			}//������ջ��������ݵ�else if��֧��β
			
			/*-------------------------------------------------------------*/
			/*                     ���������������                      */
			/*-------------------------------------------------------------*/
			if(MQTT_CMDOutPtr != MQTT_CMDInPtr){                             //if�����Ļ���˵�����������������			       
				printf("����:%s\r\n",&MQTT_CMDOutPtr[2]);                 //���������Ϣ
				
				MQTT_CMDOutPtr += BUFF_UNIT;                             	 //ָ������
				if(MQTT_CMDOutPtr==MQTT_CMDEndPtr)           	             //���ָ�뵽������β����
					MQTT_CMDOutPtr = MQTT_CMDBuf[0];          	             //ָ���λ����������ͷ				
			}//��������������ݵ�else if��֧��β	
		}//Connect_flag=1��if��֧�Ľ�β
		
		/*--------------------------------------------------------------------*/
		/*      Connect_flag=0ͬ�������Ͽ�������,����Ҫ�������ӷ�����         */
		/*--------------------------------------------------------------------*/
		else{ 
			printf("��Ҫ���ӷ�����\r\n");                 //���������Ϣ
			TIM_Cmd(TIM4,DISABLE);                           //�ر�TIM4 
			TIM_Cmd(TIM3,DISABLE);                           //�ر�TIM3  
			WiFi_RxCounter=0;                                //WiFi������������������                        
			memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);          //���WiFi���ջ����� 
			if(WiFi_Connect_IoTServer()==0){   			        //���WiFi�����Ʒ�������������0����ʾ��ȷ������if
				printf("����TCP���ӳɹ�\r\n");               //���������Ϣ
				Connect_flag = 1;                            //Connect_flag��1����ʾ���ӳɹ�	
				WiFi_RxCounter=0;                            //WiFi������������������                        
				memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);      //���WiFi���ջ����� 
				MQTT_Buff_ReInit();                          //���³�ʼ�����ͻ�����                    
			}				
		}
//	 	if(!stata)
//		   continue;
//		 stata=0;
	    	if(USART3_RX_STA&0X8000)		//���յ�һ��������GPS
	    	{
		  	rxlen=USART3_RX_STA&0X7FFF;	//�õ����ݳ���
		  	for(i=0;i<rxlen;i++)USART1_TX_BUF[i]=USART3_RX_BUF[i];	   
 		  	USART3_RX_STA=0;		   	//������һ�ν���
		  	USART1_TX_BUF[i]=0;			//�Զ���ӽ�����
		  	GPS_Analysis(&gpsx,(u8*)USART1_TX_BUF);//�����ַ���
		  	Gps_Msg_Show();				//��ʾ��Ϣ	
		   	if(upload)printf("\r\n%s\r\n",USART1_TX_BUF);//���ͽ��յ������ݵ�����1
		    }
				oiltemper1=MAX31865_GetTemp()*100;	
				 if(CHeck(data_buf))
		    {
			   count=0;
		     if(data_buf[2]&0x01) //Temperature
			   {
			     temp2=((uint16_t)data_buf[4]<<8|data_buf[5]);   
           Temperature=(float)temp2/100;
           count=2;
				   temperature1=Temperature*100;
			   }
			    if(data_buf[2]&0x02) //Humidity
			   {  
			  	 temp3=0;
			  	 temp4=0;
			     temp1=((uint16_t)data_buf[4+count]<<8)|data_buf[5+count];
				   Humidity=(float)temp1/100; 
           count+=2;
				   temp3=Humidity ; 
				   temp4=temp3;
				   humidity=temp3;
				   humidity1=temp1;
			   }
			    if(data_buf[2]&0x04) //Pressure
			   {
			     Pressure=((uint32_t)data_buf[4+count]<<16)|((uint16_t)data_buf[5+count]<<8)|data_buf[6+count];
           count+=3;
				   pressure=Pressure;
				   pressure1=Pressure;
				   LCD_ShowxNum(30+11*8,390,Pressure,6,16,0);		//��ʾ�������� 
			   }
			    if(data_buf[2]&0x08) //IAQ_accuracy��IAQ
		  	 {
		  	   IAQ_accuracy=(data_buf[4+count]&0xf0)>>4;
			  	 IAQ=(((uint16_t)data_buf[4+count]&0x000f)<<8)|data_buf[5+count];
           count+=2;
				   iaq=IAQ;
				   LCD_ShowxNum(30+14*8,410,IAQ,4,16,0);		//��ʾ��������
				   LCD_ShowxNum(30+14*8,430,IAQ_accuracy,2,16,0);		//��ʾ��������
			   }
				 if(data_buf[2]&0x10) //Gas
			   {
			     Gas =((uint32_t)data_buf[4+count]<<24)|((uint32_t)data_buf[5+count]<<16)|((uint16_t)data_buf[6+count]<<8)|data_buf[7+count]; 
           count+=4;
			  	 gas=Gas;
			  	 gas1=Gas;
				   LCD_ShowxNum(30+11*8,450,Gas,6,16,0);		//��ʾ��������
			   }
			   if(data_buf[2]&0x10)//����
				 {
				    Altitude=((int16_t)data_buf[4+count]<<8)|data_buf[5+count];
					 altitude=Altitude;
					 altitude1=Altitude;
				 }	 
		    }//if check IAQ


	   }//if(fps>K)			 
		}//while(1)	��ѭ�� 				 
	}//������			 

	
		
		

/*-------------------------------------------------*/
/*���������ɼ���ʪ�ȣ���������������               */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void TempHumi_State(void)
{

//	char temp[1024];  
//	printf("fps = %d   ", K);
//	printf("NFFT = %d", NFFT);
//	printf(temp,"\r\n ʱ������ֵ:t1:%.2f\r\n", t1);
    sprintf(temp1,"{\"oiltemper100\":%2d,\"temperature100\":%2d,\"iaq\":%2d,\"gas\":%2d,\"pressure\":%2d,\"altitude\":%2d,\"humidity100\":%2d,\"longtitude100000\":%2d,\"latitude100000\":%2d,\"speed1000\":%2d,\"IC1000\":%2d}",oiltemper1,temperature1,iaq,gas1,pressure1,altitude,humidity1,longtitude1,latitude1,speed1,ic1);  //�����ظ�ʪ���¶�����
//	  sprintf(temp2,"{\"t1:%.2f\",\"t2:%.2f\",\"t3:%.2f\",\"t4:%.2f\",\"t5:%.2f\",\"t6:%.2f\",\"t7:%.2f\",\"t8:%.2f\",\"t9:%.2f\",\"t10:%.2f\",\"t11:%.2f\",\"f1: %.2f\",\"f2: %.2f\",\"f3: %.2f\",\"f4: %.2f\"}" ,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,f1,f2,f3,f4); 
//	sprintf(temp,"{\"oiltemper100\":%2d,\"temperature100\":%2d,\"iaq\":%2d,\"gas\":%2d,\"pressure\":%2d,\"altitude\":%2d,\"humidity100\":%2d,\"longtitude100000\":%2d,\"latitude100000\":%2d,\"speed1000\":%2d,\"IC1000\":%2d,\"t1:%.2f\",\"t2:%.2f\",\"t3:%.2f\",\"t4:%.2f\",\"t5:%.2f\",\"t6:%.2f\",\"t7:%.2f\",\"t8:%.2f\",\"t9:%.2f\",\"t10:%.2f\",\"t11:%.2f\",\"f1: %.2f\",\"f2: %.2f\",\"f3: %.2f\",\"f4: %.2f\"}" ,oiltemper1,temperature1,iaq,gas1,pressure1,altitude,humidity1,longtitude1,latitude1,speed1,ic1,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,f1,f2,f3,f4);  //�����ظ�ʪ���¶�����
	//	sprintf(temp,"fps = %d  NFFT = %d  ʱ������ֵ:t5:%.2f\r\n", K,NFFT,t5);
	  MQTT_PublishQs0(P_TOPIC_NAME,temp1,strlen(temp1));   //������ݣ�������������	
//		MQTT_PublishQs0(P_TOPIC_NAME,temp2,strlen(temp2)); 
	
}

	
	
