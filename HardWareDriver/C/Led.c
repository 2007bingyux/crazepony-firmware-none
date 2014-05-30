
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
#include "Led.h"
/********************************************
              Led��ʼ������
���ܣ�
1.����Led�ӿ�IO�������
2.�ر�����Led(����Ĭ�Ϸ�ʽ)

������
Led�ӿڣ�
Led1-->PB5
Led2-->PB4
Led3-->PB1
Led4-->PB3
BT_EN-->PB2
��ӦIO=1������
********************************************/
void LedInit(void)
{
    RCC->APB2ENR|=1<<3;    //ʹ��PORTBʱ��	
    RCC->APB2ENR|=1<<4;    //ʹ��PORTCʱ��	
    
    RCC->APB2ENR|=1<<0;      //ʹ�ܸ���ʱ��	   
    GPIOB->CRL&=0XFF00000F;  //PB1,2,3,4,5�������
    GPIOB->CRL|=0X00333330;
    GPIOB->ODR|=31<<1;        //PB1,2,3,4,5����

    AFIO->MAPR|=2<<24;      //�ر�JATG,ǧ���ܽ�SWDҲ�رգ�����оƬ���ϣ��ײ�!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    LedA_off;LedB_off;LedC_off;LedD_off;
   
}



