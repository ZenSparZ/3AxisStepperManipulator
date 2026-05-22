#include "stm32f10x.h"
#include "SysTick.h"
#include "system.h"
#include "usart.h"
#include "pump.h"
#include "motor.h"
#include "dj.h"
#include "step.h"
extern float t1,t2,t3;
extern int Flag;
int main()
{   
  int state=1;
	SysTick_Init(72);//系统时钟
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//抢占优先级
	USART1_Init(115200);///串口
	USART2_Init(115200);///串口
	step_control_Init();//限位器
	step_GPIO_Init();//步进dir
	TIM3_Step_Init(999,72-1);//步进定时器
   motor_Init();//刀片直流
	duoji_Init();//剪刀
	pump_Init();//气泵初始化
	Move_to_start();
  //USART1_on(); 
	pump1=1;pump2=1;
//						Move_to(90,85,60);
//						Move_to(160,60,80);
//	cut_move(8000,0);
//	cut_move(2000,1);
 while (1)
  { 
		
		
		switch(state)
		{ 
			case 1:
			{
			  motor=1;
				cut_move(8000,0);
				motor=0;
				state=2;
				break;
			}
			case 2://去黄；
			    {
			     //USART1_on(); 
//          if(Flag==1) state=2;
//          else 
//					{  
						scissors1=1;
						Move_to(90,45,150);
		        
						Move_to(160,60,100);
			       t1=0,t2=0,t3=0;
					  
						
					   delay_ms(1000);delay_ms(1000);delay_ms(1000);
						
             scissors1=0;
					   state=3;
						//}
					 break;
		      }
			case 3://机械臂归位
			   {
			     Move_to_start();
					 state=4;
					 break;
			   }
			case 4:
			{
				cut_move(2200,1);
				state=5;
				break;
			}
			case 5:
			{
			cut_move(6000,0);
      state=6;
			break;
			}
			default :
			state=1;
			break;
			}	

  }
}

