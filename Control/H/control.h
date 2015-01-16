#ifndef _CONTROL_H_
#define _CONTROL_H_
#include "stm32f10x.h"


//#define Debug  //����������������

//����
#define SLOW_THRO 200
//����ɻ������б�Ƕ�
#define  Angle_Max  40.0
#define  YAW_RATE_MAX  180.0f/M_PI_F		//deg/s  
//������̬�����������ֿ�����ƫ�Ƶȴ����ĳ�ʼ��ƽ��
//#define  Rool_error_init   7      //����ɻ���ɳ���ƫ��Rool_error_init�����������޸�;����ƫ��Rool_error_init�����������޸�
//#define  Pitch_error_init  -5      //����ɻ���ɳ�ǰƫ��Pitch_error_init�����������޸�;����ƫ��Pitch_error_init�����������޸�
//���߲���
#define LAND_SPEED						1.2f		//m/s^2
#define ALT_VEL_MAX 					4.0f
#define THR_MIN								0.5f		//min thrust �����ݻ��غ���С���ٶ����������½��ٶȹ���ʱ�����Ź�С������ʧ�⡣������fuzzy control ��������Сʱ�ø������̬����


#define HOVER_THRU	-0.63//-0.5f  //


enum {CLIMB_RATE=0,MANUAL,LANDING};
extern uint8_t altCtrlMode;
extern float hoverThrust;
extern uint8_t zIntReset;
extern uint8_t offLandFlag;
extern float altLand;
extern uint8_t isAltLimit;
extern float thrustZSp,thrustZInt;

// PID�ṹ��
typedef struct
{
    float P;
    float I;
    float D;
    float Desired;
    float Error;
    float PreError;
    float PrePreError;
    float Increment;
    float Integ;
		float iLimit;
    float Deriv;
    float Output;
 
}PID_Typedef;


//д��Flash�����ṹ��
typedef struct
{
  u16 WriteBuf[10];       //д��flash����ʱ����
  u16 ReadBuf[10];        //��ȡFlash����ʱ����
  
}Parameter_Typedef;


void Controler(void);
void PID_INIT(void);
void PID_Calculate(void);
char  ParameterWrite(void);
void  ParameterRead(void);

void CtrlAttiAng(void);
void CtrlAttiRate(void);
void CtrlAlti(void);
void CtrlAltiVel(void);
void CtrlDynamic(void);
void CtrlMotor(void);
void CtrlTest(void);
void CtrlAttiRateNew(void);
void CtrlAttiNew(void);

void SetHeadFree(uint8_t on);

extern u16 PIDWriteBuf[3];//д��flash����ʱ���֣���NRF24L01_RXDATA[i]��ֵ 

extern PID_Typedef pitch_angle_PID;	//�ǶȻ���PID
extern PID_Typedef pitch_rate_PID;		//�����ʻ���PID

extern PID_Typedef roll_angle_PID;
extern PID_Typedef roll_rate_PID;

extern PID_Typedef yaw_angle_PID;
extern PID_Typedef yaw_rate_PID;

extern PID_Typedef	alt_PID;
extern PID_Typedef alt_vel_PID;


extern Parameter_Typedef PIDParameter;//ʵ����һ��PID��Flash����
extern Parameter_Typedef BTParameter; //ʵ����һ������Flash����


extern float gyroxGloble;
extern float gyroyGloble;

extern volatile unsigned char motorLock;
#endif


