										   							 
#include <reg52.h>
#include <intrins.h>
#include <string.h>
#include <lcd.h>
#define uchar unsigned char 
#define uint  unsigned int
#define flag  unsigned char

char *s="www.zhbit.com";

sbit lcd_bk_light=P1^3;			//lcd背光控制端口

sbit  infrared_flag=P2^0;		//红外传感器状态
sbit  lamp=P2^1;				//台灯，接继电器，低电平点亮
sbit  light_button=P2^2;		//照明led手动开关
sbit  light=P2^3;				//照明led
sbit  light_status=P2^4;		//光敏状态
sbit  pilot_light=P2^5;			//照明手动开关指示灯
sbit  beep=P2^6;				//蜂鸣器

uint  light_num=0;				//控制照明led亮的秒数
uint  infrared_num=0;			//红外延时参数

flag light_flag=0;			    //照明led自动亮的开关
flag infrared_ex_status;		//红外上一状态
flag infrared_2stimeflag;		//红外2s后关闭标志
flag infrared_switch=0;			//红外感应总开关,0/1:开关开/开关关
flag bluetooth_lamp_flag=0;		//蓝牙台灯手动开关，0/1:关闭标记/打开标记
flag lamp_close_flag=0;			//不受红外控制标记,0/1:关闭标记/打开标记
flag beepflag=0;				//声音标记，0/1:声音开/声音关
flag firebeep_flag = 0;			//烟雾传感器
flag lcd_bk_flag = 0;

void  delay(uchar i)
{
	uchar j=110;
	while(i--)
	while(j--);
}
void beepbeep()
{
	if(beepflag==0)
	{
		beep=0;
		delay(5);
		beep=1;
	}
	
}
void timebeepbeep()
{
	if(beepflag==0)
	{
		beep=0;
		delay(30);
		beep=1;
	}
}
void time0()interrupt 1
{
	TH0=(65536-50000)/256;
	TL0=(65536-50000)%256;
	if(light_flag)				//照明led定时
	{
		light_num++;
		if(light_num>20*15)
		{
			light=1;
			light_num=0;
			EX0=1;
			light_flag=0;
			//light_status=0;
		//	beepbeep();
		}
	}
	
	
	if(infrared_2stimeflag==1)		//台灯关闭时延时部分
	{
		infrared_num++;
		if(infrared_num>=20*2)
		{
			lamp=1;
			infrared_num=0;
			infrared_2stimeflag=0;
			infrared_ex_status=0;
			if(light_status==1)		//当离开红外感应时，检测环境亮度
			{
				light_flag=1;
				light=0;
			}
			else light_flag=0;
		}
	}
	
	
}
void sendmes(uchar *p)
{
	while(*p)
	{
		SBUF=*p;
		while(TI==0);
		TI=0;
		p++;
	}
}
void check()
{
	
	uchar *str1="Lamp:Always On!";
	uchar *str2="Lamp:On! ";
	uchar *str3="Lamp:Off!";
	uchar *str4="Lamp:Always Off!";
	uchar *str5="The infrared:OFF!";
	uchar *str6="The infrared:On!";
	uchar *str7="Beep:Open!";
	uchar *str8="Beep:close!";

	lcd_write_com(0x01);
	if(bluetooth_lamp_flag==1)
	{
		sendmes(str1);
		lcd_write_com(0x80);
		lcd_wstring(str1);
	}
	if(bluetooth_lamp_flag==0&&lamp==0)
	{
		sendmes(str2);
		lcd_write_com(0x80);
		lcd_wstring(str2);
	}
	if(lamp==1&&lamp_close_flag==0)
	{
		sendmes(str3);
		lcd_write_com(0x80);
		lcd_wstring(str3);
	}
	if(lamp_close_flag==1)
	{
		sendmes(str4);
		lcd_write_com(0x80);
		lcd_wstring(str4);
	}
	if(beepflag==0)
	{
		sendmes(str7);
		lcd_write_com(0xc0);
		lcd_wstring(str7);
	}
	else 
	{
		sendmes(str8);
		lcd_write_com(0xc0);
		lcd_wstring(str8);
	}

	beepbeep();
	
}

void a_com()				//台灯开，强制开，不受彻底关闭模式限制
{
	if(lamp==1)
	{
		lamp=0;
		lamp_close_flag=0;		//关闭彻底关闭模式
		bluetooth_lamp_flag=1;	
	//	beepbeep();
	}
	
}
void b_com()				//台灯关，关闭后为受红外控制状态
{
	if(lamp==0)
	{
		lamp=1;
		bluetooth_lamp_flag=0;
	//	beepbeep();
	}
	
}
void c_com()				//led照明开
{
	if(light==1)
	{
		light=0;
		light_flag=1;
	//	beepbeep();
	}
	
}
void d_com()
{
}
void e_com()
{
}
void f_com()						//看电影模式，关闭lamp，不受红外控制
{
	if(lamp_close_flag==0)			//打开标记
	{
		lamp_close_flag=1;
		if(lamp==0)
		b_com();					//关闭台灯
	//	else beepbeep();
	}
}
void g_com()						//关闭看电影模式，回到正常状态
{
	if(lamp_close_flag==1)
	{
		lamp_close_flag=0;
		beepbeep();
	}
}
void t_com()
{

}
void v_com()
{
	if(beepflag==0) beepflag=1;
	else beepflag=0;
}
void com_chose(uchar com)
{	
	switch(com)
	{
		case 'x':check();break;					   	//查询状态
		case 'a':a_com();check();break;		  			   //开灯，不受红外控制
		case 'b':b_com();check();break;				       //关灯，受红外控制
		case 'c':c_com();check();break;					   //开照明led
		case 'd':d_com();check();break;
		case 'e':e_com();check();break;
		case 'f':f_com();check();break;						//看电影模式，关灯，不受红外控制
		case 'g':g_com();check();break;						//关闭看电影模式，回正常状态
		case 't':t_com();check();break;						//查看时间
		case 'v':v_com();check();break;						//音效开关
	}
}
void serial_int() interrupt 4		//串口中断服务程序
{
	uchar com;
	if(RI)
	{
		com=SBUF;
		com_chose(com);
		RI=0;
	}

}

void Infrared()
{
	if(!infrared_switch)
	{
		if(bluetooth_lamp_flag==1)		//蓝牙手动开关
		{
			lamp=0;
		}
		else if(infrared_flag==0)		//红外感应检测到人体
		{
			if(lamp_close_flag==0)		//屏蔽红外标志
			lamp=0;						//台灯开
			infrared_ex_status=1;		//状态置为1
		}
		else if(infrared_ex_status==0)
		{
			lamp=1;
		}
		else if(infrared_ex_status==1)
		{
			infrared_2stimeflag=1;
		}
	}
		
	
	
}
void Light()
{
	if(!light_button)		
	{
		//EX0=0;			//关中断
		pilot_light=0;		//指示灯开
		if(light_flag==1) 
		{light_flag=0;}		//检测是否在自动照明模式，是就关闭自动照明模式
		light=0;
		
	}
	else 
	{
	//	EX0=1;				//开中断
		pilot_light=1;		//指示灯关
		if(light_flag==0)	//检测是否在自动照明模式，否就关闭灯
		light=1;
		
	}
	

	if(lcd_bk_flag)		 	//控制lcd背光
	{
		lcd_bk_light = 0;	
	}
	else lcd_bk_light = 1;
}
void int0()interrupt 0
{
	beep = 0;
	delay(30);
	beep = 1;
	delay(30);
}
void int1()interrupt 2
{
	lcd_bk_flag = ~lcd_bk_flag;
	beepbeep();
}

void init()
{
	TMOD=0x01;
	TMOD|=0x20;
	TH0=(65536-50000)/256;
	TL0=(65536-50000)%256;

	TH1=0xfd;
	TL1=0xfd;
	
	EA=1;
	ET0=1;
	
	EX0=1;
	IT0=0;

	EX1=1;
	IT1=1;

	TR0=1;
	
	SCON=0x50;
	PCON=0x00;
	TR1=1;
	ES=1;

	lcd_init();
	lcd_write_com(0x82);
	lcd_wstring("Welcome To");
	lcd_write_com(0xc0);
	lcd_wstring("Intelligent Home");

}

void main()
{
	
	
	init();
	
	
	while(1)
	{
		Light();
		
		Infrared();
		
	
	}
}
