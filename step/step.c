#include "system.h"
#include "step.h"
#include <math.h>
void step_control_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;  
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure); 
}

void step_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;  
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure); 


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12;  
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_14|GPIO_Pin_15;  
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);

}
void TIM3_Step_Init(uint16_t pre,uint16_t psc)
{
    GPIO_InitTypeDef        GPIO_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
		NVIC_InitTypeDef       NVIC_InitStruct;
    TIM_OCInitTypeDef       TIM_OCInitStruct;

    // 1. 开启时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // 2. PA6 PA7 复用推挽
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 3. PB0 PB1 复用推挽
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1;
	  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 4. 定时器时钟源
    TIM_InternalClockConfig(TIM3);

    // 5. 时基配置
		
	  TIM_TimeBaseStruct.TIM_Period=pre;//设置周期值
	  TIM_TimeBaseStruct.TIM_Prescaler=psc;//设置预分频系数
    TIM_TimeBaseStruct.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStruct);
		//中断更新
		TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE);
		
		NVIC_InitStruct.NVIC_IRQChannel=TIM3_IRQn;
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
		NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
		NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
		NVIC_Init(&NVIC_InitStruct);

    // 6. 统一PWM配置
    TIM_OCStructInit(&TIM_OCInitStruct);
    TIM_OCInitStruct.TIM_OCMode      = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_OCPolarity  = TIM_OCPolarity_High;
    //TIM_OCInitStruct.TIM_Pulse       = 0;

    // 7. 四路通道一次性全部开启
    TIM_OC1Init(TIM3, &TIM_OCInitStruct);
    TIM_OC2Init(TIM3, &TIM_OCInitStruct);
    TIM_OC3Init(TIM3, &TIM_OCInitStruct);
    TIM_OC4Init(TIM3, &TIM_OCInitStruct);

    // 预装载使能
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

    // 开启定时器
    TIM_Cmd(TIM3, ENABLE);
}
volatile uint32_t m1_step=0;
volatile uint32_t m2_step=0;
volatile uint32_t m3_step=0;
volatile uint32_t m4_step=0;

int flag=0;

void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

			     if(Step1==0&&flag==0)    m1_step=0;
				   if(Step2==0&&flag==0)    m2_step=0;
				   if(Step3==0&&flag==0)    m3_step=0;
        // 电机1脉冲控制
        if (m1_step > 0)
        {
            static uint8_t m1_pulse = 0;
            if (m1_pulse == 0)
            {
                TIM_SetCompare1(TIM3, 500);  // 输出高
                m1_pulse = 1;
            }
            else
            {
                TIM_SetCompare1(TIM3, 0);    // 输出低
                m1_pulse = 0;
                m1_step--;
            }
        }
        else
        {
            TIM_SetCompare1(TIM3, 0);  // 停止脉冲
        }

        // 电机2脉冲控制
        if (m2_step > 0)
        {
            static uint8_t m2_pulse = 0;
            if (m2_pulse == 0)
            {
                TIM_SetCompare2(TIM3, 500);
                m2_pulse = 1;
            }
            else
            {
                TIM_SetCompare2(TIM3, 0);
                m2_pulse = 0;
                m2_step--;
            }
        }
        else
        {
            TIM_SetCompare2(TIM3, 0);
        }

        // 电机3脉冲控制
        if (m3_step > 0)
        {
            static uint8_t m3_pulse = 0;
            if (m3_pulse == 0)
            {
                TIM_SetCompare3(TIM3, 500);
                m3_pulse = 1;
            }
            else
            {
                TIM_SetCompare3(TIM3, 0);
                m3_pulse = 0;
                m3_step--;
            }
        }
        else
        {
            TIM_SetCompare3(TIM3, 0);
        }
 
        // 所有电机都走完，再关闭中断（可选）
        if (m1_step == 0 && m2_step == 0 && m3_step == 0)
        {
            TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
        }
    }
}

//(160-th1,85-th2,th3-20)
void Move_to(float th1, float th2, float th3)
{ 
	th1=160-th1;
	th2=85-th2-30;
	th3=160-th3;
	motor_dir(1,1,1);
	m1_step=(uint32_t)(fabs(th1) * 1600/360.0f*4);
	m2_step=(uint32_t)(fabs(th2) * 1600/360.0f*4);
	m3_step=(uint32_t)(fabs(th3) * 1600/360.0f*4);
	flag=1;
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	while(m1_step>0 || m2_step > 0 || m3_step > 0);
}

void Move_to_start(void)
{  
	motor_dir(0,0,0);
	flag=0;
	m1_step=0;
	m2_step=2000;
	m3_step=2000;
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	while(Step2||Step3);	
	
	m1_step=2000;
	m2_step=0;
	m3_step=0;
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	while(Step1||Step2||Step3);	
}



void motor_dir(int a,int b,int c)
{
	if(a==1) GPIO_SetBits(GPIOB,GPIO_Pin_12);
	else GPIO_ResetBits(GPIOB,GPIO_Pin_12);
	
	if(b==1) GPIO_SetBits(GPIOC,GPIO_Pin_14);
	else GPIO_ResetBits(GPIOC,GPIO_Pin_14);
	
	if(b==1) GPIO_SetBits(GPIOC,GPIO_Pin_15);
	else GPIO_ResetBits(GPIOC,GPIO_Pin_15);
	
}
void cut_move(int th, int dir)
{
    // 1. 设置方向
    if(dir == 1)
        GPIO_SetBits(GPIOA, GPIO_Pin_4);
    else
        GPIO_ResetBits(GPIOA, GPIO_Pin_4);

    // 2. 计算需要的步数
    uint32_t total_step = (uint32_t)(th * 1600 / 360.0f * 4);
    uint8_t m4_pulse = 0; // 把变量移到循环外，保留状态

    for(uint32_t i = 0; i < total_step; i++)
    {
        if (m4_pulse == 0)
        {
            TIM_SetCompare4(TIM3, 500); // 高电平
            m4_pulse = 1;
        }
        else
        {
            TIM_SetCompare4(TIM3, 0);   // 低电平
            m4_pulse = 0;
        }
        delay_us(200); // 脉冲间隔，控制转速
    }

    // 3. 走完后强制停止输出，防止误触发
    TIM_SetCompare4(TIM3, 0);
}
