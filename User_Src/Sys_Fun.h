#ifndef _SYS_FUN_H_
#define _SYS_FUN_H_

void NVIC_INIT(void);
char SystemClock(char PLLMUL);
void PowerOn(void);
extern char SysClock;       //����洢ϵͳʱ�ӱ�������λMHz

#endif

