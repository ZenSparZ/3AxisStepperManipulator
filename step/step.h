#ifndef _step_H
#define _step_H
// 三个电机引脚定义（自己改）
// 关节1
#define STEP1_PIN    GPIO_Pin_6
#define DIR1_PIN     GPIO_Pin_12
#define STEP1_PORT   GPIOA
#define DIR1_PORT    GPIOB

// 关节2
#define STEP2_PIN    GPIO_Pin_7
#define DIR2_PIN     GPIO_Pin_14
#define STEP2_PORT   GPIOA
#define DIR2_PORT    GPIOC

// 关节3
#define STEP3_PIN    GPIO_Pin_0
#define DIR3_PIN     GPIO_Pin_15
#define STEP3_PORT   GPIOB
#define DIR3_PORT    GPIOC
//限位器
#define Step1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)
#define Step2  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)
#define Step3  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)

//初始角度
#define theta1_0   150.0f
#define theta2_0   80.0f
#define theta3_0   80.0f

    
extern int flag;
#include "system.h"
#include "Systick.h"
void step_control_Init(void);
void step_GPIO_Init(void);
void TIM3_Step_Init(uint16_t pre,uint16_t psc);
void Move_to(float th1, float th2, float th3);
void Move_to_start(void);
void motor_dir(int a,int b,int c);
void cut_move(int th,int dir);

#endif


