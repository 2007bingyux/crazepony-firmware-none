#ifndef _tim_H_
#define _tim_H_
#include "stm32f10x.h"

void TIM4_Init(char clock,int Preiod);//���ڼ��ϵͳ
void TIM3_Init(char clock,int Preiod);//��ʱ��3�ĳ�ʼ��
void TimerNVIC_Configuration(void);//��ʱ���ж�����������

extern volatile uint16_t anyCnt,anyCnt2,loop100HzCnt,loop200HzCnt;
extern uint8_t  loop200HzFlag,loop500HzFlag,loop50HzFlag,loop600HzFlag,loop100HzFlag,loop10HzFlag,loop20HzFlag;

#endif

