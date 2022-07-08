#include "reg51.h"
#include "stdio.h"

#include"1602.h"

sbit HC=P1^0;
sbit HB=P1^1;
sbit HA=P1^2;

sbit AL=P3^0;
sbit AH=P3^1;		
sbit BL=P3^6;
sbit BH=P3^3;
sbit CL=P3^4;
sbit CH=P3^5;


sbit KEY_right=P2^3;
sbit KEY_left=P2^4;

//TLC2543引脚定义
sbit ADout=P1^3;   
sbit ADin=P1^4;
sbit CS=P1^5;
sbit CLK=P1^6;
sbit EOC=P1^7;


	


unsigned char PWM = 20;
unsigned int adval;

char cnt_test_1s =0;
char dir_flag = 0; //方向
char key_flag = 0; 

char WHITH = 100;

unsigned int cnt = 0;

unsigned int cnt_speed = 0;	

unsigned int cnt_1s = 0;

char fankui = 0;


// PID变量定义
float speed_target_val = 10.0;
float speed_output_val = 0.0;
float speed_err = 0.0;
float speed_err_last = 0.0;
float speed_integral = 0.0;
float kp = 150.0, ki = 0.0002, kd = 6000.0;

int speed = 0, speed_target = 0;

/*****************************************************
函数功能：将模拟信号转换成数字信号
***************************************************/ 
// 其中 port 为通道: 通道0：port = 0x00 通道1：port = 0x10 通道2：port = 0x20 ... 

uint read2543(uchar port)				   
{
 	uchar ch,i,j;
	uint ad;
	ch=port;
	for(j=0;j<3;j++)			///////空循环三次，
	{	
		ad=0;
		ch=port;
		EOC=1;
		CS=1;
		CS=0;
		CLK=0;
		for(i=0;i<12;i++)
		{
			if(ADout) ad|=0x01;
			ADin=(bit)(ch&0x80);	
			if(i==7) ADin=1;   //设置双极性输出
			CLK=1;
			CLK=0;
			ch<<=1;
			ad<<=1;
		}
	}
	CS=1;								   /////////转换和读取数值
	while(!EOC);
	ad>>=1;
	return(ad);
}



void pid_controller()
{
	speed_err = speed_target - speed;
	speed_integral += speed_err;
	speed_output_val = (float)(kp*speed_err) + (float)(ki*speed_integral) + (float)(kd*(speed_err - speed_err_last)) ;
	
	// 调节PWM
	if(key_flag == 0)
	{		
		if(speed_output_val>=0)
		{
			dir_flag = 0;
			PWM = (unsigned char)speed_output_val;
		}
		else 
		{
			dir_flag = 1;
			PWM = 255 - (unsigned char)speed_output_val;
		}
	}
	else 
	{
		if(speed_output_val>=0)
		{
			dir_flag = 1;
			PWM = (unsigned char)speed_output_val;
		}
		else 
		{
			dir_flag = 0;
			PWM = 255 - (unsigned char)speed_output_val;
		}
	}
}

main()										
{
	
	char flag_start = 0;

	
	
	TMOD=0x11; //T	
	TL0=0xFC;    
	TH0=0x18; 
	ET0=1; 
	EA=1;
	TR0=0;    //启动T0

	EA = 1;	  //开全局中断

	L1602_init();

	
	L1602_Write_string(1,1,"aim speed:   ");


  while(1)
  {	
		
		
    adval=read2543(0x00);  //读取AD
		if(adval>1) 
		{
			flag_start=1;
//			PWM=adval*1.0/40.96;   //根据AD值设置占空比（0-100%）
			speed_target=(adval-2048)*1.0*3/2048*50/2;
			if(speed_target > 90) speed_target = 90;
		}
		else  flag_start=0;
		

		
		if( flag_start)
		{
			TR0=1;    //启动T0
	
			EA = 1;
      
      		if(read2543(0x10)>=2048) //正转
			{				
				speed=(read2543(0x10)-2048)*1.0*3/2048*50;  //读取计算转速
				L1602_Write_char(2,2,' ');
			}
			else   //反转
			{
				speed=(2048-read2543(0x10))*1.0*3/2048*50;  //读取计算转速
				L1602_Write_char(2,2,'-');
			}
			
      
			L1602_Write_char(2,3,speed_target/1000+'0');
			L1602_Write_char(2,4,speed_target%1000/100+'0');
			L1602_Write_char(2,5,speed_target%100/10+'0');
			L1602_Write_char(2,6,speed_target%10+'0');
			
		
		}
		else
		{
			TR0=0;    //关闭T0
			
			EA = 0;
			
			AH=0;AL=0;BH=0;BL=0;CH=0;CL=0;
			
		

			L1602_Write_char(2,3,' ');
			L1602_Write_char(2,4,' ');
			L1602_Write_char(2,5,' ');
			L1602_Write_char(2,6,' ');
		}	
	

	  	L1602_Write_char(2,8,'r');
		L1602_Write_char(2,9,'p');
		L1602_Write_char(2,10,'m');

		
	
	if(!KEY_right) 
	{
		while(!KEY_right);

		key_flag = 0;



	}

	if(!KEY_left)
	{
		while(!KEY_left);

		key_flag = 1;

	
	}
	

	fankui=((unsigned char)(HA)<<2) + ((unsigned char)(HB)<<1) + (unsigned char)(HC);
	
	pid_controller();

  }
}


/*********************************************************/
// 定时器0中断服务程序.
/*********************************************************/
void timer0() interrupt 1 
{  	  

	TH0  = (65535-500)/256; //50ms
	TL0  = (65535-500)%256;

	cnt++;
	if( cnt >= 100 )
	{
		cnt = 0;
	}
	
	if( cnt > PWM )
	{
		if(!dir_flag)
	   {
		
			switch(fankui)//
			{
			   case 0x06:  AH=0;AL=0;BH=1;BL=0;CH=0;CL=0;break;
				case 0x02:	AH=0;AL=0;BH=0;BL=0;CH=1;CL=0;break;
				case 0X03:	AH=0;AL=0;BH=0;BL=0;CH=1;CL=0;break;
				case 0x01:	AH=0;AL=0;BH=0;BL=1;CH=0;CL=0;break;
				case 0x05:	AH=1;AL=0;BH=0;BL=0;CH=0;CL=0;break;
				case 0x04:  AH=0;AL=0;BH=0;BL=0;CH=0;CL=1;break;
			   default: break;
			}
		}	
		else
		{
			switch(fankui)//
			{
			 case 0x06:  AH=0;AL=0;BH=0;BL=1;CH=0;CL=0;break;
			case 0x02:	AH=0;AL=0;BH=0;BL=0;CH=0;CL=1;break;
			case 0X03:	AH=0;AL=0;BH=0;BL=0;CH=0;CL=1;break;
			case 0x01:	AH=0;AL=0;BH=1;BL=0;CH=0;CL=0;break;
			case 0x05:	AH=0;AL=0;BH=0;BL=0;CH=1;CL=0;break;
			case 0x04:  AH=0;AL=0;BH=0;BL=0;CH=1;CL=0;break;
			 default: break;
			}
		}
	}
	else
	{
			
		if(!dir_flag)
	   {
		
			switch(fankui)//
			{
			   case 0x06:  AH=0;AL=1;BH=1;BL=0;CH=0;CL=0;break;
				case 0x02:	AH=0;AL=1;BH=0;BL=0;CH=1;CL=0;break;
				case 0X03:	AH=0;AL=0;BH=0;BL=1;CH=1;CL=0;break;
				case 0x01:	AH=1;AL=0;BH=0;BL=1;CH=0;CL=0;break;
				case 0x05:	AH=1;AL=0;BH=0;BL=0;CH=0;CL=1;break;
				case 0x04:  AH=0;AL=0;BH=1;BL=0;CH=0;CL=1;break;
			   default: break;
			}
		}	
		else
		{
			switch(fankui)//
			{
			 case 0x06:  AH=1;AL=0;BH=0;BL=1;CH=0;CL=0;break;
			case 0x02:	AH=1;AL=0;BH=0;BL=0;CH=0;CL=1;break;
			case 0X03:	AH=0;AL=0;BH=1;BL=0;CH=0;CL=1;break;
			case 0x01:	AH=0;AL=1;BH=1;BL=0;CH=0;CL=0;break;
			case 0x05:	AH=0;AL=1;BH=0;BL=0;CH=1;CL=0;break;
			case 0x04:  AH=0;AL=0;BH=0;BL=1;CH=1;CL=0;break;
			 default: break;
			}
		}
	}
}

