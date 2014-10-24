 #include "lcd.h"
void lcd_delay(uchar i)
{
	uchar x = 100;
	while(i--)
	while(x--);
}
uchar Read_LCD_State()
{
 	uchar state;
	lcdrs=0;lcdrw=1;lcden=1;lcd_delay(1);
	state=P0;
	lcden = 0;lcd_delay(1);
	return state;
}
void lcd_check_busy()
{
 	while((Read_LCD_State()&0x80)==0x80);
	lcd_delay(1);
}
void lcd_write_com(uchar com)
{
	lcd_check_busy();
	lcdrs=0;			//选择写命令状态
	lcdrw=0;
	P0=com;			//命令发送到P0口
	lcd_delay(1);			//延时使数据稳定
	lcden=1;			//给使能端一个高脉冲
	lcd_delay(1);			//延时
	lcden=0;			//使能端置0完成高脉冲
}


void lcd_write_data(uchar _data)
{
	lcd_check_busy();
	lcdrs=1;	
	lcdrw=0;			
	P0=_data;
	lcd_delay(1);
	lcden=1;
	lcd_delay(1);
	lcden=0;
}
void lcd_init()
{
	lcd_write_com(0x38);			//设置16*2显示，5*7点阵，8位数据接口
	lcd_write_com(0x0c);			//设置开显示，不显示光标
	lcd_write_com(0x06);			//写一个字符后地址指针加1
	lcd_write_com(0x01);			//显示清0，数据指针清0
	lcden=0;	
}
void lcd_wstring(uchar *s)
{
	while(*s)
	{
		lcd_write_data(*s++);
	}
}
void lcd_clear()
{
	lcd_write_com(0x01);
}