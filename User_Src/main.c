
/*    
      ____                      _____                  +---+
     / ___\                     / __ \                 | R |
    / /                        / /_/ /                 +---+
   / /   ________  ____  ___  / ____/___  ____  __   __
  / /  / ___/ __ `/_  / / _ \/ /   / __ \/ _  \/ /  / /
 / /__/ /  / /_/ / / /_/  __/ /   / /_/ / / / / /__/ /
 \___/_/   \__,_/ /___/\___/_/    \___ /_/ /_/____  /
                                                 / /
                                            ____/ /
                                           /_____/
*/

/* main.c file
��д�ߣ�С��  (Camel)
����E-mail��375836945@qq.com
���뻷����MDK-Lite  Version: 4.23
����ʱ��: 2014-01-28
���ܣ�
1.�ɻ�Ӳ����ʼ��
2.�ɿس��������ʼ��
3.��ʱ����
4.�ȴ��жϵ���
------------------------------------
*/


#include "config.h"        //�������е�����ͷ�ļ�

//#define  USART_REC_LEN      5        //���ջ���Ԫ��������


uint8_t SystemReady_OK=0;	      	//ϵͳ��ʼ����ɱ�־
uint32_t While1_Lasttime=0;     //�洢whileѭ����ʱ��




/********************************************
              ����LED�ĸ���״̬
********************************************/
void PowerOn()
{
  char i;            //ѭ������
    
  while(NRF24L01_RXDATA[30]!=0xA5)//��֤�յ�һ�����������ݰ�32���ֽ�,�ټ�������ĳ���
    {
        Nrf_Irq();DEBUG_PRINTLN("�ȴ�ң�ؽ���...\r\n");
        LedA_on;LedB_on;LedC_on;LedD_on;Delay(900000);LedA_off;LedB_off;LedC_off;LedD_off;Delay(900000*3);
    }
    DEBUG_PRINTLN("�Ѽ�⵽ң���ź�...\r\n");
    while((NRF24L01_RXDATA[30]==0xA5)&&(!ParameterWrite())&&NRF24L01_RXDATA[28]==0xA5)//����λ����ȡ��Ҫд��Ĳ�����������λ������ʱʹ�ã�����ÿ�ζ��³���Ĳ���
    {   
        Nrf_Irq();DEBUG_PRINTLN("�ȴ�д�����...\r\n");
        LedA_on;LedB_on;LedC_on;LedD_on;Delay(900000*3);LedA_off;LedB_off;LedC_off;LedD_off;Delay(900000);
    }
    for(i=0;i<4;i++)//ѭ����˸4��
    {
    LedA_on;LedB_off;LedC_off;LedD_off;
    Delay(900000);
    LedA_off;LedB_on;LedC_off;LedD_off;
    Delay(900000);
    LedA_off;LedB_off;LedC_on;LedD_off;
    Delay(900000);
    LedA_off;LedB_off;LedC_off;LedD_on;
    Delay(900000);
    }
    while(NRF24L01_RXDATA[27]!=0xA5)//��֤�յ�һ�����������ݰ�32���ֽ�,�ټ�������ĳ���,�ȴ�����
    {
        Nrf_Irq();DEBUG_PRINTLN("�ȴ�����...\r\n");
        LedA_on;LedB_on;LedC_on;LedD_on;Delay(1);LedA_off;LedB_off;LedC_off;LedD_off;Delay(9000);
    }
    for(i=0;i<3;i++)//�����ɹ���������˸3����ʾ
    {
    LedA_on;LedB_on;LedC_on;LedD_on;
    Delay(900000);
    LedA_off;LedB_off;LedC_off;LedD_off;
    Delay(900000);
    }
    DEBUG_PRINTLN("�����ɹ�,�������ģʽ...\r\n");
}

/********************************************
           ʹ���ڲ�DCO����ϵͳʱ��
���ܣ�
1.ʹ���ڲ�HSIʱ�Ӷ���Ƶ��4MHz����ΪPLL����
2.PLL��Ƶϵ��PLLMUL<=13
3.���������PLLMUL��PLL��Ƶϵ��
********************************************/
void SystemClock(char PLLMUL)
{
    RCC->CR|=1<<0;              //�ڲ�����ʱ��ʹ��
    while(!((RCC->CR)&(1<<1))); //�ȴ��ڲ�ʱ���ȶ�����
    RCC->CFGR|=(PLLMUL-2)<<18;  //PPL��Ƶϵ��
    RCC->CFGR|=0<<16;           //PPL����ʱ��Դ,HSI����Ƶ����ΪPLL����Դ=4MHz
    RCC->CR|=1<<24;             //PLLʹ��
    while(!((RCC->CR)&(1<<25)));//�ȴ�PLL�¶�
    RCC->CFGR|=2<<0;           //ϵͳʱ��Դ���ã�PLL�����Ϊϵͳʱ��
}
/********************************************
              �ɿ����������
���ܣ�
1.��ʼ������Ӳ��
2.��ʼ��ϵͳ����
3.��ʼ��PID����
********************************************/
int main(void)
{
  
    SystemReady_OK=0;
    SystemClock(9);   //9��Ƶ��ϵͳʱ��Ϊ36MHz
    UART1_init(36,115200); 	//����1��ʼ��
    NVIC_INIT();	     //�жϳ�ʼ��
    STMFLASH_Unlock(); //�ڲ�flash����
    LedInit();		     //IO��ʼ�� 
    delay_init(36);    //��ʱ��ʼ��
    BT_off;            //�����ر�
    MotorInit();	     //����ʼ��
    BatteryCheckInit();//��ص�ѹ����ʼ��
    IIC_Init();              //IIC��ʼ��
    MPU6050_DMP_Initialize();//��ʼ��DMP����
    while(!NRF24L01_INIT()){DEBUG_PRINTLN("��ʼ��NRF24L01����...\r\n");
    LedC_on;LedD_on;Delay(100000);LedC_off;LedD_off;Delay(7000000);}
    DEBUG_PRINTLN("NRF24L01��ʼ�����...\r\n");
    SetRX_Mode();   //������ģ��Ϊ����ģʽ
    SYSTICK_INIT(); //ϵͳ��ʱ��ʼ������������ת����yaw��
    DEBUG_PRINTLN("�Լ���ɿ�����..\r\n");
    ParameterRead();//��flash��ȡ��������
    PID_INIT();     //PID������ʼ�� 
    //PowerOn();      //����LED�Ƶĸ�����˸״̬
    BT_on;           //�Լ���ɣ�������
    SystemReady_OK = 1;//��ʼ��������ڼ��ϵͳ����ʼ�����ڿ��ƼĴ�������ʼִ��ϵͳ����
    TIM3_Init(36,1000);	  //��ʱ��3��ʼ�������Դ������
    TIM4_Init(36,1000);	  //��ʱ��4��ʼ������ʱ�������������ݣ�����PID���
   
    while (1);            //�ȴ����ݸ����жϵ���



    
}





