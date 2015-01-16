#ifndef _ReceiveData_H_
#define _ReceiveData_H_
#include "stm32f10x.h"


//RCң��
typedef struct int16_rcget
{
    float ROOL;
    float PITCH;
    float THROTTLE;
    float YAW;
}RC_GETDATA;


extern RC_GETDATA RC_DATA,RC_DATA_RAW;//���������RC����
extern uint8_t FLY_ENABLE;//����ʹ�ܶ�  7/-5    14/15

void ReceiveDataFormNRF(void);
void ReceiveDataFormUART(void);
void Send_PIDToPC(void);
void Send_AtitudeToPC(void);
//extern int  Rool_error_init;     //����ɻ���ɳ���ƫ��Rool_error_init�����������޸�;����ƫ��Rool_error_init�����������޸�
//extern int  Pitch_error_init;     //����ɻ���ɳ�ǰƫ��Pitch_error_init�����������޸�;����ƫ��Pitch_error_init�����������޸�



#endif

