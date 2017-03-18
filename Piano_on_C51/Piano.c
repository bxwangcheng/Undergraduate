#include <stdio.h>
#include <reg51.h>
#include <absacc.h>
#include <intrins.h>
#define BUSYFLAG 0x80
#define BLINK 0x01
#define NOBLINK 0x00
#define LCM_DB P1
#define KEYBOARD XBYTE[0x7f00]

unsigned int data DelayConst = 140;

sbit LCM_RS = P3^2;
sbit LCM_RW = P3^3;
sbit LCM_E = P3^4;
sbit SPEAKER = P3^5;
sbit RAM_Write = P3^6;
sbit RAM_Read =P3^7;
sbit RAM_Control = P2^7;

unsigned char checkkey();
unsigned char keyscan();
void delay(unsigned int i);
void LCMDelay(int ms);
unsigned char LCMReadState(void);
void LCMClear(void);
void LCMWriteCmd(unsigned char cmd);
void LCMWriteData(unsigned char dc);
void LCMInit(void);
void LCMGotoXY(unsigned char x,unsigned char y);
void LCMDisplayChar(unsigned char x,unsigned char y,unsigned char ch);
void LCMDisplayString(unsigned char x,unsigned char y,unsigned char* str);
void LCMBlink(unsigned char x,unsigned char y,unsigned char cmd);
void record(unsigned char);
void gap(unsigned char x);

void sound(unsigned char x,unsigned char y);

void delay8um(unsigned char x);
void beat_125(unsigned char x);
void play(unsigned char song[],unsigned char beat[]);
void playrec(unsigned char mklevel[],unsigned char rec[]);
unsigned char Displaynote(unsigned char x,unsigned char y);
unsigned char Displaylevel(unsigned char x,unsigned char y);

unsigned char xdata alltone[3][9]={	{229,217,204,193,182,172,153,136,121},
					{115,108,102,97,91,86,77,68,61},
					{57,54,51,48,45,43,38,34,30}};  //包含低中高音
unsigned char xdata tone[]={115,108,102,97,91,86,77,68,61};		//当前音调
unsigned char xdata low[]={229,217,204,193,182,172,153,136,121};	//低音
unsigned char xdata middle[]={115,108,102,97,91,86,77,68,61};		//中音
unsigned char xdata high[]={57,54,51,48,45,43,38,34,30};		//高音


unsigned char xdata song1[]={6,5,3,5,7,6,5,6,3,5,6,5,3,2,1,6,5,3,2,2,3,5,5,6,3,2,1,5,3,2,1,6,1,1,15};		//歌曲1音调
unsigned char xdata beat1[]={16,8,8,8,8,4,4,16,8,4,4,8,4,4,4,4,4,4,16,16,8,8,4,4,12,8,16,16,8,4,4,4,4,16};	//歌曲1节拍
unsigned char xdata song2[]={1,2,8,8,5,1,3,3,3,5,4,3,5,4,3,2,2,1,0,1,2,3,4,3,2,3,4,5,5,4,3,2,1,15};		//歌曲2音调
unsigned char xdata beat2[]={7,7,7,7,8,9,1,2,3,6,2,4,4,4,4,12,8,2,2,4,4,4,8,2,2,4,4,4,2,2,4,4,12,};		//歌曲2节拍
unsigned char xdata song3[]={1,2,8,8,5,1,3,3,3,5,4,3,5,4,3,2,2,1,0,1,2,3,4,3,2,3,4,5,5,4,3,2,1,15};		//歌曲3音调
unsigned char xdata beat3[]={7,7,7,7,8,9,1,2,3,6,2,4,4,4,4,12,8,2,2,4,4,4,8,2,2,4,4,4,2,2,4,4,12,};		//歌曲3节拍
unsigned char xdata rec1[5]={0,0,0,0,0};	//音轨1音调
unsigned char xdata mklevel1[5]={1,1,1,1,1};	//音轨1音高
unsigned char xdata rec2[5]={1,1,1,1,1};	//音轨2音调
unsigned char xdata mklevel2[5]={1,1,1,1,1};	//音轨2音高
unsigned char xdata rec3[5]={2,2,2,2,2};	//音轨3
unsigned char xdata mklevel3[5]={1,1,1,1,1};	//音轨3音高
unsigned char xdata csong[35];	//当前歌曲音调
unsigned char xdata cbeat[34];	//当前歌曲节拍
unsigned char xdata crec[5]={0,1,2,3,4};
unsigned char xdata cmklevel[5]={1,1,1,1,1};
unsigned char temp=0;	//判断按键是否已被记录过
unsigned char t=0;	//录制音符计数
unsigned char tk=1;	//选择音轨
unsigned char ms=1;     //选择歌曲
unsigned char level=1;	//当前音高


/*******************************************************主程序********************************************************/
void main(void)
{
	unsigned char key;
	unsigned char q=1;
	unsigned char c;
	LCMInit();
	LCMClear();
	LCMWriteCmd(0x0c);
	LCMDisplayString(0,5,"Welcome!");
	LCMDisplayString(1,0,"----------------");
	while(1)
	{
		key=keyscan();
		switch(key)
		{
			case 0:
				LCMClear();
				LCMDisplayString(0,7,"Do");
				sound(level,0);
				temp=1;
				record(0);
			break;
			
			case 1:
				LCMClear();
				LCMDisplayString(0,7,"Mi");
				sound(level,4);
				temp=1;
				record(4);
			break;
			
			case 2:
				LCMClear();
				LCMDisplayString(0,7,"Si");
				sound(level,8);
				temp=1;
				record(8);
			break;
			
			case 3:
				start3:
				for(;ms<4;)
				{
					switch(ms)
					{
						case 1:
							LCMClear();
							LCMDisplayString(0,5,"Now on");
							LCMDisplayString(1,4,"Music  1");
							for(c=0;c<35;c++)
							{
								csong[c]=song1[c];
							}
							for(c=0;c<35;c++)
							{
								cbeat[c]=beat1[c];
							}
						break;

						case 2:
							LCMClear();
							LCMDisplayString(0,5,"Now on");
							LCMDisplayString(1,4,"Music  2");
							for(c=0;c<35;c++)
							{
								csong[c]=song2[c];
							}
							for(c=0;c<35;c++)
							{
								cbeat[c]=beat2[c];
							}
						break;

						case 3:
							LCMClear();
							LCMDisplayString(0,5,"Now on");
							LCMDisplayString(1,4,"Music  3");
							for(c=0;c<35;c++)
							{
								csong[c]=song3[c];
							}
							for(c=0;c<35;c++)
							{
								cbeat[c]=beat3[c];
							}
						break;
					}
					ms++;
					goto jump3;
				}
				ms=1;
				goto start3;
			jump3:break;
			
			case 4:
				LCMClear();
				LCMDisplayString(0,7,"Do#");
				sound(level,1);
				temp=1;
				record(1);
			break;
			
			case 5:
				LCMClear();
				LCMDisplayString(0,7,"Fa");
				sound(level,5);
				temp=1;
				record(5);
			break;
			
			case 6:
				start6:
				for(;q<4;)
				{
					switch(q)
					{
						case 1:
							LCMClear();
							LCMDisplayString(1,9,"LOW");
							level=0;
						break;

						case 2:
							LCMClear();
							LCMDisplayString(1,9,"MIDDLE");
							level=1;
						break;

						case 3:
							LCMClear();
							LCMDisplayString(1,9,"HIGH");
							level=2;
						break;
					}
					q++;
					goto jump6;
				}
				q=1;
				goto start6;
			jump6:break;
								
			case 7:
				switch(ms)
					{
						case 2:
							LCMClear();
							LCMDisplayString(0,0,"Play Music 1 ..");
							play(csong,cbeat);
						break;

						case 3:
							LCMClear();
							LCMDisplayString(0,0,"Play Music 2 ..");
							play(csong,cbeat);
						break;

						case 4:
							LCMClear();
							LCMDisplayString(0,0,"Play Music 3 ..");
							play(csong,cbeat);
						break;
					}
			break;

			case 8:
				LCMClear();
				LCMDisplayString(0,7,"Re");
				sound(level,2);
				temp=1;
				record(2);
			break;
			
			case 9:
				LCMClear();
				LCMDisplayString(0,7,"So");
				sound(level,6);
				temp=1;
				record(6);
			break;
			
			case 10:
				LCMClear();
				LCMDisplayString(0,0,"Please play");
				LCMDisplayString(1,9,"5 notes");
				t=0;
			break;
			
			case 11:
				switch(tk)
					{
						case 2:
							LCMClear();
							LCMDisplayString(0,0,"1:");
							playrec(mklevel1,rec1);
						break;

						case 3:
							LCMClear();
							LCMDisplayString(0,0,"2:");
							playrec(mklevel2,rec2);
						break;

						case 4:
							LCMClear();
						  LCMDisplayString(0,0,"3:");
							playrec(mklevel3,rec3);
						break;
					}
			break;
			
			case 12:
				LCMClear();
				LCMDisplayString(0,7,"Re#");
				sound(level,3);
				temp=1;
				record(3);
			break;
			
			case 13:
				LCMClear();
				LCMDisplayString(0,7,"La");
				sound(level,7);
				temp=1;
				record(7);
			break;
			
			case 14:
				switch(tk)
					{
						case 2:
							LCMClear();
							LCMDisplayString(0,0,"Recorded to");
							LCMDisplayString(1,9,"Track 1");
							for(c=0;c<5;c++)
							{
								rec1[c]=crec[c];
								mklevel1[c]=cmklevel[c];
							}
							t=0;
						break;

						case 3:
							LCMClear();
							LCMDisplayString(0,0,"Recorded to");
							LCMDisplayString(1,9,"Track 2");
							for(c=0;c<5;c++)
							{
								rec2[c]=crec[c];
								mklevel2[c]=cmklevel[c];
							}
							t=0;
						break;

						case 4:
							LCMClear();
							LCMDisplayString(0,0,"Recorded to");
							LCMDisplayString(1,9,"Track 3");
							for(c=0;c<5;c++)
							{
								rec3[c]=crec[c];
								mklevel3[c]=cmklevel[c];
							}
							t=0;
						break;
					}
			break;

			case 15:
				start15:
				for(;tk<4;)
				{
					switch(tk)
					{
						case 1:
							LCMClear();
							LCMDisplayString(0,5,"Now on");
							LCMDisplayString(1,4,"Track  1");
						break;

						case 2:
							LCMClear();
							LCMDisplayString(0,5,"Now on");
							LCMDisplayString(1,4,"Track  2");
						break;

						case 3:
							LCMClear();
							LCMDisplayString(0,5,"Now on");
							LCMDisplayString(1,4,"Track  3");
						break;
					}
					tk++;
					goto jump15;
				}
				tk=1;
				goto start15;
			jump15:break;
		}
	}
}


/****************************************************音频模块********************************************************/
void delay8um(unsigned char x)//8ms延时函数
{
	unsigned char i,j;
	for(i=0;i<x;i++)
		for(j=0;j<1;j++);
}

void sound(unsigned char x,unsigned char y)//音调函数
{
	unsigned char i;
	for(i=0;i<60;i++)
	{
		SPEAKER=0;delay8um(alltone[x][y]);	
		SPEAKER=1;delay8um(alltone[x][y]);
	}
} 

void play(unsigned char song[],unsigned char beat[])//播放歌曲
{
	unsigned char i;
	i=0;
	while(i<34)
	{
	 	sound(level,song[i]);
	 	beat_125(beat[i]);
		i++;
	}

}

void beat_125(unsigned char x)//125ms节拍
{
	unsigned char i,j,k;
	for(i=0;i<x;i++)
		for(j=0;j<125;j++)
			for(k=0;k<120;k++);
}

void gap(unsigned char x)
{
	unsigned char i,j,k;
	for(i=0;i<x;i++)
		for(j=0;j<20;j++)
			for(k=0;k<120;k++);
}

void record(unsigned char x)
{
	if(temp)
	{
		cmklevel[t]=level;
		crec[t]=x;
		t++;
		temp=0;
	}
	if(t>4) t=0;
}

void playrec(unsigned char mklevel[],unsigned char rec[])
{
	unsigned char i;
	i=0;
	while(i<5)
	{
		Displaylevel(mklevel[i],2+(3*i));
		Displaynote(rec[i],2+(3*i));
	 	sound(mklevel[i],rec[i]);
		gap();
		i++;
	}
}

unsigned char Displaylevel(unsigned char x,unsigned char y)
{
	switch(x)
	{
		case 0:
				LCMDisplayString(0,y,"L");
		break;

		case 1:
				LCMDisplayString(0,y,"M");
		break;

		case 2:
			LCMDisplayString(0,y,"H");
		break;
	}
}

unsigned char Displaynote(unsigned char x,unsigned char y)
{
	switch(x)
	{
		case 0:
				LCMDisplayString(1,y,"Do");
		break;

		case 1:
				LCMDisplayString(1,y,"D#");
		break;

		case 2:
			LCMDisplayString(1,y,"Re");
		break;

		case 3:
			LCMDisplayString(1,y,"R#");
		break;

		case 4:
			LCMDisplayString(1,y,"Mi");
		break;

		case 5:
			LCMDisplayString(1,y,"Fa");
		break;

		case 6:
			LCMDisplayString(1,y,"So");
		break;
		
		case 7:
			LCMDisplayString(1,y,"La");
		break;
		
		case 8:
			LCMDisplayString(1,y,"Si");
		break;
	}
}

/**************************************************液晶显示屏模块*****************************************************/
void LCMDelay(int ms)
{
	unsigned int i,cnt;
	cnt = DelayConst * ms;
	for(i=0;i<cnt;i++);
}

unsigned char LCMReadState(void)
{
	unsigned char state;
	LCM_E = 0;
	LCM_RS = 0;
	LCM_RW = 1;
	LCM_E = 1;
	_nop_();
	_nop_();
	state = LCM_DB;
	LCM_E = 0;
	return state;
}

void LCMClear(void)
{
	LCMDelay(1);
	LCM_E = 0;
	LCM_RS = 0;
	LCM_RW = 0;
	LCM_DB = 0x01;
	LCM_E = 1;
	_nop_();
	_nop_();
	LCM_E = 0;
	LCMDelay(1);
}

void LCMWriteCmd(unsigned char cmd)
{
	LCMDelay(1);
	LCM_E = 0;
	LCM_RS = 0;
	LCM_RW = 0;
	LCM_DB = cmd;
	LCM_E = 1;
	_nop_();
	_nop_();
	LCM_E = 0;
}

void LCMWriteData(unsigned char dc)
{
	while(LCMReadState() & BUSYFLAG);
	LCM_RS = 1;
	LCM_RW = 0;
	LCM_DB = dc;
	LCM_E = 1;
	_nop_();
	_nop_();
	LCM_E = 0;
}

void LCMInit(void)
{
	LCMDelay(60); 
	LCMWriteCmd(0x38); 
	LCMDelay(5); 
	LCMWriteCmd(0x38); 
	LCMDelay(1); 
	LCMWriteCmd(0x38); 
	while(LCMReadState() & BUSYFLAG);
	LCMWriteCmd(0x08); 
	while(LCMReadState() & BUSYFLAG);
	LCMWriteCmd(0x01); 
	while(LCMReadState() & BUSYFLAG);
	LCMWriteCmd(0x06); 
	while(LCMReadState() & BUSYFLAG);
	LCMWriteCmd(0x0e); 
	while(LCMReadState() & BUSYFLAG);
}

void LCMGotoXY(unsigned char x,unsigned char y) 
{
	unsigned char cmd;
	if(x==0)
	{
		cmd = 0x80 | y;
	}
	else
	{
		cmd = 0x80 | 0x40 | y;
	}
	LCMWriteCmd(cmd);
	while(LCMReadState() & BUSYFLAG);
}

void LCMDisplayChar(unsigned char x,unsigned char y,unsigned char ch)
{
	LCMGotoXY(x,y);
	LCMWriteData(ch);
}

void LCMDisplayString(unsigned char x,unsigned char y,unsigned char* str)
{
	unsigned char ptr;
	ptr = 0;
	while(*(str+ptr) != 0)
	{
		LCMDisplayChar(x,(y+ptr),*(str+ptr));
		ptr++;
	}
}


/**************************************************行列式键盘模块*****************************************************/

//延时函数
void delay(unsigned int i)
{
	unsigned int j;
	for(j=0;j<i;j++);
}


//检测有无按键按下
unsigned char checkkey()
{
	unsigned char i;
	KEYBOARD=0xf0;
	i=KEYBOARD;
	i=i&0xf0;
	if(i==0xf0)return(0xff);
	else return(0);
}

//键盘扫描
unsigned char keyscan()
{
	unsigned char scancode;
	unsigned char codevalue;
	unsigned char m;
	unsigned char k;
	unsigned char i,j,c;
	unsigned char fuck[]={0xfe,0xfc,0xf8,0xf0};
	if(checkkey()==0xff)return (0xff);
	else
	{
		delay(20);
		if(checkkey()==0xff)return(0xff);
		else
		{
			scancode=0xfe;
			for(i=0;i<4;i++)
			{
				scancode=fuck[i];
				k=0x10;
				KEYBOARD=scancode;
				m=0;
				for(j=0;j<4;j++)
				{
					c=KEYBOARD&0xf0;
					if((c&k)==0)
					{
						codevalue=m+i;
						while(checkkey()!=0xff);
					}
					else
					{
						k=k<<1;
						m=m+4;
					}
				}
			}
		}	
		return(codevalue);	
	}
}
