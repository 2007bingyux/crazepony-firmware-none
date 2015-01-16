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
��д�ߣ�С��  (Camel) �� ��(Samit)
����E-mail��375836945@qq.com
���뻷����MDK-Lite  Version: 4.23
����ʱ��: 2014-01-28
���ܣ� 
1. Ӳ������
2. ���п��ƣ����ȡ����ߡ�����ͷ���Զ����䡢���ϱ���
3. ֧��App��2401 RCͬʱ����
4. App �� PC�����߼�ء����ߵ���
------------------------------------
*/
#include "SysConfig.h"
#include "config.h"        //�������е�����ͷ�ļ�
#include "imu.h"
#include "Altitude.h"
#include "CommApp.h"
#include "CommPC.h"
#include "ConfigTable.h"
#include "IMUSO3.h"
#include "control.h"
#include "FailSafe.h"

//#define STOP_MOTOR_FOREVER
 
//sw counter
uint16_t  batCnt; 
//check executing time and period in different loop
uint32_t startTime[5],execTime[5];
uint32_t realExecPrd[5];	//us , real called period in different loop
 
/********************************************
              �ɿ����������
���ܣ�
1.��ʼ������Ӳ��
2.��ʼ��ϵͳ����
********************************************/
int main(void)
{

	SystemClock_HSE(9);           //ϵͳʱ�ӳ�ʼ����ʱ��Դ�ⲿ����HSEs  8*9=72MHz;
	cycleCounterInit();				// Init cycle counter
	SysTick_Config(SystemCoreClock / 1000);	//SysTick����ϵͳtick��ʱ������ʼ�����жϣ�1ms


	UART1_init(SysClock,BT_BAUD_Set); //����1��ʼ��
	
  NVIC_INIT();	                //�жϳ�ʼ��

  STMFLASH_Unlock();            //�ڲ�flash����

  LoadParamsFromEEPROM();

  LedInit();	                //IO��ʼ��
  //delay_init(SysClock);       //�δ���ʱ��ʼ�� ,  ���ô˷�ʽ��
  BT_PowerInit();               //������Դ��ʼ����ɣ�Ĭ�Ϲر�
  MotorInit();	                //����ʼ��
  BatteryCheckInit();           //��ص�ѹ����ʼ��
  IIC_Init();                   //IIC��ʼ��
#ifdef IMU_SW										//ʹ���������
	MPU6050_initialize();
#else
  MPU6050_DMP_Initialize();     //��ʼ��DMP����
#endif        
  //HMC5883L_SetUp();           //��ʼ��������HMC5883L

  NRF24L01_INIT();              //NRF24L01��ʼ��
  SetRX_Mode();                 //������ģ��Ϊ����ģʽ

  PowerOn();                    //�����ȴ�
  BT_ATcmdWrite();          //����д����
 
	BatteryCheck();

	MS5611_Init();

	IMU_Init();			// sample rate and cutoff freq.  sample rate is too low now due to using dmp.

	 TIM4_Init(SysClock,1000);	    //��ʱ��4��ʼ������ʱ�������������ݣ�����PID�������ʱ����ʱ��ʯΪ1us��PID��������Ϊ4ms��������̬����Ƶ�� Ϊ250Hz
	 MotorPwmFlash(10,10,10,10);
		
	altCtrlMode=MANUAL;
	WaitBaroInitOffset();		//�ȴ���ѹ��ʼ���߶����
		
  while (1)                    //�ȴ����ݸ����жϵ���
  {  
		/*Use DMP in MPU6050 for imu , it's accurate but slow and time costing and time unstable */
 		//special freq for dmp. 1000/7. use 3-5ms if normal
		//if miss time becasue of other long time task, dmp maybe  need to use 10ms
#ifndef IMU_SW
		#ifdef DEBUG_NEW_CTRL_PRD
		if(anyCnt>=7)	//take about 3ms, to develop a faster control
		#else
		if(anyCnt>=5) //it will take about 9ms to read. since this prd 5ms is as same as the set outpur rate in dmp. . which fit to old control
		#endif
		{
				anyCnt=0;
				realExecPrd[0]=micros()-startTime[0];
				startTime[0]=micros();
 
				DMP_Routing();	        //DMP �߳�  ���е����ݶ����������
				DMP_getYawPitchRoll();  //��ȡ ��̬��
 
				execTime[0]=micros()-startTime[0];	//��������ִ��ʱ�䣬CPUռ����

		}
#endif
		
#ifdef HIGH_FREQ_CTRL
		if(loop200HzCnt >= 5)
		{
				loop200HzCnt=0;
#else 
		if(loop100HzCnt>=10)//)		 
		{
				loop100HzCnt=0;
#endif
				
				realExecPrd[1]=micros()-startTime[1];
				startTime[1]=micros();
				
			#ifdef IMU_SW
				IMUSO3Thread();
			#else
				IMU_Process();		 
			#endif
				accUpdated=1;
			
				//��ѹ��ȡ
				MS5611_ThreadNew();		//FSM, take aboue 0.5ms some time

				//imuУ׼
				if(imuCaliFlag)
				{
						if(IMU_Calibrate())
						{
							imuCaliFlag=0;
						//	if( IMUCheck()==1 )	//У׼�ɹ�
						//	{
								gParamsSaveEEPROMRequset=1;	//�����¼��EEPROM
								imu.caliPass=1;
						//	}
						//	else
						//		imu.caliPass=0;
						}
				} 
				
				CtrlAttiRate();
				CtrlDynamic();
				#ifndef STOP_MOTOR_FOREVER
				CtrlMotor();
				#endif
				//
				execTime[1]=micros()-startTime[1];
		}
		//Need to recieve 2401 RC instantly so as to clear reg.
		Nrf_Irq();
		//50Hz loop
		if(loop50HzFlag)
		{
				loop50HzFlag=0;
				realExecPrd[3]=micros()-startTime[3];
				startTime[3]=micros();
				
				RCDataProcess();
			  
				FlightModeFSMSimple();
				
		 	//	DetectLand();
				if(altCtrlMode==LANDING)	 
				{	  
						AutoLand();
				}
				
		 		AltitudeCombineThread();

				CtrlAlti();		 

				CtrlAttiAng();	 

			  //PC Monitor
				if(btSrc!=SRC_APP)	
					CommPCUploadHandle();	//tobe improved inside
				
				execTime[3]=micros()-startTime[3];
		} 
		//10Hz loop
		if(loop10HzFlag)
		{
				loop10HzFlag=0; 
				realExecPrd[2]=micros()-startTime[2];
				startTime[2]=micros(); 
			
				//Check battery every BAT_CHK_PRD ms
				if((++batCnt) * 100 >=BAT_CHK_PRD) 
				{
					batCnt=0; 
					BatteryCheck();
				} 
				//App monitor
				if(flyLogApp)	
				{
					CommAppUpload();
					flyLogApp=0;
				}
				
				//EEPROM Conifg Table request to write. 
				if(gParamsSaveEEPROMRequset)
				{
						gParamsSaveEEPROMRequset=0;
						SaveParamsToEEPROM();
				}

				FailSafeLostRC();
				
				FailSafeCrash();
				
				FailSafeLEDAlarm();	 
				
				LEDFSM();			//��˸
				
				execTime[2]=micros()-startTime[2];
		}
 		
		//pc  cmd process. need to return as quickly as ps
		if(pcCmdFlag)
		{
				pcCmdFlag=0;
				CommPCProcessCmd();
		}
 
  }
}