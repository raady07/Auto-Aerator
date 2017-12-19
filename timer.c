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
#include"flags.h"
#include"INIT.h"
#include"main.h"
#include"timer.h"
#include"RTC.h"

unsigned char ten_ms_cnt = 0, blink_count = 0;
unsigned char everysec = 0, Every_Minute = 0;
unsigned char lcd_cont = 0;
unsigned char TimerDelay_flag_10ms = 0;

extern unsigned char Operation_mode;
extern unsigned char Manual_key_hold_count, Manual_On_set;
extern unsigned char Error_Mode;
extern unsigned char L_ON;
extern unsigned char everysec;
extern FlowControl Bits;

extern Time SystemTime;


void  INIT_TIMER(void)
{
	T1CON			= 0; 					// Disable RTC Timer and Reset
	T3CON			= 0;					// Disable ADC timer and Reset.

	TMR1 			= 0x0000; 				// Clear RTC Timer Register.
	PR1 			= __T1_TIME; 			// Load the Period Value of 10m sec.
	T1CONbits.TCKPS = 1; 					// Select 1:8 Prescaler.
	
	TMR3 			= 0x0000; 				// Clear ADC Timer Register.
	PR3 			= __T3_TIME;				// Load the Period Value for 128th sample of 20ms.
	T3CON			= 0x0030; 				// Select 1:256 Prescalar.

	IPC0bits.T1IP	= 7;					// Timer1 - Set Interrupt Priority Level.
	IFS0bits.T1IF	= 0;					// Timer1 - Clear Interrupt Flag.
	IEC0bits.T1IE	= 1;					// Timer1 - Enable Interrupt.

	IPC1bits.T3IP	= 6;					// Timer3 - Set Interrupt Priority Level.
	IFS0bits.T3IF	= 0;					// Timer3 - Clear Interrupt Flag.
	IEC0bits.T3IE	= 1;					// Timer3 - Enable Interrupt.

	T1CONbits.TON	= 1;					// Start Timer1 for RTC.
	T3CONbits.TON	= 1;					// Start Timer3 for ADC.
}

void Clock(void)
{
	if ( SystemTime.Seconds < 59 )          	// is cummulative seconds < 59?
	{		
		SystemTime.Seconds++;               	// yes, so increment seconds
	}
	else                         				// else seconds => 59     
	{
		Every_Minute = 1;
		SystemTime.Seconds = 0x00;          	// reset seconds
		if ( SystemTime.Minutes < 59 )      	// is cummulative minutes < 59?
		{
			SystemTime.Minutes++;           	// yes, so updates minutes  
		}
		else                     				// else minutes => 59
		{
			SystemTime.Minutes = 0x00;      	// reset minutes
			if ( SystemTime.Hours < 23 )    	// is cummulative hours < 23
			{
				Read_RTC();
				Sync_with_RTCTime();
			}
			else
			{
				SystemTime.Hours = 0x00;    	// Reset time
			}		  
		}
	}   	
}

/*---------------------------------------------------------------------
  Function Name: _T1Interrupt
  Description:   Timer1 Interrupt Handler
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
void __attribute__((__interrupt__,no_auto_psv)) _T1Interrupt( void )
{
	TimerDelay_flag_10ms = 1;
	if(++blink_count>=50)
	{
		blink_count = 0;
		LED_Task();
	}	
    if(++ten_ms_cnt>=100)
	{
		ten_ms_cnt = 0;
		everysec++;
		Clock();
		LATDbits.LATD9 = !LATDbits.LATD9;			//	Toggle MCLR RUN Status LED
	}
	if(++lcd_cont > 33)								// for every 330ms
	{
		lcd_cont = 0;
		Bits.En_Display_Lcd = 1;
	}
	if( (Operation_mode == MANUAL) && (PORTFbits.RF6) && Manual_key_hold_count==0 && Manual_On_set == 0)			// RF6
		manual_mode_on = TRUE;
	
 	IFS0bits.T1IF = 0;								/* reset Timer 1 interrupt flag */
}

void TimerDelay_10ms(unsigned long int delay)
{
	TimerDelay_flag_10ms = 0;
	while(delay)
	{
		if(TimerDelay_flag_10ms == 1)
		{
			TimerDelay_flag_10ms = 0;
			delay--;
		}
	}	
}

void LED_Task(void)
{
	LED_RUN = !LED_RUN;
	if(Error_Mode == 0)
	{
		if(Operation_mode == MANUAL)
		{
			if(Load_Status == ON)
			{
				LED_MANL = !LED_MANL;
				
			}	
			else
				LED_MANL = ON;
		}
		else
			LED_MANL = OFF;
		if(Operation_mode == AUTO)
		{
			if(Load_Status == ON)
				LED_AUTO = !LED_AUTO;
			else
				LED_AUTO = ON;
		}
		else
			LED_AUTO = OFF;
		if(Operation_mode == AUTO_PLUS)
		{
			if(Load_Status == ON)
				LED_AUTOPLUS = !LED_AUTOPLUS;
			else
				LED_AUTOPLUS = ON;
		}
		else
			LED_AUTOPLUS = OFF;
	}
	else
	{
		LED_MANL = ON;
		LED_AUTO = ON;
		LED_AUTOPLUS = ON;
	}		
}

