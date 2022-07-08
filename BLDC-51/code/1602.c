#include "reg51.h"
#include"1602.h"

#define uchar unsigned char
#define uint  unsigned int


void delay(uchar x)
{
  uchar i;
  while(x--)
  for(i=0;i<110;i++);
}

/********************************************************************
* 名称 : L1602_Write_Com(uchar del)
* 功能 : 1602命令函数
* 输入 : 输入的命令值
* 输出 : 无
***********************************************************************/

void L1602_Write_Com(uchar del)
{
	RS = 0;
	RW = 0;
	P0 = del;
	E = 1;
	delay(3);
	E=0;
}

/********************************************************************
* 名称 : L1602_Write_Data(uchar del)
* 功能 : 1602写数据函数
* 输入 : 需要写入1602的数据
* 输出 : 无
***********************************************************************/

void L1602_Write_Data(uchar del)
{
	RS = 1;
	RW = 0;
	P0 = del;
	E = 1;
	delay(3);
	E=0;
}

/********************************************************************
* 名称 : L1602_init()
* 功能 : 1602初始化，请参考1602的资料
* 输入 : 无
* 输出 : 无
***********************************************************************/
void L1602_init(void)
{
	L1602_Write_Com(0x01);
	L1602_Write_Com(0x38);
	L1602_Write_Com(0x06);
	L1602_Write_Com(0x0c);
	E=0;
}

/*//装入自定义字符（汉字）
void writch()
{
  uchar i,j;
  L1602_Write_Com(0x40);
  for(i=0;i<8;i++)
    for(j=0;j<8;j++)
	  L1602_Write_Data(hanzi[i][j]);
}	*/
/********************************************************************
* 名称 : L1602_Write_char(uchar hang,uchar lie,char sign)
* 功能 : 改变液晶中某位的值，如果要让第一行，第五个字符显示"b" ，调用该函数如下
		 L1602_Write_char(1,5,'b')
* 输入 : 行，列，需要输入1602的数据
* 输出 : 无
***********************************************************************/
void L1602_Write_char(uchar hang,uchar lie,char sign)
{
	uchar a;
	if(hang == 1) a = 0x80;
	if(hang == 2) a = 0xc0;
	a = a + lie - 1;
	L1602_Write_Com(a);
	L1602_Write_Data(sign);
}

/********************************************************************
* 名称 : L1602_Write_string(uchar hang,uchar lie,uchar *p)
* 功能 : 改变液晶中某位的值，如果要让第一行，第五个字符开始显示"ab cd ef" ，调用该函数如下
	 	 L1602_Write_string(1,5,"ab cd ef;")
* 输入 : 行，列，需要输入1602的数据
* 输出 : 无
***********************************************************************/
void L1602_Write_string(uchar hang,uchar lie,uchar *p)
{
	uchar a;
	if(hang == 1) a = 0x80;
	if(hang == 2) a = 0xc0;
	a = a + lie - 1;
	L1602_Write_Com(a);
	while(1)
	{
		if(*p == '\0') break;
		L1602_Write_Data(*p);
		p++;
	}
}