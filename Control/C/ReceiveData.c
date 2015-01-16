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
ReceiveData.c file
��д�ߣ�С��  (Camel)
����E-mail��375836945@qq.com
���뻷����MDK-Lite  Version: 4.23
����ʱ��: 2014-01-28
���ܣ�
1.���պ����ļ�����������2.4G���ݣ�UART1��������
2.�������ݰ����������Ӧ�Ŀ�����
------------------------------------
*/

  
#include "ReceiveData.h"
#include "imu.h"
#include "moto.h"
#include "led.h"
#include "MPU6050.h"
#include "extern_variable.h"
#include "UART1.h"
#include "control.h"
#include "stmflash.h"
#include "dmp.h"
#include "stm32f10x_it.h"
#include "SysConfig.h"
#include "CommApp.h"


uint8_t 		FLY_ENABLE=0;//����ʹ�ܶ�  7/-5    14/15

RC_GETDATA  RC_DATA;//={0,0,0,0},RC_DATA_RAW={0,0,0,0};	// RC_DATA�Ǵ�����������ͨ

extern uint32_t lastGetRCTime;

//��������ReceiveDataFormNRF()
//���룺��
//���: ��
//���������յ���2.4Gң�����ݸ�ֵ����Ӧ�ı���
//���ߣ���
//��ע��û�����У����鲻��
void ReceiveDataFormNRF(void)
{
	
	
 if((NRF24L01_RXDATA[0] == '$')&&(NRF24L01_RXDATA[1] == 'M')&&(NRF24L01_RXDATA[2] == '<'))
	 {
		 switch(NRF24L01_RXDATA[4])
		 {
			 case MSP_SET_4CON:
												 rcData[THROTTLE]=NRF24L01_RXDATA[5]+(NRF24L01_RXDATA[6]<<8);//UdataBuf[6]<<8 | UdataBuf[5];
												 rcData[YAW]=NRF24L01_RXDATA[7]   +  (NRF24L01_RXDATA[8]<<8);   //UdataBuf[8]<<8 | UdataBuf[7];
												 rcData[PITCH]=NRF24L01_RXDATA[9] + (NRF24L01_RXDATA[10]<<8);  //UdataBuf[10]<<8 | UdataBuf[9];
												 rcData[ROLL]=NRF24L01_RXDATA[11] + (NRF24L01_RXDATA[12]<<8); //UdataBuf[12]<<8 | UdataBuf[11];
			 break;
			  case MSP_ARM_IT://MSP_ARM_IT
						armState =REQ_ARM;
				break;
			 case MSP_DISARM_IT:
					armState =REQ_DISARM;
			 break;
			 case MSP_ACC_CALI:
					imuCaliFlag = 1;
			 break;
		 }
		 
	 }	
		lastGetRCTime=millis();		//ms
}


