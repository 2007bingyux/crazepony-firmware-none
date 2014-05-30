
 /*    
  *      ____                      _____                  +---+
  *     / ___\                     / __ \                 | R |
  *    / /                        / /_/ /                 +---+
  *   / /   ________  ____  ___  / ____/___  ____  __   __
  *  / /  / ___/ __ `/_  / / _ \/ /   / __ \/ _  \/ /  / /
  * / /__/ /  / /_/ / / /_/  __/ /   / /_/ / / / / /__/ /
  * \___/_/   \__,_/ /___/\___/_/    \___ /_/ /_/____  /
  *                                                 / /
  *                                            ____/ /
  *                                           /_____/
  *                                       
  *  Crazyfile control firmware                                        
  *  Copyright (C) 2011-2014 Crazepony-II                                        
  *
  *  This program is free software: you can redistribute it and/or modify
  *  it under the terms of the GNU General Public License as published by
  *  the Free Software Foundation, in version 3.
  *
  *  This program is distributed in the hope that it will be useful,
  *  but WITHOUT ANY WARRANTY; without even the implied warranty of
  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  *  GNU General Public License for more details.
  * 
  * You should have received a copy of the GNU General Public License
  * along with this program. If not, see <http://www.gnu.org/licenses/>.
  *
  *
  * main.c
  *
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
        Nrf_Irq();
        LedA_on;LedB_on;LedC_on;LedD_on;Delay(900000);LedA_off;LedB_off;LedC_off;LedD_off;Delay(900000*3);
    }
    while((NRF24L01_RXDATA[30]==0xA5)&&(!ParameterWrite())&&NRF24L01_RXDATA[28]==0xA5)//����λ����ȡ��Ҫд��Ĳ�����������λ������ʱʹ�ã�����ÿ�ζ��³���Ĳ���
    {   
        Nrf_Irq();
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
        Nrf_Irq();
        LedA_on;LedB_on;LedC_on;LedD_on;Delay(1);LedA_off;LedB_off;LedC_off;LedD_off;Delay(200);
    }
    for(i=0;i<3;i++)//�����ɹ���������˸3����ʾ
    {
    LedA_on;LedB_on;LedC_on;LedD_on;
    Delay(900000);
    LedA_off;LedB_off;LedC_off;LedD_off;
    Delay(900000);
    }
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

    //int temp;
    SystemClock(9);   //9��Ƶ��ϵͳʱ��Ϊ36MHz
    NVIC_INIT();	     //�жϳ�ʼ��
    STMFLASH_Unlock(); //�ڲ�flash����
    LedInit();		     //IO��ʼ��   
    BT_off;            //�����ر�
    MotorInit();	     //����ʼ��
    BatteryCheckInit();//��ص�ѹ����ʼ��
    //mpu6050��ʼ��˳��
    //1.��ʼ��IIC����
    //2.���øú���MPU6050_Check()����豸�������
    //3.���豸���ڣ����ʼ�����豸�����򲻳�ʼ��
    i2cInit();                //IIC��ʼ��
    while(!MPU6050_Check())   //MPU6050���  M1,M2
    {LedA_on;LedB_on;Delay(7000000);LedA_off;LedB_off;Delay(1000000);}
    MPU6050_INIT(); //���ͨ���ų�ʼ���������ȳ�ʼ���ټ�� 
    while(!NRF24L01_INIT()) //����ģ���ʼ�� M3,M4
    {LedC_on;LedD_on;Delay(100000);LedC_off;LedD_off;Delay(7000000);}//����ģ���ʼ��,��ʼ���ɹ������ƣ����ɹ���������
    SetRX_Mode();   //������ģ��Ϊ����ģʽ
    SYSTICK_INIT(); //ϵͳ��ʱ��ʼ������������ת����yaw��
    SystemReady_OK = 1;//��ʼ��������ڼ��ϵͳ����ʼ�����ڿ��ƼĴ�������ʼִ��ϵͳ����
    PowerOn();      //����LED�Ƶĸ�����˸״̬
    BT_on;           //�Լ���ɣ�������
    ParameterRead();//��flash��ȡ��������
    PID_INIT();     //PID������ʼ�� 
    TIM4_Init(36,1000);	  //��ʱ��4��ʼ������ʱ�������������ݣ�����PID���
    TIM3_Init(36,1000);   //��ʱ��3��ʼ��
    Uart1_init(36,115200); 	//����1��ʼ��  
    while (1);            //�ȴ����ݸ����жϵ���

}





