#include "74HC595.h"
#include "delay.h"
#include "pwm.h"
/*
【HC595_CLK】-->【SHCP_OUT】
【HC595_SCK】-->【STCP_OUT】
【HC595_DAT】-->【DS_OUT】
*/
u16 HC595_HEX[]={0x200,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x100,};
u32 HC595_NUM[]={0,1,2,3,4,5,6,7,8,9};
NixieModStructure MOD[ModeBlock];	//全局结构体变量
/*==================================
* 函数名 ：HC595_Init()
* 调  用 ：void
* 参  数 ：void
* 返回值 ：void
* 结  果 ：初始化74HC595
* 备  注 ：
===================================*/
void HC595_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_HC595_PORT, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);//打开AFIO时钟

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);//Full SWJ Disabled (JTAG-DP + SW-DP)
// 改变指定管脚的映射 GPIO_Remap_SWJ_Disable SWJ 完全禁用（JTAG-DP+SW-DP）

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE); //JTAG-DP Disabled and SW-DP Enabled
// 改变指定管脚的映射 GPIO_Remap_SWJ_JTAGDisable ，JTAG-DP 禁用 + SW-DP 使能

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(HC595_PORT, &GPIO_InitStructure);

	GPIO_SetBits(HC595_PORT,GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5);
}


/*==================================
* 函数名 ：HC595_Send1Data(u8 data)
* 调  用 ：u8 data
* 参  数 ：u8 data
* 返回值 ：void
* 结  果 ：74HC595发送1B数据
* 备  注 ：
===================================*/
void HC595_Send1Data(u8 data)
{
	u8 j;
	for (j = 8; j > 0; j--)
	{
		if(data & 0x80)
			HC595_DAT=1;
		else
			HC595_DAT=0;
		
		HC595_CLK=0;delay_us(1);      //上升沿发生移位
		
		data <<= 1;
		
		HC595_CLK=1;delay_us(1);
	}
	HC595_Load();
}

/*==================================
* 函数名 ：HC595_Load(void)
* 调  用 ：void
* 参  数 ：void
* 返回值 ：void
* 结  果 ：74HC595启动传输
* 备  注 ：
===================================*/
void HC595_Load(void)
{
	HC595_SCK=0;
	HC595_SCK=1;
}
/*==================================
* 函数名 ：HC595_RowOut(u32 Data)
* 调  用 ：u32 Data
* 参  数 ：u32 Data
* 返回值 ：void
* 结  果 ：74HC595发送1帧数据
* 备  注 ：
===================================*/
void HC595_RowOut(u32 Data)
{

//	HC595_Send1Data(Data >> 32);//0x
	HC595_Send1Data(Data >> 24);//0x
	HC595_Send1Data(Data >> 16);//0x08
	HC595_Send1Data(Data >> 8);//0x0840
	HC595_Send1Data(Data >> 0);//0x084020
	HC595_Load();
}

/*==================================
* 函数名 ：HC595_ShowNum(u32 number)
* 调  用 ：u32 number
* 参  数 ：u32 number
* 返回值 ：void
* 结  果 ：74HC595显示一个int型数字
* 备  注 ：
===================================*/
void HC595_ShowNum(u32 number)//e.g. : 1234 
{
	u32 NumValH=0,NumValL=0;
	u32 a=0,b=0,c=0,d=0;//千百十个
	
	a=HC595_HEX[number/1000];		//0x		//a
	NumValH = a;
	
	b=HC595_HEX[number%1000/100]<<20;	//0x200000;	//b
	c=HC595_HEX[number%100/10]<<10;		//0x001000
	d=HC595_HEX[number%10];				//0x000004
	HC595_OE=1;
	//!!!
	if(a==0x01 || a==0x02)
	{	
		HC595_Send1Data(0x00);
		NumValL = (NumValH<<30) + b + c + d;
	}
	else 
	{
		HC595_Send1Data(NumValH>>2);
		NumValL = b + c + d; 
	}	
	//PWM(40);
	//TIM_SetCompare1(TIM1,1000);
	HC595_RowOut(NumValL);
	//PWM(60); 
}
/*==================================
* 函数名 ：HC595_SetNum(u8 *p) && HC595_Update(void)
* 调  用 ：void
* 参  数 ：void
* 返回值 ：void
* 结  果 ：74HC595更新模块内数据
* 备  注 ：
===================================*/
void HC595_SetNum(u8 *p)//"010101."
{
	int8_t i=0;
//	u16 a=0,b=0;
	u32 BLOCK=0;
	for(i=2*ModeBlock-1;i>=0;i--)
	{
		if(p[i]>='0' && p[i]<='9')
		{
			if(i%2)//个位
			{
				switch(p[i]-'0')
				{
					case 0:	MOD[BLOCK].Ones_NUM=Ones_NUM_0;break;
					case 1:	MOD[BLOCK].Ones_NUM=Ones_NUM_1;break;
					case 2:	MOD[BLOCK].Ones_NUM=Ones_NUM_2;break;
					case 3:	MOD[BLOCK].Ones_NUM=Ones_NUM_3;break;
					case 4:	MOD[BLOCK].Ones_NUM=Ones_NUM_4;break;
					case 5:	MOD[BLOCK].Ones_NUM=Ones_NUM_5;break;
					case 6:	MOD[BLOCK].Ones_NUM=Ones_NUM_6;break;
					case 7:	MOD[BLOCK].Ones_NUM=Ones_NUM_7;break;
					case 8:	MOD[BLOCK].Ones_NUM=Ones_NUM_8;break;
					case 9:	MOD[BLOCK].Ones_NUM=Ones_NUM_9;break;
				}
			}
			else //十位
			{
				switch(p[i]-'0')
				{
				case 0:MOD[BLOCK].Tens_NUM=Tens_NUM_0;break;
				case 1:MOD[BLOCK].Tens_NUM=Tens_NUM_1;break;
				case 2:MOD[BLOCK].Tens_NUM=Tens_NUM_2;break;
				case 3:MOD[BLOCK].Tens_NUM=Tens_NUM_3;break;
				case 4:MOD[BLOCK].Tens_NUM=Tens_NUM_4;break;
				case 5:MOD[BLOCK].Tens_NUM=Tens_NUM_5;break;
				case 6:MOD[BLOCK].Tens_NUM=Tens_NUM_6;break;
				case 7:MOD[BLOCK].Tens_NUM=Tens_NUM_7;break;
				case 8:MOD[BLOCK].Tens_NUM=Tens_NUM_8;break;
				case 9:MOD[BLOCK].Tens_NUM=Tens_NUM_9;break;
				}
			BLOCK++;
			}
		}
	}
	
}
void HC595_Update(u8 mode)
{
	u32 i=0,NumVal;
	for(i=0;i<ModeBlock;i++)
	{
		NumVal=(MOD[i].Ones_CLO + MOD[i].Ones_NUM 
				+ MOD[i].Tens_NUM + MOD[i].Tens_CLO)<<(10*i);
	}
	if(mode==0)
	{
		TIM_CtrlPWMOutputs(TIM1,DISABLE);	//MOE 主输出使能关闭
		HC595_OE=0;
		HC595_RowOut(NumVal);
	}
	else 
	{
		TIM_CtrlPWMOutputs(TIM1,ENABLE);
		
		PWM(100);
		HC595_RowOut(NumVal);
		PWM(50);
		
	}
	
	

}
//hasnt done
void HC595_printf(u8 *str)//【56】//56-->3B-->24bits---->>(3-1)*B
{
	u8 j=0;
	u8 a,b,c,d;
	u32 NumVal=0;

//遍历数组

		a=str[j];		//十位
		b=str[j+1];		//个位
		c=str[j+2];
		d=str[j+3];
		
		if(str[j]==' ')		a = 0;//十位		
		if(str[j+1]==' ')	b = 0;//个位
		if(str[j+1]==' ')	b = 0;//个位
		if(str[j+1]==' ')	b = 0;//个位	
		
		if(a!=0||b!=0||c!=0||d!=0)
		{
			NumVal = (a-'0')*1000+(b-'0')*100+(c-'0')*10+(d-'0');
			HC595_ShowNum(NumVal);
		}	
			
}



