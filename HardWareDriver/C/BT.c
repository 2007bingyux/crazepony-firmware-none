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
BT.c file
��д�ߣ�С��  (Camel)
����E-mail��375836945@qq.com
���뻷����MDK-Lite  Version: 4.23
����ʱ��: 2014-01-28
���ܣ�
1.����͸��ģ��ĵ�Դʹ�ܶ�BT_EN--->PB2
2.��������Դ-->BT_EN=1;
------------------------------------
*/


#include "BT.h"
#include "delay.h"
#include "UART1.h"
#include "stdio.h"
#include "string.h"
#include "sys_fun.h"
#include "control.h"
#include "Led.h"
	
BTtype CrazepoyBT;//ʵ����һ�������ṹ��
float BTstate;//�����Ƿ���Ҫд������־
	
	
/********************************************
              ������Դ��ʼ������
********************************************/
void BT_PowerInit(void)
{
    RCC->APB2ENR|=1<<3;      //ʹ��PORTBʱ��	
    GPIOB->CRL&=0XFFFFF0FF;  //PB2�������
    GPIOB->CRL|=0X00000300;
    GPIOB->ODR|=1<<2;        //PB2����
    BT_on();   

}


char Cmdreturn[CmdreturnLength];//ָ��ķ��ؽ������

/********************************************
              ������д��һ��ָ���
********************************************/
void Uart1SendaBTCmd(const char *p)
{
  char i;
	
  for(i=0;i<CmdreturnLength;i++) Cmdreturn[i] = 0;//�ͷ�ָ����ջ���
	LedA_on;
	delay_ms(100);//д��һ��ָ���ʱ500ms�ٶȽ��ջ���
	LedA_off;
  for(i=0;i<strlen(p);i++)
  UART1_Put_Char(*(p+i));  
  delay_ms(100);//д��һ��ָ���ʱ500ms�ٶȽ��ջ���
	
	
  i=0;
  while(UartBuf_Cnt(&UartRxbuf) != 0)     //�����ڻ��岻Ϊ��ʱ�������ڻ��帳ֵ��ָ��������
  Cmdreturn[i++] = UartBuf_RD(&UartRxbuf);
}

/********************************************
         �ж�һ��ָ����ǲ��ǵ����趨ֵ
         ����ֵ��0-->ָ�����趨ֵ��ƥ��
                 1-->ָ�����趨ֵƥ��
********************************************/
char CmdJudgement(const char *p)
{
  char i;
  for(i=0;i<strlen(p);i++) if(Cmdreturn[i] != *(p+i)) break;
  if(i != strlen(p)) return 0;
  return 1;
}

const char ATcmdAsk[]    =		 {"AT"};
const char ATcmdAnswer[] =     {"OK"};

const char ATcmdNameAsk[] = 	 {"AT+NAME?"};
const char ATcmdNameAnswer[] =  {"OK+NAME:Crazepony"};	//{BT_BAUD_AT};//
const char ATcmdNameSet[] = 	 {"AT+NAMECrazepony"};    //���������豸��Ϊ��Crazepony����Ȼ�����������޸ĳ� what ever you want...

const char ATcmdCodeAsk[] = 	 {"AT+PIN?"};
const char ATcmdCodeAnswer[] = {"OK+PIN:1234"};	
const char ATcmdCodeSet[] =		 {"AT+PIN1234"};          //�����������Ĭ��Ϊ1234

const char ATcmdBaudAsk[] =		 {"AT+BAUD?"};
const char ATcmdBaudAnswer[] = {"OK+BAUD:115200"};
const char ATcmdBaudSet[] =    {"AT+BAUD8"};            //�޸Ĵ˴��������޸�����������
																//baud1--->1200
																//baud2--->2400
																//baud3--->4800
																//baud4--->9600
																//baud5--->19200
																//baud6--->38400
																//baud7--->57600
																//baud8--->115200                                        


/*�õ�����͸����ǰͨ�Ų�����,���ص�ǰ������ֵ*/
u32 BT_CurBaud_Get(void)
{
	static u32 bandsel[8] = {1200,2400,4800,9600,19200,38400,57600,115200};//�����������ʱ�
  u8 i;

		BT_on();        //������
    delay_ms(500); //�ȴ������ȶ�
		/**ȷ����ǰ����������**/
			for(i=0;i<8;i++)
			{
				UART1_init(SysClock,bandsel[i]); 
				Uart1SendaBTCmd(ATcmdAsk);
				if(CmdJudgement(ATcmdAnswer) == true)
				{
				  //printf("\r\nHM-06 baud -->%d\r\n",bandsel[i]);
					break;//�õ���ǰ������ΪBandsel[i] 
				}
			}
	return bandsel[i];
}

extern void SaveParamsToEEPROM(void);

/********************************************
              д������������
********************************************/
void BT_ATcmdWrite(void)
{
	static	u32 BT_CurBaud;

	BT_CurBaud = BT_CurBaud_Get();
	if((BT_CurBaud == BT_BAUD_Set))  BTstate = BThavewrote;//��⵽������ǰ�Ĳ����ʺ��趨ֵ��ͬ����д���趨ֵ
	else 				BTstate = BTneedwrite;

				if(BTstate == BTneedwrite)
					{
						LedA_off;LedB_off;LedC_off;LedD_off;
						UART1_init(SysClock,BT_CurBaud);//�Ե�ǰ���������³�ʼ������ 
						/*��ʼ���������豸��,pin�룬������*/
						Uart1SendaBTCmd(ATcmdAsk);
						//printf("\r\n������ͨ����...\r\n");
								if(CmdJudgement(ATcmdAnswer) == true)//���������أ�������дָ��
									{
										Uart1SendaBTCmd(ATcmdNameAsk);
											if(CmdJudgement(ATcmdNameAnswer) == false)  {Uart1SendaBTCmd(ATcmdNameSet);LedA_off;LedB_on;LedC_off;LedD_on; }   
												
											else ;
										Uart1SendaBTCmd(ATcmdCodeAsk);
											if(CmdJudgement(ATcmdCodeAnswer) == false) {Uart1SendaBTCmd(ATcmdCodeSet); LedA_on;LedB_off;LedC_on;LedD_off; }
												 
											else ;
										Uart1SendaBTCmd(ATcmdBaudAsk);
											if(CmdJudgement(ATcmdBaudAnswer) == false) {
																																	Uart1SendaBTCmd(ATcmdBaudSet);
																																	LedA_off;LedB_on;LedC_off;LedD_on;												
																																	BTstate = BThavewrote;
																																	SaveParamsToEEPROM();
																																	LedA_on;LedB_on;LedC_on;LedD_on;
																																	delay_ms(1000);
																																	LedA_off;LedB_off;LedC_off;LedD_off;
																																	}//����޸Ĳ�����,��д��EEPROM
														
											else BTstate = BTneedwrite;
									
									}
								else  {BTstate = BTneedwrite; printf("\r\n������ͨ��ʧ��\r\n");}  
					}
					else ;
			UART1_init(SysClock,BT_BAUD_Set);
}

