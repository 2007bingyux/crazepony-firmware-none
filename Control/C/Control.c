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
/* Control.c file
��д�ߣ�С��  (Camel) ����
����E-mail��375836945@qq.com
���뻷����MDK-Lite  Version: 4.23
����ʱ��: 2014-01-28
���ܣ�
1.PID������ʼ��
2.���ƺ���

------------------------------------
*/
#include <stm32f10x.h>
#include "control.h"
#include "moto.h"
#include "math.h"
#include "sys_fun.h"
#include "mpu6050.h"
#include "imu.h"
#include "extern_variable.h"
#include "led.h"
#include "stmflash.h"
#include "ReceiveData.h"
#include "DMP.h"
#include "Battery.h"
#include "stdio.h"
#include "BT.h"
#include "Altitude.h"
#include "SysConfig.h"

extern uint32_t micros(void);

uint8_t offLandFlag=0;


volatile unsigned char motorLock=1;

int16_t Motor[4]={0};   //������PWM���飬�ֱ��ӦM1-M4
float rollSp =0,pitchSp =0;		//���ݶ����������¼���õ�������roll pitch
float Thro=0,Roll=0,Pitch=0,Yaw=0;


//----PID�ṹ��ʵ����----
PID_Typedef pitch_angle_PID;	//�ǶȻ���PID
PID_Typedef pitch_rate_PID;		//�����ʻ���PID

PID_Typedef roll_angle_PID;
PID_Typedef roll_rate_PID;

PID_Typedef yaw_angle_PID;
PID_Typedef yaw_rate_PID;

PID_Typedef	alt_PID;
PID_Typedef alt_vel_PID;

float gyroxGloble = 0;
float gyroyGloble = 0;


S_FLOAT_XYZ DIF_ACC;		//ʵ��ȥ�������ļ��ٶ�
S_FLOAT_XYZ EXP_ANGLE;	//�����Ƕ�	
S_FLOAT_XYZ DIF_ANGLE;	//ʵ�����������ĽǶ�	

uint32_t ctrlPrd=0;
uint8_t headFreeMode=0;
float headHold=0;

//��������Controler()
//���룺��
//���: ��
//�������ɻ����ƺ������壬����ʱ������
//���ߣ���

void Controler(void)
{     
    static char Counter_Cnt=0;
	
//		static uint32_t tPrev=0;
	
    Counter_Cnt++;
		
    /*******************����λ��������̬��Ϣ�����Ҫ��PC��λ����ʵʱ��̬,�꿪�ؿ���***************/
    #ifndef Debug
  //  Send_AtitudeToPC();     
    #else
    #endif  
    if(Counter_Cnt>=5)
    {
//				ctrlPrd=micros() - tPrev;
//				tPrev=micros();
					
				Counter_Cnt=0;
				#if(RC_SRC==NFRC)
					Nrf_Irq();           //��2.4G���տ���Ŀ�����
				//#else
				//ReceiveDataFormUART();//������͸��ģ����տ���Ŀ���������2.4G���տ���ֻ��ѡ��һ
				#endif
				PID_Calculate();     //=2ʱ����һ��,Ƶ��200HZ	
    }
		
		
}





//#define CONSTRAIN(x,min,max)  {if(x<min) x=min; if(x>max) x=max;}

//-----------λ��ʽPID-----------
void PID_Postion_Cal(PID_Typedef * PID,float target,float measure,int32_t dertT)
{
 float termI=0;
 float dt= dertT/1000000.0;
	//-----------λ��ʽPID-----------
	//���=����ֵ-����ֵ
	PID->Error=target-measure;
	
	PID->Deriv= (PID->Error-PID->PreError)/dt;
	
	PID->Output=PID->P*PID->Error+PID->I*PID->Integ+PID->D * PID->Deriv;
	
	PID->PreError=PID->Error;
	//�����ڽǶȻ��ͽ��ٶȻ���
	if(FLY_ENABLE && offLandFlag)
	{
			if(fabs(PID->Output) < Thro )		//�����Ż���ʱ������
			{
				termI=(PID->Integ) + (PID->Error) * dt;
				if(termI > - PID->iLimit && termI < PID->iLimit && PID->Output > - PID->iLimit && PID->Output < PID->iLimit)
						PID->Integ=termI;
			}
	}
	else
			PID->Integ= 0;
	
}





//��������PID_Calculate()

void PID_Calculate(void)
{  

	
  /*********************************************************
     ����������̬��ʵ����̬�Ĳ�ֵ
    *********************************************************/
    EXP_ANGLE.X = (float)(RC_DATA.ROOL);
    EXP_ANGLE.Y = (float)(RC_DATA.PITCH);
    EXP_ANGLE.Z = (float)(RC_DATA.YAW);

    DIF_ANGLE.X = EXP_ANGLE.X - Q_ANGLE.Roll;
    DIF_ANGLE.X = DIF_ANGLE.X;
    
    DIF_ANGLE.Y = EXP_ANGLE.Y - Q_ANGLE.Pitch;
    DIF_ANGLE.Y = DIF_ANGLE.Y;

    DIF_ACC.Z =  DMP_DATA.dmp_accz - ONE_G;     //Z ����ٶ�ʵ���뾲ֹʱ�Ĳ�ֵ��gΪ�����������ٶ�,��ʼ��ʱ����
  
    /*********************************************************
     PID�����㷨����
    *********************************************************/
  //------------��������------------
    //��������ԭ��Ϊ���ں��⣬���������ڻ�ʱ���⻷��PID����Ϊ0
    //�⻷�� �ơ�����Ϊ�Ƕ�,���Ϊ���ٶȡ�PID->Output��Ϊ�ڻ������롣
    PID_Postion_Cal(&pitch_angle_PID,EXP_ANGLE.Y,Q_ANGLE.Pitch,0);
    
    //�ڻ����ƣ�����Ϊ���ٶȣ����ΪPWM����
    PID_Postion_Cal(&pitch_rate_PID,pitch_angle_PID.Output,gyroyGloble,0);
    //��������ԭ��Ϊ���ں��⣬���������ڻ�ʱ���⻷��PID����Ϊ0
    
    
    //�⻷�� �ơ�����Ϊ�Ƕ�,���Ϊ���ٶȡ�PID->Output��Ϊ�ڻ������롣
    PID_Postion_Cal(&roll_angle_PID,EXP_ANGLE.X,Q_ANGLE.Roll,0);
    
    //�ڻ����ƣ�����Ϊ���ٶȣ����ΪPWM����
    PID_Postion_Cal(&roll_rate_PID,roll_angle_PID.Output,gyroxGloble,0);
    //��������ԭ��Ϊ���ں��⣬���������ڻ�ʱ���⻷��PID����Ϊ0
    

    //�⻷�� �ơ�����Ϊ�Ƕ�,���Ϊ���ٶȡ�PID->Output��Ϊ�ڻ������롣
    PID_Postion_Cal(&yaw_angle_PID,EXP_ANGLE.Z,Q_ANGLE.Yaw,0);		//-----Problem
    
    //�ڻ����ƣ�����Ϊ���ٶȣ����ΪPWM����
    PID_Postion_Cal(&yaw_rate_PID,-2*EXP_ANGLE.Z,DMP_DATA.GYROz,0);		//--------Problem tobe tested
    //��������ԭ��Ϊ���ں��⣬���������ڻ�ʱ���⻷��PID����Ϊ0
    
    
    //�������Ŷ���
    //Thr = 0.001*RC_DATA.THROTTLE*RC_DATA.THROTTLE;   //RC_DATA.THROTTLEΪ0��1000,��ҡ����������ת��Ϊ�°���������
    Thro = RC_DATA.THROTTLE;
    Thro -=  80*DIF_ACC.Z;                             //��Z����һ�θ���������
	//	Thr = Thr / (cosf(Q_ANGLE.Pitch*M_PI_F/180.0f)*cosf(Q_ANGLE.Roll*M_PI_F/180.0f));	//����б������
    
    Pitch = pitch_rate_PID.Output;
    Roll  = roll_rate_PID.Output;
    Yaw   = yaw_rate_PID.Output; 
    
   //�����ֵ�ںϵ��ĸ���� 
    Motor[2] = (int16_t)(Thro - Pitch -Roll- Yaw );    //M3  
    Motor[0] = (int16_t)(Thro + Pitch +Roll- Yaw );    //M1
    Motor[3] = (int16_t)(Thro - Pitch +Roll+ Yaw );    //M4 
    Motor[1] = (int16_t)(Thro + Pitch -Roll+ Yaw );    //M2    
    
  if((FLY_ENABLE==0xA5))
//		if(!motorLock)
			MotorPwmFlash(Motor[0],Motor[1],Motor[2],Motor[3]);   
   else                  
			MotorPwmFlash(0,0,0,0);//����ɻ��������ʱͻȻ��ת 
   // if(NRF24L01_RXDATA[10]==0xA5) 
	//		MotorPwmFlash(5,5,Motor[2],Motor[3]); //һ�����������������ȣ����Թ��ܣ���Ҫ��
 
}

//
void SetHeadFree(uint8_t on)
{
	if(on==1)
	{
		headHold=imu.yaw;
		headFreeMode=1;
	}
	else
		headFreeMode=0;
}


	
//run after get rc cmd
void CtrlAttiAng(void)
{
 

		static float yawHold=0;
		static uint32_t tPrev=0;
		float yawRateTarget=0;
		float angTarget[3]={0};
		float dt=0,t=0;
		t=micros();
		dt=(tPrev>0)?(t-tPrev):0;
		tPrev=t;
		
		if(altCtrlMode==MANUAL)
		{
			angTarget[ROLL]=(float)(RC_DATA.ROOL);
			angTarget[PITCH]=(float)(RC_DATA.PITCH);
		}
		else
		{
			angTarget[ROLL]=rollSp;
			angTarget[PITCH]=pitchSp;
		}
	//	angTarget[YAW]=(float)(RC_DATA.YAW);		//��Ϊ����ϵ
//		yawRateTarget=
//		angTarget[YAW]= (angTarget[YAW] + yawRateTarget * dt);

		if(headFreeMode)
		{
			#ifdef YAW_CORRECT
        float radDiff = -(imu.yaw - headHold) * M_PI_F / 180.0f; 
			#else
				float radDiff = (imu.yaw - headHold) * M_PI_F / 180.0f; 
			#endif
        float cosDiff = cosf(radDiff);
        float sinDiff = sinf(radDiff);
        float tarPitFree = angTarget[PITCH] * cosDiff + angTarget[ROLL] * sinDiff;
        angTarget[ROLL] = angTarget[ROLL] * cosDiff - angTarget[PITCH] * sinDiff;
        angTarget[PITCH] = tarPitFree;
		}
 
 
		PID_Postion_Cal(&pitch_angle_PID,angTarget[PITCH],imu.pitch,dt);
		PID_Postion_Cal(&roll_angle_PID,angTarget[ROLL],imu.roll,dt);
	//	PID_Postion_Cal(&yaw_angle_PID,angTarget[YAW],imu.yaw,dt);	 
}




//run in 200Hz or 400Hz loop 
void CtrlAttiRate(void)
{
 	float yawRateTarget=0;
	static uint32_t tPrev=0; 

	float dt=0,t=0;
	t=micros();
	dt=(tPrev>0)?(t-tPrev):0;
	tPrev=t;
		
		yawRateTarget=-(float)RC_DATA.YAW;
		//ע�⣬ԭ����pid��������Ӧ���� adֵ,��ת֮
		#ifdef IMU_SW
		PID_Postion_Cal(&pitch_rate_PID,pitch_angle_PID.Output,imu.gyro[PITCH]*180.0f/M_PI_F,dt);	
		PID_Postion_Cal(&roll_rate_PID,roll_angle_PID.Output,imu.gyro[ROLL]*180.0f/M_PI_F,dt);//gyroxGloble
		PID_Postion_Cal(&yaw_rate_PID,yawRateTarget,imu.gyro[YAW]*180.0f/M_PI_F,dt);//DMP_DATA.GYROz
	  #else
		//ԭ������Ӧ�� DMP��ֱ�����gyro , ��deg.  ��ԭDMP֮��Ĵ��������Ǵ����
		PID_Postion_Cal(&pitch_rate_PID,pitch_angle_PID.Output,imu.gyro[PITCH]*DMP_GYRO_SCALE,0);	
		PID_Postion_Cal(&roll_rate_PID,roll_angle_PID.Output,imu.gyro[ROLL]*DMP_GYRO_SCALE,0);//gyroxGloble
		PID_Postion_Cal(&yaw_rate_PID,yawRateTarget,imu.gyro[YAW]*DMP_GYRO_SCALE,0);          //DMP_DATA.GYROz
		#endif
		
		Pitch = pitch_rate_PID.Output;
    Roll  = roll_rate_PID.Output;
    Yaw   = yaw_rate_PID.Output; 
}

//----------to be tested--//
//cut deadband, move linear
float dbScaleLinear(float x, float x_end, float deadband)
{
	if (x > deadband) {
		return (x - deadband) / (x_end - deadband);

	} else if (x < -deadband) {
		return (x + deadband) / (x_end - deadband);

	} else {
		return 0.0f;
	}
}

//alti ctrl , output thrustZSp ,  ned frame , thrust force on z axis. 



float thrInit;

#define ALT_FEED_FORWARD  		0.5f
#define THR_MAX								1.0f		//max thrust
#define TILT_MAX 					(Angle_Max * M_PI_F / 180.0 )
const float ALT_CTRL_Z_DB = 0.1f;	//
float spZMoveRate;

uint8_t altCtrlMode;					//normal=0  CLIMB rate, normal .  tobe tested
float hoverThrust=0;
uint8_t zIntReset=1;	//integral reset at first . when change manual mode to climb rate mode
float thrustZInt=0, thrustZSp=0;
float thrustXYSp[2]={0,0};	//roll pitch
uint8_t recAltFlag=0;
float holdAlt=0;
uint8_t satZ=0,satXY=0;	//�Ƿ������


#define ALT_LIMIT							2.0f		//�޸� 3.5
uint8_t isAltLimit=0;
float altLand;
//#define DEBUG_HOLD_REAL_ALT
//only in climb rate mode and landind mode. now we don't work on manual mode
void CtrlAlti(void)
{
	float manThr=0,alt=0,velZ=0;
	float altSp=0;
	float posZVelSp=0;
	float altSpOffset,altSpOffsetMax=0;
	float dt=0,t=0;
	static float tPrev=0,velZPrev=0;
	float posZErr=0,velZErr=0,valZErrD=0;
		float thrustXYSpLen=0,thrustSpLen=0;
		float thrustXYMax=0;
	
	
	

	//get dt		//��֤dt���㲻�ܱ���ϣ����ָ��£�����dt���󣬻��ֱ�����
	if(tPrev==0)
	{
			tPrev=micros();
			return;
	}
	else
	{
			t=micros();
			dt=(t-tPrev) /1000000.0f;
			tPrev=t;
	}
	
	if(altCtrlMode==MANUAL || !FLY_ENABLE)		
		return;
	//--------------pos z ctrol---------------//
	//get current alt 
	alt=-nav.z;
	//get desired move rate from stick
	manThr=RC_DATA.THROTTLE / 1000.0f;
	spZMoveRate= -dbScaleLinear(manThr-0.5f,0.5f,ALT_CTRL_Z_DB);	// scale to -1~1 . NED frame
	spZMoveRate = spZMoveRate * ALT_VEL_MAX;	// scale to vel min max

#ifdef DEBUG_HOLD_REAL_ALT
	if(spZMoveRate==0)
	{
			if(!recAltFlag)
			{
					holdAlt=alt;
					recAltFlag=1;
					 
			}
			altSp=holdAlt;
	}
	else
	{
		recAltFlag=0;
#endif
		//get alt setpoint in CLIMB rate mode
		altSp 	=-nav.z;						//only alt is not in ned frame.
		altSp  -= spZMoveRate * dt;	 
		//limit alt setpoint
		altSpOffsetMax=ALT_VEL_MAX / alt_PID.P * 2.0f;
		altSpOffset = altSp-alt; 
		if( altSpOffset > altSpOffsetMax)		//or alt - alt > altSpOffsetMax
			altSp=alt +  altSpOffsetMax;
		else if( altSpOffset < -altSpOffsetMax)
			altSp=alt - altSpOffsetMax;
#ifdef DEBUG_HOLD_REAL_ALT
	}
#endif
	//�޸�
	if(isAltLimit)
	{
		if(altSp - altLand > ALT_LIMIT)
		{
				altSp=altLand+ALT_LIMIT;
				spZMoveRate=0;
		}
	}
	
	// pid and feedforward control . in ned frame
	posZErr= -(altSp - alt);
	posZVelSp = posZErr * alt_PID.P + spZMoveRate * ALT_FEED_FORWARD;
	//consider landing mode
	if(altCtrlMode==LANDING)
		posZVelSp = LAND_SPEED;
	//limit 
//	if(posZVelSp>ALT_VEL_MAX)
//		posZVelSp=ALT_VEL_MAX;
//	else if(posZVelSp<-ALT_VEL_MAX)
//		posZVelSp=-ALT_VEL_MAX;
	
	//--------------pos z vel ctrl -----------//
	if(zIntReset)		//tobe tested .  how to get hold throttle. give it a estimated value!!!!!!!!!!!
	{
		thrustZInt=HOVER_THRU; //-manThr;		//650/1000 = 0.65
		zIntReset=0;
	}
	velZ=nav.vz;	
	velZErr = posZVelSp - velZ;
	valZErrD = (spZMoveRate - velZ) * alt_PID.P - (velZ - velZPrev) / dt;	//spZMoveRate is from manual stick vel control
	velZPrev=velZ;
	
	thrustZSp= velZErr * alt_vel_PID.P + valZErrD * alt_vel_PID.D + thrustZInt;	//in ned frame. thrustZInt contains hover thrust
	
		//limit thrust min !!
	if(altCtrlMode!=LANDING)
	{
			if (-thrustZSp < THR_MIN) {
						thrustZSp = -THR_MIN; 
					} 
					
	}
	//�붯���������	testing
		satXY=0;
		satZ=0;
		thrustXYSp[0]= sinf(RC_DATA.ROOL * M_PI_F /180.0f) ;//Ŀ��Ƕ�ת���ٶ� ���� 
		thrustXYSp[1]= sinf(RC_DATA.PITCH * M_PI_F /180.0f) ; 	//��һ��
		thrustXYSpLen= sqrtf(thrustXYSp[0] * thrustXYSp[0] + thrustXYSp[1] * thrustXYSp[1]);
		//limit tilt max
		if(thrustXYSpLen >0.01f )
		{
			thrustXYMax=-thrustZSp * tanf(TILT_MAX);
			if(thrustXYSpLen > thrustXYMax)
			{
					float k=thrustXYMax / thrustXYSpLen;
					thrustXYSp[1] *=k;
					thrustXYSp[0] *= k;
					satXY=1;
					thrustXYSpLen= sqrtf(thrustXYSp[0] * thrustXYSp[0] + thrustXYSp[1] * thrustXYSp[1]);
			}
			
		}
		//limit max thrust!! 
		thrustSpLen=sqrtf(thrustXYSpLen * thrustXYSpLen + thrustZSp * thrustZSp);
		if(thrustSpLen > THR_MAX)
		{
				if(thrustZSp < 0.0f)	//going up
				{
							if (-thrustZSp > THR_MAX) 
							{
									/* thrust Z component is too large, limit it */
									thrustXYSp[0] = 0.0f;
									thrustXYSp[1] = 0.0f;
									thrustZSp = -THR_MAX;
									satXY = true;
									satZ = true;

								} 
								else {
									float k = 0;
									/* preserve thrust Z component and lower XY, keeping altitude is more important than position */
									thrustXYMax = sqrtf(THR_MAX * THR_MAX- thrustZSp * thrustZSp);
									k=thrustXYMax / thrustXYSpLen;
							//		float thrust_xy_abs =thrustXYSpLen;// math::Vector<2>(thrust_sp(0), thrust_sp(1)).length();
									thrustXYSp[1] *=k;
									thrustXYSp[0] *= k;
									satXY=1;
								}
				}
				else {		//going down
								/* Z component is negative, going down, simply limit thrust vector */
								float k = THR_MAX / thrustSpLen;
								thrustZSp *= k;
								thrustXYSp[1] *=k;
								thrustXYSp[0] *= k;
								satXY = true;
								satZ = true;
							}
			
		} 
		rollSp= asinf(thrustXYSp[0]) * 180.0f /M_PI_F;
		pitchSp = asinf(thrustXYSp[1]) * 180.0f /M_PI_F;				
	
	
	// if saturation ,don't integral
	if(!satZ )//&& fabs(thrustZSp)<THR_MAX
	{
			thrustZInt += velZErr * alt_vel_PID.I * dt;		//
			if (thrustZInt > 0.0f)
							thrustZInt = 0.0f;
	}
	
// 	Roll= thrustXYSp[0] * 1000;
 // Pitch = thrustXYSp[1] * 1000;

	
}
 

#define ANG_COR_COEF 50.0f
#define THR_HOLD_LEVEL 1600		//��ͣ���� �� can measure after baro

void CtrlDynamic(void)
{
		static float thrAngCorrect;	//����б������
		float  cosTilt = imu.accb[2] / ONE_G;
	
		
		if(altCtrlMode==MANUAL)
		{
			DIF_ACC.Z =  imu.accb[2] - ONE_G;
			Thro = RC_DATA.THROTTLE;
			   // Thr = Thr/(cos) ;                             //��Z����һ�θ���������
				//way1	
				//  thrAngCorrect = ANG_COR_COEF * (1-cosTilt) ;
				 //	Thr += thrAngCorrect;				//������ѹ����ʱ�����ô�������
				//way2	
				cosTilt=imu.DCMgb[2][2];
				Thro=Thro/cosTilt;
				//way3
		//	thrAngCorrect=THR_HOLD_LEVEL * (1.0f/cosTilt - 1.0);
		//	Thro += thrAngCorrect;	

		}
		else 
		{
				Thro=(-thrustZSp) * 1000;// /imu.DCMgb[2][2];  //��ǲ�����Ч��������ʱ����
				if(Thro>1000)
					Thro=1000;
			
			
		}
			 //�����ֵ�ںϵ��ĸ���� 
			Motor[2] = (int16_t)(Thro - Pitch -Roll- Yaw );    //M3  
			Motor[0] = (int16_t)(Thro + Pitch +Roll- Yaw );    //M1
			Motor[3] = (int16_t)(Thro - Pitch +Roll+ Yaw );    //M4 
			Motor[1] = (int16_t)(Thro + Pitch -Roll+ Yaw );    //M2
	
   
}

void CtrlMotor(void)
{
 
	 if((FLY_ENABLE!=0)) 
				MotorPwmFlash(Motor[0],Motor[1],Motor[2],Motor[3]);   
		else                  
				MotorPwmFlash(0,0,0,0); 
}



#define PIDParameterAdd   0    //PID����д���׵�ַΪ
#define BTParameterAdd    32   //��������д��Flash��ַΪ

Parameter_Typedef PIDParameter;//ʵ����һ��PID��Flash����
Parameter_Typedef BTParameter; //ʵ����һ������Flash����

//��������ParameterWrite()
//���룺��
//��������յ���ַ29���ֽ�Ϊ0xA5ʱ������1�����򷵻�0
//�������ɻ������󣬵���⵽д�����ģʽʱ��д������
//���ߣ���
//��ע��û�����У����鲻��
char  ParameterWrite()
{
//         PIDParameter.WriteBuf[0] = 23;
//         PIDParameter.WriteBuf[1] = 1;
//         PIDParameter.WriteBuf[2] = 4;
//   
//         BTParameter.WriteBuf[1]  = 0;
//         BTParameter.WriteBuf[2]  = 0;
  
        //STMFLASH_Write(STM32_FLASH_BASE+STM32_FLASH_OFFEST+PIDParameterAdd,PIDParameter.WriteBuf,3); //PID ����д��Flash
        STMFLASH_Write(STM32_FLASH_BASE+STM32_FLASH_OFFEST+BTParameterAdd,BTParameter.WriteBuf,3);  //�������ò���д��Flash
       
return 0;
}

//��������ParameterRead()
//���룺��
//�������
//��������ʼ��ʱ����ȡ��λ�����һ���趨�Ĳ���
//���ߣ���
//��ע��û�����У����鲻��
void  ParameterRead()
{      
  //STMFLASH_Read(STM32_FLASH_BASE+STM32_FLASH_OFFEST+PIDParameterAdd,PIDParameter.ReadBuf,3);
  STMFLASH_Read(STM32_FLASH_BASE+STM32_FLASH_OFFEST+BTParameterAdd,BTParameter.ReadBuf,3);
  
  printf("��FLASH�ж�ȡ����...\r\n");

}
 

