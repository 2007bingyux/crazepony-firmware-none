#ifndef _ReceiveData_H_
#define _ReceiveData_H_
#include "stm32f10x.h"




void ReceiveDataFormNRF(void);
void ReceiveDataFormUART(void);

extern int  Rool_error_init;     //如果飞机起飞朝左偏，Rool_error_init朝正向增大修改;朝右偏，Rool_error_init朝负向增大修改
extern int  Pitch_error_init;     //如果飞机起飞朝前偏，Pitch_error_init朝负向增大修改;朝吼偏，Pitch_error_init朝正向增大修改



#endif

