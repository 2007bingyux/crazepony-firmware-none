
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
  * debug.c - Debugging utility functions
  *
  */

#include "control.h"
#include "moto.h"
#include "sys_fun.h"
#include "mpu6050.h"
#include "imu.h"
#include "extern_variable.h"
#include "led.h"
#include "stmflash.h"
#include "ReceiveData.h"


PID  PID_Motor;//����һ��PID�ṹ��

//��������CONTROL()
//���룺��
//���: ��
//�������ɻ����ƺ������壬����ʱ������
//���ߣ���
//��ע��û�����У����鲻��
void Controler(void)
{
    static uint8_t Control_Counter = 0;	//=2ʱ����һ��,Ƶ��500HZ
    Control_Counter ++;
    IMU_CYCTIME = GET_NOWTIME(&While1_Lasttime);	//�������ζ�ȡʱ����
    MPU6050_Dataanl();//6050���ݷ���
    MPU6050_READ();	  //��6050���ݼ�ȥ��̬���������Ӧ����,��������һ�δ���
    IMU_DataPrepare();

    if(Control_Counter==2)
    {
        Control_Counter = 0;   
        Nrf_Irq();   //���տ���Ŀ�����
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
    static float Thr=0,Rool=0,Pitch=0,Yaw=0;
    long Motor[4];   //������PWM���飬�ֱ��ӦM1-M4
    IMU_Output();	   //�Ƕ��ں�
    GET_EXPRAD();    //�õ������Ƕ�
 
    //ROOL
    Rool = PID_Motor.P * DIF_ANGLE.X;
    Rool -= PID_Motor.D * GYRO_F.X; 
    //Rool = Rool*10;
   //PITCH 
    Pitch = PID_Motor.P * DIF_ANGLE.Y;
    Pitch -= PID_Motor.D * GYRO_F.Y;
    //Pitch = Pitch*10;
   //��������
    Thr = 0.001*RC_DATA.THROTTLE*RC_DATA.THROTTLE;  //RC_DATA.THROTTLEΪ0��1000,
                                                     //��ҡ����������ת��Ϊ�°���������
    Thr -=30*DIF_ACC.Z;      //��Z����һ�θ���������
   // YAW
   
    GYRO_I[0].Z += EXP_ANGLE.Z/250;
    Yaw = -40*GYRO_I[0].Z;
      

    //�����ֵ�ںϵ��ĸ���� 
    Motor[2] = (int16_t)(Thr - Pitch -Rool- Yaw );    //��  
    Motor[0] = (int16_t)(Thr + Pitch +Rool- Yaw );    //��
    Motor[3] = (int16_t)(Thr - Pitch +Rool+ Yaw );    //�� 
    Motor[1] = (int16_t)(Thr + Pitch -Rool+ Yaw );    //��    

    if((FLY_ENABLE==0xA5))    {MotorPwmFlash(Motor[0],Motor[1],Motor[2],Motor[3]);}
    else                      {MotorPwmFlash(0,0,0,0);GYRO_I[0].Z=0;GYRO_I[1].Z=0;GYRO_I[2].Z=0;}//����ɻ��������ʱͻȻ��ת 


  
}


int PIDParameterAdd=0;//PID����д���׵�ַΪ0,ռ3���ֽ�
int ErrorParameterAdd=10;//��ʼ����������д���ַΪ10,ռ2���ֽ�
u16 PIDWriteBuf[3];       //д��flash����ʱ����  PID����
u16 PRWriteBuf[2];        //д��flash����ʱ����  �������

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
    PID_Motor.P = 0.4;//��������
    PID_Motor.I = 0;//��������
    PID_Motor.D = 1.5;//΢������  ��������ǲ��Թ�������������һ��
#endif
  
    PID_Motor.POUT = 0;
    PID_Motor.IOUT = 0;
    PID_Motor.DOUT = 0;
  
    PID_Motor.IMAX = 300;
    PID_Motor.LastError = 0;
    PID_Motor.PrerError = 0;
}

//��������ParameterRead()
//���룺��
//�������
//��������ʼ��ʱ����ȡ��λ�����һ���趨�Ĳ���
//���ߣ���
//��ע��û�����У����鲻��
void  ParameterRead()
{
  u16 PitchRoolBuf[2];
  STMFLASH_Read(STM32_FLASH_BASE+STM32_FLASH_OFFEST+PIDParameterAdd,PIDreadBuf,3);
  STMFLASH_Read(STM32_FLASH_BASE+STM32_FLASH_OFFEST+ErrorParameterAdd,PitchRoolBuf,2);

#ifdef ParameterReadFromFlash
  
  Pitch_error_init= PitchRoolBuf[0];
  Rool_error_init = PitchRoolBuf[1];//ͨ����λ��д�뵽�ɻ���Ƭ��flash��������ʼ��ʱ��ȡ��Ŀǰֻ����������ô˹���
  
#else
  
    Pitch_error_init= 0;  //����ɻ���ɳ�ǰƫ��Pitch_error_init�����������޸�;����ƫ��Pitch_error_init�����������޸�
    Rool_error_init = 1;//����ɻ���ɳ���ƫ��Rool_error_init�����������޸�;����ƫ��Rool_error_init�����������޸�,��λ ����

#endif

}



