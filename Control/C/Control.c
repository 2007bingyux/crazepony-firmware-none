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
��д�ߣ�С��  (Camel)
����E-mail��375836945@qq.com
���뻷����MDK-Lite  Version: 4.23
����ʱ��: 2014-01-28
���ܣ�
1.PID������ʼ��
2.���ƺ���

------------------------------------
*/
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
PID  PID_Motor;         //����һ��PID�ṹ��
S_FLOAT_XYZ DIF_ACC;		//ʵ��ȥ�������ļ��ٶ�
S_FLOAT_XYZ EXP_ANGLE;	//�����Ƕ�	
S_FLOAT_XYZ DIF_ANGLE;	//ʵ�����������ĽǶ�	
float Yaw_Init;         


float Yawtest[3];


//��������Controler()
//���룺��
//���: ��
//�������ɻ����ƺ������壬����ʱ������
//���ߣ���
//��ע��û�����У����鲻��
void Controler(void)
{   
    static uint8_t Control_Counter = 0;	//=2ʱ����һ��,Ƶ��500HZ
    
    Control_Counter ++;
    DMP_Routing();	        //DMP �߳�  ���е����ݶ����������
    DMP_getYawPitchRoll();  //��ȡ ��̬��
    if(Control_Counter==2)
    {
        Control_Counter = 0;   
        Nrf_Irq();         //���տ���Ŀ�����
        ReceiveDataFormUART();
        PID_Calculate();   //=2ʱ����һ��,Ƶ��500HZ	
        
    }
       
    
}



//��������PID_Calculate()
//���룺��
//���: ��
//�������ɻ�������PIDʵ�ֺ���
//���ߣ���
//��ע��û�����У����鲻��
void PID_Calculate(void)
{
    static float Thr=0,Roll=0,Pitch=0,Yaw=0;
    long Motor[4];   //������PWM���飬�ֱ��ӦM1-M4
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
  
//     DIF_ANGLE.Z = EXP_ANGLE.Z;// - (Q_ANGLE.Yaw-Yaw_Init);
//   
  
    DIF_ACC.Z =  DMP_DATA.dmp_accz - g;     //Z ����ٶ�ʵ���뾲ֹʱ�Ĳ�ֵ��gΪ�����������ٶȣ���DMP.h�к궨��
    /*********************************************************
     PID�����㷨����
    *********************************************************/
    //������
    Pitch =  PID_Motor.P * DIF_ANGLE.Y;      //DIF_ANGLE.YΪY��������ǶȺ͵�ǰʵ�ʽǶȵ����Ƕ�
    Pitch -= PID_Motor.D * DMP_DATA.GYROy;   //DMP_DATA.GYROyΪY������Ƕȵ�΢�֣���Y��Ľ��ٶ� ��λ ��/s
    //�����
    Roll = PID_Motor.P* DIF_ANGLE.X;        //DIF_ANGLE.xΪX��������ǶȺ͵�ǰʵ�ʽǶȵ����Ƕ�
    Roll -= PID_Motor.D * DMP_DATA.GYROx;   //DMP_DATA.GYROxΪX������Ƕȵ�΢�֣���X��Ľ��ٶ� ��λ ��/s
    //�������Ŷ���
    //Thr = 0.001*RC_DATA.THROTTLE*RC_DATA.THROTTLE;   //RC_DATA.THROTTLEΪ0��1000,��ҡ����������ת��Ϊ�°���������
    Thr = RC_DATA.THROTTLE;
    Thr -=30*DIF_ACC.Z;                             //��Z����һ�θ���������
   

     DMP_DATA.GYROz+=EXP_ANGLE.Z;
     Yaw=-25*DMP_DATA.GYROz; 


     
   //�����ֵ�ںϵ��ĸ���� 
    Motor[2] = (int16_t)(Thr - Pitch - Roll - Yaw );    //M3  
    Motor[0] = (int16_t)(Thr + Pitch + Roll - Yaw );    //M1
    Motor[3] = (int16_t)(Thr - Pitch + Roll + Yaw );    //M4 
    Motor[1] = (int16_t)(Thr + Pitch - Roll + Yaw );    //M2    
    
    if((FLY_ENABLE==0xA5))MotorPwmFlash(Motor[0],Motor[1],Motor[2],Motor[3]);   
    else                  MotorPwmFlash(0,0,0,0);//����ɻ��������ʱͻȻ��ת 
    if(NRF24L01_RXDATA[10]==0xA5) MotorPwmFlash(5,5,Motor[2],Motor[3]); //һ������������������
     
}

int PowerCounterAdd=0;//��������ͳ��ֵ��ŵ�ַ��ռ1���ֽ�
int PIDParameterAdd=2;//PID����д���׵�ַΪ0,ռ3���ֽ�
int ErrorParameterAdd=5;//��ʼ����������д���ַΪ10,ռ2���ֽ�


u16 PIDWriteBuf[3];       //д��flash����ʱ����  PID����
u16 PRWriteBuf[2];        //д��flash����ʱ����  �������
u16 PowerCouter[3];       //��������ͳ��ֵ
//��������ParameterWrite()
//���룺��
//��������յ���ַ29���ֽ�Ϊ0xA5ʱ������1�����򷵻�0
//�������ɻ������󣬵���⵽д�����ģʽʱ��д������
//���ߣ���
//��ע��û�����У����鲻��
char  ParameterWrite()
{

  //PID����д��ɿ�flash.
if(NRF24L01_RXDATA[29]==0xA5)
{
        PIDWriteBuf[0]=NRF24L01_RXDATA[0];
        PIDWriteBuf[1]=NRF24L01_RXDATA[1];
        PIDWriteBuf[2]=NRF24L01_RXDATA[2];//дPID����
        PRWriteBuf[0] =NRF24L01_RXDATA[3];
        PRWriteBuf[1] =NRF24L01_RXDATA[4];//д������Ϊ�˱��ڴ�����������ƫ����100
        STMFLASH_Write(STM32_FLASH_BASE+STM32_FLASH_OFFEST+PIDParameterAdd,PIDWriteBuf,3); //PID ����д��
        STMFLASH_Write(STM32_FLASH_BASE+STM32_FLASH_OFFEST+ErrorParameterAdd,PRWriteBuf,2); //������� ����д�룬�ڻ�������ĵ������ֽڿ�ʼ
     
return 1;
}

return 0;
}



/*********************************
��������PID�ͳ�ʼ��Ư��������Դ
*********************************/

//#define ParameterReadFromFlash

//PID��ʼ��������ȡ���黺�棬ȫ������
u16 PIDreadBuf[3]; 
//��������PID_INIT()
//���룺��
//���: ��
//������PID������ʼ��
//���ߣ���
//��ע��û�����У����鲻��
void PID_INIT(void) 
{
    //PID_RP.P--->PIDreadBuf[0]//
    //PID_RP.I--->PIDreadBuf[1]//
    //PID_RP.D--->PIDreadBuf[2]// ��ʼ��ʱ���ڲ�flash��ȡ�趨ֵ���������  �����³�����ܵ���PID����
#ifdef ParameterReadFromFlash
    PID_Motor.P = PIDreadBuf[0]/10.0;//��������
    PID_Motor.I = PIDreadBuf[1]/10.0;//��������
    PID_Motor.D = PIDreadBuf[2]/10.0;//΢������  //ͨ����λ��д�뵽�ɻ���Ƭ��flash��������ʼ��ʱ��ȡ��Ŀǰֻ����������ô˹���

#else
  
    PID_Motor.P = 1.6;                 //��������
    PID_Motor.I = 0;                   //��������
    PID_Motor.D = 0.8;                 //΢������ 

#endif
  
    printf("PID��ʼ�����...\r\n");
}

//��������ParameterRead()
//���룺��
//�������
//��������ʼ��ʱ����ȡ��λ�����һ���趨�Ĳ���
//���ߣ���
//��ע��û�����У����鲻��
void  ParameterRead()
{
  u16 PitchRoolBuf[2];   //���������ʼ����ȡ����

  STMFLASH_Read(STM32_FLASH_BASE+STM32_FLASH_OFFEST+PIDParameterAdd,PIDreadBuf,3);
  STMFLASH_Read(STM32_FLASH_BASE+STM32_FLASH_OFFEST+ErrorParameterAdd,PitchRoolBuf,2);
  STMFLASH_Read(STM32_FLASH_BASE+STM32_FLASH_OFFEST+PowerCounterAdd,PowerCouter,3);
  printf("��FLASH�ж�ȡ����...\r\n");
#ifdef ParameterReadFromFlash
  Pitch_error_init= PitchRoolBuf[0];
  Rool_error_init = PitchRoolBuf[1];//ͨ����λ��д�뵽�ɻ���Ƭ��flash��������ʼ��ʱ��ȡ��Ŀǰֻ����������ô˹���
  
#else
    Pitch_error_init= 0;  //����ɻ���ɳ�ǰƫ��Pitch_error_init�����������޸�;����ƫ��Pitch_error_init�����������޸�
    Rool_error_init = 0;//����ɻ���ɳ���ƫ��Rool_error_init�����������޸�;����ƫ��Rool_error_init�����������޸�

#endif

  printf("FLASH������ȡ���...\r\n");
  
}



