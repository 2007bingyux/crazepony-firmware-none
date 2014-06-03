
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
#include "tim.h"
#include "config.h"



#define Debug  //����������������


int LedCounter;//LED��˸����ֵ


//�������
void TIM4_IRQHandler(void)		//1ms�ж�һ��,���ڳ����ȡ6050��
{
    if( TIM_GetITStatus(TIM4 , TIM_IT_Update) != RESET ) 
    {     
       
          Controler(); //���ƺ���

      
      
          LedCounter++;
          if(BatteryAD>BatteryADmin)//����ص�ѹ���趨ֵ֮��ʱ������ģʽ
          {
          if(LedCounter==50){ LedA_off;LedB_off;}   //ң�ض�ʹ�ܺ�������ʾ        
          else if(LedCounter==1000){LedCounter=0;LedA_on;LedB_on;}
          }
          else //��ص�ѹ��ʱ��������ʾ
          {
          if(LedCounter==50){ LedA_off;LedB_off;LedC_off;LedD_off;}   //ң�ض�ʹ�ܺ�������ʾ        
          else if(LedCounter==100){LedCounter=0;LedA_on;LedB_on;LedC_on;LedD_on;}
          }
          if(LedCounter>=1001)LedCounter=0;
      
          TIM_ClearITPendingBit(TIM4 , TIM_FLAG_Update);   //����жϱ�־   
    }
}



int DebugCounter;             //��ӡ��Ϣ���ʱ��������ֵ
void TIM3_IRQHandler(void)		//��ӡ�жϷ������
{
    if( TIM_GetITStatus(TIM3 , TIM_IT_Update) != RESET ) 
    {     
       
  #ifdef Debug
           DebugCounter++;
           BatteryAD=GetBatteryAD();//��ص�ѹ���
          if( DebugCounter==1000){
            DebugCounter=0;
            printf(" ******************************************************************\r\n");
            printf(" *       ____                      _____                  +---+   *\r\n");
            printf(" *      / ___\\                     / __ \\                 | R |   *\r\n");
            printf(" *     / /                        / /_/ /                 +---+   *\r\n");
            printf(" *    / /   ________  ____  ___  / ____/___  ____  __   __        *\r\n");
            printf(" *   / /  / ___/ __ `/_  / / _ \\/ /   / __ \\/ _  \\/ /  / /        *\r\n");
            printf(" *  / /__/ /  / /_/ / / /_/  __/ /   / /_/ / / / / /__/ /         *\r\n");
            printf(" *  \\___/_/   \\__,_/ /___/\\___/_/    \\___ /_/ /_/____  /          *\r\n");
            printf(" *                                                  / /           *\r\n");
            printf(" *                                             ____/ /            *\r\n");
            printf(" *                                            /_____/             *\r\n");
            printf(" ******************************************************************\r\n");
            printf("\r\n");
            printf(" Crazepony-II���棺ϵͳ��������...\r\n"); 
            printf("\r\n");
            printf("\r\n--->����ʵʱ��̬�㲥��Ϣ<---\r\n");
            printf("\r\n");
            printf(" ƫ����---> %f��\r\n",(float)Q_ANGLE.Yaw);
            printf(" ������---> %f��\r\n",(float)Q_ANGLE.Pitch);
            printf(" �����---> %f��\r\n",(float) Q_ANGLE.Roll);
            printf(" ==================\r\n");
            printf(" X�������Ƕ�---> %f��\r\n",(float)EXP_ANGLE.X);
            printf(" Y�������Ƕ�---> %f��\r\n",(float)EXP_ANGLE.Y);
            printf(" ==================\r\n");
            printf(" X�����Ƕ�---> %f��\r\n",(float)DIF_ANGLE.X);
            printf(" Y�����Ƕ�---> %f��\r\n",(float)DIF_ANGLE.Y);
            printf("==================\r\n");
            printf(" X����ٶ�---> %f\r\n",(float) DMP_DATA.ACCx);
            printf(" Y����ٶ�---> %f\r\n",(float) DMP_DATA.ACCy);
            printf(" Z����ٶ�---> %f\r\n",(float) DMP_DATA.ACCz);
            printf(" ==================\r\n");
            printf(" X��DMP���ٶ�---> %f\r\n",(float) DMP_DATA.GYROx);
            printf(" Y��DMP���ٶ�---> %f\r\n",(float) DMP_DATA.GYROy);
            printf(" Z��DMP���ٶ�---> %f\r\n",(float) DMP_DATA.GYROz);
            printf("==================\r\n");
            printf(" ���M1 PWMֵ---> %d\r\n",TIM2->CCR1);
            printf(" ���M2 PWMֵ---> %d\r\n",TIM2->CCR2);
            printf(" ���M3 PWMֵ---> %d\r\n",TIM2->CCR3);
            printf(" ���M4 PWMֵ---> %d\r\n",TIM2->CCR4);
            printf("==================\r\n");
            printf(" ��ص�ѹ---> %d\r\n",(int) BatteryAD);
            printf("==================\r\n");
                
#else      
             
#endif
        }
        TIM_ClearITPendingBit(TIM3 , TIM_FLAG_Update);   //����жϱ�־   
    }
}



//��ʱ��4��ʼ���������жϴ���PID
void TIM4_Init(char clock,int Preiod)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  //��ʱ��
    
    TIM_DeInit(TIM4);

    TIM_TimeBaseStructure.TIM_Period = Preiod;
    TIM_TimeBaseStructure.TIM_Prescaler = clock-1;//��ʱ1ms
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    
    TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM4,TIM_FLAG_Update);

    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM4,ENABLE);
    DEBUG_PRINTLN("��ʱ��4��ʼ�����...\r\n");
    
}	


//��ʱ��3��ʼ��
void TIM3_Init(char clock,int Preiod)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  //��ʱ��
    
    TIM_DeInit(TIM3);

    TIM_TimeBaseStructure.TIM_Period = Preiod;
    TIM_TimeBaseStructure.TIM_Prescaler = clock-1;//��ʱ1ms
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM3,TIM_FLAG_Update);

    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM3,ENABLE);
    DEBUG_PRINTLN("��ʱ��3��ʼ�����...\r\n");
}		



void TimerNVIC_Configuration()
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    /* NVIC_PriorityGroup 2 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    //TIM3
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    //TIM4
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

} 

