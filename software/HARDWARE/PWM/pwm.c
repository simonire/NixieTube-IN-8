#include "pwm.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK Mini STM32������
//PWM  ��������			   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/12/03
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  
uint16_t indexWave[] = {
	1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 4,
	4, 5, 5, 6, 7, 8, 9, 10, 11, 13,
	15, 17, 19, 22, 25, 28, 32, 36,
	41, 47, 53, 61, 69, 79, 89, 102,
	116, 131, 149, 170, 193, 219, 250,
	284, 323, 367, 417, 474, 539, 613,
	697, 792, 901, 1024, 1024, 901, 792,
	697, 613, 539, 474, 417, 367, 323,
	284, 250, 219, 193, 170, 149, 131, 
	116, 102, 89, 79, 69, 61, 53, 47, 41,
	36, 32, 28, 25, 22, 19, 17, 15, 13, 
	11, 10, 9, 8, 7, 6, 5, 5, 4, 4, 3, 3,
	2, 2, 2, 2, 1, 1, 1, 1
};

uint16_t POINT_NUM = sizeof(indexWave)/sizeof(indexWave[0]);//110

//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM1_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);// 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);  //ʹ��GPIO����ʱ��ʹ��
	                                                                     
   //���ø�����Ϊ�����������,���TIM1 CH1��PWM���岨��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //TIM_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 80K
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  ����Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0; //���ô�װ�벶��ȽϼĴ������ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
	
	TIM_CtrlPWMOutputs(TIM1,ENABLE);	//MOE �����ʹ��	

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH1Ԥװ��ʹ��	 
	
	TIM_ARRPreloadConfig(TIM1, ENABLE); //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
	
	TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1
   
}

void TIM1_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
		{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
		}
}
void PWM(u32 Val)//0-100
{
	u8 dir=0;
	
	Val+=5;
	
	if(Val<=(POINT_NUM/2))
	{
		for(dir = 0;dir < Val;dir++)	   					 
		{	
			TIM_SetCompare1(TIM1,indexWave[dir]);
			delay_ms(10);
		}	
	}
	else if(Val>(POINT_NUM/2) && Val<=POINT_NUM)
	{
		for(dir = (POINT_NUM/2);dir < POINT_NUM-10;dir++)	   					 
		{	
			TIM_SetCompare1(TIM1,indexWave[dir]);
			delay_ms(10);
		}	
	}
//	for(dir = 0;dir < POINT_NUM;dir++)	   					 
//	{	
//		TIM_SetCompare1(TIM1,indexWave[dir]);
//		delay_ms(50);
//	}	
}
