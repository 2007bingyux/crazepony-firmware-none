
//--------------------------------------------------------------------|
//      ____                      _____                  +---+        |
//     / ___\                     / __ \                 | R |        |
//    / /                        / /_/ /                 +---+        |
//   / /   ________  ____  ___  / ____/___  ____  __   __             |
//  / /  / ___/ __ `/_  / / _ \/ /   / __ \/ _  \/ /  / /             |
// / /__/ /  / /_/ / / /_/  __/ /   / /_/ / / / / /__/ /              |
// \___/_/   \__,_/ /___/\___/_/    \___ /_/ /_/____  /               |
//                                                 / /                |
//                                            ____/ /                 |
//                                           /_____/                  |                
//---------------------------------------------------------------------
//文件：mian.c
//编写者：小马
//编译环境：keil4
//初版时间: 2014-05-24
//适用版本：Crazepony-II第三版
//功能：
//1.飞机硬件初始化
//2.参数初始化
//3.定时器开
//4.等待中断到来


#include "config.h"        //包含所有的驱动头文件




/********************************************
              飞控主函数入口
功能：
1.初始化各个硬件
2.初始化系统参数
3.开定时器4等待数据中断到来
4.开定时器3串口广播实时姿态以及相关信息
********************************************/
int main(void)
{
    SystemClock(9);               //系统时钟初始化
    UART1_init(SysClock,115200); 	//串口1初始化
    NVIC_INIT();	                //中断初始化
    STMFLASH_Unlock();            //内部flash解锁
    LedInit();		                //IO初始化 
    delay_init(SysClock);         //滴答延时初始化
    BT_PowerInit();               //蓝牙电源初始化完成，默认关闭
    MotorInit();	                //马达初始化
    BatteryCheckInit();           //电池电压监测初始化
    IIC_Init();                   //IIC初始化
    MPU6050_DMP_Initialize();     //初始化DMP引擎
    PID_INIT();                   //PID参数初始化 
    ParameterRead();              //Flash参数读取
    NRF24L01_INIT();              //NRF24L01初始化
    SetRX_Mode();                 //设无线模块为接收模式
    PowerOn();                    //开机等待
    BT_on();                      //蓝牙开
    TIM3_Init(36,2000);	          //定时器3初始化，调试串口输出
    TIM4_Init(36,1000);	          //定时器4初始化，定时采样传感器数据，更新PID输出
    while (1)                     //等待数据更新中断到来
    {

      
//    switch(UART1_Get_Char())//检测串口发送数据，做相应处理
//       {
//         case 'w':
//                  LedA_on;LedB_on;LedC_on;LedD_on;
//                  //TxBuf[0]++;
//                  //UART1_Put_Package(TxBuf);
//                   LedA_on;LedB_on;LedC_on;LedD_on;
//                   MotoPWM+=10;
//                   UART1_Put_Char(0x5a);
//                  break;
//         case 's':
//                  LedA_off;LedB_off;LedC_off;LedD_off;
//                  UART1_Put_Char(0xaa);
//                  MotoPWM-=10;
//                  break;
//         default :break;
//       } 
//        MotorPwmFlash(MotoPWM,MotoPWM,MotoPWM,MotoPWM);    
//       
    }
}





