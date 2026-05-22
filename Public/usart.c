#include "usart.h"
#include "system.h"
int fputc(int ch,FILE *p)  //函数默认的，在使用printf函数时自动调用
{
	USART_SendData(USART2,(u8)ch);	
	while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
	return ch;
}

/*******************************************************************************
* 函 数 名         : USART1_Init
* 函数功能		   : USART1初始化函数
* 输    入         : bound:波特率
* 输    出         : 无
*******************************************************************************/ 
void USART1_Init(u32 bound)
{
   //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	 //打开时钟
 
	
	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TX			   //串口输出PA9
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	    //复用推挽输出
	GPIO_Init(GPIOA,&GPIO_InitStructure);  /* 初始化串口输入IO */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//RX			 //串口输入PA10
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		  //模拟输入
	GPIO_Init(GPIOA,&GPIO_InitStructure); /* 初始化GPIO */
	

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
	USART_Cmd(USART1, DISABLE);  //使能串口1 
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
		
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);//开启相关中断

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、	
}

/*******************************************************************************
* 函 数 名         : USART1_IRQHandler
* 函数功能		   : USART1中断函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/ 
// 全局变量
int16_t raw1,raw2,raw3;
volatile float  t1,t2,t3;
volatile int Flag=1;
void USART1_IRQHandler(void)
{
    uint8_t res;
    if(USART_GetITStatus(USART1, USART_IT_RXNE))
    {
        res = USART_ReceiveData(USART1);
        
        // 检测帧头
        if(res == 0xAA)
        {
            uint8_t dat[7];
            // 连续读取后面7个字节
            for(int i = 0; i < 7; i++)
            {
                while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
                dat[i] = USART_ReceiveData(USART1);
            }

            // 校验帧尾合法
            if(dat[6] == 0xBB)
            {
                // 小端拼接 有符号short，完全对应Python
                raw1 = (int16_t)(dat[0] | dat[1] << 8);
                raw2 = (int16_t)(dat[2] | dat[3] << 8);
                raw3 = (int16_t)(dat[4] | dat[5] << 8);

                // 还原真实角度
                t1 = raw1 / 100.0f;
							  if(t1>=180) t1=180;
							  else if(t1<70) t1=70;
							
                t2 = raw2 / 100.0f;
							  if(t2>=45) t2=45;
							  else if(t2<0) t2=0;
							
                t3 = raw3 / 100.0f;
							 if(t3>=130) t3=130;
							  else if(t3<10) t3=10;
               printf("%.2f  %.2f  %.2f ",t1,t2,t3);
							 Flag=0;
							USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
							USART_Cmd(USART1, DISABLE);
                // 直接在这里调用电机运动函数即可
            }
        }
    }
}

void USART1_on(void)
{ 	
	  USART_Cmd(USART1, ENABLE);
	  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}


void USART2_Init(u32 bound)
{
   //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	 //打开时钟
 
	
	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;//TX			   //串口输出PA2
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	    //复用推挽输出
	GPIO_Init(GPIOA,&GPIO_InitStructure);  /* 初始化串口输入IO */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;//RX			 //串口输入PA3
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		  //模拟输入
	GPIO_Init(GPIOA,&GPIO_InitStructure); /* 初始化GPIO */
	

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART2, &USART_InitStructure); //初始化串口1
	
	USART_Cmd(USART2, ENABLE);  //使能串口1 
	
	USART_ClearFlag(USART2, USART_FLAG_TC);
		
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
}

///*******************************************************************************
//* 函 数 名         : USART2_IRQHandler
//* 函数功能		   : USART2中断函数
//* 输    入         : 无
//* 输    出         : 无
//*******************************************************************************/ 
//void USART2_IRQHandler(void)                	//串口2中断服务程序
//{
//	u8 r;
//	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断
//	{
//		r =USART_ReceiveData(USART2);//(USART1->DR);	//读取接收到的数据
//		USART_SendData(USART2,r);
//		while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET);
//	} 
//	USART_ClearFlag(USART2,USART_FLAG_TC);
//} 	

