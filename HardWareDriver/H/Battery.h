#ifndef _Battery_H_
#define _Battery_H_
#include "stm32f10x.h"


#define  BatteryADmin   1800        //���ջ���Ԫ��������


void BatteryCheckInit(void);	
u16 Get_Adc_Average(u8 ch,u8 times);             
int GetBatteryAD(void);     
extern int BatteryAD;
#endif
                
        



