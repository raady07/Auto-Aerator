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
 
#include "INIT.h"
#include "KP_InputFunctions.h"
#include "XLCD2.h"
#include "UART.h"
#include "RTC.h"
#include "flags.h"
#include "StringManipulated.h"
#include "timer.h"
#include "Ex_EEPROM.h"
#include "24lc256.h"
#include "main.h"
#include "uart.h"
#include <reset.h>
#include"adcMUX.h"

#define     ShowScreen(x,y,z) Xaxis = x, Yaxis = y, Zaxis = z

void ChangePassword( void );
extern void PCPutInt1(unsigned long int i);
extern void Switch_OFF_FEEDER(void);

unsigned char EnteredPW[5], *pEnteredPW = 0;
unsigned char CursorPos =0, KPHBuff[4], KPMBuff[4], *pHour =0, *pMin =0;
unsigned char KPHBuff_off[4];
unsigned char KPMBuff_off[4];
extern unsigned char Yaxis, Xaxis, Zaxis, KP_Data, CIndex;
unsigned char MasterPW[5] = "8722";
extern FlowControl Bits;
extern Time SystemTime;
extern Time Each_Auto_Shift_Time;
LCDflags Lcd = { 0b00000000000 };
unsigned char Lcd_Z = 0;
extern char Line1Buff[16];
extern Time ONTime[], OFFTime[], FirstONTime, No_Of_ON_Hours, No_Of_OFF_Hours;
unsigned char position = AUTO;
//unsigned char Feed_Loaded	=	FALSE;

                                   /*12345678901234567890*/
static const char SPAC[] =          "                    ";  /* Black Spcace   */
static const char MPWD[] =          "  MASTER PASSWORD   ";
static const char PA_[] =           "   ENTER PASSWORD   ";
static const char MENU[] =          "        MENU        ";  /* Main Menu Name */
static const char PE_[] =           "        ____        ";

static const char HardReset[] =     "< DEFAULT SETTINGS  ";
static const char Time_[] =         "   HH:MM - __:__    ";
static const char Date_[] =         "DD:MM:YY - __:__:__ ";
static const char Ero[] =           "        Error       ";
static const char RE_[] =           "  ->Yes '0'    Cnl  ";


static const char SETTIME[]=        "      SET TIME     >";  /* Set Time Screen name */
static const char SETDATE[]=        "<     SET DATE     >";  /* Set Date Screen name */
static const char AUTO_P_SETTNGS_[] = "<  AUTO+ SETTINGS  >";
static const char NO_OF_SHIFTS_[] = "    No. OF SHIFTS   ";
static const char ON_TIME_[]=		"   ON TIME - __:__  "; //"   ON TIME - HH:MM  ";
static const char OFF_TIME_[]=		"  OFF TIME - __:__  "; //"  OFF TIME - HH:MM  ";
static const char CHNG_PSWD[] =		"<  CHANGE PASSWORD  ";
static const char CHNG_PSWD_1[] =	"   CHANGE PASSWORD  ";
static const char SAVE_CANCEL[] =   "   SAVE    CANCEL   ";
static const char AUTO_[] =			"<       AUTO       >";
static const char AUTO_PLUS_[] =	"<      AUTO+        ";
static const char MANUAL_[] =		"       MANUAL      >";
static const char ENTER_CANCEL[] =	"  ENTER     CANCEL  ";
static const char OPP_MODE_[] =		"   OPERATION MODE  >";
static const char OPP_MODE_1[] =	"   OPERATION MODE   ";
static const char AUTO_MODE_[] =	"<   AUTO SETTINGS  >";
static const char PWR_FAIL_ADJ_[] =	"< ADJ PWR FAIL TIME>";
static const char ENABLE_[] =		"       ENABLE      >";
static const char DISABLE_[] =		"<      DISABLE      ";
static const char PWR_LOG_[] =		"<  POWER FAIL LOG  >";
static const char HLV_CUT_MENU[] =	"<      HLV CUT     >";
static const char HLV_CUT_[] =		"       HLV CUT      ";
static const char HV_CUT_[] =		"   HV CUT : ___     ";
static const char LV_CUT_[] =		"   LV CUT : ___     ";
static const char NOOFSHI_[] =		"         _          ";
static const char CALIB_[]	=		"<    CALIBRATION   >";
static const char R_PHASE_VOL_[] =	"  R PHASE: ___._ V  ";
static const char Y_PHASE_VOL_[] =	"  Y PHASE: ___._ V  ";
static const char B_PHASE_VOL_[] =	"  B PHASE: ___._ V  ";
static const char LOAD2LOAD[] = 	"<LOAD TO LOAD DELAY>"; // Masterscreen3 - Set Load to Load Delay
static const char RSTMENUPWD[] = 	"<  RESET PASSWORD  >";	// Masterscreen5 - Reset Menu Password.
static const char RSTMENUPWD_[] = 	"  RESTORE PASSWORD  ";	
unsigned int Load_Delay = 10;
unsigned char process_lcd = 0;
unsigned char process_lcd_buff[20];
static unsigned char *ptr;
unsigned char value_entered = 0;
unsigned char temp_noofshifts = 0;
unsigned char process_time_buff[8][9];
unsigned char shifts_count = 0;
unsigned char No_of_Sifts = DFLT_NO_OF_SHIFTS;
unsigned char HLV_mode = DFLT_HLV_MODE;
unsigned int temp_HV = 0, temp_LV = 0;
float User_Ent_R_phase = 0;
float User_Ent_Y_phase = 0;
float User_Ent_B_phase = 0;
float Mult_Fact_R = 0.3;
float Temp_Mult_Fact_R = 0;
float Temp_Mult_Fact_Y = 0;

extern unsigned long int Calib_RMS_R_ph, Calib_RMS_Y_ph, Calib_RMS_B_ph;
float Mult_Fact_Y = 0.3;
float Mult_Fact_B = 0.3;

extern unsigned char Master_Mode;

unsigned int R_int = 0;
unsigned char R_float = 0;
unsigned int Y_int = 0;
unsigned char Y_float = 0;
unsigned int B_int = 0;
unsigned char B_float = 0;

void  PasswordEnter( unsigned char PswdType )
{
	if( Lcd.Z0 == 0)
	{
		XLCDL1home();
		XLCDPutRamString(SPAC);
		XLCDL2home();
		if(PswdType == 0)				XLCDPutRamString(PA_);
		else if(PswdType == 1)			XLCDPutRamString(MPWD);
		XLCDL4home();
		XLCDPutRamString(SPAC);
		XLCDL3home();
		XLCDPutRamString(PE_);                  
		Lcd.Z0 = 1,KP_Data = 0,CursorPos = 0;
		MoveLeft(12);
		XLCD_BLINKOON();
		pEnteredPW = EnteredPW;
	}
	if(Lcd.Z0 == 1)
	{
		if(KP_Data > 47 && KP_Data < 58 && CursorPos < 4)
		{
			CursorPos++;
			XLCDPut('*');
			*pEnteredPW++ = KP_Data;
			if(CursorPos == 4)		XLCD_CURSOR_OFF();
			KP_Data =0;
		}
		else if ( KP_Data == 'E' && CursorPos == 4)
		{
			if( !PswdType )												// check for main password
			{
				if( MemoryCMP(ExstingPW,EnteredPW) )
				{
					Lcd.Z0 = 0, KP_Data = 0, pEnteredPW = 0;
					Bits.PassWord = 1, ShowScreen(0,1,0);
				}
				else
				{
					ShowScreen(0,0,0),Bits.PassWord = 0;                 /* Go to the Main Menu if Wrong Password is pressed */
					Lcd.Z0 = 0, KP_Data = 0;
				}
			}
			else if( PswdType )											// check for Hard reset password
			{
				if( MemoryCMP(MasterPW,EnteredPW) )
				{
					Lcd.Z0 = 0,KP_Data =0,pEnteredPW = 0;
					process_lcd = 1;
					if(Xaxis == 0)		ShowScreen(0,1,0);
					else
						Lcd.Z12 = 1;
				}
				else
				{
					ShowScreen(0,2,0);                 /* Go to the Main Menu if Wrong Password is pressed */
					Lcd.Z0 = 0, KP_Data =0;
				}
			}
			MemoryClear(EnteredPW);
		}
		else if (KP_Data == 'C')
		{
			Bits.PassWord = 0, pEnteredPW = 0;                 /* Go to the Main Menu if Wrong Password is pressed */
			Lcd.Z0 = 0,KP_Data =0;
			Master_Mode = 0;
			MemoryClear(EnteredPW);
			ShowScreen(0,0,0);
		}
	}
}

void MenuFunction(void )
{
	XLCD_CURSOR_OFF();
	XLCDL1home();
	XLCDPutRamString(SPAC);
	XLCDL2home();
	XLCDPutRamString(MENU);
	XLCDL3home();
	if(Master_Mode == 0)
		MenuScreens(  );
	else if(Master_Mode == 1)
		Master_MenuScreens(  );
	if( KP_Data == 'E')
		Zaxis = Xaxis + 1;
	XLCDL4home();
	XLCDPutRamString(SPAC);
}

void MenuScreens( void )
{
	switch(Xaxis)
	{
		case 0:	XLCDPutRamString(OPP_MODE_);  
				break;
		case 1:	XLCDPutRamString(AUTO_P_SETTNGS_);
				break;
		case 2:	XLCDPutRamString(AUTO_MODE_);
				break;
		case 3:	XLCDPutRamString(HLV_CUT_MENU);
				break;
		case 4:	XLCDPutRamString(CHNG_PSWD);
				break;
		default:XLCDPutRamString(SPAC);
	}
}

void CustomScreens( void )
{
	switch(Zaxis)
  	{
		case 1:	SetOperationMode();
				break;
		case 2:	Auto_Plus_Settings();
				break;
		case 3:	AutoSettings();					// set Duration Mode: ON/OFF time base or No of Hours
				break;
		case 4:	Change_HLV_Cut();
				break;
   		case 5:	ChangePassword();
         		break;
   		default:ErrorScreen();
         		break;
   }
}

void Master_MenuScreens( void )
{
	switch(Xaxis)
	{
		case 0:	XLCDPutRamString(SETTIME);
				break;
		case 1:	XLCDPutRamString(SETDATE);
				break;
		case 2: XLCDPutRamString(LOAD2LOAD);
				break;
		case 3: XLCDPutRamString(CALIB_);  
				break;
		case 4: XLCDPutRamString(RSTMENUPWD);  
				break;
		case 5: XLCDPutRamString(HardReset);
				break;
		default:XLCDPutRamString(SPAC);
	}
}

void Master_CustomScreens( void )
{
	switch(Zaxis)
  	{
		case 1:	SetTime();							// set RTC time and system time
				break;
		case 2:	SetDate();							// set RTC date
				break;
		case 3: SetLoadDelay();						// set load delay 
				break;
		case 4:	Calibration();
				break;
		case 5:	ResetMenuPassword();				// Reset Menu Password
				break;
   		case 6:	SetHardResetFunction();
         		break;
   		default:ErrorScreen();
         		break;
   }
}

void SetTime(void)
{
	unsigned char bHour =0, bMin =0;
	if( Lcd.Z0 == 0)
	{
		memset(process_lcd_buff,0x00,20);
		XLCDL1home();
		XLCDPutRamString(SETTIME);
		XLCDL3home();
		XLCDPutRamString(SPAC);
		XLCDL4home();
		XLCDPutRamString(SAVE_CANCEL); 
		XLCDL2home();
		XLCDPutRamString(Time_); 
		MoveLeft(9);	 
		Lcd.Z0 = 1,KP_Data = 0,CursorPos = 0;
		XLCD_BLINKOON();
		value_entered = 0;
		ptr = process_lcd_buff;
	}
	if(Lcd.Z0 == 1)
	{
		if(KP_Data == 'U' && CursorPos>0)
		{
			ptr--;
			KP_Data = 0;
			CursorPos--;
			MoveLeft(1)
			if(CursorPos == 1)
				MoveLeft(1)
		}
		else if(KP_Data == 'D' && CursorPos < 3 && CursorPos<value_entered)
		{
			ptr++;
			KP_Data = 0;
			CursorPos++;
			MoveRight(1)
			if(CursorPos == 2)
				MoveRight(1)
		}	
		else if( KP_Data > 47 && KP_Data < 58 && CursorPos < 4)
		{
			CursorPos++;
			value_entered++;
			XLCDPut(KP_Data);
			*ptr++ = KP_Data;
			KP_Data = 0;
			if(CursorPos == 2)
				MoveRight(1)
			if(CursorPos == 4)
			{
				ptr--;
				MoveLeft(1)
				CursorPos--;
			}
		}
		if( KP_Data == 'C')
		{
			Lcd.Z0 = 0,KP_Data =0;
			value_entered = 0;
			CursorPos = 0;
			ShowScreen(Zaxis-1,1,0);
		}
		else if (KP_Data == 'S' && value_entered >= 4)
		{
			bHour =  ConvertSTRING_CHAR(&process_lcd_buff[0],2);
			bMin  =  ConvertSTRING_CHAR(&process_lcd_buff[2],2);
			if( bHour < 24 && bMin < 60)
			{
				SystemTime.Hours = bHour,SystemTime.Minutes = bMin,Bits.RTCSetNewTime = 1;
				W_RTC_Time(SystemTime.Hours, SystemTime.Minutes, 0);
				Char_WriteEEPROM_ex(1,  _RTCSetNewTime);        // To check whether Time is set			
				Char_WriteEEPROM_ex(bHour,  eepromsysTimeHr);
				Char_WriteEEPROM_ex(bMin,   eepromsysTimeMin);
				if(Operation_mode == AUTO)
					Check_Present_Auto_Shift_Time();
				else if(Operation_mode == AUTO_PLUS)
				{
					Decide_AutoPlus_Shift_Position();
				}	
			}
			Lcd.Z0 = 0,KP_Data =0,pMin =0, pHour =0;
			value_entered = 0;
			CursorPos = 0;
			ShowScreen(Zaxis-1,1,0);
		}
	}
}

void SetONOFFTime(unsigned char WHAT_TIME)
{
	if( Lcd.Z0 == 0)
	{
		memset(&process_time_buff[WHAT_TIME][0],0x00,9);
		XLCDL1home();
		MemoryClear((unsigned char*)Line1Buff);
		sprintf(Line1Buff, "     SHIFT: %d       ",(WHAT_TIME+1));
		XLCDPutRamString(Line1Buff); 
		XLCDL4home();
		if(shifts_count+1==temp_noofshifts)
			XLCDPutRamString(SAVE_CANCEL); 
		else
			XLCDPutRamString(ENTER_CANCEL); 
		XLCDL3home();
		XLCDPutRamString(OFF_TIME_); 
		XLCDL2home();
		XLCDPutRamString(ON_TIME_); 
		MoveLeft(7);	 
		Lcd.Z0 = 1,KP_Data = 0;
		XLCD_BLINKOON();
		ptr = &process_time_buff[WHAT_TIME][0];
		value_entered = 0;
		CursorPos = 0;
	}
	if(Lcd.Z0 == 1)
	{
		if(KP_Data == 'U' && CursorPos>0)
		{
			ptr--;
			KP_Data = 0;
			CursorPos--;
			MoveLeft(1)
			if(CursorPos == 1 || CursorPos == 5)
				MoveLeft(1)
			else if(CursorPos == 3)
				MoveRight(25)
		}
		else if(KP_Data == 'D' && CursorPos < 7 && CursorPos<value_entered)
		{
			ptr++;
			KP_Data = 0;
			CursorPos++;
			MoveRight(1)
			if(CursorPos == 2 || CursorPos == 6)
				MoveRight(1)
			else if(CursorPos == 4)
				MoveLeft(25)
		}	
		else if( KP_Data > 47 && KP_Data < 58 && CursorPos < 8)
		{
			CursorPos++;
			value_entered++;
			XLCDPut(KP_Data);
			*ptr++ = KP_Data;
			KP_Data = 0;
			if(CursorPos == 2|| CursorPos == 6)
				MoveRight(1)
			else if(CursorPos == 4)
				MoveLeft(25)
			if(CursorPos == 8)
			{
				CursorPos--;
				MoveLeft(1)
				ptr--;
			}	
		}
		else if( KP_Data == 'C')
		{
			Lcd.Z0 = 0,KP_Data =0;
			value_entered = 0;
			CursorPos = 0;
			shifts_count = 0;
			process_lcd = 0;
			ShowScreen(Zaxis-1,1,0);
		}
		else if( (KP_Data == 'E') && ((shifts_count+1)<temp_noofshifts) && (value_entered >= 8) )
		{
			KP_Data = 0;
			Lcd.Z0 = 0;
			value_entered = 0;
			CursorPos = 0;
			if(ValidateTime())
				shifts_count++;
			else
			{
				WrongValues_errorScreen(0);
			}	
		}
		else if( (shifts_count+1==temp_noofshifts) && (KP_Data == 'S') && (value_entered >= 8) )
		{
			KP_Data = 0;
			Lcd.Z0 = 0;
			value_entered = 0;
			CursorPos = 0;
			if(ValidateTime())
				shifts_count++;
			else
			{
				WrongValues_errorScreen(0);
			}	
		}	

	}
}

void SetHardResetFunction( void )
{
	if( Lcd.Z12 == 0 )
		PasswordEnter( 1 );
	else if( Lcd.Z12 == 1 )
	{
		if( Lcd.Z11 == 0 )
		{
			XLCDL2home();
			XLCDPutRamString(HardReset); 
			XLCDL3home();
			XLCDPutRamString(ENTER_CANCEL);
			Lcd.Z11 = 1,KP_Data = 0;
		}
		else if(Lcd.Z11 == 1)
		{
			if( KP_Data == 'C')
				ShowScreen(Zaxis-1,1,0),Lcd.Z11 = 0,KP_Data =0,Lcd.Z12 = 0;
			else if (KP_Data == 'E')
			{
				Char_WriteEEPROM_ex(0xFF,  EE_Compile_Len);    /* To Check whether System is starting for the first time	*/
				TimerDelay_10ms(5);
//				Reset();
			//	PORStatReset;
				__asm__ volatile ("reset") ;
			}
		}
	}
}

void ChangePassword( void )
{
	if( Lcd.Z7 == 0)
	{
		XLCDL2home();
		XLCDPutRamString(" ENTER NEW PASSWORD "); 
		XLCDL4home();
		XLCDPutRamString(SAVE_CANCEL); 
		XLCDL3home();
		XLCDPutRamString(PE_); 
		MoveLeft(12);	                                    /*  */
		XLCD_BLINKOON();	                     			    /* Blinking the cursor                                  */
		Lcd.Z7 = 1,KP_Data = 0,CursorPos =0;
		pEnteredPW = EnteredPW;
	}
	else if(Lcd.Z7 == 1)
	{
		if( KP_Data > 47 && KP_Data < 58 )       /* Checking if any numerical values are pressed         */
		{
			CursorPos++;				          /* Counting the number of characters                    */
			if(CursorPos > 0 && CursorPos < 5) /* only first 4 characters are taken as password        */
			{
				XLCDPut('*');					  /* Masking the key by '*'                               */
				*pEnteredPW++ = KP_Data;
				*pEnteredPW   ='\0';
			}
			if(CursorPos == 4)
				XLCD_CURSOR_OFF();
			KP_Data =0;
		}
		if (CursorPos >= 4 && KP_Data == 'S')
		{
			String_WriteEEPROM_ex(LCD_KEY, EnteredPW, 4);		// write default password
			StringCopy(EnteredPW,ExstingPW);
			MemoryClear(EnteredPW);
			Lcd.Z7 = 0,KP_Data =0,pEnteredPW =0,ShowScreen(Zaxis-1,1,0);
		}
		else if (KP_Data == 'C')
		{
			MemoryClear(EnteredPW); 
			Lcd.Z7 = 0,KP_Data =0,pEnteredPW =0,ShowScreen(Zaxis-1,1,0);
		}
	}
}

void SetLoadDelay(void)
{
	if(Lcd.Z12 == 0)
	{
		MemoryClear((unsigned char *)Line1Buff);
		XLCDL1home();
		XLCDPutRamString(SPAC);
		XLCDL4home();
		XLCDPutRamString(SAVE_CANCEL);
		XLCDL3home();
		XLCDPutRamString(SPAC); 	
		XLCDL2home();
		sprintf(Line1Buff,"   DELAY: %03d SEC   ",Load_Delay);
		XLCDPutRamString(Line1Buff); 
		sprintf(Line1Buff,"%03d",Load_Delay);
		strcpy((char *)KPHBuff,(const char*)Line1Buff);
		MoveLeft(10);
		Lcd.Z12 = 1, KP_Data = 0,CursorPos = 0;
		XLCD_BLINKOON();
		ptr = KPHBuff;
		MemoryClear((unsigned char *)Line1Buff);
	}
	else if(Lcd.Z12 == 1)
	{
		if( KP_Data > 47 && KP_Data < 58 && CursorPos < 3)
		{
			CursorPos++;
			XLCDPut(KP_Data);
			*ptr++ = KP_Data;
			if(CursorPos == 3)
				XLCD_CURSOR_OFF();
			KP_Data =0;
		}
		else if(KP_Data == 'S')
		{
			if(CursorPos == 3)
			{
				Load_Delay = ConvertSTRING_Int((unsigned char*)&KPHBuff[0],3);
				ptr = (unsigned char*)&Load_Delay;
				String_WriteEEPROM_ex(2,ptr, EE_LOAD_DELAY);		// write Load delay values
				Lcd.Z12 = 0,CursorPos = 0;
				ShowScreen(Zaxis-1,1,0);
			}	
			KP_Data = 0;
			
		}
		else if(KP_Data == 'U' && CursorPos > 0)
		{
			CursorPos--;
			ptr--;
			KP_Data = 0;
			MoveLeft(1);
			if(CursorPos != 3)
				XLCD_BLINKOON();
		}
		else if(KP_Data == 'D' && CursorPos <3)
		{
			CursorPos++;
			ptr++;
			KP_Data = 0;
			MoveRight(1);
			if(CursorPos == 3)
				XLCD_CURSOR_OFF();
		}
		else if(KP_Data == 'C')
		{
			KP_Data = 0, Lcd.Z12 = 0, CursorPos = 0;
			XLCD_CURSOR_OFF();
			ShowScreen(Zaxis-1,1,0);
		}
	}
}
void ResetMenuPassword(void)
{
	if(Lcd.Z13 == 0)
	{
		XLCDL1home();
		XLCDPutRamString(SPAC);
		XLCDL4home();
		XLCDPutRamString(SAVE_CANCEL); 
		XLCDL3home();
		XLCDPutRamString(SPAC); 
		XLCDL2home();
		XLCDPutRamString(RSTMENUPWD_); 
		Lcd.Z13 = 1,KP_Data = 0;
	}
	else if(Lcd.Z13 == 1)
	{
		if(KP_Data == 'S')
		{
			Lcd.Z13 = 0;
			strcpy((char*)ExstingPW,LCD_PASSWORD);
			String_WriteEEPROM_ex(4,(unsigned char*)LCD_PASSWORD,LCD_KEY);		 // Writing Default Password - LCD.
			ShowScreen(Zaxis-1,1,0);
		}
		else if(KP_Data == 'C')
		{
			ShowScreen(Zaxis-1,1,0);
			Lcd.Z13 = 0;
		}
		KP_Data = 0;
	}
}

void ErrorScreen(void )
{
	XLCDL1home();
	XLCDPutRamString(Ero); 
	XLCDL2home();
	XLCDPutRamString(SPAC); 
}

void SetOperationMode(void)
{	
	if( Lcd.Z15 == 0 )
	{
		XLCDL1home();
		XLCDPutRamString(OPP_MODE_1); 
		XLCDL2home();
		XLCDPutRamString(SPAC); 
		XLCDL3home();
		if(Operation_mode == MANUAL)
			XLCDPutRamString(MANUAL_), CursorPos = 0;
		else if(Operation_mode == AUTO)
			XLCDPutRamString(AUTO_), CursorPos = 1;
		else if(Operation_mode == AUTO_PLUS)
			XLCDPutRamString(AUTO_PLUS_), CursorPos = 2;
		XLCDL4home();
		XLCDPutRamString(SAVE_CANCEL); 
		Lcd.Z15 = 1,KP_Data = 0;
		position = Operation_mode;
	}
	else if( Lcd.Z15 == 1 )
	{
		if( ((KP_Data == 'D') && (CursorPos==0)) || ((KP_Data == 'U') && (CursorPos==2)) )
		{
			if(KP_Data == 'D')
				CursorPos++;
			else
				CursorPos--;
			KP_Data = 0;
			XLCDL3home();
			XLCDPutRamString(AUTO_); 
			position = AUTO;
		}
		else if( KP_Data == 'D' && (CursorPos==1) )
		{
			KP_Data = 0;
			CursorPos++;
			XLCDL3home();
			XLCDPutRamString(AUTO_PLUS_); 
			position = AUTO_PLUS;
		}
		else if( KP_Data == 'U' && (CursorPos==1) )
		{
			CursorPos--;
			XLCDL3home();
			XLCDPutRamString(MANUAL_); 
			position = MANUAL;
		}
		else if( KP_Data == 'S' )
		{
			if(Operation_mode != position)								// if operation mode is changed
			{
				Manual_On_set = FALSE;
				Char_WriteEEPROM_ex(Manual_On_set,   	 EE_MAN_ON_SET);
				Load_Status = 0;
				Char_WriteEEPROM_ex(Load_Status,  EE_LOAD_STATUS);
			    Switch_OFF_FEEDER();
			}	
			Operation_mode = position;
			if(Operation_mode == AUTO || Operation_mode == AUTO_PLUS)
			{
				Manual_On_set = FALSE;
				Char_WriteEEPROM_ex(Manual_On_set,   	 EE_MAN_ON_SET);
			}	
			if(Operation_mode == AUTO)
				Check_Present_Auto_Shift_Time();
			else if(Operation_mode == AUTO_PLUS)
				Decide_AutoPlus_Shift_Position();   

			Char_WriteEEPROM_ex(Operation_mode,EE_OPR_MODE);
			ShowScreen(Zaxis-1,1,0), Lcd.Z15 = 0;
		}
		else if( KP_Data == 'C' )
		{
			ShowScreen(Zaxis-1,1,0), Lcd.Z15 = 0;
		}	
	}
}

void AutoSettings(void)
{
	unsigned char h1 = 0,m1 = 0;
	unsigned char h2 = 0,m2 = 0;
	unsigned char h3 = 0,m3 = 0;
	
	if(process_lcd == 0)
	{
		memset(process_lcd_buff,0x00,20);
		XLCDL4home();
		XLCDPutRamString(SAVE_CANCEL); 
		XLCDL1home();
//		R_PHASE_VOL_[] =   "  R PHASE: ___._ V  ";
		sprintf(Line1Buff, " ON TIME   - %02d:%02d  ",FirstONTime.Hours,FirstONTime.Minutes);
		XLCDPutRamString(Line1Buff); 
		XLCDL2home();
		sprintf(Line1Buff, " RUN HOURS - %02d:%02d  ",No_Of_ON_Hours.Hours,No_Of_ON_Hours.Minutes);
		XLCDPutRamString(Line1Buff); 
		XLCDL3home();
		sprintf(Line1Buff, " OFF HOURS - %02d:%02d  ",No_Of_OFF_Hours.Hours,No_Of_OFF_Hours.Minutes);
		XLCDPutRamString(Line1Buff); 
		MoveLeft(27);	 
		XLCD_BLINKOON();
		process_lcd = 1,KP_Data = 0;
		CursorPos = 0;
		sprintf((char*)process_lcd_buff, "%02d%02d%02d%02d%02d%02d",FirstONTime.Hours,FirstONTime.Minutes,No_Of_ON_Hours.Hours,No_Of_ON_Hours.Minutes,No_Of_OFF_Hours.Hours,No_Of_OFF_Hours.Minutes);
		ptr = process_lcd_buff;
		putsUART1((unsigned int *)process_lcd_buff);
	}
	else if(process_lcd == 1)
	{
		if(KP_Data == 'U' && CursorPos>0)
		{
			ptr--;
			KP_Data = 0;
			CursorPos--;
			MoveLeft(1)
			if(CursorPos == 1 || CursorPos == 5 || CursorPos == 9)
				MoveLeft(1)
			else if(CursorPos == 7)
				MoveRight(25)
			else if(CursorPos == 3)
				MoveLeft(35)
		}
		else if(KP_Data == 'D' && CursorPos < 11)
		{
			ptr++;
			KP_Data = 0;
			CursorPos++;
			MoveRight(1)
			if(CursorPos == 2 || CursorPos == 6 || CursorPos == 10)
				MoveRight(1)
			else if(CursorPos == 4)
				MoveRight(35)
			else if(CursorPos == 8)
				MoveLeft(25)
		}	
		else if( KP_Data > 47 && KP_Data < 58 && CursorPos < 12)
		{
			CursorPos++;
			XLCDPut(KP_Data);
			*ptr++ = KP_Data;
			KP_Data = 0;
			if(CursorPos == 2 || CursorPos == 6 || CursorPos == 10)
				MoveRight(1)
			else if(CursorPos == 4)
				MoveRight(35)
			else if(CursorPos == 8)
				MoveLeft(25)
			if(CursorPos == 12)
			{
				CursorPos--;
				ptr--;
				MoveLeft(1)
			}	
		}
		else if(KP_Data == 'S')
		{
			h1 =  ConvertSTRING_CHAR(&process_lcd_buff[0],2);
			m1  =  ConvertSTRING_CHAR(&process_lcd_buff[2],2);
			h2 =  ConvertSTRING_CHAR(&process_lcd_buff[4],2);
			m2  =  ConvertSTRING_CHAR(&process_lcd_buff[6],2);
			h3 =  ConvertSTRING_CHAR(&process_lcd_buff[8],2);
			m3  =  ConvertSTRING_CHAR(&process_lcd_buff[10],2);
//			PCPutInt1(h1);
//			PCPutInt1(m1);
//			PCPutInt1(h2);
//			PCPutInt1(m2);
//			PCPutInt1(h3);
//			PCPutInt1(m3);
//			PCPutInt1(CursorPos);
			if( h1 < 24 &&  h2 < 24 && h3 <24 && m1 < 60 && m2 < 60 && m3 < 60 && ((60*(h2+h3)+m2+m3)<=1440) && ((60*(h2+h3)+m2+m3)>1))
			{
			putsUART1((unsigned int*)"\n\rSuccess");
				FirstONTime.Hours 	= h1;
				FirstONTime.Minutes 	= m1;
				No_Of_ON_Hours.Hours 	= h2;
				No_Of_ON_Hours.Minutes 	= m2;
				No_Of_OFF_Hours.Hours 	= h3;
				No_Of_OFF_Hours.Minutes = m3;
				Char_WriteEEPROM_ex(FirstONTime.Hours,		EE_FIRST_ONTIME_HH);
				Char_WriteEEPROM_ex(FirstONTime.Minutes,		EE_FIRST_ONTIME_MM);
				Char_WriteEEPROM_ex(No_Of_ON_Hours.Hours,		EE_ON_HOURS_HH);
				Char_WriteEEPROM_ex(No_Of_ON_Hours.Minutes,		EE_ON_HOURS_MM);
				Char_WriteEEPROM_ex(No_Of_OFF_Hours.Hours,		EE_OFF_HOURS_HH);
				Char_WriteEEPROM_ex(No_Of_OFF_Hours.Minutes,		EE_OFF_HOURS_MM);
				process_lcd = 0,KP_Data =0,CursorPos = 0;
				Operation_mode = position;
				Char_WriteEEPROM_ex(position,EE_OPR_MODE);
				Each_Auto_Shift_Time.Value = No_Of_ON_Hours.Value + No_Of_OFF_Hours.Value;
				CheckTimeBoundaries(&Each_Auto_Shift_Time);
				Calc_No_Of_Auto_Shits_Cnts();
				Check_Present_Auto_Shift_Time();
				ShowScreen(Zaxis-1,1,0);
			}
			else
			{
				putsUART1((unsigned int*)"\n\rfail");
				process_lcd = 0,KP_Data =0,CursorPos = 0;
				WrongValues_errorScreen(0);
//				ShowScreen(Zaxis-1,1,0);
			}	
		}		
		else if(KP_Data == 'C')
		{
			process_lcd = 0,KP_Data =0,CursorPos = 0;
			ShowScreen(Zaxis-1,1,0);
		}	
		else if(KP_Data == 'E')
		{
			KP_Data = 0;
			if(CursorPos<2)
			{
				MoveRight(40-CursorPos)
				ptr = ptr + 4-CursorPos;
				CursorPos = 4;
			}
			else if(CursorPos<4)
			{
				MoveRight(39-CursorPos)
				ptr = ptr + 4-CursorPos;
				CursorPos = 4;
			}
			else if(CursorPos<6)
			{
				MoveLeft(16+CursorPos)
				ptr = ptr + 8-CursorPos;
				CursorPos = 8;
			}		
			else if(CursorPos<8)
			{
				MoveLeft(17+CursorPos)
				ptr = ptr + 8-CursorPos;
				CursorPos = 8;
			}		
		}	
	}
}


void Auto_Plus_Settings(void)
{
	if( process_lcd == 0 )
	{
		XLCDL1home();
		XLCDPutRamString(SPAC); 
		XLCDL2home();
		XLCDPutRamString(NO_OF_SHIFTS_); 
		XLCDL4home();
		XLCDPutRamString(ENTER_CANCEL); 
		XLCDL3home();
		XLCDPutRamString(NOOFSHI_);
		process_lcd = 1,KP_Data = 0;
//		CursorPos = 0;
		MoveLeft(11);	 
		XLCD_BLINKOON();
	}
	else if( process_lcd == 1 )						// for taking no of shits
	{
		if( KP_Data > 48 && KP_Data < 57 )
		{
			XLCDPut(KP_Data);
//			CursorPos++;
			temp_noofshifts = KP_Data - 0x30;		// assigning no of shits in to temparory variable
			shifts_count = 0;
			KP_Data = 0;
			MoveLeft(1);	 
//			XLCD_CURSOR_OFF();
		}	
		else if(KP_Data == 'E')
		{
			process_lcd = 2;
			KP_Data = 0;
//			CursorPos = 0;
		}	
		else if(KP_Data == 'C')
		{
			process_lcd = 0,KP_Data =0,CursorPos = 0;
			shifts_count = 0;
			temp_noofshifts = 0;
			ShowScreen(Zaxis-1,1,0);
		}	
	}	
	else if( process_lcd == 2 )						// for taking no of shits
	{
		if(shifts_count<temp_noofshifts)
			SetONOFFTime(shifts_count);
		else										// save all shift settings
		{
			SaveShiftSettings();
			Decide_AutoPlus_Shift_Position();
			process_lcd = 0,KP_Data =0;//,CursorPos = 0;
			shifts_count = 0;
			temp_noofshifts = 0;
			ShowScreen(Zaxis-1,1,0);
		}	
	}	
}
	
void SetPwrFailAdjust(void)
{
}

void ShowPowerLog(void)
{
}
	
void SetDate(void)
{
	unsigned char bDate =0, bMonth =0, bYear = 0;

	if( Lcd.Z0 == 0)
	{
		memset(process_lcd_buff,0x00,20);
		XLCDL1home();
		XLCDPutRamString(SETDATE);
		XLCDL3home();
		XLCDPutRamString(SPAC);
		XLCDL4home();
		XLCDPutRamString(SAVE_CANCEL); 
		XLCDL2home();
		XLCDPutRamString(Date_); 
		MoveLeft(9);	 
		Lcd.Z0 = 1,KP_Data = 0,CursorPos = 0;
		XLCD_BLINKOON();
		value_entered = 0;
		ptr = process_lcd_buff;
	}
	if(Lcd.Z0 == 1)
	{
		if(KP_Data == 'U' && CursorPos>0)
		{
			ptr--;
			KP_Data = 0;
			CursorPos--;
			MoveLeft(1)
			if(CursorPos == 1 || CursorPos == 3)
				MoveLeft(1)
		}
		else if(KP_Data == 'D' && CursorPos < 5 && CursorPos<value_entered)
		{
			ptr++;
			KP_Data = 0;
			CursorPos++;
			MoveRight(1)
			if(CursorPos == 2 || CursorPos == 4)
				MoveRight(1)
		}	
		else if( KP_Data > 47 && KP_Data < 58 && CursorPos < 6)
		{
			CursorPos++;
			value_entered++;
			XLCDPut(KP_Data);
			*ptr++ = KP_Data;
			KP_Data = 0;
			if(CursorPos == 2 || CursorPos == 4)
				MoveRight(1)
			if(CursorPos == 6)
			{
				MoveLeft(1)
				CursorPos--;
				ptr--;
			}	
		}
		if( KP_Data == 'C')
		{
			Lcd.Z0 = 0,KP_Data =0;
			value_entered = 0;
			CursorPos = 0;
			ShowScreen(Zaxis-1,1,0);
		}
		else if (KP_Data == 'S')
		{
			bDate =  ConvertSTRING_CHAR(&process_lcd_buff[0],2);
			bMonth  =  ConvertSTRING_CHAR(&process_lcd_buff[2],2);
			bYear  =  ConvertSTRING_CHAR(&process_lcd_buff[4],2);
			if( bDate < 32 &&  bMonth < 13 && bYear < 100 )
			{
				W_RTC_Date(bDate, bMonth, bYear, 0);
//				Compute_Off_Time();
			}
			Lcd.Z0 = 0,KP_Data =0;
			value_entered = 0;
			CursorPos = 0;
			ShowScreen(Zaxis-1,1,0);
		}
	}
}

void Calibration(void)
{
	R_int = 0;
	R_float = 0;
	Y_int = 0;
	Y_float = 0;
	B_int = 0;
	B_float = 0;
	static int onetime_process = 0;
	
	if( process_lcd == 0 )
		PasswordEnter( 1 );
	else if( process_lcd == 1 )
	{
		memset(process_lcd_buff,0x00,20);
		XLCDL4home();
		XLCDPutRamString(SAVE_CANCEL); 
		XLCDL1home();
		XLCDPutRamString(R_PHASE_VOL_); 
		XLCDL2home();
		XLCDPutRamString(Y_PHASE_VOL_); 
		XLCDL3home();
		XLCDPutRamString(B_PHASE_VOL_); 
		MoveLeft(29);	 
		XLCD_BLINKOON();
		process_lcd = 2,KP_Data = 0;
		CursorPos = 0;
		ptr = process_lcd_buff;
		onetime_process = 0;
	}
	else if(process_lcd == 2)
	{
		if(KP_Data == 'U' && CursorPos>0)
		{
			ptr--;
			KP_Data = 0;
			CursorPos--;
			MoveLeft(1)
			if(CursorPos == 2 || CursorPos == 6 || CursorPos == 10)
				MoveLeft(1)
			else if(CursorPos == 7)
				MoveRight(25)
			else if(CursorPos == 3)
				MoveLeft(35)
		}
		else if(KP_Data == 'D' && CursorPos < 11)
		{
			ptr++;
			KP_Data = 0;
			CursorPos++;
			MoveRight(1)
			if(CursorPos == 3 || CursorPos == 7 || CursorPos == 11)
				MoveRight(1)
			else if(CursorPos == 4)
				MoveRight(35)
			else if(CursorPos == 8)
				MoveLeft(25)
		}	
		else if( KP_Data > 47 && KP_Data < 58 && CursorPos < 12)
		{
			CursorPos++;
			XLCDPut(KP_Data);
			*ptr++ = KP_Data;
			KP_Data = 0;
			if(CursorPos == 3 || CursorPos == 7 || CursorPos == 11)
				MoveRight(1)
			else if(CursorPos == 4)
			{
				onetime_process = 0;
				MoveRight(35)
			}
			else if(CursorPos == 8)
			{
				onetime_process = 1;
				MoveLeft(25)
			}
			if(CursorPos == 12)
			{
				CursorPos--;
				ptr--;
				MoveLeft(1)
			}	
		}
		else if(KP_Data == 'S')
		{
			R_int =  ConvertSTRING_Int(&process_lcd_buff[0],3);
			R_float  =  ConvertSTRING_CHAR(&process_lcd_buff[3],1);
			Y_int =  ConvertSTRING_Int(&process_lcd_buff[4],3);
			Y_float  =  ConvertSTRING_CHAR(&process_lcd_buff[7],1);
			B_int =  ConvertSTRING_Int(&process_lcd_buff[8],3);
			B_float  =  ConvertSTRING_CHAR(&process_lcd_buff[11],1);
			if( R_int &&  Y_int &&  B_int )
			{
//				putsUART1((unsigned int*)"\n\rSuccess");
				Mult_Fact_R = Temp_Mult_Fact_R;
				Mult_Fact_Y = Temp_Mult_Fact_Y;
				User_Ent_B_phase 	= B_int + B_float/10.0;
				Mult_Fact_B = User_Ent_B_phase/(float)Calib_RMS_B_ph;
//				printf("\nCalib_RMS_B_ph%fMult_Fact_B=%f",Calib_RMS_B_ph,Mult_Fact_B);
//				printf("\nR:%f",User_Ent_R_phase);
//				printf("\nY:%f",User_Ent_Y_phase);
//				printf("\nB:%f",User_Ent_B_phase);
				
				ptr = (unsigned char*)&Mult_Fact_R;
				String_WriteEEPROM_ex(EE_MULT_FACT_R, ptr, 4);
				ptr = (unsigned char*)&Mult_Fact_Y;
				String_WriteEEPROM_ex(EE_MULT_FACT_Y, ptr, 4);
				ptr = (unsigned char*)&Mult_Fact_B;
				String_WriteEEPROM_ex(EE_MULT_FACT_B, ptr, 4);
//				printf("\nMult_Fact_R%f Mult_Fact_Y=%f Mult_Fact_B%f",Mult_Fact_R,Mult_Fact_Y,Mult_Fact_B);
				process_lcd = 0,KP_Data =0,CursorPos = 0;
				ShowScreen(Zaxis-1,1,0);
			}
			else
			{
//				putsUART1((unsigned int*)"\n\rfail");
				process_lcd = 1,KP_Data =0,CursorPos = 0;
				WrongValues_errorScreen(1);
//				ShowScreen(Zaxis-1,1,0);
			}	
		}		
		else if(KP_Data == 'C')
		{
			process_lcd = 0,KP_Data =0,CursorPos = 0;
			ShowScreen(Zaxis-1,1,0);
		}	
		else if(KP_Data == 'E')
		{
			KP_Data = 0;
			if(CursorPos<2)
			{
				MoveRight(40-CursorPos)
				ptr = ptr + 4-CursorPos;
				CursorPos = 4;
			}
			else if(CursorPos<4)
			{
				MoveRight(39-CursorPos)
				ptr = ptr + 4-CursorPos;
				CursorPos = 4;
			}
			else if(CursorPos<6)
			{
				MoveLeft(16+CursorPos)
				ptr = ptr + 8-CursorPos;
				CursorPos = 8;
			}		
			else if(CursorPos<8)
			{
				MoveLeft(17+CursorPos)
				ptr = ptr + 8-CursorPos;
				CursorPos = 8;
			}		
		}
		if(CursorPos == 4 && onetime_process == 0)
		{
			onetime_process = 1;
			R_int =  ConvertSTRING_Int(&process_lcd_buff[0],3);
			R_float  =  ConvertSTRING_CHAR(&process_lcd_buff[3],1);
			User_Ent_R_phase 	= R_int + R_float/10.0;
			Temp_Mult_Fact_R = User_Ent_R_phase/(float)Calib_RMS_R_ph;
//			printf("\nCalib_RMS_R_ph=%ld,Mult_Fact_R=%f",Calib_RMS_R_ph,Mult_Fact_R);
		}	
		if(CursorPos == 8 && onetime_process == 1)
		{
			onetime_process = 2;
			Y_int =  ConvertSTRING_Int(&process_lcd_buff[4],3);
			Y_float  =  ConvertSTRING_CHAR(&process_lcd_buff[7],1);
			User_Ent_Y_phase 	= Y_int + Y_float/10.0;
			Temp_Mult_Fact_Y = User_Ent_Y_phase/(float)Calib_RMS_Y_ph;
//			printf("\nCalib_RMS_Y_ph=%ld,Mult_Fact_Y=%f",Calib_RMS_Y_ph,Mult_Fact_Y);
		}	
	}
}
	
void Change_HLV_Cut(void)
{
	unsigned char parse_success =0;
//	unsigned char *ptr;
	
	if( process_lcd == 0 )
	{
		XLCDL1home();
		XLCDPutRamString(HLV_CUT_); 
		XLCDL2home();
		XLCDPutRamString(SPAC); 
		XLCDL3home();
		if(HLV_mode == ENABLE)
			XLCDPutRamString(ENABLE_), CursorPos = 0;
		else if(HLV_mode == DISABLE)
			XLCDPutRamString(DISABLE_), CursorPos = 1;
		XLCDL4home();
		XLCDPutRamString(SAVE_CANCEL); 
		process_lcd = 1,KP_Data = 0;
		position = HLV_mode;
	}
	else if( process_lcd == 1 )
	{
		if( ((KP_Data == 'D') && (CursorPos==0)) )
		{
			if(KP_Data == 'D')
				CursorPos++;
			KP_Data = 0;
			XLCDL3home();
			XLCDPutRamString(DISABLE_); 
			position = DISABLE;
		}
		else if( KP_Data == 'U' && (CursorPos==1) )
		{
			CursorPos--;
			XLCDL3home();
			XLCDPutRamString(ENABLE_); 
			position = ENABLE;
		}
		else if( KP_Data == 'S' )
		{
			if(position == ENABLE)
			{
				process_lcd = 2;
			}
			else
			{	
				HLV_mode = position;
				Char_WriteEEPROM_ex(HLV_mode, EE_HLV_MODE);
				ShowScreen(Zaxis-1,1,0), process_lcd = 0;
			}
		}
		else if( KP_Data == 'C' )
		{
			ShowScreen(Zaxis-1,1,0), process_lcd = 0;
		}	
	}
	else if(process_lcd == 2)
	{
		memset(process_lcd_buff,0x00,20);
		XLCDL4home();
		XLCDPutRamString(SAVE_CANCEL); 
		XLCDL1home();
		XLCDPutRamString(HLV_CUT_); 
		XLCDL2home();
		sprintf(Line1Buff, "   HV CUT : %03d     ",HV_Cut_Value);
		XLCDPutRamString(Line1Buff); 
		XLCDL3home();
		sprintf(Line1Buff, "   LV CUT : %03d     ",LV_Cut_Value);
		XLCDPutRamString(Line1Buff); 
//		MoveLeft(7);	 
		MoveRight(12);	 
		XLCD_BLINKOON();
		process_lcd = 3,KP_Data = 0;
		CursorPos = 0;
		sprintf((char *)process_lcd_buff, "%03d%03d",HV_Cut_Value,LV_Cut_Value);
		ptr = process_lcd_buff;
		putsUART1((unsigned int*)process_lcd_buff);
	}
	else if(process_lcd == 3)
	{
		if(KP_Data == 'U' && CursorPos>0)
		{
			ptr--;
			KP_Data = 0;
			CursorPos--;
			MoveLeft(1)
//			if(CursorPos == 1 || CursorPos == 5 || CursorPos == 9)
//				MoveLeft(1)
			if(CursorPos == 2)
				MoveRight(23)
//			else if(CursorPos == 2)
//				MoveLeft(35)
		}
		else if(KP_Data == 'D' && CursorPos < 5)
		{
			ptr++;
			KP_Data = 0;
			CursorPos++;
			MoveRight(1)
//			if(CursorPos == 2 || CursorPos == 6 || CursorPos == 10)
//				MoveRight(1)
//			else if(CursorPos == 4)
//				MoveRight(35)
			if(CursorPos == 3)
				MoveLeft(23)
		}	
		else if( KP_Data > 47 && KP_Data < 58 && CursorPos < 6)
		{
			CursorPos++;
			XLCDPut(KP_Data);
			*ptr++ = KP_Data;
			KP_Data = 0;
//			if(CursorPos == 2 || CursorPos == 6 || CursorPos == 10)
//				MoveRight(1)
//			else if(CursorPos == 4)
//				MoveRight(35)
			if(CursorPos == 3)
				MoveLeft(23)
			if(CursorPos == 6)
			{
				CursorPos--;
				ptr--;
				MoveLeft(1)
			}	
		}
		else if(KP_Data == 'S')
		{
			temp_HV =  ConvertSTRING_Int(&process_lcd_buff[0],3);
			temp_LV  =  ConvertSTRING_Int(&process_lcd_buff[3],3);
				putsUART1((unsigned int*)"\n\rtemp_HV_CUT=");
				PCPutInt1(temp_HV);
				putsUART1((unsigned int*)"\n\rtemp_LV_CUT=");
				PCPutInt1(temp_LV);
			if( (temp_HV <= PH3_MAX_HVCUT) && (temp_HV >= PH3_MIN_HVCUT) && (temp_LV <= PH3_MAX_LVCUT) && (temp_LV >= PH3_MIN_LVCUT) )
				Phase_Mode = THREE_PHASE, parse_success = 1;
			else if( (temp_HV <= PH1_MAX_HVCUT) && (temp_HV >= PH1_MIN_HVCUT) && (temp_LV <= PH1_MAX_LVCUT) && (temp_LV >= PH1_MIN_LVCUT) )
				Phase_Mode = SINGLE_PHASE, parse_success = 1;
			else
				parse_success = 0;

			if( parse_success == 1)
			{
				putsUART1((unsigned int*)"\n\rSuccess");
				HV_Cut_Value = temp_HV;
				LV_Cut_Value = temp_LV;
				ptr = (unsigned char*)&HV_Cut_Value;
				String_WriteEEPROM_ex(EE_HV_CUT_VALUE, ptr, 2);				// write default high voltage cutoff value
				ptr = (unsigned char*)&LV_Cut_Value;
				String_WriteEEPROM_ex(EE_LV_CUT_VALUE, ptr, 2);				// write default low voltage cutoff value
				Char_WriteEEPROM_ex(Phase_Mode,	EE_PHASE_MODE);				// write default operating phase mode

				process_lcd = 0,KP_Data =0,CursorPos = 0;
				putsUART1((unsigned int*)"\n\rHV_CUT=");
				PCPutInt1(HV_Cut_Value);
				putsUART1((unsigned int*)"\n\rLV_CUT=");
				PCPutInt1(LV_Cut_Value);
				putsUART1((unsigned int*)"\n\rPHASE MODE=");
				PCPutInt1(Phase_Mode);

				if(HLV_mode != position)								// if operation mode is changed
				{
					HLV_mode = position;
					Char_WriteEEPROM_ex(HLV_mode, EE_HLV_MODE);
				}	
				ShowScreen(Zaxis-1,1,0);
			}
			else
			{
				putsUART1((unsigned int*)"\n\rfail");
				process_lcd = 2,KP_Data =0,CursorPos = 0;
				WrongValues_errorScreen(1);
//				ShowScreen(Zaxis-1,1,0);
			}	
		}		
		else if(KP_Data == 'C')
		{
			process_lcd = 0,KP_Data =0,CursorPos = 0;
			ShowScreen(Zaxis-1,1,0);
		}	
		else if(KP_Data == 'E')
		{
			KP_Data = 0;
			if(CursorPos<3)
			{
				MoveLeft(20+CursorPos)
				ptr = ptr + 3-CursorPos;
				CursorPos = 3;
			}		
		}	
	}
}	

unsigned char ValidateTime(void)
{
	unsigned char h1 = 0, m1 = 0, h2 = 0, m2 = 0;
	
	h1 = ConvertSTRING_CHAR(&process_time_buff[shifts_count][0],2);
	m1 = ConvertSTRING_CHAR(&process_time_buff[shifts_count][2],2);
	h2 = ConvertSTRING_CHAR(&process_time_buff[shifts_count][4],2);
	m2 = ConvertSTRING_CHAR(&process_time_buff[shifts_count][6],2);
	if(h1 < 24 && h2 < 24 && m1 < 60 && m2 < 60)
		return 1;
	else
		return 0;
}

void WrongValues_errorScreen(unsigned char type)
{
	XLCDL1home();
	XLCDPutRamString(SPAC); 
	XLCDL2home();
	if(type == 0)
		XLCDPutRamString(" WRONG TIME ENTERED "); 
	else if(type == 1)
		XLCDPutRamString("WRONG VALUES ENTERED"); 
	XLCDL3home();
	XLCDPutRamString(SPAC); 
	XLCDL4home();
	XLCDPutRamString(SPAC); 
	TimerDelay_10ms(300);
}
				
void SaveShiftSettings(void)
{
	unsigned int Start_address = 0;
	unsigned char i = 0;

	AssignTimingValues();
	No_of_Sifts = temp_noofshifts;
	Char_WriteEEPROM_ex(No_of_Sifts,		EE_NO_OF_SHIFT_SET);

	Start_address = EE_ONTIME_HH_1;
	for(i = 0; i < No_of_Sifts; i++)
	{
		Char_WriteEEPROM_ex(ONTime[i].Hours,	Start_address++);					// write default ontime hours value
		Char_WriteEEPROM_ex(ONTime[i].Minutes,	Start_address++);					// write default ontime minutes value
		Char_WriteEEPROM_ex(OFFTime[i].Hours,	Start_address++);					// write default off time hours value
		Char_WriteEEPROM_ex(OFFTime[i].Minutes,	Start_address++);					// write default offtime minutes value
	}
	PrintShiftTimings();
}

void AssignTimingValues(void)
{
	unsigned char h1 = 0, m1 = 0, h2 = 0, m2 = 0;
	unsigned char i = 0;
	
	for(i = 0; i<temp_noofshifts; i++)
	{
		h1 = ConvertSTRING_CHAR(&process_time_buff[i][0],2);
		m1 = ConvertSTRING_CHAR(&process_time_buff[i][2],2);
		h2 = ConvertSTRING_CHAR(&process_time_buff[i][4],2);
		m2 = ConvertSTRING_CHAR(&process_time_buff[i][6],2);
		ONTime[i].Hours = h1;
		ONTime[i].Minutes = m1;
		ONTime[i].Seconds = 0;
		OFFTime[i].Hours = h2;
		OFFTime[i].Minutes = m2;
		OFFTime[i].Seconds = 0;
	}
}		

void PrintShiftTimings(void)
{
	putsUART1((unsigned int*)"\nShift1:\nON:");
	PCPutInt1(ONTime[0].Hours);
	WriteUART1(':');
	PCPutInt1(ONTime[0].Minutes);
	putsUART1((unsigned int*)"\nOFF:");
	PCPutInt1(OFFTime[0].Hours);
	WriteUART1(':');
	PCPutInt1(OFFTime[0].Minutes);

	putsUART1((unsigned int*)"\nShift2:\nON:");
	PCPutInt1(ONTime[1].Hours);
	WriteUART1(':');
	PCPutInt1(ONTime[1].Minutes);
	putsUART1((unsigned int*)"\nOFF:");
	PCPutInt1(OFFTime[1].Hours);
	WriteUART1(':');
	PCPutInt1(OFFTime[1].Minutes);

	putsUART1((unsigned int*)"\nShift3:\nON:");
	PCPutInt1(ONTime[2].Hours);
	WriteUART1(':');
	PCPutInt1(ONTime[2].Minutes);
	putsUART1((unsigned int*)"\nOFF:");
	PCPutInt1(OFFTime[2].Hours);
	WriteUART1(':');
	PCPutInt1(OFFTime[2].Minutes);

	putsUART1((unsigned int*)"\nShift4:\nON:");
	PCPutInt1(ONTime[3].Hours);
	WriteUART1(':');
	PCPutInt1(ONTime[3].Minutes);
	putsUART1((unsigned int*)"\nOFF:");
	PCPutInt1(OFFTime[3].Hours);
	WriteUART1(':');
	PCPutInt1(OFFTime[3].Minutes);

	putsUART1((unsigned int*)"\nShift5:\nON:");
	PCPutInt1(ONTime[4].Hours);
	WriteUART1(':');
	PCPutInt1(ONTime[4].Minutes);
	putsUART1((unsigned int*)"\nOFF:");
	PCPutInt1(OFFTime[4].Hours);
	WriteUART1(':');
	PCPutInt1(OFFTime[4].Minutes);

	putsUART1((unsigned int*)"\nShift6:\nON:");
	PCPutInt1(ONTime[5].Hours);
	WriteUART1(':');
	PCPutInt1(ONTime[5].Minutes);
	putsUART1((unsigned int*)"\nOFF:");
	PCPutInt1(OFFTime[5].Hours);
	WriteUART1(':');
	PCPutInt1(OFFTime[5].Minutes);

	putsUART1((unsigned int*)"\nShift7:\nON:");
	PCPutInt1(ONTime[6].Hours);
	WriteUART1(':');
	PCPutInt1(ONTime[6].Minutes);
	putsUART1((unsigned int*)"\nOFF:");
	PCPutInt1(OFFTime[6].Hours);
	WriteUART1(':');
	PCPutInt1(OFFTime[6].Minutes);

	putsUART1((unsigned int*)"\nShift8:\nON:");
	PCPutInt1(ONTime[7].Hours);
	WriteUART1(':');
	PCPutInt1(ONTime[7].Minutes);
	putsUART1((unsigned int*)"\nOFF:");
	PCPutInt1(OFFTime[7].Hours);
	WriteUART1(':');
	PCPutInt1(OFFTime[7].Minutes);
	putsUART1((unsigned int*)"\n\rSettings saved");
}

