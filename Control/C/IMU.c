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
#include "imu.h"
#include "moto.h"
#include "ReceiveData.h"
#include "extern_variable.h"
#include "math.h"
#include "led.h"
#include "DMP.h"


uint8_t FLY_ENABLE=0;
S_FLOAT_ANGLE  Q_ANGLE;	
S_FLOAT_XYZ DIF_ACC;		//��ּ��ٶ�
S_FLOAT_XYZ EXP_ANGLE;	//�����Ƕ�	
S_FLOAT_XYZ DIF_ANGLE;	//ʵ�����������ĽǶ�	


//��������GET_EXPRAD()
//���룺��
//���: ��
//������ΪPID����������׼��
//���ߣ���
//��ע�����������Ƕ�,���ӿ���ʱ�����Ƕ�Ϊ0,0
void GET_EXPRAD(void)			
{
    EXP_ANGLE.X = (float)(RC_DATA.ROOL);
    EXP_ANGLE.Y = (float)(RC_DATA.PITCH);
    EXP_ANGLE.Z = (float)(RC_DATA.YAW);
    
    DIF_ANGLE.X = EXP_ANGLE.X - Q_ANGLE.Roll;
    DIF_ANGLE.X = DIF_ANGLE.X*20;
  
    DIF_ANGLE.Y = EXP_ANGLE.Y - Q_ANGLE.Pitch;
    DIF_ANGLE.Y = DIF_ANGLE.Y*20;
    DIF_ACC.Z =  (DMP_DATA.ACCz/100)-77;//Z����ٶȲ��ֵ����֤�ɻ���ɲ���ܳ�
  
}
