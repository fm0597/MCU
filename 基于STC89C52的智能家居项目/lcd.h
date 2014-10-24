#ifndef __lcd_H__
#define __lcd_H__

#include <reg52.h>
#include<intrins.h>

#define uchar unsigned char

sbit lcdrs = P1^0;   //¶¨Òå¶Ë¿Ú 
sbit lcdrw = P1^1;
sbit lcden = P1^2;

void  lcd_delay(uchar i);
uchar Read_LCD_State();
void lcd_check_busy();
void lcd_write_com(uchar com);
void lcd_write_data(uchar _data);
void lcd_init();
void lcd_wstring(uchar *s);
void lcd_clear();
