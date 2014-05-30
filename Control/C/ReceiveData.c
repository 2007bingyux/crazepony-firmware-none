
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
#include "ReceiveData.h"
#include "imu.h"
#include "moto.h"
#include "led.h"
#include "MPU6050.h"
#include "extern_variable.h"



//����ɻ������б�Ƕ�
#define  Angle_Max  30.0

//������̬�����������ֿ�����ƫ�Ƶȴ����ĳ�ʼ��ƽ��
int  Rool_error_init;      //����ɻ���ɳ���ƫ��Rool_error_init�����������޸�;����ƫ��Rool_error_init�����������޸�
int  Pitch_error_init;     //����ɻ���ɳ�ǰƫ��Pitch_error_init�����������޸�;����ƫ��Pitch_error_init�����������޸�



RC_GETDATA   RC_DATA;	//���������RC����


//��������GET_RCDATA()
//���룺��
//���: ��
//���������յ���ң�����ݸ�ֵ����Ӧ�ı���
//���ߣ���
//��ע��û�����У����鲻��
void Get_ReceiveData(void)
{
 
    //�⼸������ң����ֱ�ӷ����������ģ�Ҫ�����������ʹ�� 
    //ROOL
    RC_DATA.ROOL=NRF24L01_RXDATA[3]-50;//��50����������
    RC_DATA.ROOL =Rool_error_init+(RC_DATA.ROOL/50.0)*Angle_Max; 
    RC_DATA.ROOL=(RC_DATA.ROOL > Angle_Max)  ? (Angle_Max):(RC_DATA.ROOL);
    RC_DATA.ROOL=(RC_DATA.ROOL < -Angle_Max) ? (-Angle_Max):(RC_DATA.ROOL);
    //PITCH
    RC_DATA.PITCH=NRF24L01_RXDATA[2]-50+Pitch_error_init;//��50����������
    RC_DATA.PITCH =Pitch_error_init+(RC_DATA.PITCH/50.0)*Angle_Max;
    RC_DATA.PITCH=(RC_DATA.PITCH > Angle_Max)  ? (Angle_Max):(RC_DATA.PITCH);
    RC_DATA.PITCH=(RC_DATA.PITCH < -Angle_Max) ? (-Angle_Max):(RC_DATA.PITCH);
    //YAW
    RC_DATA.YAW = 50-NRF24L01_RXDATA[4];
    //RC_DATA.YAW = 0;                      //YAW�ǿ������
    RC_DATA.YAW = (RC_DATA.YAW/50.0)*Angle_Max;
    RC_DATA.YAW=(RC_DATA.YAW > Angle_Max)  ? (Angle_Max):(RC_DATA.YAW);
    RC_DATA.YAW=(RC_DATA.YAW < -Angle_Max) ? (-Angle_Max):(RC_DATA.YAW);

    RC_DATA.THROTTLE=NRF24L01_RXDATA[0]+(NRF24L01_RXDATA[1]<<8);
    FLY_ENABLE = NRF24L01_RXDATA[31];   //0xA5��0�������Ƿ�ʹ�ܷ��У���ң��������
}
