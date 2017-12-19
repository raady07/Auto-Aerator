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
 #ifndef adcMUX
	#define adcMUX

	#include"uart.h"
	#include<math.h>


	#define OPV	5.03					// Operating voltage.
	#define dcoffset13 2050.0			// DC Offset voltage signint RB13.
	#define dcoffset14 2049.0			// DC Offset voltage signint RB14.
	#define dcoffset15 2049.0			// DC Offset voltage signint RB15.
	#define gain 	244.444				// Default Gain
	#define gain_R 244.444				// Operational gain
	#define gain_Y 244.444				// Operational gain
	#define gain_B 244.444				// Operational gain
	#define NO_OF_SAMPLES	128
	#define NO_OF_AVG_SAMPLES	16
	#define mulfac_R	0.3	//((gain_R*OPV)/4096.0)
	#define mulfac_Y	0.3	//((gain_Y*OPV)/4096.0)
	#define mulfac_B	0.3	//((gain_B*OPV)/4096.0)
	#define	mulfac	0.00122

	void INIT_ADC(void);
	void adcdisplay(unsigned long int calval1,unsigned long int calval2,unsigned long int calval3);
#endif

