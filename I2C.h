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
 #ifndef __I2C_H
	#define __I2C_H
	#include	<P30F5011.h>
	#include "INIT.h"
	
	#define __I2C_ON          0b1010000000100100   	//	0xFFFF I2C module enabled ( 0b1000000100111111 )
	#define __I2C_OFF         0x7FFF 					//	I2C module disabled 
	void INIT_I2C(void);
	void OpenI2C(unsigned int config1, unsigned int config2);
	void StartI2C(void);
	void RestartI2C(void);
	void StopI2C(void);
	void IdleI2C(void);
	unsigned char MasterReadI2C(void);
	char MasterWriteI2C(unsigned char data_out);
	void NotAckI2C(void);
	void AckI2C(void);
	void WaitI2C(unsigned int cnt);
	void ConfigIntI2C(unsigned int config);
	void CloseI2C(void);
	char DataRdyI2C(void);
	unsigned int MastergetsI2C(unsigned int length, unsigned char * rdptr, unsigned int i2c_data_wait);
	unsigned int MasterputsI2C(unsigned char * wrptr);
		
	/*********************************************************************/
	/*              Header File for I2C module Library routines          */
	/*********************************************************************/
	
	#if defined(__dsPIC30F__)
	#include <p30fxxxx.h>
	#elif defined(__dsPIC33F__)
	#include <p33Fxxxx.h>
	#elif defined(__PIC24H__)
	#include <p24Hxxxx.h>
	#elif defined(__PIC24E__)
	#include <p24Exxxx.h>
	#elif defined(__dsPIC33E__)
	#include <p33Exxxx.h>
	#endif
	
	#ifndef I2C_H
	#define I2C_H
	
	/* I2C module definitions for dsPIC30F devices only */
	
	#ifdef _MI2CIF 
	
	/* List of SFRs for I2C module */
	/* This list contains the SFRs with default (POR) values to be used for configuring I2C module */
	/* The user can modify this based on the requirement */
	
	#define I2CRCV_VALUE               0x0000
	#define I2CTRN_VALUE               0x00FF
	#define I2CBRG_VALUE               0x0000
	#define I2CCON_VALUE               0x0000
	#define I2CSTAT_VALUE              0x0000
	#define I2CADD_VALUE               0x0000
	
	/* I2CCON register Configuration bit definitions */
	
	#define I2C_ON                       0xFFFF    /*I2C module enabled */
	#define I2C_OFF                       0x7FFF    /*I2C module disabled */
	
	#define I2C_IDLE_STOP              0xFFFF    /*stop I2C module in Idle mode */
	#define I2C_IDLE_CON               0xDFFF    /*continue I2C module in Idle mode */
	
	#define I2C_CLK_REL                0xFFFF    /*release clock */
	#define I2C_CLK_HLD                0xEFFF    /*hold clock  */
	
	#define I2C_IPMI_EN                0xFFFF    /*IPMI mode enabled */
	#define I2C_IPMI_DIS               0xF7FF    /*IPMI mode not enabled */
	
	#define I2C_10BIT_ADD              0xFFFF    /*I2CADD is 10-bit address */
	#define I2C_7BIT_ADD               0xFBFF    /*I2CADD is 7-bit address */
	
	#define I2C_SLW_DIS                0xFFFF    /*Disable Slew Rate Control for 100KHz */
	#define I2C_SLW_EN                 0xFDFF    /*Enable Slew Rate Control for 400KHz */
	
	#define I2C_SM_EN                  0xFFFF    /*Enable SM bus specification */
	#define I2C_SM_DIS                 0xFEFF    /*Disable SM bus specification */
	
	#define I2C_GCALL_EN               0xFFFF    /*Enable Interrupt when General call address is received. */
	#define I2C_GCALL_DIS              0xFF7F    /*Disable General call address. */
	
	#define I2C_STR_EN                 0xFFFF    /*Enable clock stretching */
	#define I2C_STR_DIS                0xFFBF    /*disable clock stretching */
	
	#define I2C_ACK                    0xFFDF    /*Transmit 0 to send ACK as acknowledge */
	#define I2C_NACK                   0xFFFF    /*Transmit 1 to send NACK as acknowledge*/
	
	#define I2C_ACK_EN                 0xFFFF    /*Initiate Acknowledge sequence */
	#define I2C_ACK_DIS                0xFFEF    /*Acknowledge condition Idle */
	
	#define I2C_RCV_EN                 0xFFFF    /*Enable receive mode */
	#define I2C_RCV_DIS                0xFFF7    /*Receive sequence not in progress */
	
	#define I2C_STOP_EN                0xFFFF    /*Initiate Stop sequence */
	#define I2C_STOP_DIS               0xFFFB    /*Stop condition Idle */
	
	#define I2C_RESTART_EN             0xFFFF    /*Initiate Restart sequence */
	#define I2C_RESTART_DIS            0xFFFD    /*Start condition Idle */
	
	#define I2C_START_EN               0xFFFF    /*Initiate Start sequence */
	#define I2C_START_DIS              0xFFFE    /*Start condition Idle */
	
	/* Priority for Slave I2C Interrupt */
	
	#define SI2C_INT_PRI_7             0xFFFF
	#define SI2C_INT_PRI_6             0xFFFE
	#define SI2C_INT_PRI_5             0xFFFD
	#define SI2C_INT_PRI_4             0xFFFC
	#define SI2C_INT_PRI_3             0xFFFB
	#define SI2C_INT_PRI_2             0xFFFA
	#define SI2C_INT_PRI_1             0xFFF9
	#define SI2C_INT_PRI_0             0xFFF8
	
	/* Slave I2C Interrupt Enable/Disable */
	
	#define SI2C_INT_ON                0xFFFF
	#define SI2C_INT_OFF               0xFFF7
	
	/* Priority for Master I2C Interrupt */
	
	#define MI2C_INT_PRI_7             0xFFFF
	#define MI2C_INT_PRI_6             0xFFEF
	#define MI2C_INT_PRI_5             0xFFDF
	#define MI2C_INT_PRI_4             0xFFCF
	#define MI2C_INT_PRI_3             0xFFBF
	#define MI2C_INT_PRI_2             0xFFAF
	#define MI2C_INT_PRI_1             0xFF9F
	#define MI2C_INT_PRI_0             0xFF8F
	
	/* Master I2C Interrupt Enable/Disable */
	
	#define MI2C_INT_ON                0xFFFF
	#define MI2C_INT_OFF               0xFF7F
	
	/* Macros to  Enable/Disable interrupts and set Interrupt priority of SI2C module*/
	#define EnableIntSI2C                    IEC0bits.SI2CIE = 1
	#define DisableIntSI2C                   IEC0bits.SI2CIE = 0
	#define SetPriorityIntSI2C(priority)     IPC3bits.SI2CIP = (priority)
	
	/* Macros to  Enable/Disable interrupts and set Interrupt priority of MI2C module*/
	#define EnableIntMI2C                    IEC0bits.MI2CIE = 1
	#define DisableIntMI2C                   IEC0bits.MI2CIE = 0
	#define SetPriorityIntMI2C(priority)     IPC3bits.MI2CIP = (priority)
	
	
	#endif
	
	/* I2C1 module definitions for dsPIC33F and PIC24H devices only */
	
	#ifdef _MI2C1IF
	
	/* List of SFRs for I2C1 module */
	/* This list contains the SFRs with default (POR) values to be used for configuring I2C1 module */
	/* The user can modify this based on the requirement */
	
	#define I2C1RCV_VALUE               0x0000
	#define I2C1TRN_VALUE               0x00FF
	#define I2C1BRG_VALUE               0x0000
	#define I2C1CON_VALUE               0x0000
	#define I2C1STAT_VALUE              0x0000
	#define I2C1ADD_VALUE               0x0000
	
	/* I2CCON register Configuration bit definitions */
	
	#define I2C1_ON                     0xFFFF    /*I2C module enabled */
	#define I2C1_OFF                    0x7FFF    /*I2C module disabled */
	
	#define I2C1_IDLE_STOP              0xFFFF    /*stop I2C module in Idle mode */
	#define I2C1_IDLE_CON               0xDFFF    /*continue I2C module in Idle mode */
	
	#define I2C1_CLK_REL                0xFFFF    /*release clock */
	#define I2C1_CLK_HLD                0xEFFF    /*hold clock  */
	
	#define I2C1_IPMI_EN                0xFFFF    /*IPMI mode enabled */
	#define I2C1_IPMI_DIS               0xF7FF    /*IPMI mode not enabled */
	
	#define I2C1_10BIT_ADD              0xFFFF    /*I2CADD is 10-bit address */
	#define I2C1_7BIT_ADD               0xFBFF    /*I2CADD is 7-bit address */
	
	#define I2C1_SLW_DIS                0xFFFF    /*Disable Slew Rate Control for 100KHz */
	#define I2C1_SLW_EN                 0xFDFF    /*Enable Slew Rate Control for 400KHz */
	
	#define I2C1_SM_EN                  0xFFFF    /*Enable SM bus specification */
	#define I2C1_SM_DIS                 0xFEFF    /*Disable SM bus specification */
	
	#define I2C1_GCALL_EN               0xFFFF    /*Enable Interrupt when General call address is received. */
	#define I2C1_GCALL_DIS              0xFF7F    /*Disable General call address. */
	
	#define I2C1_STR_EN                 0xFFFF    /*Enable clock stretching */
	#define I2C1_STR_DIS                0xFFBF    /*disable clock stretching */
	
	#define I2C1_ACK                    0xFFDF    /*Transmit 0 to send ACK as acknowledge */
	#define I2C1_NACK                   0xFFFF    /*Transmit 1 to send NACK as acknowledge*/
	
	#define I2C1_ACK_EN                 0xFFFF    /*Initiate Acknowledge sequence */
	#define I2C1_ACK_DIS                0xFFEF    /*Acknowledge condition Idle */
	
	#define I2C1_RCV_EN                 0xFFFF    /*Enable receive mode */
	#define I2C1_RCV_DIS                0xFFF7    /*Receive sequence not in progress */
	
	#define I2C1_STOP_EN                0xFFFF    /*Initiate Stop sequence */
	#define I2C1_STOP_DIS               0xFFFB    /*Stop condition Idle */
	
	#define I2C1_RESTART_EN             0xFFFF    /*Initiate Restart sequence */
	#define I2C1_RESTART_DIS            0xFFFD    /*Start condition Idle */
	
	#define I2C1_START_EN               0xFFFF    /*Initiate Start sequence */
	#define I2C1_START_DIS              0xFFFE    /*Start condition Idle */
	
	/* Priority for Slave I2C1 Interrupt */
	
	#define SI2C1_INT_PRI_7             0xFFFF
	#define SI2C1_INT_PRI_6             0xFFFE
	#define SI2C1_INT_PRI_5             0xFFFD
	#define SI2C1_INT_PRI_4             0xFFFC
	#define SI2C1_INT_PRI_3             0xFFFB
	#define SI2C1_INT_PRI_2             0xFFFA
	#define SI2C1_INT_PRI_1             0xFFF9
	#define SI2C1_INT_PRI_0             0xFFF8
	
	/* Slave I2C1 Interrupt Enable/Disable */
	
	#define SI2C1_INT_ON                0xFFFF
	#define SI2C1_INT_OFF               0xFFF7
	
	/* Priority for Master I2C1 Interrupt */
	
	#define MI2C1_INT_PRI_7             0xFFFF
	#define MI2C1_INT_PRI_6             0xFFEF
	#define MI2C1_INT_PRI_5             0xFFDF
	#define MI2C1_INT_PRI_4             0xFFCF
	#define MI2C1_INT_PRI_3             0xFFBF
	#define MI2C1_INT_PRI_2             0xFFAF
	#define MI2C1_INT_PRI_1             0xFF9F
	#define MI2C1_INT_PRI_0             0xFF8F
	
	/* Master I2C1 Interrupt Enable/Disable */
	
	#define MI2C1_INT_ON                0xFFFF
	#define MI2C1_INT_OFF               0xFF7F
	
	/* Macros to  Enable/Disable interrupts and set Interrupt priority of SI2C1 module*/
	#define EnableIntSI2C1                    IEC1bits.SI2C1IE = 1
	#define DisableIntSI2C1                   IEC1bits.SI2C1IE = 0
	#define SetPriorityIntSI2C1(priority)     IPC4bits.SI2C1IP = (priority)
	
	/* Macros to  Enable/Disable interrupts and set Interrupt priority of MI2C1 module*/
	#define EnableIntMI2C1                    IEC1bits.MI2C1IE = 1
	#define DisableIntMI2C1                   IEC1bits.MI2C1IE = 0
	#define SetPriorityIntMI2C1(priority)     IPC4bits.MI2C1IP = (priority)
	
	
	#endif
	
	/* I2C2 module definitions for dsPIC33F and PIC24H devices only */
	
	#ifdef _MI2C2IF
	
	/* List of SFRs for I2C2 module */
	/* This list contains the SFRs with default (POR) values to be used for configuring I2C1 module */
	/* The user can modify this based on the requirement */
	
	#define I2C2RCV_VALUE               0x0000
	#define I2C2TRN_VALUE               0x00FF
	#define I2C2BRG_VALUE               0x0000
	#define I2C2CON_VALUE               0x0000
	#define I2C2STAT_VALUE              0x0000
	#define I2C2ADD_VALUE               0x0000
	
	/* I2C2CON register Configuration bit definitions */
	
	#define I2C2_ON                     0xFFFF    /*I2C module enabled */
	#define I2C2_OFF                    0x7FFF    /*I2C module disabled */
	
	#define I2C2_IDLE_STOP              0xFFFF    /*stop I2C module in Idle mode */
	#define I2C2_IDLE_CON               0xDFFF    /*continue I2C module in Idle mode */
	
	#define I2C2_CLK_REL                0xFFFF    /*release clock */
	#define I2C2_CLK_HLD                0xEFFF    /*hold clock  */
	
	#define I2C2_IPMI_EN                0xFFFF    /*IPMI mode enabled */
	#define I2C2_IPMI_DIS               0xF7FF    /*IPMI mode not enabled */
	
	#define I2C2_10BIT_ADD              0xFFFF    /*I2CADD is 10-bit address */
	#define I2C2_7BIT_ADD               0xFBFF    /*I2CADD is 7-bit address */
	
	#define I2C2_SLW_DIS                0xFFFF    /*Disable Slew Rate Control for 100KHz */
	#define I2C2_SLW_EN                 0xFDFF    /*Enable Slew Rate Control for 400KHz */
	
	#define I2C2_SM_EN                  0xFFFF    /*Enable SM bus specification */
	#define I2C2_SM_DIS                 0xFEFF    /*Disable SM bus specification */
	
	#define I2C2_GCALL_EN               0xFFFF    /*Enable Interrupt when General call address is received. */
	#define I2C2_GCALL_DIS              0xFF7F    /*Disable General call address. */
	
	#define I2C2_STR_EN                 0xFFFF    /*Enable clock stretching */
	#define I2C2_STR_DIS                0xFFBF    /*disable clock stretching */
	
	#define I2C2_ACK                    0xFFDF    /*Transmit 0 to send ACK as acknowledge */
	#define I2C2_NACK                   0xFFFF    /*Transmit 1 to send NACK as acknowledge*/
	
	#define I2C2_ACK_EN                 0xFFFF    /*Initiate Acknowledge sequence */
	#define I2C2_ACK_DIS                0xFFEF    /*Acknowledge condition Idle */
	
	#define I2C2_RCV_EN                 0xFFFF    /*Enable receive mode */
	#define I2C2_RCV_DIS                0xFFF7    /*Receive sequence not in progress */
	
	#define I2C2_STOP_EN                0xFFFF    /*Initiate Stop sequence */
	#define I2C2_STOP_DIS               0xFFFB    /*Stop condition Idle */
	
	#define I2C2_RESTART_EN             0xFFFF    /*Initiate Restart sequence */
	#define I2C2_RESTART_DIS            0xFFFD    /*Start condition Idle */
	
	#define I2C2_START_EN               0xFFFF    /*Initiate Start sequence */
	#define I2C2_START_DIS              0xFFFE    /*Start condition Idle */
	
	/* Priority for Slave I2C2 Interrupt */
	
	#define SI2C2_INT_PRI_7             0xFFFF
	#define SI2C2_INT_PRI_6             0xFFFE
	#define SI2C2_INT_PRI_5             0xFFFD
	#define SI2C2_INT_PRI_4             0xFFFC
	#define SI2C2_INT_PRI_3             0xFFFB
	#define SI2C2_INT_PRI_2             0xFFFA
	#define SI2C2_INT_PRI_1             0xFFF9
	#define SI2C2_INT_PRI_0             0xFFF8
	
	/* Slave I2C2 Interrupt Enable/Disable */
	
	#define SI2C2_INT_ON                0xFFFF
	#define SI2C2_INT_OFF               0xFFF7
	
	/* Priority for Master I2C2 Interrupt */
	
	#define MI2C2_INT_PRI_7             0xFFFF
	#define MI2C2_INT_PRI_6             0xFFEF
	#define MI2C2_INT_PRI_5             0xFFDF
	#define MI2C2_INT_PRI_4             0xFFCF
	#define MI2C2_INT_PRI_3             0xFFBF
	#define MI2C2_INT_PRI_2             0xFFAF
	#define MI2C2_INT_PRI_1             0xFF9F
	#define MI2C2_INT_PRI_0             0xFF8F
	
	/* Master I2C2 Interrupt Enable/Disable */
	
	#define MI2C2_INT_ON                0xFFFF
	#define MI2C2_INT_OFF               0xFF7F
	
	/* Macros to  Enable/Disable interrupts and set Interrupt priority of SI2C2 module*/
	#define EnableIntSI2C2                    IEC3bits.SI2C2IE = 1
	#define DisableIntSI2C2                   IEC3bits.SI2C2IE = 0
	#define SetPriorityIntSI2C2(priority)     IPC12bits.SI2C2IP = (priority)
	
	/* Macros to  Enable/Disable interrupts and set Interrupt priority of MI2C1 module*/
	#define EnableIntMI2C2                    IEC3bits.MI2C2IE = 1
	#define DisableIntMI2C2                   IEC3bits.MI2C2IE = 0
	#define SetPriorityIntMI2C2(priority)     IPC12bits.MI2C2IP = (priority)
	
	
	#endif
	
	#endif    /*I2C_H  */
	
	#endif
	
