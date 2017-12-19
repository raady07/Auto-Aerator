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

/*
**Note 3:
Fcy 	=	Fosc/4	= (Source Osc freq * PLL mul)/(program post scalar *4)
Tcy 	= 	1/ Fcy
A minimum of 333.33 ns is required for TAD,
		TAD 	=	(Tcy(ADCS + 1))/2
	from this 
		ADCS 	=	(2*TAD/Tcy) - 1
		
		------------------------------------------------------
		Fosc				ADCS		ADCON3bits.ADCS		ADCON3bits.SAMC
		XT 					 4.3328			000000			15 ** once 15TAD is enough
		XT PLL \w  4x		20.3328			000100			15 ** once 15TAD is enough
		XT PLL \w  8x		41.3328 		010100			15 ** once 2 15TAD required
		XT PLL \w 16x		84.3248 		101010			 
*/

#include"INIT.h"
#include"adcMUX.h"

unsigned short int adcflag = 1,j=0, i = 0; 
signed int adc_temp1,adc_temp2,adc_temp3;
unsigned long int calval1=0,calval2=0,calval3=0;
float R_Phase_Volt=0,Y_Phase_Volt=0,B_Phase_Volt=0;

unsigned long int samples_buff[16];
unsigned long int Calib_RMS_R_ph = 0,Calib_RMS_Y_ph = 0, Calib_RMS_B_ph = 0;
extern float Mult_Fact_R, Mult_Fact_Y, Mult_Fact_B;
unsigned char samples_done = 0;
float adcbufval_13 = 0,adcbufval_14 = 0,adcbufval_15 = 0,adcbufval_13_1 = 0,adcbufval_14_1 = 0,adcbufval_15_1 = 0;

void INIT_ADC(void){
	
	TRISBbits.TRISB13 = 1; 				// AN13 is used - RB13 CH0
	TRISBbits.TRISB14 = 1; 				// AN14 is used - RB14 CH0
	TRISBbits.TRISB15 = 1; 				// AN15 is used - RB15 CH0

	// ADCON1
	ADCON1bits.ADON 	= 0; 			// turn off ADC
	ADCON1bits.ADSIDL	= 0;			// Continue in idle mode.
	ADCON1bits.FORM		= 0;			// integer
	ADCON1bits.SSRC		= 2;			// (0 - Manual, 2 - Timer3) End Sampling
	ADCON1bits.ASAM		= 1;			// Auto sampling off -0 / on -1	
	
	// ADCON2 
	ADCON2bits.VCFG		= 0;			// VRef H - AVDD , L - AVss  000
	ADCON2bits.CSCNA	= 1;			// No Scan for MUX A setting
	ADCON2bits.SMPI		= 2;			// Interrupt at end of each sample		
	ADCON2bits.BUFM		= 0;			// buffer 16 word 
	ADCON2bits.ALTS 	= 0;			// MUX A input mux setting
	
	// ADCON3
	ADCON3bits.SAMC		= 16;			// ** Note 3
	ADCON3bits.ADRC		= 0;			// Clock source - internal
	ADCON3bits.ADCS		= 0b010100;		// ** Note 3
		
	// ADCHS 
	ADCHSbits.CH0NB		= 0;
	ADCHSbits.CH0SB		= 0;			// connecting to CH0 AN15
	ADCHSbits.CH0NA		= 0;			// ch0 -ve i/p -VRef.
	ADCHSbits.CH0SA		= 0;			// connecting to CH0 AN14
	
	// ADPCFG 
	ADPCFG				= 0x1FFF;		// Selecting inputs AN13,14,15

	// ADCSSL
	ADCSSL				= 0xE000;		// Scan Select AN13,14,15.
	
	IEC0bits.ADIE = 1;					// ADC interrupt is enabled.
    ADCON1bits.ADON		= 1;			// Turn on ADC Module.
}


void __attribute__((__interrupt__,auto_psv)) _T3Interrupt(void)
{
	if(adcflag == 0 )
	{
		adcflag=1;
		IFS0bits.ADIF = 0;
	}
	IFS0bits.T3IF = 0; //Clear Timer3 interrupt flag
}

void __attribute__((__interrupt__,no_auto_psv)) _ADCInterrupt(void)
{
	adc_temp1 = ((signed int)ADCBUF0 - dcoffset13);
	adc_temp2 = ((signed int)ADCBUF1 - dcoffset14);
	adc_temp3 = ((signed int)ADCBUF2 - dcoffset15);
	//printf("adc values %d %d %d\n", adc_temp1, adc_temp2, adc_temp3);
	calval1 += pow(abs(adc_temp1),2);
	calval2 += pow(abs(adc_temp2),2);
	calval3 += pow(abs(adc_temp3),2);
	i++;
	if(i>(NO_OF_SAMPLES-1))
	{
		i = 0;
		adcdisplay(calval1,calval2,calval3);
		calval1 = 0;
		calval2 = 0;
		calval3 = 0;
	}	
	adcflag = 0;
}

void adcdisplay(unsigned long int calval1,unsigned long int calval2,unsigned long int calval3)
{
	Calib_RMS_R_ph 	= adcbufval_13	=	sqrt(calval1/NO_OF_SAMPLES);
	Calib_RMS_Y_ph 	= adcbufval_14	=	sqrt(calval2/NO_OF_SAMPLES);
	Calib_RMS_B_ph 	= adcbufval_15	=	sqrt(calval3/NO_OF_SAMPLES);
	adcbufval_13 	= ((float)adcbufval_13*Mult_Fact_R);
	adcbufval_14 	= ((float)adcbufval_14*Mult_Fact_Y);
	adcbufval_15 	= ((float)adcbufval_15*Mult_Fact_B);	
	adcbufval_13_1	+=	adcbufval_13;
	adcbufval_14_1	+=	adcbufval_14;
	adcbufval_15_1	+=	adcbufval_15;
	j++;
	if(j>(NO_OF_AVG_SAMPLES-1))
	{
		j = 0;
		R_Phase_Volt = (float)adcbufval_13_1/NO_OF_AVG_SAMPLES;
		Y_Phase_Volt = (float)adcbufval_14_1/NO_OF_AVG_SAMPLES;
		B_Phase_Volt = (float)adcbufval_15_1/NO_OF_AVG_SAMPLES;
		//printf("R:%f, B:%f, Y:%f\n",R_Phase_Volt, B_Phase_Volt, Y_Phase_Volt);
		adcbufval_13_1=0;
		adcbufval_14_1=0;
		adcbufval_15_1=0;
		samples_done = 1;
	}		
}
