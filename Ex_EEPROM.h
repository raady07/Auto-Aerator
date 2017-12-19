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
#define isNotFirstTime    0x0000
#define _RTCSetNewTime    0x0001
#define eepromsysTimeHr   0x0002
#define eepromsysTimeMin  0x0003
#define eepromsysTimeSec  0x0004
#define eepromUserReset   0x0006
#define eepromSysTeset    0x0007
#define LCD_KEY			  0x0008	// - 0x000b	// 4 bytes

#define	EE_OPR_MODE			0x0014		// auto+ / auto / manual mode address
#define EE_ONTIME_HH_1		0x0015
#define	EE_ONTIME_MM_1		0x0016
#define	EE_OFFTIME_HH_1		0x0017
#define	EE_OFFTIME_MM_1		0x0018
#define EE_ONTIME_HH_2		0x0019
#define	EE_ONTIME_MM_2		0x001A
#define	EE_OFFTIME_HH_2		0x001B
#define	EE_OFFTIME_MM_2		0x001C
#define EE_ONTIME_HH_3		0x001D
#define	EE_ONTIME_MM_3		0x001E
#define	EE_OFFTIME_HH_3		0x001F
#define	EE_OFFTIME_MM_3		0x0020
#define EE_ONTIME_HH_4		0x0021
#define	EE_ONTIME_MM_4		0x0022
#define	EE_OFFTIME_HH_4		0x0023
#define	EE_OFFTIME_MM_4		0x0024
#define EE_ONTIME_HH_5		0x0025
#define	EE_ONTIME_MM_5		0x0026
#define	EE_OFFTIME_HH_5		0x0027
#define	EE_OFFTIME_MM_5		0x0028
#define EE_ONTIME_HH_6		0x0029
#define	EE_ONTIME_MM_6		0x002A
#define	EE_OFFTIME_HH_6		0x002B
#define	EE_OFFTIME_MM_6		0x002C
#define EE_ONTIME_HH_7		0x002D
#define	EE_ONTIME_MM_7		0x002E
#define	EE_OFFTIME_HH_7		0x002F
#define	EE_OFFTIME_MM_7		0x0030
#define EE_ONTIME_HH_8		0x0031
#define	EE_ONTIME_MM_8		0x0032
#define	EE_OFFTIME_HH_8		0x0033
#define	EE_OFFTIME_MM_8		0x0034

//#define EE_OPR_MODE			0x0035
#define EE_FIRST_ONTIME_HH	0x0036
#define EE_FIRST_ONTIME_MM	0X0037
#define EE_FIRST_OFFTIME_HH	0x0038
#define EE_ON_HOURS_HH		0x0039
#define EE_ON_HOURS_MM		0x003A
#define EE_OFF_HOURS_HH		0x003B
#define EE_OFF_HOURS_MM		0x003C

#define EE_NO_OF_SHIFT_SET	0x003D

#define EE_MAN_ONTIME_HH	0x003E
#define EE_MAN_ONTIME_MM	0x003F
#define EE_MAN_ONTIME_SS	0x0040
#define EE_MAN_ON_SET		0x0041
#define EE_RUN_HRS_HH		0x0042
#define EE_RUN_HRS_MM		0x0043
#define EE_RUN_HRS_SS		0x0044
#define EE_FISRTMAN_OFF_SET	0x0045
#define EE_PREV_MAN_ON_TIME_HH	0x0046
#define EE_PREV_MAN_ON_TIME_MM	0x0047
#define EE_PREV_MAN_ON_TIME_SS	0x0048
#define EE_PREV_MAN_OFF_TIME_HH	0x0049
#define EE_PREV_MAN_OFF_TIME_MM	0x004A
#define EE_PREV_MAN_OFF_TIME_SS	0x004B

#define EE_AUTO_ONTIME_HH	0x004C
#define EE_AUTO_ONTIME_MM	0x004D
#define EE_AUTO_ONTIME_SS	0x004E
#define EE_AUTO_OFFTIME_HH	0x004F
#define EE_AUTO_OFFTIME_MM	0x0050
#define EE_AUTO_OFFTIME_SS	0x0051

#define EE_LOAD_STATUS		0x0052
#define EE_POWER_FAIL_STATE			0x0053
#define EE_PWR_FAIL_HH				0x0054
#define	EE_PWR_FAIL_MM				0x0055
#define	EE_PWR_FAIL_SS				0x0056
#define EE_PWR_RESUME_HH			0x0057
#define	EE_PWR_RESUME_MM			0x0058
#define	EE_PWR_RESUME_SS			0x0059

#define	PREV_R_PHASE_STATUS_		0x005A
#define	PREV_Y_PHASE_STATUS_		0x005B
#define	PREV_B_PHASE_STATUS_		0x005C
#define EE_HLV_MODE					0x005D

#define EE_HV_CUT_VALUE				0x005E		// 2bytes
#define EE_LV_CUT_VALUE				0x0060		// 2bytes
#define EE_PHASE_MODE				0x0062

#define EE_MULT_FACT_R				0x0063		// 4bytes
#define EE_MULT_FACT_Y				0x0067		// 4bytes
#define EE_MULT_FACT_B				0x006B		// 4bytes

//#define	EE_FEEDER_ON		0x001F
//#define	EE_Count_ON_Cycles	0x0020
//#define	EE_TCF_KGS_BYTE1	0x0021
//#define	EE_TCF_KGS_BYTE2	0x0022
//#define	EE_TCF_KGS_BYTE3	0x0023
//#define	EE_TCF_KGS_BYTE4	0x0024
//#define	EE_FEED_ERROR		0x0025
//#define EE_FEED_COMPLETE	0x0026
#define EE_LOAD_DELAY		0x006F

#define	EE_Compile_Len		0x0073
#define	EE_Compile_Date		0x0094

#define TRAP_val			0x00A8		// Reset condition storing values.
#define IOPUWR_val			0x00A9
#define EXTR_val			0x00AA
#define SWDT_val			0x00AB
#define WDTO_val			0x00AC
#define BOR_val				0x00AD
#define POR_val				0x00AE

