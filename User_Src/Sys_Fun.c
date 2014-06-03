#include "config.h"
#include "extern_variable.h"



char SysClock;       //����洢ϵͳʱ�ӱ�������λMHz

/********************************************
           ϵͳ�ж����ȼ�����
���ܣ�
1.�����ж����ȼ����ú���ͳһ��װΪ���ж����ȼ����ó�ʼ��
********************************************/
void NVIC_INIT(void)
{
    TimerNVIC_Configuration();//��ʱ���ж�����
    UART1NVIC_Configuration();//����1�ж�����
}

/********************************************
           ʹ���ڲ�DCO����ϵͳʱ��
���ܣ�
1.ʹ���ڲ�HSIʱ�Ӷ���Ƶ��4MHz����ΪPLL����
2.PLL��Ƶϵ��PLLMUL<=13
3.���������PLLMUL��PLL��Ƶϵ��
********************************************/
char SystemClock(char PLLMUL)
{
    RCC->CR|=1<<0;              //�ڲ�����ʱ��ʹ��
    while(!((RCC->CR)&(1<<1))); //�ȴ��ڲ�ʱ���ȶ�����
    RCC->CFGR|=(PLLMUL-2)<<18;  //PPL��Ƶϵ��
    RCC->CFGR|=0<<16;           //PPL����ʱ��Դ,HSI����Ƶ����ΪPLL����Դ=4MHz
    RCC->CR|=1<<24;             //PLLʹ��
    while(!((RCC->CR)&(1<<25)));//�ȴ�PLL�¶�
    RCC->CFGR|=2<<0;           //ϵͳʱ��Դ���ã�PLL�����Ϊϵͳʱ��
    SysClock=4*PLLMUL;         //����ϵͳʱ�ӣ���λMHz
    return SysClock;
}
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
