/*-------------------------------------------------------------------------
 * Developed for Enersys Energy Solutions Private Limited, Hyderabad, Indiaresearch
 * research on Auto Feeder. 
 * If you use the code cite the paper
 * Appana, D. K., Alam, M. W., & Basnet, B. (2016) "A Novel Design of Feeder System 
 * for Aqua Culture Suitable for Shrimp Farming," International Journal of Hybrid 
 * Information Technology, 9(4), 199-212.
 * http://www.sersc.org/journals/IJHIT/vol9_no4_2016/18.pdf
 * (c) Copyright 2016 Enersys Energy Solutions, All rights reserved
 *-------------------------------------------------------------------------*/
#include"KP_InputFunctions.h"
#include"flags.h"
#include"INIT.h"
#include"lcd.h"
#include"main.h"
#include"StringManipulated.h"
#include"timer.h"
#include"uart.h"
#include"XLCD2.h"

#define     ShowScreen(x,y,z) Xaxis = x, Yaxis = y, Zaxis = z
#define	AUTO		1
#define	MANUAL		0
static void KeyPadScan( void );

extern unsigned char Compile_Date[25];
static const char Blank_[] =          "                    ";  /* Black Spcace   */
static const char R_Phase_OFF_[] =    "     R PHASE OFF    ";  
static const char Y_Phase_OFF_[] =    "     Y PHASE OFF    ";
static const char B_Phase_OFF_[] =    "     B PHASE OFF    ";
static const char RY_Phase_OFF_[] =   "    R Y PHASE OFF   ";  
static const char YB_Phase_OFF_[] =   "    Y B PHASE OFF   ";
static const char BR_Phase_OFF_[] =   "    R B PHASE OFF   ";
static const char RYB_Phase_OFF_[] =  "   R Y B PHASE OFF  ";
static const char RYB_PH2PH_[] =	  "    R    Y    B     ";
static const char PHASE_ERROR_[] =	  "    PHASE ERROR     ";
static const char HV_CUT_[] =	  	  "       HV CUT       ";
static const char LV_CUT_[] =	  	  "       LV CUT       ";

const unsigned char  keypad[]   = {0x41,0x42,0x44,0x48,0x81,0x82,0x84,0x88,0x11,0x12,0x14,0x18,0x21,0x22,0x24,0x28};

const unsigned char character[] = {'2' ,'5' ,'8' ,'0' ,'3' ,'6' ,'9' ,'C' ,'M' ,'U' ,'D' ,'E' ,'1' ,'4' ,'7' ,'S' };


unsigned char Yaxis = 0, Xaxis= 0, Zaxis = 0, KP_Data=0;
unsigned int TimeOut = 0;
char Line1Buff[20];
static unsigned char ShiftScreen = 0;
//extern Time TimeOnRTC;
extern FlowControl Bits;
extern LCDflags Lcd;
extern Time SystemTime;
extern Time StartTime;
extern unsigned char Lcd_Z;
extern Time ONTime, OFFTime, MANUAL_ONTime, PREV_MAN_ONTime, PREV_MAN_OFFTime;
extern Time RunHours;
extern unsigned char Operation_mode;			// system in auto or manual
extern unsigned char Manual_On_set;
extern unsigned char firsttime_Manual_off;
extern Time Auto_ONTime;
extern Time Auto_OFFTime;
extern Time Auto_Next_ONTime;
extern Time Prev_OFF_Time;
extern Time Power_Fail;
extern Time Power_Resume;
extern unsigned int Total_Auto_Shifts_Count;
extern unsigned int Present_Auto_Shift_Count;
extern unsigned char Present_Auto_Plus_shift_count;
extern float R_Phase_Volt,Y_Phase_Volt,B_Phase_Volt;
float RY_PH2PH_Volt = 0;
float YB_PH2PH_Volt = 0;
float BR_PH2PH_Volt = 0;
float RYB_PH2PH_Volt = 0;
KEYs KP_Port;

unsigned char Master_Mode = 0;
unsigned char Master_Mode_Count = 0;
extern unsigned char Error_Mode;

void LCDTask(void)
{
	MemoryClear((unsigned char*)Line1Buff);
	KeyPadScan();
	switch(Yaxis)
	{
		case 0:
			MainScreen();
			break;
		case 1:
			if(Master_Mode == 0)
			{
				if( Zaxis != 0)
					CustomScreens();
				else
					MenuFunction();
			}
			else
			{
				if( Zaxis != 0)
					Master_CustomScreens();
				else
					MenuFunction();
			}	
			break;
		case 2:
			PORTDbits.RD7 = 1;
			if(Master_Mode == 0)
				PasswordEnter(0);
			else if(Master_Mode == 1)
				PasswordEnter(1);
			break;
		case 3:
			ChangePassword();
			break;
		default:
			ErrorScreen();
	}
}

void MainScreen(void)
{
	if(Operation_mode == AUTO)
		Auto_mode_dflt_Display();
	else if(Operation_mode == MANUAL)
		Manual_mode_dflt_Display();
	else if(Operation_mode == AUTO_PLUS)
		Auto_Plus_mode_dflt_Display();
}

void Manual_mode_dflt_Display(void)
{
	MemoryClear((unsigned char*)Line1Buff);
	XLCDL1home();
	sprintf(Line1Buff, " MANUAL    %02d:%02d:%02d ", SystemTime.Hours, SystemTime.Minutes, SystemTime.Seconds);
	XLCDPutRamString(Line1Buff);
	if( ShiftScreen <= 15)
	{	
		XLCDL2home();
		if(Manual_On_set == TRUE)
			sprintf(Line1Buff, " ON TIME   %02d:%02d:%02d ",MANUAL_ONTime.Hours,MANUAL_ONTime.Minutes,MANUAL_ONTime.Seconds);
		else
			sprintf(Line1Buff, " ON TIME   --:--:-- ");
		XLCDPutRamString(Line1Buff);
		XLCDL3home();
		sprintf(Line1Buff, " PW FAIL   %02d:%02d:%02d ",Power_Fail.Hours,Power_Fail.Minutes,Power_Fail.Seconds);
		XLCDPutRamString(Line1Buff);
		XLCDL4home();
		if(firsttime_Manual_off == 0)		/// for first time
			XLCDPutRamString(" PREV ON   --:--:--");
		else
		{
			sprintf(Line1Buff, " PREV ON   %02d:%02d:%02d ",PREV_MAN_ONTime.Hours,PREV_MAN_ONTime.Minutes,PREV_MAN_ONTime.Seconds);
			XLCDPutRamString(Line1Buff);
		}	
	} 
	else if( (ShiftScreen >15) && (ShiftScreen <=30) )
	{
		XLCDL2home();
		if(Manual_On_set == TRUE)
		{
			sprintf(Line1Buff, " RUN HOURS %02d:%02d:%02d ",RunHours.Hours,RunHours.Minutes,RunHours.Seconds);
		}
		else
			sprintf(Line1Buff, " RUN HOURS --:--:-- ");
		XLCDPutRamString(Line1Buff);
		XLCDL3home();
		sprintf(Line1Buff, " PW RESUME %02d:%02d:%02d ",Power_Resume.Hours,Power_Resume.Minutes,Power_Resume.Seconds);
		XLCDPutRamString(Line1Buff);
		XLCDL4home();
		if(firsttime_Manual_off == 0)		/// for first time
			XLCDPutRamString(" PREV OFF  --:--:--");
		else
		{
			sprintf(Line1Buff, " PREV OFF  %02d:%02d:%02d ",PREV_MAN_OFFTime.Hours,PREV_MAN_OFFTime.Minutes,PREV_MAN_OFFTime.Seconds);
			XLCDPutRamString(Line1Buff);
		}	
	}
	else if( (ShiftScreen >30) && (ShiftScreen <=45) )
	{
		RYB_Phase2Phase_Display();
	}	
	ShiftScreen++;
	if( ShiftScreen > 45)
		ShiftScreen = 0;
	XLCD_CURSOR_OFF();
}

void Auto_mode_dflt_Display(void)
{
	XLCDL1home();
	MemoryClear((unsigned char*)Line1Buff);
	sprintf(Line1Buff, " AUTO (%d/%d)%02d:%02d:%02d ",(Present_Auto_Shift_Count+1),Total_Auto_Shifts_Count,SystemTime.Hours, SystemTime.Minutes, SystemTime.Seconds);
	XLCDPutRamString(Line1Buff);
	if( ShiftScreen <= 15)
	{	
		XLCDL2home();
		sprintf(Line1Buff, " ON TIME   %02d:%02d:%02d ",Auto_ONTime.Hours,Auto_ONTime.Minutes,Auto_ONTime.Seconds);
		XLCDPutRamString(Line1Buff);
		XLCDL3home();
		sprintf(Line1Buff, " PW FAIL   %02d:%02d:%02d ",Power_Fail.Hours,Power_Fail.Minutes,Power_Fail.Seconds);
		XLCDPutRamString(Line1Buff);
		XLCDL4home();
		sprintf(Line1Buff, " NEXT ON   %02d:%02d:%02d ",Auto_Next_ONTime.Hours,Auto_Next_ONTime.Minutes,Auto_Next_ONTime.Seconds);
		XLCDPutRamString(Line1Buff);
	} 
	else if( (ShiftScreen >15) && (ShiftScreen <=30) )
	{
		XLCDL2home();
		sprintf(Line1Buff, " OFF TIME  %02d:%02d:%02d ",Auto_OFFTime.Hours,Auto_OFFTime.Minutes,Auto_OFFTime.Seconds);
		XLCDPutRamString(Line1Buff);
		XLCDL3home();
		sprintf(Line1Buff, " PW RESUME %02d:%02d:%02d ",Power_Resume.Hours,Power_Resume.Minutes,Power_Resume.Seconds);
		XLCDPutRamString(Line1Buff);
		XLCDL4home();
		sprintf(Line1Buff, " PREV OFF  %02d:%02d:%02d ",Prev_OFF_Time.Hours,Prev_OFF_Time.Minutes,Prev_OFF_Time.Seconds);
		XLCDPutRamString(Line1Buff);
	}
	else if( (ShiftScreen >30) && (ShiftScreen <=45) )
	{
		RYB_Phase2Phase_Display();
	}	
	ShiftScreen++;
	if( ShiftScreen > 45)
		ShiftScreen = 0;
	XLCD_CURSOR_OFF();
}

void Auto_Plus_mode_dflt_Display(void)
{
	XLCDL1home();
	MemoryClear((unsigned char*)Line1Buff);
	sprintf(Line1Buff, " AUTO+(%d/%d)%02d:%02d:%02d ",(Present_Auto_Plus_shift_count+1),No_of_Sifts,SystemTime.Hours, SystemTime.Minutes, SystemTime.Seconds);
	XLCDPutRamString(Line1Buff);	
	if( ShiftScreen <= 15)
	{	
		XLCDL2home();
		sprintf(Line1Buff, " ON TIME   %02d:%02d:%02d ",Auto_ONTime.Hours,Auto_ONTime.Minutes,Auto_ONTime.Seconds);
		XLCDPutRamString(Line1Buff);
		XLCDL3home();
		sprintf(Line1Buff, " PW FAIL   %02d:%02d:%02d ",Power_Fail.Hours,Power_Fail.Minutes,Power_Fail.Seconds);
		XLCDPutRamString(Line1Buff);
		XLCDL4home();
		sprintf(Line1Buff, " NEXT ON   %02d:%02d:%02d ",Auto_Next_ONTime.Hours,Auto_Next_ONTime.Minutes,Auto_Next_ONTime.Seconds);
		XLCDPutRamString(Line1Buff);
	} 
	else if( (ShiftScreen >15) && (ShiftScreen <=30) )
	{
		XLCDL2home();
		sprintf(Line1Buff, " OFF TIME  %02d:%02d:%02d ",Auto_OFFTime.Hours,Auto_OFFTime.Minutes,Auto_OFFTime.Seconds);
		XLCDPutRamString(Line1Buff);
		XLCDL3home();
		sprintf(Line1Buff, " PW RESUME %02d:%02d:%02d ",Power_Resume.Hours,Power_Resume.Minutes,Power_Resume.Seconds);
		XLCDPutRamString(Line1Buff);
		XLCDL4home();
		sprintf(Line1Buff, " PREV OFF  %02d:%02d:%02d ",Prev_OFF_Time.Hours,Prev_OFF_Time.Minutes,Prev_OFF_Time.Seconds);
		XLCDPutRamString(Line1Buff);
	}
	else if( (ShiftScreen >30) && (ShiftScreen <=45) )
	{
		RYB_Phase2Phase_Display();
	}	
	ShiftScreen++;
	if( ShiftScreen > 45)
		ShiftScreen = 0;
	XLCD_CURSOR_OFF();
}

void RYB_Phase2Phase_Display(void)
{
	XLCDL2home();
	MemoryClear((unsigned char*)Line1Buff);
	RY_PH2PH_Volt = ((R_Phase_Volt + Y_Phase_Volt)/2.0)*1.732;
	YB_PH2PH_Volt = ((Y_Phase_Volt + B_Phase_Volt)/2.0)*1.732;
	BR_PH2PH_Volt = ((B_Phase_Volt + R_Phase_Volt)/2.0)*1.732;
	
	sprintf(Line1Buff, "  %4.1f %4.1f %4.1f ",(double)R_Phase_Volt,(double)Y_Phase_Volt,(double)B_Phase_Volt);
	XLCDPutRamString(Line1Buff); 
	XLCDL3home();
	if( (R_PHASE == OFF) || (Y_PHASE == OFF) || (B_PHASE == OFF) )
	{
		if( (R_PHASE == OFF) && (Y_PHASE == ON) && (B_PHASE == ON) )
			XLCDPutRamString(R_Phase_OFF_);
		else if( (R_PHASE == ON) && (Y_PHASE == OFF) && (B_PHASE == ON) )
			XLCDPutRamString(Y_Phase_OFF_);
		else if( (R_PHASE == ON) && (Y_PHASE == ON) && (B_PHASE == OFF) )
			XLCDPutRamString(B_Phase_OFF_);
		else if( (R_PHASE == OFF) && (Y_PHASE == OFF) && (B_PHASE == ON) )
			XLCDPutRamString(RY_Phase_OFF_);
		else if( (R_PHASE == ON) && (Y_PHASE == OFF) && (B_PHASE == OFF) )
			XLCDPutRamString(YB_Phase_OFF_);
		else if( (R_PHASE == OFF) && (Y_PHASE == ON) && (B_PHASE == OFF) )
			XLCDPutRamString(BR_Phase_OFF_);
		else if( (R_PHASE == OFF) && (Y_PHASE == OFF) && (B_PHASE == OFF) )
			XLCDPutRamString(RYB_Phase_OFF_);
	}
	else if(Error_Mode == 2)
	{
		MemoryClear((unsigned char*)Line1Buff);
		sprintf(Line1Buff, "  LV CUT: %3d Volts ",LV_Cut_Value);
		XLCDPutRamString(Line1Buff); 
	}	
	else if(Error_Mode == 3)
	{
		MemoryClear((unsigned char*)Line1Buff);
		sprintf(Line1Buff, "  HV CUT: %3d Volts ",HV_Cut_Value);
		XLCDPutRamString(Line1Buff); 
	}
	else
	{
		XLCDPutRamString(Blank_); 
	}
	XLCDL4home();
	MemoryClear((unsigned char*)Line1Buff);
	if( (R_PHASE == OFF) || (Y_PHASE == OFF) || (B_PHASE == OFF) )				//	Ravi
		XLCDPutRamString(Blank_); 
	else
	{	
		sprintf(Line1Buff, "  3 PH: %4.1f Volts ",(double)RYB_PH2PH_Volt);
		XLCDPutRamString(Line1Buff); 
	}	
}

void INIT_LED(void)
{
	DDR_LED_MRUN	= OUTPUT;			//	LED ON Board	
 	DDR_LED_RUN		= OUTPUT;			//	Toggle MCLR RUN Status LED on LED Board	
 	DDR_LED_AUTO	= OUTPUT;			//	Toggle Auto Status LED on LED Board
 	DDR_LED_MANL	= OUTPUT;			//	Toggle Manual Status LED on LED Board
 	DDR_LED_AUTOPLUS= OUTPUT;			//	Toggle Error Status LED on LED Board	
}

void INIT_KEYPAD(void)
{
	ADCON1bits.ADON		=	0;			//	Disable ADC		
	ADPCFG				=	0xFFFF;		//	CONFIGURE PORTB as DIGITAL IO
	DCICON1bits.DCIEN	=	0;			//	DCI Diabled
	DDR_KEY_MANL		=	INPUT;					//  manual key

	DDR_KP_R1 = OUTPUT;
	DDR_KP_R2 = OUTPUT;
	DDR_KP_R3 = OUTPUT;
	DDR_KP_R4 = OUTPUT;
	DDR_KP_C1 = INPUT;
	DDR_KP_C2 = INPUT;
	DDR_KP_C3 =	INPUT;
	DDR_KP_C4 = INPUT;
}

static void KeyPadScan( void )
{
    unsigned char ii =0, i =0, cKeyPad =0,KP_V = 0;
	Write_KeyPad(0xF0);                //init/re-init port 
	KP_V = Read_KeyPad();
	if(KP_V!= 0xF0)
	{
		for(i=0;i<4;i++)
		{
			Write_KeyPad(0x80>>i);
			for(ii=0;ii<16;ii++)
			{
				KP_V = Read_KeyPad();
				if(KP_V == keypad[ii])
				{ 			                           
					KP_Data = cKeyPad = character[ii];
					printf("keypad:%c\n",KP_Data);
					i=4,TimeOut =0;
					break;
				}
			}
		}
	}
	#ifdef KEY_LOCK
		if( cKeyPad == 'M' && Bits.PassWord == 0 && Master_Mode == 0)
		{
			Yaxis = 2;
			cKeyPad = 0;
		}
	#endif
	if( (KP_Data == 'E') && (PORTFbits.RF6) )
	{
		putsUART1((unsigned int*)"\n\rMASTER COUNT");
		Master_Mode_Count++;
		if(Master_Mode_Count > 15)
			Master_Mode = 1, Yaxis = 2;
	}	
	else if(cKeyPad == 'C' && Zaxis == 0 )
	{
		if (Yaxis == 1)
		{
			Yaxis--,Xaxis = 0;
			if (Yaxis == 0)
				Master_Mode = 0,Bits.PassWord = 0;
		}
	}
	else if( (Yaxis == 1) && (cKeyPad == 'U') && (Master_Mode == 0) )
	{
		if( (Xaxis >= 1)  )
			Xaxis--;
		if(Operation_mode == MANUAL && Xaxis == 2)
			Xaxis = 0;
		if(Operation_mode == AUTO && Xaxis == 1)
			Xaxis = 0;
		if(Operation_mode == AUTO_PLUS && Xaxis == 2)
			Xaxis = 1;
	}
	else if( (Yaxis == 1) && (cKeyPad == 'D') && (Master_Mode == 0) )
	{
		if( Xaxis < 4 )
			Xaxis++;
		if(Operation_mode == MANUAL && Xaxis == 1)
			Xaxis = 3;
		if(Operation_mode == AUTO && Xaxis == 1)
			Xaxis = 2;
		if(Operation_mode == AUTO_PLUS && Xaxis == 2)
			Xaxis = 3;
	}
	else if( (Yaxis == 1) && (cKeyPad == 'U') && (Master_Mode == 1) )
	{
		if( (Xaxis >= 1)  )
			Xaxis--;
	}
	else if( (Yaxis == 1) && (cKeyPad == 'D') && (Master_Mode == 1) )
	{
		if( Xaxis < 5 )
			Xaxis++;
	}
	else
	{
		Master_Mode_Count = 0;
	}	
	if( Yaxis != 0)
		TimeOut++;
	if(LCD_BL_P == ON)
		TimeOut++;
	if( TimeOut >= 540 )
	{
		Xaxis = 0, Yaxis = 0, Zaxis = 0,TimeOut =0, 
		Lcd.Z0 =0,Lcd.Z1 =0,Lcd.Z2 =0,Lcd.Z3 =0,Lcd.Z4 =0,Lcd.Z5 =0,Lcd.Z6 =0,Lcd.Z7 =0,Bits.PassWord =0,Lcd.Z8 = 0,Lcd.Z9 = 0,
		PORTDbits.RD7 = 1,Lcd.Z10 = 0,Lcd_Z = 0,Lcd.Z11 = 0,Lcd.Z12 = 0,Lcd.Z13 = 0,Lcd.Z14 = 0,Lcd.Z15 = 0,Lcd.Z16 = 0,Lcd.Z17 = 0,Lcd.Z18 = 0;
		process_lcd = 0, CursorPos = 0, shifts_count = 0, Master_Mode = 0;
	}
	cKeyPad = 0; 
}


void LCD_StartUP_Screen(void)
{
	TimerDelay_10ms(200);
	XLCDL1home();
	sprintf(Line1Buff, " ENERSYS ENERGY     ");
	XLCDPutRamString(Line1Buff);
	XLCDL2home();
	sprintf(Line1Buff, "          SOLUTIONS ");
	XLCDPutRamString(Line1Buff);
	XLCDL3home();
	sprintf(Line1Buff, " AUTO AERATOR V 2.0");
	XLCDPutRamString(Line1Buff);
	XLCDL4home();
	sprintf(Line1Buff, " DATE: %s ",Compile_Date);
	XLCDPutRamString(Line1Buff);
	XLCD_CURSOR_OFF();
	TimerDelay_10ms(100);
}
void Write_KeyPad(unsigned char chr)
{
	KP_Port._KEYPORT = chr;
	KP_C1	= KP_Port.C0; 
	KP_C2	= KP_Port.C1;  
	KP_C3	= KP_Port.C2;
	KP_C4	= KP_Port.C3;
	KP_R1	= KP_Port.R0;
	KP_R2	= KP_Port.R1;
	KP_R3	= KP_Port.R2;
	KP_R4	= KP_Port.R3;
}

unsigned char Read_KeyPad(void)
{
	KP_Port.C0 = R_KP_C0;
	KP_Port.C1 = R_KP_C1;
	KP_Port.C2 = R_KP_C2;
	KP_Port.C3 = R_KP_C3;
	KP_Port.R0 = R_KP_R0;
	KP_Port.R1 = R_KP_R1;
	KP_Port.R2 = R_KP_R2;
	KP_Port.R3 = R_KP_R3;
	return KP_Port._KEYPORT;
}
