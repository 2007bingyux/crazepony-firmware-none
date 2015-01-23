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
Filename:	FailSafe.c
Author:		�� ��С��
------------------------------------
*/

#include "FailSafe.h"
#include "SysConfig.h"
#include "config.h"        //�������е�����ͷ�ļ�
#include "imu.h"
#include "Altitude.h"
#include "CommApp.h"
#include "CommPC.h"
#include "ConfigTable.h"
#include "IMUSO3.h"
#include "control.h"

uint32_t newTime=0;
uint8_t lostRCFlag=0,autoLanded=0;

#define LAND_THRO 				500 
#define LAND_THRO_ALT_VEL 200 
#define LOST_RC_TIME_MAX  1000

//copter crash down
void FailSafeCrash(void)
{
			 
			if(fabs(imu.pitch)>80 || fabs(imu.roll)>80 )
			{
			//	RC_DATA.THROTTLE=LAND_THRO;
				MotorPwmFlash(0,0,0,0);
				FLY_ENABLE=0;
			} 
}
//
void FailSafeLostRC(void)
{
	uint16_t lostRCTime=0;
	
	//���й��̶�ʧRC��� 
			//	if(offLandFlag)	// RC_DATA.THROTTLE range :0-1000 ; �½�����
			//	{
					newTime=millis();	//ms
					lostRCTime=(newTime>lastGetRCTime)?(newTime-lastGetRCTime):(65536-lastGetRCTime+newTime);
					if(lostRCTime > LOST_RC_TIME_MAX) 	 
					{
						if(offLandFlag)
						{
						#ifdef lostRC_Landing //auto landing -->  testing 
							
								altCtrlMode=LANDING;
								rcData[0]=1500;rcData[1]=1500;rcData[2]=1500;rcData[3]=1500;
							//	lastGetRCTime=newTime;
						#else
							FLY_ENABLE=0;    // disable crazepony 
						#endif
						}
						lostRCFlag=1;
					}
					else 
					{
					  //altCtrlMode=CLIMB_RATE;// relink 
						lostRCFlag=0;
					}
						
		//		}
}
//��˸״̬�ɼ���ϵͳ�ı�־����,���ȼ����ΰ��ж�˳������
void FailSafeLEDAlarm(void)
{
		
		LEDCtrl.event=E_READY;
	

		if(!imu.ready)		//����imu׼��
			LEDCtrl.event=E_CALI;
		if(lostRCFlag)
			LEDCtrl.event=E_LOST_RC;	

		if(!imu.caliPass)
			LEDCtrl.event=E_CALI_FAIL;
		
		if(Battery.alarm)
			LEDCtrl.event=E_BAT_LOW;
		
		if(imuCaliFlag)
			LEDCtrl.event=E_CALI;
		
		if(autoLanded && !FLY_ENABLE)
			LEDCtrl.event=E_AUTO_LANDED;
		  
		if((Battery.chargeSta))			//battery charge check
			LEDCtrl.event = E_BatChg;
}

//
void AutoLand(void)
{
	static uint32_t landStartTime=0;
	uint32_t landTime=0;
	
	if(offLandFlag)
	{
			if(landStartTime==0)
				landStartTime=millis();
			landTime=millis() - landStartTime;
//			if(landed==1)
			if( landTime>4000 /*&& ( -nav.vz <0.35)  &&( -thrustZSp < 0.6)*/  )	//�����ؼ�� //||(fabs(nav.vz)<0.1 && fabs(nav.az)<0.1)
			{
					altCtrlMode=MANUAL;
					FLY_ENABLE=0;
		//			LedC_on;LedD_on;
					offLandFlag=0;
					landStartTime=0;
					autoLanded=1;
		 	//		landed=0;
		//			landTime=0;
			}
	}
	else
	{
			altCtrlMode=MANUAL;
			FLY_ENABLE=0;
		//	LedC_on;LedD_on;
			autoLanded=0;
		
	}
}
//for copter launch and flight mode switch.  it's raw and simple for climb rate mode now
//TOBE IMPROVED
void FlightModeFSMSimple(void)
{
			if(FLY_ENABLE)
			{
					if(RC_DATA.THROTTLE>=600 )
					{ 
							if(altCtrlMode!=CLIMB_RATE)
							{
							// 		hoverThrust=-0.65f;
									zIntReset=1;
									thrustZSp=0;
									altCtrlMode=CLIMB_RATE;
									offLandFlag=1;
									altLand=-nav.z;		//��¼���ʱ�ĸ߶�
									#ifdef AUTO_MW
									SetHeadFree(1);
									#endif
							}
					}
					else
					{
							if(altCtrlMode==MANUAL)
							{
									RC_DATA.THROTTLE=SLOW_THRO;
							}
					}
					
			} 
}
