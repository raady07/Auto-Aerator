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
#include"timer.h"
#define	MANUAL					0
#define	AUTO					1
#define	AUTO_PLUS				2

#define	R_PH_MIN_SEN_VOLT		10
#define	Y_PH_MIN_SEN_VOLT		10
#define	B_PH_MIN_SEN_VOLT		10

#define	DFLT_SYS_MODE			AUTO_PLUS			// 1-auto, 0-manual
#define DFLT_FIRST_ONTIME_HH	7					//	Auto Mode First ON Time Hours
#define DFLT_FIRST_ONTIME_MM	0
#define DFLT_ON_HOURS_HH		3					//	Auto Mode RUN Hours	
#define DFLT_ON_HOURS_MM		0
#define DFLT_OFF_HOURS_HH		2					//	Auto Mode OFF Hours
#define DFLT_OFF_HOURS_MM		30

#define DFLT_NO_OF_SHIFTS		5

#define DFLT_ONTIME_HH_1		7
#define DFLT_ONTIME_MM_1		0
#define	DFLT_OFFTIME_HH_1		9
#define	DFLT_OFFTIME_MM_1		0
#define DFLT_ONTIME_HH_2		10
#define	DFLT_ONTIME_MM_2		0
#define	DFLT_OFFTIME_HH_2		12
#define	DFLT_OFFTIME_MM_2		0
#define DFLT_ONTIME_HH_3		13
#define	DFLT_ONTIME_MM_3		0
#define	DFLT_OFFTIME_HH_3		15
#define	DFLT_OFFTIME_MM_3		0
#define DFLT_ONTIME_HH_4		16
#define	DFLT_ONTIME_MM_4		0
#define	DFLT_OFFTIME_HH_4		18
#define	DFLT_OFFTIME_MM_4		0
#define DFLT_ONTIME_HH_5		19
#define	DFLT_ONTIME_MM_5		0
#define	DFLT_OFFTIME_HH_5		6
#define	DFLT_OFFTIME_MM_5		0
#define DFLT_ONTIME_HH_6		0
#define	DFLT_ONTIME_MM_6		0
#define	DFLT_OFFTIME_HH_6		0
#define	DFLT_OFFTIME_MM_6		0
#define DFLT_ONTIME_HH_7		0
#define	DFLT_ONTIME_MM_7		0
#define	DFLT_OFFTIME_HH_7		0
#define	DFLT_OFFTIME_MM_7		0
#define DFLT_ONTIME_HH_8		0
#define	DFLT_ONTIME_MM_8		0
#define	DFLT_OFFTIME_HH_8		0
#define	DFLT_OFFTIME_MM_8		0

#define MAX_Loads				4
#define Load_Interval			10
#define	PH3_MAX_HVCUT			570
#define PH3_MIN_HVCUT			400
#define	PH1_MAX_HVCUT			300
#define PH1_MIN_HVCUT			240

#define	PH3_MAX_LVCUT			330
#define PH3_MIN_LVCUT			280
#define	PH1_MAX_LVCUT			220
#define PH1_MIN_LVCUT			160

#define	DFLT_HV_CUT				440
#define DFLT_LV_CUT				290

#define THREE_PHASE				0
#define SINGLE_PHASE			1
#define DFLT_PHASE				THREE_PHASE		// 0 for 3PHASE, 1 for 1PHASE
#define DFLT_HLV_MODE			ENABLE

extern unsigned char ExstingPW[5];
extern unsigned char system_mode;			// system in auto or manual
extern unsigned char manual_mode_on;
extern unsigned char NoOfShifts;
extern unsigned char Manual_On_set;
extern unsigned char Operation_mode;
extern unsigned char Load_Status;


extern unsigned int HV_Cut_Value;
extern unsigned int LV_Cut_Value;
extern unsigned char Phase_Mode;
extern unsigned char R_PHASE, Y_PHASE, B_PHASE;

//void Print_RTC(void);
//void Print_System_Time(void);
void Sync_with_RTCTime(void);
void CheckForFirstTime(void);
void Run_Hours_counter(void);
void Make_all_values_zero(void);
unsigned char ONorOFF(Time on_time_, Time off_time_);
void Switch_OFF_FEEDER(void);
void Load1_1(unsigned char ln);
void InitAllPheriperals(void);
void INIT_RELAY(void);
void Run_Hours_counter(void);
void CheckTimeBoundaries(Time *temp);
void Calc_No_Of_Auto_Shits_Cnts(void);
void Check_Present_Auto_Shift_Time(void);
void Convert_Sec_To_Time(unsigned long int sec, Time *temp);
void Auto_functionality(void);
void Auto_Plus_functionality(void);
void Decide_AutoPlus_Shift_Position(void);
void Auto_Plus_Load_Control(void);
void Phase_Monitor(void);
void Decide_RYB_Sensors(void);
void PrintPresentStatus(void);
void Show_Parameters(void);

void resetmonitoring(void);
