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

void MenuScreens(void);
void Master_MenuScreens(void);
void MenuFunction( void );
void CustomFunction( void );
void CustomScreens( void );
void Master_CustomScreens( void );
void SetTime( void );
void SetHardResetFunction( void );
void ErrorScreen(void );
void PasswordEnter( unsigned char PswdType );
void AutoSettings(void);
void SetONOFFTime(unsigned char WHAT_TIME );
void SetNoOfShits(void);
void SetOperationMode(void);
void SetPwrFailAdjust(void);
void SetDate(void);
void ShowPowerLog(void);
void Auto_Plus_Settings(void);
void SaveShiftSettings(void);
unsigned char ValidateTime(void);
void WrongValues_errorScreen(unsigned char);
void AssignTimingValues(void);
void PrintShiftTimings(void);
void Change_HLV_Cut(void);
void Calibration(void);
void SetLoadDelay(void);
void ResetMenuPassword(void);
#define	KP_OLD
#define	KEY_LOCK

#define KeyPadPort 		PORTB
#define KeyPadDir  		TRISB


extern unsigned char ExstingPW[5] ;
extern unsigned char No_of_Sifts;
extern unsigned char process_lcd;
extern unsigned char CursorPos;
extern unsigned char shifts_count;
extern unsigned char HLV_mode;
extern float Mult_Fact_R, Mult_Fact_Y, Mult_Fact_B;


//(Xaxis != NoOfShifts) && 
