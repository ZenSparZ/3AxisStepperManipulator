#include "stm32f10x.h"
#include "SysTick.h"
#include "system.h"
#include "usart.h"
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
	Move_to_start();

	Move_to(90,85,60);
	Move_to(160,60,80);
 while (1)
  { 
	  USART1_on(); 
      	
				Move_to(t1,45,150);
		        
				Move_to(160,t2,t3);
			       t1=0,t2=0,t3=0;
						
       
			     Move_to_start();
			

  }
}

