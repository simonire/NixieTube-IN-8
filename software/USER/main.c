#include "delay.h"
#include "sys.h"

#include "usart.h"
#include "pwm.h"

#include "74HC595.h"

#include "iic.h"
#include "ds3231.h"

/************************************************/
extern Calendar_OBJ calendar; 		//日历结构体
u8 calendarstr[100];				//日历数组打印数据流
/************************************************
        TX     RX
串口1   PA9   PA10
串口2   PA2   PA3  未用到    
串口3   PB10  PB11 未用到
**************************
【74595】
	DAT = PB3
	CLK = PB4
	SCK = PB5
	OE  = PA8
【DS3231】
	SCL = PA1
	SDA = PA2
************************************************/

void BSP_Init(void)
{
	delay_init();	    	 //延时函数初始化	  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    TIM1_PWM_Init(1024-1,200-1);//不分频。PWM频率=72000/(899+1)=80Khz//1024-1,200-1 
	uart_init(115200);	 //串口初始化为115200
	
	HC595_Init();
	DS3231_Init();
}
int main(void)
{
	u8 str[50];
	BSP_Init();
//	Set_DS3231_Time(19,12,28,23,29,00,6);
	while(1)
	{	
		Get_DS3231_Time();
		TIM_SetCompare1(TIM1,1250);
		
		if(calendar.hour == 24)
			calendar.hour = 0;
		if(calendar.sec == 60)
			calendar.sec = 0;
		if(calendar.min == 60)
			calendar.min = 0;
	
		printf(" %02d:%02d:%02d ",calendar.hour,calendar.min,calendar.sec);
		
		sprintf((char *)str,"%02d%02d",calendar.min,calendar.sec);
		HC595_printf(str);
				
	}
	
	

}	

/*


		//20:10:36
		
//		switch(times)
//		{
//		case 1:
//			sprintf((char *)str,"%02d%02d",calendar.hour,calendar.min);
//			break;
//		case 2:
//			sprintf((char *)str,"%01d%02d%01d",(calendar.hour)%10,calendar.min,(calendar.sec)/10);
//			break;	
//		case 3:
			sprintf((char *)str,"%02d%02d",calendar.min,calendar.sec);
//			break;	
//		}
//		times++;
		HC595_printf(str);
		
	//	PWM(40);
		

	
	//	PWM(80);
	//	
	
	//	delay_ms(250);
		



*/


