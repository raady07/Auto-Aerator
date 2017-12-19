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
#include "24lc256.h"
#include "adcMUX.h"
#include "INIT.h"
#include "i2c.h"
#include "KP_InputFunctions.h"
#include "flags.h"
#include "lcd.h"
#include "RTC.h"
#include "Ex_EEPROM.h"
#include "main.h"
#include "timer.h"
#include "uart.h"
#include "xlcd2.h"
// ext osc 8 Mhz
#ifdef	INTERNAL_OSC_PLL16
	_FOSC( CSW_FSCM_OFF & FRC_PLL16 );
#endif

#ifdef	INTERNAL_OSC_PLL8
	_FOSC( CSW_FSCM_OFF & FRC_PLL8 );
#endif

#ifdef	INTERNAL_OSC
	_FOSC( CSW_FSCM_OFF & FRC );
#endif

#ifdef	PLL_16
	_FOSC( CSW_FSCM_OFF & XT_PLL16 );
#endif
#ifdef PLL_8
	_FOSC( CSW_FSCM_OFF & XT_PLL8 );
#endif
#ifdef PLL_4
	_FOSC( CSW_FSCM_OFF & XT_PLL4 );
#endif

_FWDT( WDTPSB_10 & WDTPSA_512 & WDT_ON ); // Watchdog timer = 2ms *10*512 = 10.24 sec.
_FBORPOR( PWRT_64 & PBOR_OFF & BORV_45 & MCLR_EN );
_FGS( CODE_PROT_ON );

void static Switch_ON_Feeder(void);
void static Multiple_Loads(void);

FlowControl Bits = {0b10000000};
Time SystemTime;
Time ONTime[8];
Time OFFTime[8];
Time FirstONTime;
Time No_Of_ON_Hours;
Time No_Of_OFF_Hours;
Time Each_Auto_Shift_Time;
Time MANUAL_ONTime;
Time RunHours;
Time PREV_MAN_ONTime;
Time PREV_MAN_OFFTime;
Time Auto_ONTime;
Time Auto_OFFTime;
Time Auto_Next_ONTime;
Time Auto_Next_OFFTime;
Time Prev_OFF_Time;
Time Power_Fail;
Time Power_Resume;
Time TimeOnRTC;
Time oneday;

unsigned char ExstingPW[5];
unsigned char NoOfShifts = 2;
unsigned char firsttime = 0;				// stores the value of weather controller running for first time or not
unsigned char Manual_On_set = 0;
unsigned char Load_Status = OFF;
unsigned char powerfail = 0;
unsigned char R_PHASE = 0, Y_PHASE = 0, B_PHASE = 0;
unsigned char Operation_mode = DFLT_SYS_MODE;			// system in auto or manual
unsigned char manual_mode_on = 0;
unsigned char Manual_key_hold_count = 0;
unsigned char ReadBuff[30];
unsigned char Compile_Time[9] = __TIME__;
unsigned char Compile_Date[25] = __DATE__;
unsigned char firsttime_Manual_off = 0;
unsigned int Total_Auto_Shifts_Count = 0;
unsigned int Present_Auto_Shift_Count = 0;
extern unsigned char  Sec, Min, Hour, DOW, Date, Month, Year;


unsigned char Auto_Plus_load = 0;
unsigned char Present_Auto_Plus_shift_count = 0;
unsigned char Prev_R_Phase_Status = 1;
unsigned char Prev_Y_Phase_Status = 1;
unsigned char Prev_B_Phase_Status = 1;
unsigned char Pres_RYB_Phase_Status = 0;

unsigned int HV_Cut_Value = DFLT_HV_CUT;
unsigned int LV_Cut_Value = DFLT_LV_CUT;
unsigned char Phase_Mode = DFLT_PHASE;
extern float R_Phase_Volt,Y_Phase_Volt,B_Phase_Volt;
extern unsigned char samples_done;
extern float RYB_PH2PH_Volt;
extern unsigned char Every_Minute, everysec;
unsigned char Error_Mode = 0;

unsigned char temp_Secs=0, num = 0, L_ON = 0, temp_cnt1 =0;

int	main(void)
{
	unsigned char clg = 0; // used for printing parameters.
	InitAllPheriperals();

	Calc_No_Of_Auto_Shits_Cnts();
	if(Operation_mode == AUTO)					Check_Present_Auto_Shift_Time();
	else if(Operation_mode == AUTO_PLUS)		Decide_AutoPlus_Shift_Position();
	if(Manual_On_set == TRUE && Pres_RYB_Phase_Status == ON && Operation_mode == MANUAL)			// if manual mode on
	{
		Load_Status = ON;
		Char_WriteEEPROM_ex(Load_Status,  EE_LOAD_STATUS);
	    Switch_ON_Feeder();
	}
	Decide_RYB_Sensors();
	//PrintPresentStatus();
	while(1)
	{
		Multiple_Loads();	
		if(SystemTime.Seconds == 0 && clg == 0) 	clg =1, Show_Parameters();
		else if(SystemTime.Seconds!=0 && clg !=0) 	clg = 0;
		if(samples_done == 1)
		{
			samples_done = 0;
			if(R_Phase_Volt < 10)
				R_Phase_Volt = 0;
			if(Y_Phase_Volt < 10)
				Y_Phase_Volt = 0;
			if(B_Phase_Volt < 10)
				B_Phase_Volt = 0;
		}	
		if(everysec >= 1)
		{
			everysec = 0;
			Decide_RYB_Sensors();
			Phase_Monitor();
			if( Load_Status == ON && (Pres_RYB_Phase_Status == OFF || ( (HLV_mode==ENABLE) &&((RYB_PH2PH_Volt < LV_Cut_Value)||(RYB_PH2PH_Volt > HV_Cut_Value)) ) ) )							// load off when any one of phase is OFF
			{
				Load_Status = OFF;
				Char_WriteEEPROM_ex(Load_Status,  EE_LOAD_STATUS);
			    Switch_OFF_FEEDER();
			}	
			else if( Load_Status == OFF && Operation_mode == MANUAL && Manual_On_set == TRUE)	// load on when power on in all phase in manual mode
			{
				if( (Pres_RYB_Phase_Status==ON) && ( (HLV_mode==DISABLE) || ((Pres_RYB_Phase_Status==ON) && (HLV_mode==ENABLE) && (RYB_PH2PH_Volt>LV_Cut_Value) && (RYB_PH2PH_Volt<HV_Cut_Value)) ) )
				{
					Load_Status = ON;
					Char_WriteEEPROM_ex(Load_Status,  EE_LOAD_STATUS);
			    	Switch_ON_Feeder();
				}   
			}	
			if(HLV_mode == ENABLE)
			{
				if(Pres_RYB_Phase_Status == OFF)
					Error_Mode = 1;
				else if(RYB_PH2PH_Volt < LV_Cut_Value)
					Error_Mode = 2;
				else if(RYB_PH2PH_Volt > HV_Cut_Value)
					Error_Mode = 3;
				else
					Error_Mode = 0;
			}
				
			if(Operation_mode == AUTO)
			{
				Auto_functionality();
			}	
			else if(Operation_mode == AUTO_PLUS)
			{
				Auto_Plus_functionality();
			}	
			if(Manual_On_set == TRUE && Pres_RYB_Phase_Status == ON)							// manual mode run hours count when load is running
				Run_Hours_counter();
			if( (Operation_mode == MANUAL) && (PORTFbits.RF6) && Manual_On_set==TRUE)
			{
				Manual_key_hold_count++;
				if(Manual_key_hold_count > 5)
				{
					PREV_MAN_OFFTime.Value = SystemTime.Value;
					PREV_MAN_ONTime.Value = MANUAL_ONTime.Value;
					Manual_On_set = FALSE;
					firsttime_Manual_off = 1;
					Load_Status = OFF;
					Char_WriteEEPROM_ex(Load_Status,  EE_LOAD_STATUS);
					Switch_OFF_FEEDER();
					Char_WriteEEPROM_ex(Manual_On_set,   	 	 EE_MAN_ON_SET);
					Char_WriteEEPROM_ex(firsttime_Manual_off,	 EE_FISRTMAN_OFF_SET);
					Char_WriteEEPROM_ex(PREV_MAN_ONTime.Hours,   EE_PREV_MAN_ON_TIME_HH);
					Char_WriteEEPROM_ex(PREV_MAN_ONTime.Minutes, EE_PREV_MAN_ON_TIME_MM);
					Char_WriteEEPROM_ex(PREV_MAN_ONTime.Seconds, EE_PREV_MAN_ON_TIME_SS);
					Char_WriteEEPROM_ex(PREV_MAN_OFFTime.Hours,  EE_PREV_MAN_OFF_TIME_HH);
					Char_WriteEEPROM_ex(PREV_MAN_OFFTime.Minutes, EE_PREV_MAN_OFF_TIME_MM);
					Char_WriteEEPROM_ex(PREV_MAN_OFFTime.Seconds, EE_PREV_MAN_OFF_TIME_SS);
				}
			}
			else if(!PORTFbits.RF6)
				Manual_key_hold_count = 0;
			if(SystemTime.Value == 0)
			{
				firsttime_Manual_off = 0;
				Char_WriteEEPROM_ex(Manual_On_set,   	 EE_MAN_ON_SET);
			}
		}
		if(Bits.En_Display_Lcd == 1)
		{
			Bits.En_Display_Lcd = 0;
			LCDTask();
		}
	    if(manual_mode_on == TRUE && Manual_key_hold_count == 0 && Manual_On_set == FALSE && Pres_RYB_Phase_Status == ON && (HLV_mode==DISABLE || (HLV_mode==ENABLE && RYB_PH2PH_Volt>LV_Cut_Value && RYB_PH2PH_Volt<HV_Cut_Value)))				// Check hardware Manual Push Button Is pressed
	    {
		    manual_mode_on = FALSE;
		    Manual_On_set = TRUE;
		    MANUAL_ONTime.Value = SystemTime.Value;
		    RunHours.Value = 0;
		    Switch_ON_Feeder();
			Load_Status = ON;
			Char_WriteEEPROM_ex(Load_Status,  EE_LOAD_STATUS);
			Char_WriteEEPROM_ex(MANUAL_ONTime.Hours,  EE_MAN_ONTIME_HH);
			Char_WriteEEPROM_ex(MANUAL_ONTime.Minutes,   EE_MAN_ONTIME_MM);
			Char_WriteEEPROM_ex(MANUAL_ONTime.Seconds,   EE_MAN_ONTIME_SS);
			Char_WriteEEPROM_ex(Manual_On_set,   	 EE_MAN_ON_SET);
			Char_WriteEEPROM_ex(RunHours.Hours,  EE_RUN_HRS_HH);
			Char_WriteEEPROM_ex(RunHours.Minutes,   EE_RUN_HRS_MM);
			Char_WriteEEPROM_ex(RunHours.Seconds,   EE_RUN_HRS_SS);
		}
		ClrWdt();
	}	
}

void Sync_with_RTCTime(void)
{
	SystemTime.Seconds = Sec;
	SystemTime.Minutes = Min;
	SystemTime.Hours = Hour;
}	

void CheckForFirstTime(void)
{    	
	unsigned char Compile_len = 0;				// stores the length of the compile date and time
	unsigned char Read_compile_len = 0;			// stores the length of the compile date and time from eeprom
	unsigned char i = 0;
	unsigned int Start_address = 0;
	unsigned char *ptr;
	
	strcat((char*)Compile_Date,(const char*)Compile_Time);			// string attach for compile date and time
	Compile_len = strlen((const char*)Compile_Date);			// read compile date and time string length
	
	putsUART1((unsigned int*)"\n\rC-Date:");
	putsUART1((unsigned int*)Compile_Date);
	putsUART1((unsigned int*)"\n\r");
	Read_compile_len = Char_ReadEEPROM_ex(EE_Compile_Len);	// read compile date and time string length from eeprom
	putsUART1((unsigned int*)"\n\rC-Len:");
	PCPutInt1(Read_compile_len);
	putsUART1((unsigned int*)"\n\r");
	if(Read_compile_len != 0xFF)				// if it not for first time
	{
		String_ReadEEPROM_ex(EE_Compile_Date, ReadBuff, Read_compile_len);
	putsUART1((unsigned int*)"\n\rE-Date:");
	putsUART1((unsigned int*)ReadBuff);
	putsUART1((unsigned int*)"\n\r");
		if( strcmp((const char *)ReadBuff, (const char *)Compile_Date) == 0)		
			firsttime = 1;			// compile version is same
		else
			firsttime = 0;			// compile version is different
	}
	else							// if it is for fist time	// for new board the value is always 0xff
		firsttime = 0;
	
	Make_all_values_zero();
   	if(firsttime != 1)				// if it is for fist time
   	{
		putsUART1((unsigned int*)"\n\rFirst Time");
		Char_WriteEEPROM_ex(Compile_len,  EE_Compile_Len);		// read compile date and time length
		putsUART1((unsigned int*)"\n\rDEBUG-Date:");
		putsUART1((unsigned int*)Compile_Date);
		putsUART1((unsigned int*)"\n\r");
		String_WriteEEPROM_ex(EE_Compile_Date, Compile_Date, Compile_len);	// read compile date and time string
	    Char_WriteEEPROM_ex(0,  eepromsysTimeHr);    			// write systemTime Hour
	    Char_WriteEEPROM_ex(0,  eepromsysTimeMin);    			// write systemTime Minutes
		Char_WriteEEPROM_ex(0,  eepromsysTimeSec);    			// write SystemTime Seconds
	    Char_WriteEEPROM_ex(0,  _RTCSetNewTime);    			// To check whether Time is set
		strcpy((char*)ExstingPW,LCD_PASSWORD);
		String_WriteEEPROM_ex(LCD_KEY, (unsigned char*)LCD_PASSWORD, 4);		// write default password

		Char_WriteEEPROM_ex(DFLT_SYS_MODE,		EE_OPR_MODE);				// write default system operating mode
		No_of_Sifts = DFLT_NO_OF_SHIFTS;
		Char_WriteEEPROM_ex(No_of_Sifts,		EE_NO_OF_SHIFT_SET);
		ONTime[0].Hours 		= DFLT_ONTIME_HH_1;
		ONTime[0].Minutes 		= DFLT_ONTIME_MM_1;
		OFFTime[0].Hours 		= DFLT_OFFTIME_HH_1;
		OFFTime[0].Minutes 		= DFLT_OFFTIME_MM_1;
		ONTime[1].Hours 		= DFLT_ONTIME_HH_2;
		ONTime[1].Minutes 		= DFLT_ONTIME_MM_2;
		OFFTime[1].Hours 		= DFLT_OFFTIME_HH_2;
		OFFTime[1].Minutes 		= DFLT_OFFTIME_MM_2;
		ONTime[2].Hours 		= DFLT_ONTIME_HH_3;
		ONTime[2].Minutes 		= DFLT_ONTIME_MM_3;
		OFFTime[2].Hours 		= DFLT_OFFTIME_HH_3;
		OFFTime[2].Minutes 		= DFLT_OFFTIME_MM_3;
		ONTime[3].Hours 		= DFLT_ONTIME_HH_4;
		ONTime[3].Minutes 		= DFLT_ONTIME_MM_4;
		OFFTime[3].Hours 		= DFLT_OFFTIME_HH_4;
		OFFTime[3].Minutes 		= DFLT_OFFTIME_MM_4;
		ONTime[4].Hours 		= DFLT_ONTIME_HH_5;
		ONTime[4].Minutes 		= DFLT_ONTIME_MM_5;
		OFFTime[4].Hours 		= DFLT_OFFTIME_HH_5;
		OFFTime[4].Minutes 		= DFLT_OFFTIME_MM_5;
		ONTime[5].Hours 		= DFLT_ONTIME_HH_6;
		ONTime[5].Minutes 		= DFLT_ONTIME_MM_6;
		OFFTime[5].Hours 		= DFLT_OFFTIME_HH_6;
		OFFTime[5].Minutes 		= DFLT_OFFTIME_MM_6;
		ONTime[6].Hours 		= DFLT_ONTIME_HH_7;
		ONTime[6].Minutes 		= DFLT_ONTIME_MM_7;
		OFFTime[6].Hours 		= DFLT_OFFTIME_HH_7;
		OFFTime[6].Minutes 		= DFLT_OFFTIME_MM_7;
		ONTime[7].Hours 		= DFLT_ONTIME_HH_8;
		ONTime[7].Minutes 		= DFLT_ONTIME_MM_8;
		OFFTime[7].Hours 		= DFLT_OFFTIME_HH_8;
		OFFTime[7].Minutes 		= DFLT_OFFTIME_MM_8;		
		Start_address = EE_ONTIME_HH_1;
		for(i = 0; i < 8; i++)
		{
			Char_WriteEEPROM_ex(ONTime[i].Hours,	Start_address++);					// write default ontime hours value
			Char_WriteEEPROM_ex(ONTime[i].Minutes,	Start_address++);					// write default ontime minutes value
			Char_WriteEEPROM_ex(OFFTime[i].Hours,	Start_address++);					// write default off time hours value
			Char_WriteEEPROM_ex(OFFTime[i].Minutes,	Start_address++);					// write default offtime minutes value
		}
		
		Char_WriteEEPROM_ex(Operation_mode,		EE_OPR_MODE);
		FirstONTime.Value = ONTime[0].Value;
		FirstONTime.Hours = DFLT_FIRST_ONTIME_HH;
		Char_WriteEEPROM_ex(DFLT_FIRST_ONTIME_HH,		EE_FIRST_ONTIME_HH);
		FirstONTime.Minutes = DFLT_FIRST_ONTIME_MM;
		Char_WriteEEPROM_ex(DFLT_FIRST_ONTIME_MM,		EE_FIRST_ONTIME_MM);
		No_Of_ON_Hours.Hours = DFLT_ON_HOURS_HH;
		Char_WriteEEPROM_ex(DFLT_ON_HOURS_HH,		EE_ON_HOURS_HH);
		No_Of_ON_Hours.Minutes = DFLT_ON_HOURS_MM;
		Char_WriteEEPROM_ex(DFLT_ON_HOURS_MM,		EE_ON_HOURS_MM);
		No_Of_OFF_Hours.Hours = DFLT_OFF_HOURS_HH;
		Char_WriteEEPROM_ex(DFLT_OFF_HOURS_HH,		EE_OFF_HOURS_HH);
		No_Of_OFF_Hours.Minutes = DFLT_OFF_HOURS_MM;
		Char_WriteEEPROM_ex(DFLT_OFF_HOURS_MM,		EE_OFF_HOURS_MM);
		Char_WriteEEPROM_ex(Manual_On_set,   	 EE_MAN_ON_SET);						// write default manual on key pressing status i.e FLASE
		Char_WriteEEPROM_ex(MANUAL_ONTime.Hours,  EE_MAN_ONTIME_HH);
		Char_WriteEEPROM_ex(MANUAL_ONTime.Minutes,   EE_MAN_ONTIME_MM);
		Char_WriteEEPROM_ex(MANUAL_ONTime.Seconds,   EE_MAN_ONTIME_SS);
		Char_WriteEEPROM_ex(RunHours.Hours,  EE_RUN_HRS_HH);
		Char_WriteEEPROM_ex(RunHours.Minutes,   EE_RUN_HRS_MM);
		Char_WriteEEPROM_ex(RunHours.Seconds,   EE_RUN_HRS_SS);
		Char_WriteEEPROM_ex(firsttime_Manual_off,EE_FISRTMAN_OFF_SET);
		Char_WriteEEPROM_ex(PREV_MAN_ONTime.Hours,  EE_PREV_MAN_ON_TIME_HH);
		Char_WriteEEPROM_ex(PREV_MAN_ONTime.Minutes,   EE_PREV_MAN_ON_TIME_MM);
		Char_WriteEEPROM_ex(PREV_MAN_ONTime.Seconds,   EE_PREV_MAN_ON_TIME_SS);
		Char_WriteEEPROM_ex(PREV_MAN_OFFTime.Hours,  EE_PREV_MAN_OFF_TIME_HH);
		Char_WriteEEPROM_ex(PREV_MAN_OFFTime.Minutes,   EE_PREV_MAN_OFF_TIME_MM);
		Char_WriteEEPROM_ex(PREV_MAN_OFFTime.Seconds,   EE_PREV_MAN_OFF_TIME_SS);
		
		Auto_ONTime.Value = FirstONTime.Value;
		Char_WriteEEPROM_ex(Auto_ONTime.Hours,  EE_AUTO_ONTIME_HH);
		Char_WriteEEPROM_ex(Auto_ONTime.Minutes,   EE_AUTO_ONTIME_MM);
		Char_WriteEEPROM_ex(Auto_ONTime.Seconds,   EE_AUTO_ONTIME_SS);

		Auto_OFFTime.Value = FirstONTime.Value + No_Of_ON_Hours.Value;
		CheckTimeBoundaries(&Auto_OFFTime);
		Char_WriteEEPROM_ex(Auto_OFFTime.Hours,  	EE_AUTO_OFFTIME_HH);
		Char_WriteEEPROM_ex(Auto_OFFTime.Minutes,   EE_AUTO_OFFTIME_MM);
		Char_WriteEEPROM_ex(Auto_OFFTime.Seconds,   EE_AUTO_OFFTIME_SS);

		Char_WriteEEPROM_ex(Load_Status,  EE_LOAD_STATUS);						// write default load status i.e. 0
		Char_WriteEEPROM_ex(Prev_R_Phase_Status,   PREV_R_PHASE_STATUS_);
		Char_WriteEEPROM_ex(Prev_Y_Phase_Status,   PREV_Y_PHASE_STATUS_);
		Char_WriteEEPROM_ex(Prev_B_Phase_Status,   PREV_B_PHASE_STATUS_);

		Char_WriteEEPROM_ex(powerfail,	EE_POWER_FAIL_STATE);
		Char_WriteEEPROM_ex(Power_Fail.Hours,	EE_PWR_FAIL_HH);
		Char_WriteEEPROM_ex(Power_Fail.Minutes,	EE_PWR_FAIL_MM);
		Char_WriteEEPROM_ex(Power_Fail.Seconds,	EE_PWR_FAIL_SS);
		Char_WriteEEPROM_ex(Power_Resume.Hours,		EE_PWR_RESUME_HH);
		Char_WriteEEPROM_ex(Power_Resume.Minutes,	EE_PWR_RESUME_MM);
		Char_WriteEEPROM_ex(Power_Resume.Seconds,	EE_PWR_RESUME_SS);

		Char_WriteEEPROM_ex(0,	TRAP_val);
		Char_WriteEEPROM_ex(0,	IOPUWR_val);
		Char_WriteEEPROM_ex(0,	EXTR_val);
		Char_WriteEEPROM_ex(0,	SWDT_val);
		Char_WriteEEPROM_ex(0,	WDTO_val);
		Char_WriteEEPROM_ex(0,	BOR_val);
		Char_WriteEEPROM_ex(0,	POR_val);

		ptr = (unsigned char*)&HV_Cut_Value;
		String_WriteEEPROM_ex(EE_HV_CUT_VALUE, ptr, 2);				// write default high voltage cutoff value
		ptr = (unsigned char*)&LV_Cut_Value;
		String_WriteEEPROM_ex(EE_LV_CUT_VALUE, ptr, 2);				// write default low voltage cutoff value
		Char_WriteEEPROM_ex(Phase_Mode,	EE_PHASE_MODE);				// write default operating phase mode
		Char_WriteEEPROM_ex(HLV_mode,   	 EE_HLV_MODE);

		ptr = (unsigned char*)&Mult_Fact_R;
		String_WriteEEPROM_ex(EE_MULT_FACT_R, ptr, 4);
		ptr = (unsigned char*)&Mult_Fact_Y;
		String_WriteEEPROM_ex(EE_MULT_FACT_Y, ptr, 4);
		ptr = (unsigned char*)&Mult_Fact_B;
		String_WriteEEPROM_ex(EE_MULT_FACT_B, ptr, 4);
    }
	else if(firsttime == 1)
	{
		putsUART1((unsigned int*)"\n\rNot First Time");
		/************System Time ******************/
        SystemTime.Hours = Char_ReadEEPROM_ex(eepromsysTimeHr);
		SystemTime.Minutes = Char_ReadEEPROM_ex(eepromsysTimeMin);
		SystemTime.Seconds = Char_ReadEEPROM_ex(eepromsysTimeSec);
		
       	/********* RTC Time Set ********************/
        Bits.RTCSetNewTime = Char_ReadEEPROM_ex(_RTCSetNewTime);
       	/********* UserReset   ********************/
        Bits.UserReset = Char_ReadEEPROM_ex(eepromUserReset);

	  	/*********  Key ****************************/
	  	String_ReadEEPROM_ex(LCD_KEY, ExstingPW, 4);
		ExstingPW[4] = '\0';

		No_of_Sifts			= Char_ReadEEPROM_ex(EE_NO_OF_SHIFT_SET);
		Operation_mode 		= Char_ReadEEPROM_ex(EE_OPR_MODE);
		
		Start_address = EE_ONTIME_HH_1;
		for(i=0; i<8; i++)
		{
			ONTime[i].Hours 	= Char_ReadEEPROM_ex(Start_address++);
			ONTime[i].Minutes 	= Char_ReadEEPROM_ex(Start_address++);
			OFFTime[i].Hours 	= Char_ReadEEPROM_ex(Start_address++);
			OFFTime[i].Minutes 	= Char_ReadEEPROM_ex(Start_address++);
		}
		FirstONTime.Value = ONTime[0].Value;
		FirstONTime.Hours 	= Char_ReadEEPROM_ex(EE_FIRST_ONTIME_HH);
		FirstONTime.Minutes 	= Char_ReadEEPROM_ex(EE_FIRST_ONTIME_MM);
		No_Of_ON_Hours.Hours 	= Char_ReadEEPROM_ex(EE_ON_HOURS_HH);
		No_Of_ON_Hours.Minutes 	= Char_ReadEEPROM_ex(EE_ON_HOURS_MM);
		No_Of_OFF_Hours.Hours 	= Char_ReadEEPROM_ex(EE_OFF_HOURS_HH);
		No_Of_OFF_Hours.Minutes = Char_ReadEEPROM_ex(EE_OFF_HOURS_MM);
		Manual_On_set 			= Char_ReadEEPROM_ex(EE_MAN_ON_SET);
		MANUAL_ONTime.Hours		= Char_ReadEEPROM_ex(EE_MAN_ONTIME_HH);
		MANUAL_ONTime.Minutes	= Char_ReadEEPROM_ex(EE_MAN_ONTIME_MM);
		MANUAL_ONTime.Seconds	= Char_ReadEEPROM_ex(EE_MAN_ONTIME_SS);
		RunHours.Hours		= Char_ReadEEPROM_ex(EE_RUN_HRS_HH);
		RunHours.Minutes	= Char_ReadEEPROM_ex(EE_RUN_HRS_MM);
		RunHours.Seconds	= Char_ReadEEPROM_ex(EE_RUN_HRS_SS);
		firsttime_Manual_off= Char_ReadEEPROM_ex(EE_FISRTMAN_OFF_SET);
		PREV_MAN_ONTime.Hours		= Char_ReadEEPROM_ex(EE_PREV_MAN_ON_TIME_HH);
		PREV_MAN_ONTime.Minutes		= Char_ReadEEPROM_ex(EE_PREV_MAN_ON_TIME_MM);
		PREV_MAN_ONTime.Seconds		= Char_ReadEEPROM_ex(EE_PREV_MAN_ON_TIME_SS);
		PREV_MAN_OFFTime.Hours		= Char_ReadEEPROM_ex(EE_PREV_MAN_OFF_TIME_HH);
		PREV_MAN_OFFTime.Minutes	= Char_ReadEEPROM_ex(EE_PREV_MAN_OFF_TIME_MM);
		PREV_MAN_OFFTime.Seconds	= Char_ReadEEPROM_ex(EE_PREV_MAN_OFF_TIME_SS);

		Prev_R_Phase_Status	= Char_ReadEEPROM_ex(PREV_R_PHASE_STATUS_);
		Prev_Y_Phase_Status	= Char_ReadEEPROM_ex(PREV_Y_PHASE_STATUS_);
		Prev_B_Phase_Status	= Char_ReadEEPROM_ex(PREV_B_PHASE_STATUS_);

		Load_Status	= Char_ReadEEPROM_ex(EE_LOAD_STATUS);

		powerfail = Char_ReadEEPROM_ex(EE_POWER_FAIL_STATE);
		Power_Fail.Hours	= Char_ReadEEPROM_ex(EE_PWR_FAIL_HH);
		Power_Fail.Minutes	= Char_ReadEEPROM_ex(EE_PWR_FAIL_MM);
		Power_Fail.Seconds	= Char_ReadEEPROM_ex(EE_PWR_FAIL_SS);
		Power_Resume.Hours	= Char_ReadEEPROM_ex(EE_PWR_RESUME_HH);
		Power_Resume.Minutes = Char_ReadEEPROM_ex(EE_PWR_RESUME_MM);
		Power_Resume.Seconds = Char_ReadEEPROM_ex(EE_PWR_RESUME_SS);

		ptr = (unsigned char*)&HV_Cut_Value;
		String_ReadEEPROM_ex(EE_HV_CUT_VALUE, ptr, 2);
		ptr = (unsigned char*)&LV_Cut_Value;
		String_ReadEEPROM_ex(EE_LV_CUT_VALUE, ptr, 2);
		Phase_Mode = Char_ReadEEPROM_ex(EE_PHASE_MODE);
		HLV_mode = Char_ReadEEPROM_ex( EE_HLV_MODE);
		
		ptr = (unsigned char*)&Mult_Fact_R;
		String_ReadEEPROM_ex(EE_MULT_FACT_R, ptr, 4);
		ptr = (unsigned char*)&Mult_Fact_Y;
		String_ReadEEPROM_ex(EE_MULT_FACT_Y, ptr, 4);
		ptr = (unsigned char*)&Mult_Fact_B;
		String_ReadEEPROM_ex(EE_MULT_FACT_B, ptr, 4);
		
	}								// End else if
	oneday.Hours = 24;
	oneday.Minutes = 0;
	oneday.Seconds = 0;
	Each_Auto_Shift_Time.Value = No_Of_ON_Hours.Value + No_Of_OFF_Hours.Value;
	CheckTimeBoundaries(&Each_Auto_Shift_Time);
//	PrintPresentStatus();
}									// End CheckForFirstTime()
	
void Make_all_values_zero(void)
{
	unsigned char i = 0;
	
	for(i=0; i<8;i++)
	{
		ONTime[i].Value = 0;
		OFFTime[i].Value = 0;
	}
	SystemTime.Res = 0;
	TimeOnRTC.Res = 0;
	FirstONTime.Value 		= 0;
	No_Of_OFF_Hours.Value 	= 0;
	MANUAL_ONTime.Value 	= 0;
	RunHours.Value 			= 0;
	PREV_MAN_ONTime.Value 	= 0;
	PREV_MAN_OFFTime.Value 	= 0;
	Auto_ONTime.Value 		= 0;
	Auto_OFFTime.Value 		= 0;
	Auto_Next_ONTime.Value 	= 0;
	Auto_Next_OFFTime.Value = 0;
	oneday.Value 			= 0;
	Each_Auto_Shift_Time.Value = 0;
	Auto_Next_ONTime.Value 	= 0;
	Auto_Next_OFFTime.Value = 0;
	Prev_OFF_Time.Value 	= 0;
	Power_Fail.Value 		= 0;
	Power_Resume.Value 		= 0;
}


unsigned char ONorOFF(Time on_time_, Time off_time_)
{	
	if(on_time_.Value == off_time_.Value)
		return 0;
		
	if(on_time_.Value < off_time_.Value)  							// On/Off are on the same day        
   	{
    	if( (SystemTime.Value >= on_time_.Value) && ( SystemTime.Value < off_time_.Value))
	   		return 1;
	   	else
	    	return 0;
	}
	else                              							// If On/Off are on different days 
	{
		if( (SystemTime.Value >= off_time_.Value) && ( SystemTime.Value < on_time_.Value))
	   		return 0;
	   	else
	   		return 1;
   }   
}


void Switch_OFF_FEEDER(void)
{
	L_ON = 0;								//	Ravi
	Latch_1 	= 	ENABLE;				//	Enable Latch
	Relay_1		=	LOW;
	Relay_2		=	LOW;
	Relay_3		=	LOW;
	TimerDelay_10ms(1);
	Latch_1		= 	DISABLE;				//	Disable Latch	
}

void static Switch_ON_Feeder(void)
{
	L_ON = 1;								//	Modified by Ravi
	
	Latch_1 	= 	ENABLE;				// Enable Latch
	Relay_1		=	HIGH;
	Relay_2		=	HIGH;
	Relay_3		=	HIGH;
	TimerDelay_10ms(1);
	Latch_1 	= 	DISABLE;				// Disable Latch
}

void static Multiple_Loads()
{
	if( (L_ON == 1) && (SystemTime.Seconds != temp_Secs))				//	 Added by Ravi
	{
		temp_Secs = SystemTime.Seconds;
		temp_cnt1++;
//		printf("\n\n\r temp_cnt1 = %d\n", temp_cnt1);		
		if(temp_cnt1>= Load_Interval)									// Load Interval defined in main.h
		{
			temp_cnt1 = 0;
			Load1_1(num++);
		}
		if(num > MAX_Loads)
		{
			L_ON = 0;
			num = 0;
			temp_Secs = 0;
			temp_cnt1 = 0;
		}
	}		
}

void Load1_1(unsigned char ln)
{
	printf("\n\n\r ln = %d\n", ln);
	switch(ln){	
		case 1:	
				Latch_1 	= 	ENABLE;				// Enable Latch
				Relay_1		=	HIGH;
				Relay_2		=	HIGH;
				Relay_3		=	DISABLE;
				TimerDelay_10ms(1);
				Latch_1 	= 	DISABLE;			// Disable Latch
				break;
		case 2: 
				Latch_1 	= 	ENABLE;				// Enable Latch
				Relay_1		=	HIGH;
				Relay_2		=	HIGH;
				Relay_3		=	HIGH;
				TimerDelay_10ms(1);
				Latch_1 	= 	DISABLE;			// Disable Latch				
				break;
		default: 
				Latch_1 	= 	ENABLE;				// Enable Latch
				Relay_1		=	HIGH;
				Relay_2		=	HIGH;
				Relay_3		=	HIGH;
				TimerDelay_10ms(1);
				Latch_1 	= 	DISABLE;				// Disable Latch
				break;
	}	
}

void InitAllPheriperals(void)
{
	EEPROM_WP_EN;									//	Enable EEPROM Write Protect 
	INIT_KEYPAD();
	INIT_XLCD();
	INIT_TIMER();
	INIT_UART1();	
	INIT_I2C();
	INIT_LED();
	INIT_ADC();
	INIT_RELAY();	
	LCD_StartUP_Screen();
    SystemTime.Value = 0;
	CheckForFirstTime();
	Read_RTC();
	Sync_with_RTCTime();
	resetmonitoring();
	putsUART1((unsigned int*)"\n\rSYSTEM READY");
	Show_Parameters();
}

void INIT_RELAY(void)
{
	DDR_LATCH_1	=	OUTPUT;
	DDR_Relay_1	=	OUTPUT;	
	DDR_Relay_2	=	OUTPUT;	
	DDR_Relay_3	=	OUTPUT;	
	DDR_Relay_4	=	OUTPUT;
	DDR_KEY_MANL = INPUT;
}

void Run_Hours_counter(void)
{
	if ( RunHours.Seconds < 59 )          	// is cummulative seconds < 59?
	{
		RunHours.Seconds++;               	// yes, so increment seconds
	}
	else                         				// else seconds => 59     
	{
		RunHours.Seconds = 0x00;          	// reset seconds
		if ( RunHours.Minutes < 59 )      	// is cummulative minutes < 59?
		{
			RunHours.Minutes++;           	// yes, so updates minutes  
		}
		else                     				// else minutes => 59
		{
			RunHours.Minutes = 0x00;      	// reset minutes
			if ( RunHours.Hours < 99 )    	// is cummulative hours < 23
			{
				RunHours.Hours ++;        	// Yes, so update hours
			}
			else
			{
				RunHours.Hours = 0x00;    	// Reset time
			}		  
		}
	}
	if(RunHours.Seconds == 0)				// save time in eeprom
	{
		Char_WriteEEPROM_ex(RunHours.Hours,  	EE_RUN_HRS_HH);
		Char_WriteEEPROM_ex(RunHours.Minutes,   EE_RUN_HRS_MM);
		Char_WriteEEPROM_ex(RunHours.Seconds,   EE_RUN_HRS_SS);
	}	 	
}

void CheckTimeBoundaries(Time *temp)
{
	unsigned char counter = 0;
	
	if(temp->Seconds > 59)
	{
		counter = temp->Seconds/60;
		temp->Seconds = temp->Seconds % 60;
		temp->Minutes += counter;
	}
	if(temp->Minutes > 59)
	{
		counter = temp->Minutes/60;
		temp->Minutes = temp->Minutes % 60;
		temp->Hours += counter;
	}
	if(temp->Hours > 23)
	{
		temp->Hours = temp->Hours % 24;
	}	
}

void Calc_No_Of_Auto_Shits_Cnts(void)
{
	Time temp;	
	temp.Value = 0;
	temp.Value = No_Of_ON_Hours.Value + No_Of_OFF_Hours.Value;
	CheckTimeBoundaries(&temp);
	Total_Auto_Shifts_Count = 86400/((unsigned long int)temp.Hours*60*60 + (unsigned long int)temp.Minutes*60 + (unsigned long int)temp.Seconds);
	putsUART1((unsigned int*)"\r\nTotal AUTO SHIFTS COUNT=");
	PCPutInt1(Total_Auto_Shifts_Count);
	WriteUART1('\n');
}

void Check_Present_Auto_Shift_Time(void)
{
	Time Completed;
	Time FirstOFFTime;
	unsigned int i = 0;
	unsigned long int sec_temp = 0;
	unsigned char next_value = 0;
	
	Completed.Value = 0;
	FirstOFFTime.Value = 0;
	
	CheckTimeBoundaries(&FirstOFFTime);
	if(SystemTime.Value >= FirstONTime.Value)			// on same day
	{
		sec_temp = ((unsigned long int)(SystemTime.Hours)*3600 + (unsigned long int)(SystemTime.Minutes)*60 + (unsigned long int)SystemTime.Seconds) - ((unsigned long int)(FirstONTime.Hours)*3600 + (unsigned long int)(FirstONTime.Minutes)*60 + (unsigned long int)FirstONTime.Seconds);
		Convert_Sec_To_Time(sec_temp, &Completed);
	}
	else												// on next day
	{
		sec_temp = ((unsigned long int)(FirstONTime.Hours)*3600 + (unsigned long int)(FirstONTime.Minutes)*60 + (unsigned long int)FirstONTime.Seconds) - ((unsigned long int)(SystemTime.Hours)*3600 + (unsigned long int)(SystemTime.Minutes)*60 + (unsigned long int)SystemTime.Seconds);
		Convert_Sec_To_Time(sec_temp, &Completed);
		sec_temp = 86400 - ((unsigned long int)(Completed.Hours)*3600 + (unsigned long int)(Completed.Minutes)*60 + (unsigned long int)Completed.Seconds);
		Convert_Sec_To_Time(sec_temp, &Completed);
	}
	Present_Auto_Shift_Count = (unsigned long int)((unsigned long int)Completed.Hours*3600+(unsigned long int)Completed.Minutes*60+(unsigned long int)Completed.Seconds)/(unsigned long int)((unsigned long int)Each_Auto_Shift_Time.Hours*3600+(unsigned long int)Each_Auto_Shift_Time.Minutes*60+(unsigned long int)Each_Auto_Shift_Time.Seconds);
	Auto_ONTime.Value = FirstONTime.Value;
	if(Present_Auto_Shift_Count == Total_Auto_Shifts_Count)
		Present_Auto_Shift_Count--;
	while(i < Present_Auto_Shift_Count)
	{
		i++;
		Auto_ONTime.Value += Each_Auto_Shift_Time.Value;
		CheckTimeBoundaries(&Auto_ONTime);
	}
	Auto_OFFTime.Value = Auto_ONTime.Value + No_Of_ON_Hours.Value;
	CheckTimeBoundaries(&Auto_OFFTime);
	
	if(i < (Total_Auto_Shifts_Count-1))
	{
		Auto_Next_ONTime.Value = Auto_ONTime.Value + Each_Auto_Shift_Time.Value;
		CheckTimeBoundaries(&Auto_Next_ONTime);
		Auto_Next_OFFTime.Value = Auto_Next_ONTime.Value + No_Of_ON_Hours.Value;
		CheckTimeBoundaries(&Auto_Next_OFFTime);
	}
	else
	{
		Auto_Next_ONTime.Value = FirstONTime.Value;
		Auto_Next_OFFTime.Value = Auto_Next_ONTime.Value + No_Of_ON_Hours.Value;
		CheckTimeBoundaries(&Auto_Next_OFFTime);
	}

	// calculation for when reached off time assigning next ontime to present ontime 	
	if(Auto_ONTime.Value < Auto_OFFTime.Value)  							// On/Off are on the same day        
   	{
    	if( (SystemTime.Value >= Auto_ONTime.Value) && ( SystemTime.Value < Auto_OFFTime.Value))
	   	{
		   	// do nothing because it is ON Time
		   	if(Present_Auto_Shift_Count != 0)
		   	{
			   	Prev_OFF_Time.Value = FirstONTime.Value + (Present_Auto_Shift_Count-1)*Each_Auto_Shift_Time.Value + No_Of_ON_Hours.Value;
				CheckTimeBoundaries(&Prev_OFF_Time);
			}
			else
			{
			   	Prev_OFF_Time.Value = FirstONTime.Value + (Total_Auto_Shifts_Count-1)*Each_Auto_Shift_Time.Value + No_Of_ON_Hours.Value;
				CheckTimeBoundaries(&Prev_OFF_Time);
			}		
	   	}	
	   	else		// off Time
	   	{
		   	Prev_OFF_Time.Value = Auto_OFFTime.Value;
		   	Auto_ONTime.Value = Auto_Next_ONTime.Value;
		   	Auto_OFFTime.Value = Auto_Next_OFFTime.Value;
		   	next_value = Present_Auto_Shift_Count+2;
		   	if(next_value >= Total_Auto_Shifts_Count)
		   		next_value = next_value - Total_Auto_Shifts_Count;
			Auto_Next_ONTime.Value = FirstONTime.Value + next_value * Each_Auto_Shift_Time.Value;
			CheckTimeBoundaries(&Auto_Next_ONTime);
			Auto_Next_OFFTime.Value = Auto_Next_ONTime.Value + No_Of_ON_Hours.Value;
			CheckTimeBoundaries(&Auto_Next_OFFTime);
		}
	}
	else                              							// If On/Off are on different days 
	{
		if( (SystemTime.Value >= Auto_OFFTime.Value) && ( SystemTime.Value < Auto_ONTime.Value))		// off time
		{
		   	Prev_OFF_Time.Value = Auto_OFFTime.Value;
		   	Auto_ONTime.Value = Auto_Next_ONTime.Value;
		   	Auto_OFFTime.Value = Auto_Next_OFFTime.Value;
		   	next_value = Present_Auto_Shift_Count+2;
		   	if(next_value >= Total_Auto_Shifts_Count)
		   		next_value = next_value - Total_Auto_Shifts_Count;
			Auto_Next_ONTime.Value = FirstONTime.Value + next_value * Each_Auto_Shift_Time.Value;
			CheckTimeBoundaries(&Auto_Next_ONTime);
			Auto_Next_OFFTime.Value = Auto_Next_ONTime.Value + No_Of_ON_Hours.Value;
			CheckTimeBoundaries(&Auto_Next_OFFTime);
		}
	   	else
	   	{
		   	// do nothing because it is ON Time
		   	if(Present_Auto_Shift_Count != 0)
		   	{
			   	Prev_OFF_Time.Value = FirstONTime.Value + (Present_Auto_Shift_Count-1)*Each_Auto_Shift_Time.Value + No_Of_ON_Hours.Value;
				CheckTimeBoundaries(&Prev_OFF_Time);
			}
			else
			{
			   	Prev_OFF_Time.Value = FirstONTime.Value + (Total_Auto_Shifts_Count-1)*Each_Auto_Shift_Time.Value + No_Of_ON_Hours.Value;
				CheckTimeBoundaries(&Prev_OFF_Time);
			}		
	   	}	
   }

	putsUART1((unsigned int*)"\nPresent_Auto_Shit_Count=");
	PCPutInt1(Present_Auto_Shift_Count);
	WriteUART1('\n');
//	Show_Parameters();
}

void Convert_Sec_To_Time(unsigned long int sec, Time *temp)
{
	temp->Value =0;
	temp->Hours = sec/3600;
	temp->Minutes = (sec%3600)/60;
	temp->Seconds = (sec%3600)%60;
}

void Auto_functionality(void)
{
	if(Every_Minute == 1)
	{
		Every_Minute = 0;
		Check_Present_Auto_Shift_Time();
		if(ONorOFF(Auto_ONTime, Auto_OFFTime))
		{
			if( (Load_Status == OFF) && (Pres_RYB_Phase_Status == ON) && ( (HLV_mode==DISABLE) || ((Pres_RYB_Phase_Status==ON) && (HLV_mode==ENABLE) && (RYB_PH2PH_Volt>LV_Cut_Value) && (RYB_PH2PH_Volt<HV_Cut_Value)) ) )
			{
				Load_Status = ON;
				Char_WriteEEPROM_ex(Load_Status,  EE_LOAD_STATUS);
				Switch_ON_Feeder();
			}
		}
		else
		{
			if(Load_Status == ON)
			{
				Load_Status = OFF;
				Char_WriteEEPROM_ex(Load_Status,  EE_LOAD_STATUS);
				Switch_OFF_FEEDER();
			}
		}	
	}	
}

void Auto_Plus_functionality(void)
{
	if(Every_Minute == 1)
	{
		Every_Minute = 0;
		Decide_AutoPlus_Shift_Position();
		Auto_Plus_Load_Control();
		PrintPresentStatus();
	}	
}	

void Decide_AutoPlus_Shift_Position(void)
{
	unsigned char Next_Shift_count = 0;
	unsigned char Prev_shift_count = 0;
	unsigned char shift_count = 0;
	while(shift_count < No_of_Sifts)
	{
		if(ONorOFF(ONTime[shift_count], OFFTime[shift_count]))
		{
			Auto_Plus_load = 1;
			break;
		}	
		shift_count++;
	}
	if(shift_count == No_of_Sifts)
	{
		shift_count = 0;
		while(shift_count < No_of_Sifts)
		{
			Next_Shift_count = shift_count + 1;
			if(Next_Shift_count >= No_of_Sifts)
				Next_Shift_count = 0;
			if(ONorOFF(OFFTime[shift_count], ONTime[Next_Shift_count]))
			{
				Auto_Plus_load = 0;
				shift_count++;
				Next_Shift_count++;
				break;
			}	
			shift_count++;
		}
	}
	else
	{
		Next_Shift_count = shift_count + 1;
	}	
	if(shift_count >= No_of_Sifts)
		shift_count = 0;
		
	if(Next_Shift_count >= No_of_Sifts)
		Next_Shift_count = 0;

	if(shift_count == 0)
		Prev_shift_count = No_of_Sifts - 1;
	else Prev_shift_count = shift_count-1;

	Present_Auto_Plus_shift_count = shift_count;
	Auto_ONTime.Value = ONTime[shift_count].Value;
	Auto_OFFTime.Value = OFFTime[shift_count].Value;
	Auto_Next_ONTime.Value = ONTime[Next_Shift_count].Value;
	Auto_Next_OFFTime.Value = OFFTime[Next_Shift_count].Value;
	Prev_OFF_Time.Value = OFFTime[Prev_shift_count].Value;
//	Show_Parameters();
}

void Auto_Plus_Load_Control(void)
{
	if(Auto_Plus_load == 1)
	{
		if( (Load_Status == OFF) && (Pres_RYB_Phase_Status == ON) && ( (HLV_mode==DISABLE) || ((Pres_RYB_Phase_Status==ON) && (HLV_mode==ENABLE) && (RYB_PH2PH_Volt>LV_Cut_Value) && (RYB_PH2PH_Volt<HV_Cut_Value)) ) )
		{
			Load_Status = ON;
			Char_WriteEEPROM_ex(Load_Status,  EE_LOAD_STATUS);
			Switch_ON_Feeder();
		}
	}
	else
	{
		if(Load_Status == ON)
		{
			Load_Status = OFF;
			Char_WriteEEPROM_ex(Load_Status,  EE_LOAD_STATUS);
			Switch_OFF_FEEDER();
		}
	}	
}

void Phase_Monitor(void)
{
	if(R_PHASE && Y_PHASE && B_PHASE)
		Pres_RYB_Phase_Status = ON;
	else
		Pres_RYB_Phase_Status = OFF;
	
	if( ( (R_PHASE == OFF) || (Y_PHASE == OFF) || (B_PHASE == OFF) ) && (powerfail == FALSE) )
	{
		putsUART1((unsigned int*)"\nPhase State Change-OFF");
		Power_Fail.Value = SystemTime.Value;
		powerfail = TRUE;
		Char_WriteEEPROM_ex(powerfail,	EE_POWER_FAIL_STATE);
		Char_WriteEEPROM_ex(Power_Fail.Hours,	EE_PWR_FAIL_HH);
		Char_WriteEEPROM_ex(Power_Fail.Minutes,	EE_PWR_FAIL_MM);
		Char_WriteEEPROM_ex(Power_Fail.Seconds,	EE_PWR_FAIL_SS);
	}
	else if( ( (R_PHASE == ON) && (Y_PHASE == ON) && (B_PHASE == ON) ) && (powerfail == TRUE) )
	{
		putsUART1((unsigned int*)"\nPhase State Change-ON");
		Power_Resume.Value = SystemTime.Value;
		powerfail = FALSE;
		Char_WriteEEPROM_ex(powerfail,	EE_POWER_FAIL_STATE);
		Char_WriteEEPROM_ex(Power_Resume.Hours,		EE_PWR_RESUME_HH);
		Char_WriteEEPROM_ex(Power_Resume.Minutes,	EE_PWR_RESUME_MM);
		Char_WriteEEPROM_ex(Power_Resume.Seconds,	EE_PWR_RESUME_SS);
	}	
	if(R_PHASE != Prev_R_Phase_Status)
	{
		Prev_R_Phase_Status = R_PHASE;
		Char_WriteEEPROM_ex(Prev_R_Phase_Status,   PREV_R_PHASE_STATUS_);
	}	
	if(Y_PHASE != Prev_Y_Phase_Status)
	{
		Prev_Y_Phase_Status = Y_PHASE;
		Char_WriteEEPROM_ex(Prev_Y_Phase_Status,   PREV_Y_PHASE_STATUS_);
	}	
	if(B_PHASE != Prev_B_Phase_Status)
	{
		Prev_B_Phase_Status = B_PHASE;
		Char_WriteEEPROM_ex(Prev_B_Phase_Status,   PREV_B_PHASE_STATUS_);
	}	
}

void Decide_RYB_Sensors(void)
{
	RYB_PH2PH_Volt = ((R_Phase_Volt + Y_Phase_Volt + B_Phase_Volt)/3.0)*1.732;
	#ifdef HWV2
		#ifdef RYB_VOLT_SENSOR_MODE
			if(R_Phase_Volt < R_PH_MIN_SEN_VOLT)
				R_PHASE = 0;
			else
				R_PHASE = 1;
			if(Y_Phase_Volt < Y_PH_MIN_SEN_VOLT)
				Y_PHASE = 0;
			else
				Y_PHASE = 1;
			if(B_Phase_Volt < B_PH_MIN_SEN_VOLT)
				B_PHASE = 0;
			else
				B_PHASE = 1;
		#else
			R_PHASE = 1;
			Y_PHASE = 1;
			B_PHASE = 1;

		#endif
	#else
		R_PHASE = 1;
		Y_PHASE = 1;
		B_PHASE = 1;
	#endif
}	

void PrintPresentStatus(void)
{
	printf("\r\nLOAD STATUS=%d\nR_PHASE=%d\nY_PHASE=%d\nB_PHASE=%d\r\nPres_RYB_Phase_Status%d\nHLV_mode=%d\n",Load_Status,R_PHASE,Y_PHASE,B_PHASE,Pres_RYB_Phase_Status,HLV_mode);
}

void Show_Parameters(void)
{
	printf("\r\nSystem Time:%02d:%02d:%02d",SystemTime.Hours, SystemTime.Minutes, SystemTime.Seconds);
	printf("\r\nR:%3.3f, B:%3.3f, Y:%3.3f\n",(double)R_Phase_Volt, (double)B_Phase_Volt, (double)Y_Phase_Volt);
	printf("\r\nAuto_ONTime      = %02d:%02d:%02d", Auto_ONTime.Hours, Auto_ONTime.Minutes, Auto_ONTime.Seconds);
	printf("\nAuto_OFFTime     = %02d:%02d:%02d",Auto_OFFTime.Hours, Auto_OFFTime.Minutes, Auto_OFFTime.Seconds);
	printf("\nAuto_Next_ONTime = %02d:%02d:%02d",Auto_Next_ONTime.Hours, Auto_Next_ONTime.Minutes, Auto_Next_ONTime.Seconds);
	printf("\nAuto_Next_OFFTime= %02d:%02d:%02d",Auto_Next_OFFTime.Hours, Auto_Next_OFFTime.Minutes, Auto_Next_OFFTime.Seconds);
	printf("\nPREV_OFFTime     = %02d:%02d:%02d",Prev_OFF_Time.Hours, Prev_OFF_Time.Minutes, Prev_OFF_Time.Seconds);
	
	printf("\r\nReset Values:\nTRAP : %03d", Char_ReadEEPROM_ex(TRAP_val));
	printf("\nIOPU : %03d", Char_ReadEEPROM_ex(IOPUWR_val));
	printf("\nEXTR : %03d", Char_ReadEEPROM_ex(EXTR_val));
	printf("\nSWDT : %03d", Char_ReadEEPROM_ex(SWDT_val));
	printf("\nWDTO : %03d", Char_ReadEEPROM_ex(WDTO_val));
	printf("\nBOR  : %03d", Char_ReadEEPROM_ex(BOR_val));
	printf("\nPOR  : %03d", Char_ReadEEPROM_ex(POR_val));
	
	PrintPresentStatus();
}

void resetmonitoring(void)
{
	unsigned char j = 0;			
	if(RCONbits.BOR && RCONbits.POR == 0)			// Brown Out Reset
	{
		j = Char_ReadEEPROM_ex(BOR_val);
		j++;
		Char_WriteEEPROM_ex(j,BOR_val);
		RCONbits.BOR = 0;	
	}
	if(RCONbits.POR && RCONbits.EXTR)				// Power on Reset, when power on reset occurs clear all reset flags.
	{
		j = Char_ReadEEPROM_ex(POR_val);
		j++;
		Char_WriteEEPROM_ex(j,POR_val);
		RCONbits.POR  = 0;
		RCONbits.BOR  = 0;
		RCONbits.EXTR = 0;	
	}
	else if(RCONbits.POR == 0 &&RCONbits.EXTR)		// MCLR pin reset
	{
		j = Char_ReadEEPROM_ex(EXTR_val);
		j++;
		Char_WriteEEPROM_ex(j,EXTR_val);
		RCONbits.POR = 0;
		RCONbits.BOR = 0;
		RCONbits.EXTR = 0;
	}
	if(RCONbits.SWDTEN)								// Software WDT Enable is not cleared.
	{											
		j = Char_ReadEEPROM_ex(SWDT_val);
		j++;
		Char_WriteEEPROM_ex(j,SWDT_val);
	}
	if(RCONbits.WDTO)								// WatchDog Timer TimeOut Reset
	{												
		j = Char_ReadEEPROM_ex(WDTO_val);
		j++;
		Char_WriteEEPROM_ex(j,WDTO_val);
		RCONbits.WDTO = 0;	
	}
	
	if(RCONbits.TRAPR)								// Trap Conflict detected.
	{
		j = Char_ReadEEPROM_ex(TRAP_val);
		j++;
		Char_WriteEEPROM_ex(j,TRAP_val);
		RCONbits.TRAPR = 0;
	}	
	if(RCONbits.IOPUWR)								// Illegal Opcode detected.
	{
		j = Char_ReadEEPROM_ex(IOPUWR_val);
		j++;
		Char_WriteEEPROM_ex(j,IOPUWR_val);
		RCONbits.TRAPR = 0;
	}	
}
