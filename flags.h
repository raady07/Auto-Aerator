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
typedef struct {
	unsigned _T_ipset:1;
	unsigned RTCSetNewTime:1;
	unsigned PassWord:1;
	unsigned UserReset:1;
	unsigned PCDEBUG:1;
	unsigned PWRUP:1;
	unsigned HostTaskShift:1;
//	unsigned TimeToConnMstr:1;
	unsigned En_Display_Lcd:1;
} FlowControl;

typedef struct {
	unsigned Z0:1;
	unsigned Z1:1;
	unsigned Z2:1;
	unsigned Z3:1;
	unsigned Z4:1;
	unsigned Z5:1;
	unsigned Z6:1;
	unsigned Z7:1;
	unsigned Z8:1;
	unsigned Z9:1;
	unsigned Z10:1;
	unsigned Z11:1;
	unsigned Z12:1;
	unsigned Z13:1;
	unsigned Z14:1;
	unsigned Z15:1;
	unsigned Z16:1;
	unsigned Z17:1;
	unsigned Z18:1;
	unsigned Z19:1;
} LCDflags;	

