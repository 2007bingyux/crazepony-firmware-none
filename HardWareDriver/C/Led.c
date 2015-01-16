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
led.c file
��д�ߣ�С��  (Camel)
����E-mail��375836945@qq.com
���뻷����MDK-Lite  Version: 4.23
����ʱ��: 2014-01-28
���ܣ�
1.�ɻ��ĸ�����led IO�ڳ�ʼ��
2.��ʼ��Ĭ�Ϲر�����LED��
------------------------------------
*/

#include "Led.h"
#include "UART1.h"
#include "config.h"

LED_t LEDCtrl;
//�ӿ��Դ�
LEDBuf_t LEDBuf;


/********************************************
              Led��ʼ������
���ܣ�
1.����Led�ӿ�IO�������
2.�ر�����Led(����Ĭ�Ϸ�ʽ)
������
Led�ӿڣ�
Led1-->PA11
Led2-->PA8
Led3-->PB1
Led4-->PB3
��ӦIO=1������
********************************************/
void LedInit(void)
{
    RCC->APB2ENR|=1<<2;    //ʹ��PORTAʱ��	
    RCC->APB2ENR|=1<<3;    //ʹ��PORTBʱ��	

    RCC->APB2ENR|=1<<0;      //ʹ�ܸ���ʱ��	   
    GPIOB->CRL&=0XFFFF0F0F;  //PB1,3�������
    GPIOB->CRL|=0X00003030;
    GPIOB->ODR|=5<<1;        //PB1,3����
  
    GPIOA->CRH&=0XFFFF0FF0;  //PA8,11�������
    GPIOA->CRH|=0X00003003;
    GPIOA->ODR|=9<<0;        //PA1,11����
  
    AFIO->MAPR|=2<<24;      //�ر�JATG,ǧ���ܽ�SWDҲ�رգ�����оƬ���ϣ��ײ�!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    LedA_off;LedB_off;LedC_off;LedD_off;
  //  printf("״̬LED�Ƴ�ʼ�����...\r\n");
}

//�ײ���� ��10Hz
void LEDReflash(void)
{
 
		if(LEDBuf.bits.A)
			LedA_on;
		else
			LedA_off;
		
		if(LEDBuf.bits.B)
			LedB_on;
		else
			LedB_off;
		
		if(LEDBuf.bits.C)
			LedC_on;
		else
			LedC_off;
		
		if(LEDBuf.bits.D)
			LedD_on;
		else
			LedD_off;
		
}

//�¼�������
void LEDFSM(void)
{
//	static uint16_t cnt=0;
//	uint8_t event=0;
	
	switch(LEDCtrl.event)
	{
		case E_READY:
				if(++LEDCtrl.cnt >= 3)		//0 1 2 in loop, 0 on ,else off
					LEDCtrl.cnt=0;
				if(LEDCtrl.cnt==0)
						LEDBuf.byte =LA|LB;
				else
						LEDBuf.byte =0;
			break;
		case E_CALI:
				LEDBuf.byte=LA|LB;
			break;
		case E_BAT_LOW:
				if(++LEDCtrl.cnt >= 3)		//0 1  in loop
					LEDCtrl.cnt=0;
				if(LEDCtrl.cnt==0)
						LEDBuf.byte =0x0f;
				else
						LEDBuf.byte =0;
			break;
		case E_CALI_FAIL:
				if(++LEDCtrl.cnt >= 4)
					LEDCtrl.cnt=0;
				if(LEDCtrl.cnt<2)
						LEDBuf.byte =LC|LD;
				else
						LEDBuf.byte =LA|LB;
			break;
		case E_LOST_RC:
				if(++LEDCtrl.cnt >= 4)
					LEDCtrl.cnt=0;
				LEDBuf.byte= 1<<LEDCtrl.cnt ;
//				if(LEDCtrl.cnt==0)
//						LEDBuf.byte =LA;
//				else
//						LEDBuf.byte =0;
			break;
		case E_AUTO_LANDED:
				 LEDBuf.byte=0x0f;
			break;
	}
	
	LEDReflash();
}


