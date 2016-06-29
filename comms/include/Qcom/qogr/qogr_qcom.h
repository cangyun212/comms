/*	Copyright 1997-2013 State of Queensland - Office of Liquor and Gaming Regulation (OLGR)

	Distribution of this source code is restricted without the
	written permission of the OLGR.

	Use of the source code or any part thereof is at the users risk who
	agrees to indemnify OLGR against any damages or loss arising out of
	such use.

	OLGR authorised organisations may use this source code or extraction's
	from it, in their own software for the sole purpose of implementing the
	OLGR QCOM EGM protocol.

	Please direct any problems or queries to: notify@qogr.qld.gov.au
*/
/*	-------------------------------------------------------------------
	QCOM.H

	Header File Description:

	This file contains all the QCOM Protocol structures used to implement
	Version 1.5.3 & v1.6 of the QCOM protocol.
	It also contains the function prototypes used by QCOM.C

    This module maps the protocol onto C structs so it works very well with an
    IDE that has a code-completion facility.

    See QCOM.CPP for the revision history

    ---    This module is released as a part of the QCOM SDK    ---

    Todo:
        Further typecasting of message fields eg. typedef BYTE qcom_polladdresstype etc.
        Investigate removing re fields from message data
        moveable 'repeated entry' fields

*/
/*  Compilation requirements
    ------------------------

-   This source code assumes least significant bit/byte first
-   The structures in this file must be complied with byte alignment
    or the resulting code will not work
-   This code compiles correctly using the following compilers:-
        16 bit Borland C++ 3.1 &
        32 bit Borland compilers all versions
        DJGPP 32 bit GNU open source compiler
*/
#ifndef QCOM_H
#define QCOM_H

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------------
//  Compiler independent definitions

#define UCHAR   u8
#define USHORT  u16
#define UINT    u16
#define ULONG   u32

// failsafe check for type sizes
#ifndef __GNUC__                // GNUC doesnt support sizeof in directives
//#if (sizeof(ULONG) != 4)
//#error long int must be 4 bytes for this module to function
//#endif
#endif

#define BYTE    UCHAR
#define WORD    USHORT
#define BOOLEAN UCHAR
#define BOOL    UCHAR
#ifndef __GNUC__
#define BITS    BYTE            // msvc prefer this to get the same size of the union structure as gcc
#else
#define BITS    WORD            // For var:x; defs. This may also be BYTE (but some compilers perfer WORD)
#endif                          // However sizeof(var:x) where x <= 8 must be 1

#ifndef __GNUC__                // GNUC doesnt support sizeof in directives
//#if (sizeof(WORD) != 2)
//#error short integers must be 2 bytes for this module to function
//#endif
#endif

//#define FALSE   0
//#define TRUE    1
#define OFF     FALSE
#define NO      FALSE

#define QCOM_BAUD_RATE              19200
#define QCOM_EGM_COMMS_TIMEOUT      10      // in seconds
#define QCOM_MAX_NAKS               3       // no. of consecutive NAK for a communications fault
#define QCOM_MAX_VAR_PER_GAME       16      // Was 8 in QCOM v1.5
#define QCOM_MAX_VAR_PER_GAME_V1_5  8
#define QCOM_MAX_PROGR_LEV_PER_GAME 8
#define QCOM_MAX_PHA_SH_LENGTH      20      // length in bytes of seeds/hash fields
#define QCOM_MAX_PSA_SS_LENGTH_V1_5 4       // QPv1.5 was 8 bytes long and only 4 were used

#define QCOM_MAX_ECT                999999l // Maximum ECT to EGM in cents (QCOM v1.5 only)

#define QCOM_PINC_MULT              10000.0 // multiplier for Percentage Increment fields
#define QCOM_RTP_MULT				100.0	// multiplier for RTP fields
#define QCOM_ROUNDER(floatvalue, mult) (floatvalue*mult + 0.5)  // should be used to avoid truncation errors when casting from float to integer values in the protocol

#define QCOM_MINRTP                 5000    // * QCOM_RTP_MULT
#define QCOM_MAXRTP                 10000   // * QCOM_RTP_MULT

//--------------------------------------------------------------------------
//  QCOM Message Function Codes

//  QCOM Poll Message function codes

#define QCOM_HI_POLL_FC         0x3A    //  Next highest unused poll function code
#define QCOM_FTPALDP_FC         0x39    //  FTP Activate Last Download Poll
#define QCOM_FTPRDSP_FC         0x38    //  FTP Request Download Status Poll
#define QCOM_FTPRP_FC           0x37    //  FTP Reset Poll
#define QCOM_FTPCDRP_FC         0x36    //  FTP Commence Download Request Poll
#define QCOM_EGMCRP_FC          0x33    //  EGM Configuration Request Poll
#define QCOM_EGMCP_FC           0x31    //  EGM Configuration Poll
#define QCOM_EGMGCP_FC          0x2F    //  EGM Game Configuration Poll
#define QCOM_EGMVCP_FC          0x2E    //  EGM Variation Change Poll
#define QCOM_EGMPP_FC           0x2D    //  EGM Parameters Poll
#define QCOM_PCP_FC             0x2C    //  Progressive Configuration Poll (QCOM v1.6)
#define QCOM_EXTJIP_FC          0x2A    //  External Jackpot Information Poll (QOCM v1.6)
#define QCOM_PHRP_FC            0x29    //  Program Hash Request Poll
#define QCOM_SALRP_FC           0x27    //  System Lockup Request Poll
#define QCOM_ECTLRP_FC          0x24    //  ECT Lockup Reset Poll
#define QCOM_ECTEGMLRP_FC       0x23    //  ECT from EGM Lockup Request Poll
#define QCOM_ECTEGMP_FC         0x22    //  ECT to EGM Poll
#define QCOM_TORACKP_FC         0x21    //  Ticket Out Request Acknowledgement Poll (QCOM v1.6)
#define QCOM_TIRACKP_FC         0x20    //  Ticket In Request Acknowledgement Poll (QCOM v1.6)
#define QCOM_CCLRP_FC           0x1E    //  Cancel Credit Lockup Request Poll
#define QCOM_EGMGMP_FC          0x1B    //  EGM General Maintenance Poll
#define QCOM_RALEP_FC           0x17    //  Request All Logged Events Poll
#define QCOM_PEP_FC             0x16    //  Purge events Poll
#define QCOM_DLSALAP_FC         0x12    //  DLSA/CRE Lockup Acknowledgement Poll
#define QCOM_DLSACP_FC          0x11    //  DLSA/CRE Configuration Poll
#define QCOM_NAMP_FC            0x10    //  Note Acceptor Maint Poll
#define QCOM_HTPMP_FC           0x0F    //  Hopper/Ticker Printer Maint Poll
#define QCOM_LPAAP_FC           0x0E    //  Linked Progressive Award Acknowledged Poll
#define QCOM_EGMGRP_FC          0x0C    //  EGM General Reset Poll
#define QCOM_SPAMA_FC           0x08    //  Specific Promotional Message poll
#define QCOM_SPAMB_FC           0x07    //  Specific Promotional Message poll
#define QCOM_EGMTLMP_FC         0x05    //  Tower Light Maintenance
#define QCOM_GSP_FC             0x01    //  General Status Poll
#define QS_CUSTOM_MSG_FC        0x00    //  Reserved by QSIM to denote custom poll FCs for testing

//--------------------------------------------------------------------------
//  Broadcast Message Function Codes

#define QCOM_BROADCAST_POLL_FC  0xFF    //  Broadcast Poll Function Code
#define QCOM_BROADCAST_FTP_FC   0xFE    //  Function Code of an FTP Broadcast
#define QCOM_BROADCAST_SEEK_FC  0xFC    //  Seek EGM Broadcast Poll Address and Fucntion Code (QCOM v1.6)

#define QCOM_BMLPCA_FC          0x02    //  Extended Broadcast - Linked Progressive Current Amounts Broadcast Poll
#define QCOM_BMGPM_FC           0x03    //  Extended Broadcast - General Promotional Message
#define QCOM_BMEGMPAC_FC        0x04    //  Extended Broadcast - EGM Poll Address Configuration
#define QCOM_BMSD_FC            0xFE    //  Extended Broadcast - Site Details

#define QCOM_NO_EXTENDED_BROADCAST_DATA_FC  (0xFF)      // Reserved, used by QSIM

#define QCOM_BROADCAST_ADDRESS  0xFF    //  Default Poll Address of global messages


//--------------------------------------------------------------------------
//  Response Function Codes from EGM

enum QCOM_RESP_FC {
    QCOM_RESP_RESERVED_FC,  //          Do not allocate this FC
    QCOM_GSR_FC,            //  0x01    General Status Response
    QCOM_FTPDSR_FC,         //  0x02    FTP Download Status Response
    QCOM_RESP_SPARE_03_FC,
    QCOM_NASR_FC,           //  0x04    Note Acceptor status Response (QCOM v1.6)
    QCOM_RESP_SPARE_05_FC,
//  QCOM_DLLSR_FC,          //  0x05    DLL status Response (QCOM v1.6)
    QCOM_DLSASR_FC,         //  0x06    CRE Status Response
    QCOM_PMR_FC,            //  0x07    Progressive Meters Response
    QCOM_PCR_FC,            //  0x08    Progressive Configuration Response
    QCOM_BMR_FC,            //  0x09    Bet Meters Response (QCOM v1.6)
    QCOM_MGVMR_FC,          //  0x0A    Multi-Game/Variation Meters Response
    QCOM_PCMR_FC,           //  0x0B    Players Choice Meters Response (QCOM v1.6)
    QCOM_MGCR_FC,           //  0x0C    Meter Group/Contribution Response
    QCOM_RESP_SPARE_0D_FC,
    QCOM_ECTEGMACKR_FC,     //  0x0E    ECT to EGM Ack Response (QCOM v1.6)
    QCOM_PEPAR_FC,          //  0x0F    Purge Event Poll Acknowledgement Response
    QCOM_ER_FC,             //  0x10    Event Response
    QCOM_RESP_SPARE_11_FC,
    QCOM_RESP_SPARE_12_FC,
    QCOM_EGMGCR_FC,         //  0x13    EGM Game Config Response
    QCOM_RESP_SPARE_14_FC,
    QCOM_RESP_SPARE_15_FC,
    QCOM_EGMCR_FC,          //  0x16    EGM Configuration Response
    QCOM_RESP_SPARE_17_FC,
    QCOM_RESP_SPARE_18_FC,
    QCOM_PHR_FC,            //  0x19    Program Hash Response
    QCOM_RESP_SPARE_1A_FC,
    QCOM_RESP_SPARE_1B_FC,
    QCOM_RESP_SPARE_1C_FC,
    QCOM_RESP_SPARE_1D_FC,
    QCOM_RESP_SPARE_1E_FC,
    QCOM_RESP_SPARE_1F_FC,
    QCOM_SEEKEGM_FC,        //  0x20    Seek EGM Broadcast Response (QCOM v1.6)
    QCOM_HI_RESP_FC         //  0x21    Next highest unused poll function code

    }; // response function codes

//--------------------------------------------------------------------------
// QCOM Event Codes
//--------------------------------------------------------------------------

// Offsets

#define QC_EC_FAULT_CONDITIONS_START            0x0000
#define QC_EC_LOCKUP_EVENT_START                0x1000
#define QC_EC_ADVISORY_EVENTS_START             0x2000
#define QC_EC_UNUMBERED_EVENT_START             0x3000
#define QC_EC_END_OF_EVENTS                     0x4000
#define QC_EC_SYSTEM_EVENT_START                0x8000

// Fault Condition Event Codes

#define QC_EC_COIN_IN_FAULT                     0x0004
#define QC_EC_EXCESS_COIN_REJ                   0x0005
#define QC_EC_YO_YO                             0x0006
#define QC_EC_DIVERTER_FAULT                    0x0007
#define QC_EC_HOPPER_LEVEL_MISMATCH             0x0009
#define QC_EC_HOPPER_EMPTY                      0x000A
#define QC_EC_HOPPER_JAMMED                     0x000B
#define QC_EC_HOPPER_RUNAWAY                    0x000C
#define QC_EC_HOPPER_DISCONNECTED               0x000D
#define QC_EC_MECH_METERS_DISC                  0x000E
#define QC_EC_PROGR_CONTR_FAULT                 0x0010
#define QC_EC_IO_CONTROLLER_FAULT               0x0011
#define QC_EC_TOUCH_SCREEN_FAULT                0x0015
#define QC_EC_MAN_SPEC_FAULT_A                  0x0017
#define QC_EC_MAN_SPEC_FAULT_B                  0x0018
#define QC_EC_MAN_SPEC_FAULT_EXT                0x0019 // QCOM v1.6
#define QC_EC_STEPPER_REEL_FAULT                0x001A
#define QC_EC_BONUS_DEVICE_FAULT                0x001B
#define QC_EC_MECH_METER_DISC_FAULT             0x001E
#define QC_EC_NOTE_STACKER_FULL                 0x0020
#define QC_EC_NOTE_ACCEPTOR_JAMMED              0x0021
#define QC_EC_NOTE_ACCEPTOR_DISCONNECTED        0x0022
#define QC_EC_NOTE_ACCEPTOR_FAULT               0x0023
#define QC_EC_EXCESS_NOTE_REJ                   0x0024  // QCOM v1.6
#define QC_EC_NOTE_ACCEPTOR_YOYO                0x0025
#define QC_EC_LOW_NV_RAM_BATTERY                0x0027
#define QC_EC_LOW_PF_DOOR_DET_BATT              0x0028
#define QC_EC_EEPROM_FAULT                      0x0029
#define QC_EXT_EC_EGM_RAM_CLEARED               0x002A
#define QC_EC_TICKET_PAPER_OUT                  0x002C
#define QC_EC_TICKET_PAPER_JAM                  0x002D
#define QC_EC_TICKET_GENERAL_FAULT              0x002E
#define QC_EXT_EC_TICKET_PRINT_FAIL             0x002F  // QCOM v1.6
#define QC_EC_LOW_MEMORY                        0x0030
#define QC_EC_BAD_POWER_DOWN                    0x0031
#define QC_EC_AUX_DISP_FAILURE                  0x0032
#define QC_EC_PRI_DISP_FAILURE                  0x0033
#define QC_EC_TER_DISP_FAILURE                  0x0034
#define QC_EC_LIC_KEY_FAIL                      0x0035

//  Lockup Event Codes

#define QC_EXT_EC_LP_AWARD                      0x1000
#define QC_EXT_EC_SAP_AWARD                     0x1001  // QCOM v1.5
#define QC_EXT_EC_LARGE_WIN                     0x1002
#define QC_EXT_EC_CANCEL_CREDIT                 0x1003

// Advisory Event Event Codes

#define QC_EC_ALL_FAULTS_CLEAR                  0x2000
#define QC_EC_LOCKUP_CLEAR                      0x2001
#define QC_EC_CANCEL_CREDIT_CANCELLED           0x2002
#define QC_EC_RESERVED_QSIM                     0x2003
#define QC_EC_EGM_EVENT_QUEUE_FULL              0x2006
#define QC_EC_COMM_TIMEOUT                      0x2007
#define QC_EXT_EC_VAR_ENABLED                   0x2008
#define QC_EC_EGM_RTC_REFRESHED                 0x2009
#define QC_EXT_EC_REFILL_RECORDED               0x200A
#define QC_EC_EGM_POWER_UP                      0x200B
#define QC_EC_EGM_POWER_DOWN                    0x200C
#define QC_EC_EGM_MAIN_DOOR_OPENED              0x2010
#define QC_EC_EGM_MAIN_DOOR_CLOSED              0x2011
#define QC_EC_CASH_BOX_DOOR_OPENED              0x2012
#define QC_EC_CASH_BOX_DOOR_CLOSED              0x2013
#define QC_EC_PROCESSOR_DOOR_OPENED             0x2014
#define QC_EC_PROCESSOR_DOOR_CLOSED             0x2015
#define QC_EC_BELLY_PANEL_DOOR_OPENED           0x2016
#define QC_EC_BELLY_PANEL_DOOR_CLOSED           0x2017
#define QC_EC_NOTE_STACKER_HIGH_LEVEL_WARN      0x201A
#define QC_EC_NOTE_ACCEPTOR_DOOR_OPENED         0x2020
#define QC_EC_NOTE_ACCEPTOR_DOOR_CLOSED         0x2021
#define QC_EC_NOTE_ACCEPTOR_STACKER_REMOVED     0x2022
#define QC_EC_NOTE_ACCEPTOR_STACKER_RETURNED    0x2023
#define QC_EC_NOTE_STACKER_CLEARED              0x2024

#define QC_EC_PWR_OFF_PROCESSOR_DOOR_ACCESS     0x2025
#define QC_EC_PWR_OFF_CASH_DOOR_ACCESS          0x2026
#define QC_EC_PWR_OFF_MAIN_DOOR_ACCESS          0x2027
#define QC_EC_PWR_OFF_NOTE_ACCPTR_DOOR_ACCESS   0x2028
#define QC_EC_PWR_OFF_MECH_METER_DOOR_ACCESS    0x2029
#define QC_EC_CASHBOX_CLEARED                   0x202A

#define QC_EXT_EC_TICKET_PRINTED                0x2030
#define QC_EC_TICKET_INK_LOW                    0x2031
#define QC_EC_NOTE_STACKER_FULL_NOTICE          0x2032
#define QC_EC_STACKER_HIGH_LEVEL                0x2033
#define QC_EC_TICKET_PAPER_LOW                  0x2034
#define QC_EC_INVALID_EGM_CONFIGURATION         0x2035
#define QC_EC_INVALID_GAME_CONFIGURATION        0x2036
#define QC_EXT_INVALID_PROGR_CONFIGURATION      0x2037
#define QC_EC_PROCESSOR_OVER_TEMP               0x2040  // QCOM v1.6
#define QC_EC_COOLING_FAN_FAILURE               0x2042  // QCOM v1.6
#define QC_EC_CALL_SERVICE_TECHNICIAN           0x2043  // QCOM v1.6
// 0x2045...0x2049 reserved for QCOM FTP
#define QC_EC_MECH_METER_DOOR_OPENED            0x204A  // QCOM v1.6
#define QC_EC_MECH_METER_DOOR_CLOSED            0x204B  // QCOM v1.6
#define QC_EC_AUX_DOOR_OPENED                   0x204C  // QCOM v1.6
#define QC_EC_AUX_DOOR_CLOSED                   0x204D  // QCOM v1.6
#define QC_EC_RECOVERABLE_RAM_CORRUPTION        0x204E  // QCOM v1.6
#define QC_EC_PID_SESSION_STARTED               0x204F  // QCOM v1.6
#define QC_EC_DENOM_CHANGED                     0x2050  // QCOM v1.6
#define QC_EC_INVALID_DENOMINATION              0x2051  // QCOM v1.6
#define QC_EC_HOPPER_CALIBRATED                 0x2052  // QCOM v1.6
#define QC_EC_PRGSSVE_CFG_CHANGED               0x2053  // QCOM v1.6
#define QC_EC_NP_TOP_PRIZE_HIT                  0x2054  // QCOM v1.6
#define QC_EC_INVALID_TICKET_ACK                0x2055  // QCOM v1.6
#define QC_EC_PERIOD_METERS_RESET               0x2056  // QCOM v1.6.1
#define QC_EC_ICON_DISP_ENABLED                 0x2057  // QCOM v1.6.1 (optional for QLD)
#define QC_EC_HOPPER_RUNAWAY_OVERPAY            0x2058  // QCOM v1.6.3
#define QC_EC_LIC_KEY_DETECTED                  0x2059  // QCOM v1.6.3
#define QC_EC_NON_PROD_LIC_KEY_DETECTED         0x205A  // QCOM v1.6.3
#define QC_EC_TICKET_IN_TIMEOUT					0x205B	// QCOM v1.6.5
#define QC_EC_TICKET_IN_ABORTED					0x205C	// QCOM v1.6.5
#define QC_EC_TRANSACTION_DENIED                0x205D  // QCOM v1.6.6i

// Unnumbered Advisory Event Event Codes

#define QC_EXT_EC_HOPPER_PAYOUT                 0x3000
#define QC_EXT_EC_RES_CC_LOCKUP                 0x3001
#define QC_EC_EGM_TEMP_EVENT_QUEUE_FULL         0x3002
#define QC_EXT_EC_NEW_GAME_SELECTED             0x3003
#define QC_EXT_EC_ECT_FROM_EGM                  0x3004
#define QC_EXT_EC_DLSA                          0x3005
#define QC_EXT_EC_SLUR                          0x3006  // QCOM v1.6
//#define QC_EXT_EC_AUTO_LP                     0x3007
#define QC_EXT_EC_SAP_UN_V2                     0x3008  // QCOM v1.6
#define QC_EXT_EC_ECT_FROM_EGM_V2               0x3009  // QCOM v1.6
#define QC_EXT_EC_RES_CC_LOCKUP_V2              0x3010  // QCOM v1.6
#define QC_EXT_EC_TICKET_OUT_LOCKUP             0x3011  // QCOM v1.6
#define QC_EXT_EC_TO_PRINT_OK                   0x3012  // QCOM v1.6
#define QC_EXT_EC_TICKET_IN_REQUEST             0x3013  // QCOM v1.6
#define QC_EC_NEW_PID_SESSION_START             0x3014  // QCOM v1.6.3
#define QC_EXT_EC_PID_SESSION_STOP              0x3015  // QCOM v1.6.3

#define QCOM_HIGHEST_EGM_EC ((WORD)0x7FFF)  // Highest allowable EGM event code
#define QCOM_DEFAULT_ESEQ   0x01            // Default event sequence number

//--------------------------------------------------------------------------
//  Data Link Layer Defines

#define QCOM_MAX_MSG_LENGTH         257
#define QCOM_MAX_MSG_LEN            (QCOM_MAX_MSG_LENGTH-2) // Maximum value for the DLL->LEN field
#define QCOM_MAX_MSG_LENGTH_V1_5    160                     // QCOM v1.5 EGMs
#define QCOM_MAX_MSG_LEN_V1_5       (QCOM_MAX_MSG_LENGTH-2) // Maximum value for the DLL->LEN field
#define QCOM_MIN_MSG_LEN            4   // Minimum value for the DLL->LEN field  (CNTL + FC + CRC)

// QSIM Data Link Layer Length
#define QCOM_DLL_LEN        (sizeof(qc_dlltype)+sizeof(qc_messageCRCtype))
                                // Adrs to CRC including function code,
                                // excluding message data
#define QCOM_LEN_OFFSET     2   // Offset into QCOM message type of LEN field
//--------------------------------------------------------------------------
//
// Meter Group IDs
//

enum QCOM_Meter_ID {
    QCOM_METERID_STROKE,
    QCOM_METERID_TURNOVER,
    QCOM_METERID_WINS,
    QCOM_METERID_CANCEL_CREDIT,
    QCOM_METERID_TICKET_OUT,
    QCOM_METERID_CENTS_IN,
    QCOM_METERID_CENTS_OUT,
    QCOM_METERID_HPSAP_WINS,        // was SAP wins in QCOM v1.5 is now reserved in QCOM v1.6
    QCOM_METERID_LP_WINS,
    QCOM_METERID_TICKET_IN,         // QCOM v1.6
    QCOM_METERID_PIDACCESSED,       // QCOM v1.6
    QCOM_METERID_GAMESWON,          // QCOM v1.6
    QCOM_METERID_SAP_PWCM_WINS,     // QCOM v1.6.1
    QCOM_METERID_EGMDLJPWINS,       // NZL v1.5/1.6

    QCOM_METERID_RESERVED0E,
    QCOM_METERID_RESERVED0F,


    QCOM_METERID_REFILLS,
    QCOM_METERID_COINS_IN,
    QCOM_METERID_COINS_OUT,
    QCOM_METERID_CASH_BOX,
    QCOM_METERID_CASHLESS_IN,
    QCOM_METERID_CASHLESS_OUT,
    QCOM_METERID_NOTES_IN,
    QCOM_METERID_NOTES_IN_COUNT,
    QCOM_METERID_RCRF_TURNOVER,
    QCOM_METERID_RCRF_WIN,
    QCOM_METERID_REJ_NOTES,         // QCOM v1.6
    QCOM_METERID_GAMBLE_TURN,       // QCOM v1.6
    QCOM_METERID_GAMBLE_WIN,        // QCOM v1.6
    QCOM_METERID_COINS_CLEARED,     // QCOM v1.6.2
    QCOM_METERID_NOTES_CLEARED,     // QCOM v1.6.2
    QCOM_METERID_RESERVED1F,


    QCOM_METERID_5_NOTES,
    QCOM_METERID_10_NOTES,
    QCOM_METERID_20_NOTES,
    QCOM_METERID_50_NOTES,
    QCOM_METERID_100_NOTES
    };

#define QCOM_METERS_PER_GROUP   16      // 16 Meters per Group
// Macro which returns meter group number for a given Meter Group ID
#define QCOM_MET_GROUP(MGID)    (MGID / QCOM_METERS_PER_GROUP)
// Macro which returns meter offset number within its group for a given Meter Group ID
#define QCOM_MET_OFFSET(MGID)   (MGID % QCOM_METERS_PER_GROUP)

//--------------------------------------------------------------------------
//  Miscellaneous QCOM protocol defines

// Default Response States
#define QCOM_DRS_IDLE   0x01
#define QCOM_DRS_PLAY   0x02
#define QCOM_DRS_PLYF   0x03    // In Play - Feature (in v1.5 also gamble)
#define QCOM_DRS_HPC    0x04    // Hopper Collect
#define QCOM_DRS_RCR    0x05    // Residual Credit Removal
#define QCOM_DRS_RCRL   0x06    // Residual Credit Removal Lockup
#define QCOM_DRS_CCL    0x07    // Cancel Credit
#define QCOM_DRS_LWL    0x08    // Large Win
#define QCOM_DRS_LP     0x09    // Linked Progressive
#define QCOM_DRS_SAP    0x0A    // Stand-Alone Progressive
#define QCOM_DRS_SAL    0x0B    // System Lockup
#define QCOM_DRS_ECT    0x0C    // Elec. Cash Transfer
#define QCOM_DRS_DLSA   0x0D    // DLSA/CRE Lockup
#define QCOM_DRS_RESE   0x0E    // reserved
#define QCOM_DRS_TICKP  0x0F    // Printing Cash Ticket Out
#define QCOM_DRS_CTOL   0x10    // Cash Ticket Out Lockup
#define QCOM_DRS_RES11  0x11    // reserved
#define QCOM_DRS_PLYG   0x12    // In Play - Gamble

// Progressive group IDs (PGID)

#define QCOM_PGID_CFG_REQ   0x0000
#define QCOM_PGID_SAP       0xFFFF
#define QCOM_PGID_NP        0xFFFF

//  Game Variation Numbers (GVN)

#define QCOM_GVN_RESERVED   0x0000

//  Poll Sequence Numbers

#define QCOM_NEXT_PSN(PSN)  ((PSN+1) % 256)

//  DLSA

#define QCOM_MAX_DLSA       8

//---------------------------------------------------------------------------
//  QCOM Protocol Overlay Structures
//
//	Refer to the OLGR QCOM Protocol document for more information
//
//  The following typedefs overlay the QCOM protocol message formats
//  for easier programming.
//
//  This module maps the protocol onto C structs so it works very well with an
//  IDE that has a code-completion facility.
//
//  It is suggested the reader skip to the root structues - see "qcpt" & "qcrt"
//	at the end of the module. I.e. read from the bottom up.
//
//	NOTE! QCOM 3 is a byte aligned protocol and thus
//  The following structures and types must be compiled in
//	byte aligned mode or the module will not work
//---------------------------------------------------------------------------


#if defined(__WIN32__) || defined(WIN32) || defined(_WIN32)
#pragma pack(push, 1)              // Force byte alignment for QCOM
#endif
#ifdef __GNUC__
#define HANDLE_SYSV_PRAGMA 1    // allow pragma
#pragma pack(push,1)            // Force byte alignment for QCOM
#endif
 // test for byte alignment
typedef struct {
    BYTE    a;
    BITS    abit:1;
    BYTE    b;
    } qc_testaligntype;         // used for testing byte alignment only
// The above struct must be 3 bytes in size or module wont work
// If byte alignment is on, then this struct will be 3 bytes in size

#ifndef __GNUC__                // GNUC doesnt support sizeof in directives
//#if (sizeof(qc_testaligntype) != 3)
//#error Byte Alignment Required
//#endif
#endif
//--------------------------------------------------------------------------


//  ...................................................................
//  QCOM Data Link Layer

#define QCOM_DLL_LENGTH_OFFSET  2
#define QCOM_DLLVER_1_5         0
#define QCOM_DLLVER_1_6         1

typedef BYTE qc_fctype;

typedef struct {
    BYTE        PollAddress;
    BYTE        Length;
    union {
        BYTE CNTL;
        struct {
            BITS res:5;
            BITS DLLVER:2;      // Data Link Layer version
            BITS GBLNAK:1;
            } bits_resp;
        struct {
            BITS res:7;
            BITS ACK:1;
            } bits_poll;
    } ControlByte;
    qc_fctype   FunctionCode;
    } qc_dlltype;

typedef WORD        qc_messageCRCtype;

//  ...................................................................
//  QCOM Date and Time format
typedef struct {        // the following fields are packed BCD
    BYTE    seconds;
    BYTE    minutes;
    BYTE    hours;
    BYTE    day;
    BYTE    month;
    BYTE    year;
    } qc_timedatetype;

//  ...................................................................
//  Serial Number Format
#define QCOM_MID_DIV 1000000l  // used to access MID
typedef union {
    ULONG   SER;
    struct {
        BYTE    SER[3];     // Serial number
        BYTE    MID;        // Manufacturer ID
        } SerialNo;
    } qc_serialnotype;

//  ...................................................................
//  TITO Authorization Number format
typedef union {
        BYTE    AUTHNO[16];
/*        struct {
            ULONG LSB;
            ULONG MSB;
            } hlparts;*/
        } qc_titoauthnotype;

//---------------------------------------------------------------------------
//  QCOM Response types

//  ...................................................................
//  General Status Response QCOM_GSR_FC

#define QCOM_DRS_MAX	    0x15		// Total number of defined EGM General Status Response STATE strings

typedef struct {
    union {
        BYTE    FLGA;
        struct {
            BITS MainDoor:1;
            BITS CashBoxDoor:1;
            BITS ProcessorDoor:1;
            BITS BellyPanelDoor:1;
            BITS NoteAcceptorDoor:1;
            BITS NoteStacker:1;
            BITS MechMeterDoor:1;   // QCOM v1.6
            BITS AuxDoor:1;         // QCOM v1.6
            } bits;
        } FLGA;
    union {
        BYTE    FLGB;
        struct {
            BITS Fault:1;
            BITS Audit:1;
            BITS Test:1;
            BITS Cmode:1;       // QCOM v1.6
            BITS TIrack:1;      // QCOM v1.6
            BITS res:3;
            } bits;
        } FLGB;
    BYTE    STATE;
    } qc_gsrtype;

//  ...................................................................
//  Note Acceptor Status Response QCOM v1.6 QCOM_NASR_FC

#define QCOM_NASR_NADS_SIZE 40

typedef struct {
    union {
        WORD    FLGA;
        struct {
            WORD res:15;
            WORD full:1;
            } bits;
        struct {
            BYTE LSB;
            union {
                BYTE MSB;
                struct {
                    BITS res:7;
                    BITS full:1;
                    } bits;
                } MSB;
            } bytes;
    } FLGA;
    union {
        WORD    NAFLG;
        struct {
            BYTE LSB;
            BYTE MSB;
            } bytes;
        struct {
            BYTE LSB;
            BITS five:1;
            BITS ten:1;
            BITS twenty:1;
            BITS fifty:1;
            BITS hundred:1;
            BITS res:3;
            } bits;
        } NAFLG;
    ULONG   RES;
    char    NADS[QCOM_NASR_NADS_SIZE];
    } qc_nasrtype;

//  ...................................................................
//  FTP Download Status Response QCOM_FTPDSR_FC

typedef enum  {
    QCOM_FTP_STATE_IDLE,
    QCOM_FTP_STATE_RST,
    QCOM_FTP_STATE_ACP,
    QCOM_FTP_STATE_ACK,
    QCOM_FTP_STATE_NAK,
    QCOM_FTP_STATE_VER,
    QCOM_FTP_STATE_OK,
    QCOM_FTP_STATE_CRC,
    QCOM_FTP_STATE_ACT,
    QCOM_FTP_STATE_FLR,
    QCOM_FTP_MAXSTATES      // end state marker
    } qcom_ftp_enum_states;

typedef struct {
    BYTE    STATE;          // FTP download state (see qcom_ftp_enum_states above)
    WORD    DID;            // Download ID
    BYTE    PollAddress;    // Poll address used for FTP broadcasts
    ULONG   CLEN;           // total bytes downloaded
    ULONG   DCRC;           // CRC of file being dowloaded
    } qc_ftpdsrtype;        // Download Status Response

//  ...................................................................
//  Progressive Meters Response  QCOM_PMR_FC

#define QCOM_REMAX_PMR      QCOM_MAX_PROGR_LEV_PER_GAME

typedef struct {
    union {
        BYTE    PLEV;
        struct {
            BITS level:3;
            BITS res:4;
            BITS LP:1;
            } bits;
        } PLEV;
    ULONG   CAMT;       // cents
    } qc_pmrretype;     // repeated entry in progressive meters response

typedef struct {        //  Progressive Meters Response QCOM v1.5
    WORD    GVN;
    WORD    PGID;
    ULONG   PAMT;       // cents
    BYTE    NUM;
    qc_pmrretype re[QCOM_REMAX_PMR];
    } qc_pmrtype;

#define QCOM_PMR_LENGTH(msgptr)     (sizeof(qc_pmrtype) - sizeof(qc_pmrretype)*QCOM_REMAX_PMR + msgptr->Data.pmr.NUM*sizeof(qc_pmrretype))

typedef struct {
    union {
        BYTE    PLEV;
        struct {
            BITS level:3;
            BITS res:4;
            BITS LP:1;
            } bits;
        } PLEV;
    ULONG   CAMT;           // cents
    WORD    HITS;
    ULONG   WINS;           // cents
    float   HRATE;          // (units: probability per cent bet)
    } qc_pmrretype2;        // repeated entry in progressive meters response qCOM v1.6

typedef struct {            //  Progressive Meters Response QCOM v1.6
    WORD    GVN;
    WORD    PGID;
    ULONG   PAMT;           // cents
    BYTE    ONUM;           // Reserved (used to be NUM in QCOM v1.5)
    BYTE    PVAR;           // reserved
    ULONG   PMRRES;         // reserved
    BYTE    NUM;
    BYTE    SIZ;            // new. fixed @ sizeof(qc_pmrretype2)
    qc_pmrretype2 re[QCOM_REMAX_PMR];
    } qc_pmrtype2;  // QCOM v1.6

// The following macro returns the length of the progressive meters resposne
#define QCOM_PMR_LENGTH2(msgptr)    (sizeof(qc_pmrtype2) - sizeof(qc_pmrretype2)*QCOM_REMAX_PMR + msgptr->Data.pmr2.NUM*msgptr->Data.pmr2.SIZ)

// Macro for building/accessing repeated PMR2 repeated entries with customised SIZ's
// Pre-conditions: qcrt must be defined & SIZ valid
#define QCOM_PMRRE2(i) ((qc_pmrretype2 *)((BYTE *)qcrt->Data.pmr2.re + (i)*qcrt->Data.pmr2.SIZ))
// Usage e.g. QCOM_PMRRE2(0)->CAMT


// The following macros are for working with HRATELSB & HRATEMSB
// The following macro converts uint64 (MSB|LSB) to a double
//#define QCOM_DOUBLE_HRATE(LSB, MSB) ((double)LSB + (double)MSB * 4294967296.0)
// The following macros convert a double to create HRATELSB & HRATEMSB field for inserton into PM Response
// dhitrate must be a double prec. floating point no.
//#define QCOM_HRATEMSB(dhitrate) ((ULONG)((double)dhitrate / 4294967296.0))
//#define QCOM_HRATELSB(dhitrate) ((ULONG)((double)dhitrate - (double)QCOM_HRATEMSB((double)dhitrate) * 4294967296.0))

//  ...................................................................
//  Progressive Configuration Response QCOM_PCR_FC

typedef struct {            // Repeated entry type for PCR
        union {
        BYTE    PLF;
        struct {
            BITS    lev:3;
            BITS    res:4;
            BITS    saplp:1; // QCOM v1.6.1
            } bits;
        } PLF;
        ULONG       SUP;    // cents
        ULONG       PINC;   // x 10000 use QCOM_PINC_MULT (e.g. 2.5385% = 25385)
        ULONG       CEIL;   // cents
        ULONG       AUXRTP; // RTP x 10000 reserved (e.g. 2.5385% = 25385)
        } qc_pcrretype;

// Macros to access Progressive Configuration Response repeated entry data with a custom SIZ
// Pre-condition: qcrt must be defined and SIZ must valid
#define QCOM_PCRRE(i) ((qc_pcrretype *)(((BYTE *)&qcrt->Data) + sizeof(qc_pcrtype) + (i)*qcrt->Data.pcr.SIZ))
// E.g. QCOM_PCRRE(i)->SUP

typedef struct {
    WORD    GVN;
    BYTE    VAR;
    ULONG   RES;    // reserved
    union {
        BYTE    NUM;    // 1..8
        struct {
            BITS    num:4;
            BITS    res:3;
            BITS    customSAP:1;
            } bits;
        } NUM;
    BYTE    SIZ;
    // Repeated entry field follows here
    //qc_pcrretype re[QCOM_MAX_PROGR_LEV_PER_GAME];
    } qc_pcrtype;

// The following macro returns the length of the Progressive Configuration Response message data
#define QCOM_PCR_LENGTH(msgptr) (sizeof(qc_pcrtype) + msgptr->Data.pcr.NUM.bits.num*msgptr->Data.pcr.SIZ)
//#define QCOM_PCR_LENGTH   (sizeof(qc_pcrtype) - sizeof(qc_pcrretype)*QCOM_MAX_PROGR_LEV_PER_GAME + qcrt->Data.pcr.NUM.bits.num*sizeof(qc_pcrretype))

//  ...................................................................
//  Bet Meters Response (QCOM v1.6) QCOM_BMR_FC

#define QCOM_BMR_MAX_CMET   50

typedef struct {
    WORD    GVN;
    BYTE    RESV;
    ULONG   RESD;
    WORD    FLG;
    WORD    MAXL;       // max lines
    WORD    MAXB;       // max bet per line
    WORD    GFA;
    WORD    GFB;
    ULONG   RESE;
    BYTE    SIZ;
    ULONG   CMET[QCOM_BMR_MAX_CMET];
    } qc_bmrtype;

#define QCOM_BMR_LENGTH(msgptr)  (sizeof(qc_bmrtype) - (QCOM_BMR_MAX_CMET*sizeof(ULONG) - msgptr->Data.bmr.GFA*msgptr->Data.bmr.GFB*msgptr->Data.bmr.SIZ))

// Macro for building/accessing repeated BMR repeated entries with customised SIZ's
// Pre-conditions: qcpt must be defined & GFA,GFB & SIZ must be valid
#define QCOM_BMRRE(i) ((ULONG *)((BYTE *)qcrt->Data.bmr.CMET + (i)*qcrt->Data.bmr.SIZ))
// Usage e.g. *QCOM_BMRRE(i)

//  ...................................................................
//  Player Choice Meters Response (QCOM v1.6) QCOM_PCMR_FC

#define QCOM_PCMR_MAX_RE    16

typedef struct {
    WORD    GVN;
    WORD    FLG;
    BYTE    NUM;
    BYTE    SIZ;
    ULONG   PMET[QCOM_PCMR_MAX_RE];
    } qc_pcmrtype;

#define QCOM_PCMR_LENGTH(msgptr)  (sizeof(qc_pcmrtype) - (QCOM_PCMR_MAX_RE - msgptr->Data.pcmr.NUM)*msgptr->Data.pcmr.SIZ)

// Macros to access PCMR repeated entry data with a custom SIZ
// Pre-condition: qcrt must be defined and SIZ must valid
#define QCOM_PCMR(i) ((ULONG *)(((BYTE *)&qcrt->Data.pcmr.PMET) + (i)*qcrt->Data.pcmr.SIZ))
// E.g. *QCOM_PCMR(i)


//  ...................................................................
//  Multi-Game/Variation Meters Response QCOM_MGVMR_FC

#define QCOM_METERS_PER_VAR     5   // (QCOM v1.5 had only 4)

enum QCOM_Multigame_Meter_ID {
    QCOM_MGVM_STROKE,
    QCOM_MGVM_TURNOVER,
    QCOM_MGVM_WINS,
    QCOM_MGVM_PWINS,
    QCOM_MGVM_GAMESWON
    };

typedef struct {
    WORD    GVN;
    BYTE    VAR;
    union {
        struct {
            ULONG   STR;
            ULONG   TURN;   // cents
            ULONG   WIN;    // cents
            ULONG   PWIN;   // cents
            ULONG   GWON;   // cents QCOM v1.6 EGMs only !!!
            } meter;
        ULONG   meterarray[QCOM_METERS_PER_VAR];
        } meters;
    } qc_mgvmrtype;


//  ...................................................................
//  Meter Group/Contribution Response  QCOM_MGCR_FC

#define QCOM_REMAX_MGCR     8       // maximum group meters per response
#define QCOM_MGCR_PLVL      0xFF    // PLVL field constant

typedef struct {
    BYTE    MGID;
    ULONG   MET;
    } qc_mgcrretype;    // repeated entry type

//  Meter Group Response - Link Progressive contribution
typedef struct {
    WORD    PGID;
    BYTE    PLVL;       // not used, must be set to 0xff
    ULONG   CAMT;       // cents
    } qc_progrconttype;

typedef struct {
    union {
        BYTE    FLG;
        struct {
            BITS num:3; // No. of meters in response minus one
            BITS res:4;
            BITS LP:1;
            } bits;
        } FLG;
    WORD    LGVN;
    qc_mgcrretype re[QCOM_REMAX_MGCR];
    } qc_mgcrtype;

//  Use the following define to access the LP contribution
//  This macro must be used because the LP contr. may start
//  at a different point depending on how many meters were in the response)
//  e.g. qc_qsrtpcont->PGID = 0x1233;
#define qc_qsrtpcont ((qc_progrconttype *)(&qcrt->Data.mgcr.re[qcrt->Data.mgcr.FLG.bits.num+1]))

//-------------------------------------------------------------------------
//  ECT to EGM Acknowlegdement Response  QCOM v1.6+  QCOM_ECTEGMACKR_FC

typedef struct {
    BYTE    FLG;
    BYTE    ECTPSN;
    ULONG   TCCIM;      // Total EGM Cashles Credit In Meter (Group ID 0x14)
    } qc_ectacktype;

//-------------------------------------------------------------------------
//  Purge Events Poll Acknowledgement Response QCOM_PEPAR_FC

#define QCOM_PEPAR_V1_5_LEN 0   // Length of QCOM v1.5 response

typedef struct {
    BYTE FLG;           // not used (QCOM v1.6+)
    BYTE PPSN;          // purge events poll PSN (QCOM v1.6+ only)
    } qc_pepacktype;

//-------------------------------------------------------------------------
//  Extended Event Data Types

//	RAM Cleared event extended data QC_EXT_EC_EGM_RAM_CLEARED
typedef struct {
    qc_serialnotype SN;
    } qc_extd_ramcleared;

//      Link Progressive Award extended event data QC_EXT_EC_LP_AWARD
typedef struct {
    WORD    GVN;
    BYTE    VAR;
    WORD    PGID;
    union {
        BYTE PLVL;
        struct {
            BITS    level:3;
            BITS    res:5;
            } bits;
        } PLVL;
    ULONG   PAMT;           // cents
    } qc_extd_lpaward;

//      Stand Alone Progressive Award extended event data QC_EXT_EC_SAP_AWARD QC_EXT_EC_SAP_UN_V2
typedef struct {
    WORD    GVN;
    BYTE    VAR;
    union {
        BYTE PLVL;
        struct {
            BITS    level:3;
            BITS    res:5;
            } bits;
        } PLVL;
    ULONG   PAMT;           // cents
    } qc_extd_sapaward;

//	Large Win or Jackpot extended event data QC_EXT_EC_LARGE_WIN
typedef struct {
    WORD    GVN;
    BYTE    VAR;
    ULONG   WAMT;           // cents
    } qc_extd_largewin;

//  NPWINP hit extended event data
typedef struct {
    WORD    GVN;
    BYTE    VAR;
    } qc_extd_npwinp;

//  Cancel Credit extended event data
typedef struct {
    ULONG CC;               // cents
    } qc_extd_cc;

//	DLSA/CRE Hit event extended data QC_EXT_EC_DLSA
typedef struct {
    union {
        BYTE LEV;
        struct {
            BITS    level:3;
            BITS    res:2;
            BITS    lockup:1;
            BITS    res1:2;
            } bits;
        } LEV;
    ULONG   RATE;
    } qc_extd_DLSAhit;

typedef struct {
    union {
        BYTE    YESNO;
        struct {
            BITS    yesno:1;
            BITS    res:7;
            } bits;
        } YESNO;
    } qc_extd_SLUR;

//  Game Variation Enabled, New Game Selected extended & Inv. Progr Cfg extended event data
//  QC_EXT_INVALID_PROGR_CONFIGURATION QC_EC_NP_TOP_PRIZE_HIT QC_EXT_EC_NEW_GAME_SELECTED
//  QC_EXT_EC_VAR_ENABLED
typedef struct {
    WORD    GVN;
    BYTE    VAR;
    } qc_extd_gve;

//	Refill Recorded extended event data  QC_EXT_EC_REFILL_RECORDED
typedef struct {
    ULONG   AMT;            // cents
    } qc_extd_refill;

//	Cash Ticket Printed extended event data QC_EXT_EC_TICKET_PRINTED
typedef struct {
    qc_serialnotype SN;
    ULONG       TAC;
    ULONG       AMT;        // cents
    } qc_extd_ticket;

//	Hopper Payout extended event data QC_EXT_EC_HOPPER_PAYOUT
typedef struct {
    ULONG   AMT;            // cents
    } qc_extd_hopper;

//	Residual CC Lockup extended event data  QC_EXT_EC_RES_CC_LOCKUP
typedef struct {
    WORD    CC;
    } qc_extd_rcc;  // QCOM v1.5

typedef struct {    // QC_EXT_EC_RES_CC_LOCKUP_V2
    ULONG   CC;     // cents
    } qc_extd_rcc2; // QCOM v1.6

#define QCOM_MAN_SPEC_FAULT_MAX_TSIZE   16

typedef struct {
    char    REASON[QCOM_MAN_SPEC_FAULT_MAX_TSIZE];
    } qc_extd_manspecfault;  // QCOM v1.6

typedef struct { // QC_EC_DENOM_CHANGED
    ULONG   DEN;            // cents
    } qc_extd_egmdenchanged;

typedef struct { // QC_EC_HOPPER_LEVEL_MISMATCH
    long signed int SML;
    } qc_extd_hoplevmis;

//  Progressive Configuration Changed extended event data QC_EC_PRGSSVE_CFG_CHANGED (QCOM v1.6)
typedef struct {
    WORD    GVN;
    BYTE    VAR;
    ULONG   PRTP;     // GVN's total progressive RTP x 10000 (e.g. 2.5385% = 25385)
    } qc_extd_pcfgc;

//	ECT from EGM extended event data QC_EXT_EC_ECT_FROM_EGM
typedef struct {
    ULONG   CAMT;               // BCD in cents
    } qc_extd_ectegm;
typedef struct {
    ULONG   CAMT;               // Hex in cents
    } qc_extd_ectegm2;

// Cash Ticket Out Lockup Request QC_EXT_EC_TICKET_OUT_LOCKUP
typedef struct {
    WORD    TSER;
    ULONG   TAMT;               // cents
    } qc_extd_ticketout;

// Cash Ticket Out Print Successful QC_EXT_EC_TO_PRINT_OK
typedef struct {
    WORD    TSER;
    ULONG   TAMT;               // cents
    } qc_extd_ticketoutok;

// Cash Ticket Out Print Failure QC_EXT_EC_TICKET_PRINT_FAIL
typedef struct {
    WORD                TSER;
    ULONG               TAMT;   // cents
    } qc_extd_ticketoutfail;

// Cash Ticket In Request QC_EXT_EC_TICKET_IN_REQUEST, QC_EC_TICKET_IN_TIMEOUT, QC_EC_TICKET_IN_ABORTED
typedef struct {
    qc_titoauthnotype authno;
    } qc_extd_ticketinreq;

typedef struct
{
    u8 HOURS;
    u8 MINUTES;
    u8 CID;
    u32 SPD;
    int NWL;
    u8 SPS;
} qc_extd_pidsessionstop;

// Hopper Overpay Amount QC_EC_HOPPER_OPAY_AMT
typedef struct {
	ULONG	AMT;            // cents
	} qc_extd_opayamt;

// EGM License Key Missing/Failure QC_EC_LIC_KEY_FAIL, QC_EC_LIC_KEY_DETECTED, QC_EC_NON_PROD_LIC_KEY_DETECTED
typedef struct {
    BYTE    KEYID[8];
    } qc_extd_lickey;

//  Extended event data types
//  -------------------------

#define QCOM_MAX_EXT_EVENT_SIZE     16  // Maximum bytes

typedef union {
    BYTE                    EXTD[QCOM_MAX_EXT_EVENT_SIZE];
    qc_extd_ramcleared      RC;
    qc_extd_lpaward         LP;
    qc_extd_sapaward        SAP;
    qc_extd_largewin        LW;
    qc_extd_npwinp          NPWINP;
    qc_extd_cc              CC;
    qc_extd_gve             GVE;
    qc_extd_refill          R;
    qc_extd_ticket          TP;
    qc_extd_hopper          HP;
    qc_extd_rcc             RCCL;
    qc_extd_rcc2            RCCL2;
    qc_extd_ectegm          ECTEGM;
    qc_extd_ectegm2         ECTEGM2;
    qc_extd_DLSAhit         DLSA;
    qc_extd_SLUR            SLUR;
    qc_extd_manspecfault    MSF;
    qc_extd_egmdenchanged   EGMDC;
    qc_extd_hoplevmis       HLM;
    qc_extd_pcfgc           PRGCC;
    qc_extd_ticketout       TOR;
    qc_extd_ticketoutok     TOPOK;
    qc_extd_ticketoutfail   TOPFAIL;
    qc_extd_ticketinreq     TIR;
    qc_extd_pidsessionstop  PIDSTOP;
    qc_extd_opayamt         HOPAMT;
    qc_extd_lickey          KEYID;
    } qc_exteventtypes;

//-----------------------------------
//  Event Response   QCOM_ER_FC

typedef struct {
    BYTE    SEQ;
    WORD    ECOD;
    union {
        BYTE    ESIZ;
        struct {
            BITS size:5;
            BITS res:3;
            } bits;
        } ESIZ;
    qc_timedatetype TIMEDATE;
    //  Optional extended event data follows here
    qc_exteventtypes    EXTD;
    } qc_ertype;


//  ...................................................................
//  EGM Game Config Response  QCOM_EGMGCR_FC

#define QCOM_REMAX_EGMGCR   QCOM_MAX_VAR_PER_GAME

typedef struct {
    BYTE    VAR;
    WORD    PRET;       // BCD value in QCOM v1.5, hex x 100 in QCOM v1.6
    } qc_egmgcrretype;

typedef struct {
    WORD    GVN;
    BYTE    VAR;
    union {
        BYTE    FLG;
        struct {
            BITS res:4;
            BITS LPonly:1;          // QCOM v1.6 only
            BITS varlock:1;         // QCOM v1.6 only
            BITS hotswitching:1;
            BITS GEF:1;
            } bits;
        } FLG;
    BYTE    PNUM;
    BYTE    PLBM;
    WORD    PGID;
    BYTE    NUM;
    BYTE    SIZ;
    qc_egmgcrretype re[QCOM_REMAX_EGMGCR];
    } qc_egmgcrtype;

#define QCOM_EGMGCR_LENGTH(msgptr) (sizeof(qc_egmgcrtype)-sizeof(qc_egmgcrretype)*QCOM_REMAX_EGMGCR+msgptr->Data.egmgcr.NUM*msgptr->Data.egmgcr.SIZ)

// Macro for building/accessing repeated GCR repeated entries with customised SIZ's
// Pre-conditions: qcrt must be defined & SIZ valid
#define QCOM_GCRRE(i) ((qc_egmgcrretype *)((BYTE *)qcrt->Data.egmgcr.re + (i)*qcrt->Data.egmgcr.SIZ))
// Usage e.g. QCOM_GCRRE(i)->VAR

//  ...................................................................
//  EGM Configuration Response  QCOM_EGMCR_FC

#define QCOM_EGMCR_FLGA_MASK_MECHM  0x01
#define QCOM_EGMCR_FLGA_MASK_RTC    0x02
#define QCOM_EGMCR_FLGA_MASK_PROGR  0x04
#define QCOM_EGMCR_FLGA_MASK_TOUCH  0x08
#define QCOM_EGMCR_FLGA_MASK_TP     0x10
#define QCOM_EGMCR_FLGA_MASK_COIN   0x20
#define QCOM_EGMCR_FLGA_MASK_HOPPER 0x40
#define QCOM_EGMCR_FLGA_MASK_NOTE   0x80

#define QCOM_EGMCR_FLGB_MASK_AUXD   0x40
#define QCOM_EGMCR_FLGB_MASK_TI     0x80


typedef struct {
    qc_serialnotype SN;
    BYTE    NPRV;
    union {
        BYTE    FLGA;
        struct {
            BITS res:1;
            BITS RTC:1;
            BITS Progr:1;
            BITS Touch:1;
            BITS Ticket:1;
            BITS Coin:1;
            BITS Hopper:1;
            BITS Note:1;
            } bits;
        } FLGA;
    BYTE    FLGB;
    WORD    DEN;
    WORD    TOK;
    WORD    BGVN;
    BYTE    NUMG;
    BYTE    NUME;
    WORD    LGVN;
    BYTE    LVAR;
    } qc_egmcrtype; // QCOM v1.5 version

typedef struct {
    qc_serialnotype SN;
    BYTE    NPRV;
    union {
        BYTE    FLGA;
        struct {
            BITS MechMeters:1;  // v1.6
            BITS RTC:1;
            BITS Progr:1;
            BITS Touch:1;
            BITS Ticket:1;
            BITS Coin:1;
            BITS Hopper:1;
            BITS Note:1;
            } bits;
        } FLGA;
    union {
        BYTE    FLGB;
        struct {
            BITS res:6;
            BITS AuxDiplay:1;   // v1.6
            BITS TicketIn:1;    // v1.6
            } bits;
        } FLGB;
    WORD    OLDDEN;     // not used in QCOM v1.6
    WORD    OLDTOK;     // not used in QCOM v1.6
    WORD    BGVN;
    BYTE    NUMG;
    BYTE    NUME;
    WORD    LGVN;
    BYTE    LVAR;

    // QCOM v1.6 additional fields
    BYTE    FLGSL;
    union {
        BYTE FLGSH;
        struct {
            BITS res:6;
            BITS SharedProgressive:1;
            BITS DenomSwitching:1;
            } bits;
        } FLGSH;
    ULONG   DEN2;       // hex cents
    ULONG   TOK2;       // hex cents
    ULONG   MAXDEN;     // hex cents
    ULONG   RES;
    WORD    MINRTP;     // hex * 100
    WORD    MAXRTP;     // hex * 100
    WORD    MAXSD;      // hex
    WORD    MAXLINES;   // hex
    ULONG   MAXBET;     // hex cents
    ULONG   MAXNPWIN;   // hex in cents
    ULONG   MAXPWIN;    // hex in cents
    ULONG   MAXECT;     // hex in cents

    } qc_egmcrtype2; // QCOM v1.6 version

//  ...................................................................
//  Program Hash Response QCOM_PHR_FC

typedef struct {
    BYTE hash[QCOM_MAX_PHA_SH_LENGTH];
    } qc_phr2hashtype; // QCOM v1.6

typedef struct {
    BYTE            RES;
    qc_phr2hashtype HASH;
    } qc_phrtype2;   // QCOM v1.6

typedef struct {
    BYTE    RES;
    ULONG   HASHL;
    ULONG   HASHH;
    } qc_psrtype;   // QCOM v1.5


//  ...................................................................
//  DLSA/CRE Status Response  QCOM_DLSASR_FC

#define QCOM_REMAX_DLSASR   QCOM_MAX_DLSA

typedef struct {
    union {
        BYTE    LEV;
        struct {
            BITS level:3;
            BITS res:2;
            BITS lockup:1;
            BITS lockedup:1;
            BITS res1:1;
            } bits;
        } LEV;
    ULONG   RATE;
    } qc_dlsasrretype;      // repeated entry for DLSA/CRE Status Response

typedef struct {
    BYTE    NUM;
    BYTE    SIZ;
    qc_dlsasrretype re[QCOM_REMAX_DLSASR];
    } qc_dlsasrtype;

// Macro for building/accessing repeated DLSACR repeated entries with customised SIZ's
// Pre-conditions: qcpt must be defined & SIZ valid
#define QCOM_DLSACRRE(i) ((qc_dlsasrretype *)((BYTE *)qcrt->Data.dlsasr.re + (i)*qcrt->Data.dlsasr.SIZ))
// Usage e.g. QCOM_DLSACRRE(0)->RATE

//  ...................................................................
//  Seek EGM Broadcast Response  QCOM_SEEKEGM_FC (QCOM v1.6)

typedef struct {
    qc_serialnotype SN;
    } qc_segmbrtype;


//-------------------------------------------------------------------------
//  QCOM Poll Message Types
//

//  ...................................................................
//  FTP Commence Download Request Poll
typedef struct {
    WORD    DID;
    BYTE    PollAddress;    // Poll address used for FTP broadcasts
    ULONG   DLEN;
    ULONG   DCRC;
    } qc_ftpcdrptype;

//  ...................................................................
//  EGM Configuration Poll    QCOM_EGMCP_FC

#define QCOM_JUR_AU_QLD_CLUBS_HOTELS	0x00
#define QCOM_JUR_AU_QLD_CASINOS	        0x01
#define QCOM_JUR_NZ	                    0x02
#define QCOM_JUR_AU_VIC	                0x03
#define QCOM_JUR_MAX                    0x03

typedef struct {
    qc_serialnotype SN;
    BYTE    JUR;        // hex
    BYTE    FLG;
    WORD    DEN;        // BCD
    WORD    TOK;        // BCD
    } qc_egmcptype;     // QCOM v1.5 version

typedef struct {
    qc_serialnotype SN;
    BYTE    JUR;        // hex
    BYTE    FLG;
    WORD    DEN;        // BCD
    WORD    TOK;        // BCD
    // QCOM v1.6 additional fields
    ULONG   DEN2;       // hex cents
    ULONG   TOK2;       // hex cents
    ULONG   MAXDEN;     // hex cents
    ULONG   RES;
    WORD    MINRTP;     // hex * 100
    WORD    MAXRTP;     // hex * 100
    WORD    MAXSD;      // hex
    WORD    MAXLINES;   // hex
    ULONG   MAXBET;     // hex cents
    ULONG   MAXNPWIN;   // hex in cents
    ULONG   MAXPWIN;    // hex in cents
    ULONG   MAXECT;     // hex in cents
    } qc_egmcptype2;    // QCOM v1.6 version

//  ...................................................................
//  EGM Configuration Request Poll  QCOM_EGMCRP_FC
typedef struct {
    BITS res:5;         // Reserved
    BITS MEF:1;         // Set/Reset Machine Enabled Flag
    BITS GCR:1;         // Request all Game Configs
    BITS PSN:1;         // Reset all PSNs
    } qc_egmcrptype;


//  ...................................................................
//  EGM Game Configuration Poll  QCOM_EGMGCP_FC

#define QCOM_REMAX_EGMGCP   QCOM_MAX_PROGR_LEV_PER_GAME

typedef union {             // GLFG field
    BYTE    GFLG;
    struct {
        BITS res:6;
        BITS varlock:1;     // QCOM v1.6 egms only
        BITS GEF:1;
        } bits;
    } qc_egmgcptype_GFLG;

typedef struct {
    union {
        BYTE    PFLG;
        struct {
            BITS res:7;
            BITS LP:1;
            } bits;
        } PFLG;
    ULONG   CAMT;           // cents
    } qc_egmcpretype;

typedef struct {
    WORD GVN;
    BYTE VAR;
    qc_egmgcptype_GFLG GFLG;
    WORD    PGID;
    BYTE    PNUM;
    BYTE    SIZ;
    qc_egmcpretype  re[QCOM_REMAX_EGMGCP];
    } qc_egmgcptype;

// Macro for building/accessing repeated GCP entires with customised SIZ's
// Pre-conditions: qcpt must be defined & SIZ valid
#define QCOM_EGMGCPRE(i) ((qc_egmcpretype *)((BYTE *)qcpt->Data.egmgcp.re + (i)*qcpt->Data.egmgcp.SIZ))
// Usage e.g. QCOM_EGMGCPRE(0)->CAMT

//  ...................................................................
//  Game Configuration Change Poll  QCOM_EGMGCCP_FC
typedef struct {
    WORD    GVN;
    BYTE    VAR;
    union {
        BYTE    GFLG;
        struct {
            BITS res:7;
            BITS GEF:1;
            } bits;
        } GFLG;
    WORD    PGID;
    } qc_egmvcptype;

//  ...................................................................
//  EGM Parameters Poll  QCOM_EGMPP_FC
typedef struct {   // QCOM v1.6
    union {
        BYTE    FLG;
        struct {
            BITS res:5;
            BITS resfeat:1;
            BITS autoplay:1;
            BITS crlimitmode:1;
            } bits;
        } FLG;
    BYTE    OPR;
    ULONG   LWIN;           // cents
    ULONG   CRLIMIT;        // cents
    BYTE    DUMAX;
    ULONG   DULIMIT;        // cents
    // New fields for QCOM v1.6:
    short signed int TZADJ; // units of minutes
    ULONG   PWRTIME;        // seconds
    BYTE    PID;
    WORD    EODT;           // End of Day Time.  Units: Minutes since midnight (0...1439)
    ULONG   NPWINP;         // cents
    ULONG   SAPWINP;        // cents
    } qc_egmpptype2;

typedef struct {    // QCOM v1.5
    union {
        BYTE    FLG;
        struct {
            BITS res:8;
            } bits;
        } FLG;
    BYTE    OPR;
    ULONG   LWIN;           // cents
    ULONG   CRLIMIT;        // cents
    BYTE    DUMAX;
    ULONG   DULIMIT;        // cents
    } qc_egmpptype;

//  ...................................................................
//  Progressive Configuration Poll (QCOM v1.6 only) QCOM_PCP_FC

#define QCOM_REMAX_PCP  QCOM_MAX_PROGR_LEV_PER_GAME

typedef struct {
    union {
        BYTE    PLF;
        struct {
            BITS lev:3;
            BITS res:5;
            } bits;
        } PLF;
    ULONG SUP;          // cents
    ULONG PINC;         // x 10000 use QCOM_PINC_MULT (e.g. 2.5385% = 25385)
    ULONG CEIL;         // cents
    ULONG AUXRTP;       // x 10000 use QCOM_PINC_MULT (e.g. 2.5385% = 25385)
    } qc_pcpretype;

typedef struct {
    WORD GVN;
    WORD RES;
    ULONG FLG;
    BYTE NUM;
    BYTE SIZ;
    qc_pcpretype re[QCOM_REMAX_PCP];
} qc_pcptype;

// Macro for building/accessing repeated PCP repeated entries with customised SIZ's
// Pre-conditions: qcpt must be defined & SIZ valid
#define QCOM_PCPRE(i) ((qc_pcpretype *)((BYTE *)qcpt->Data.pcp.re + (i)*qcpt->Data.pcp.SIZ))
// Usage e.g. QCOM_PCPRE(0)->SUP

//  ...................................................................
//  External Jackpot Information Poll QCOM_EXTJIP_FC QCOM v1.6

#define QCOM_EXTJIP_MAX_LNAME   16
#define QCOM_EXTJIP_RTP_MULT    100.0
#define QCOM_REMAX_EXTJIP       QCOM_MAX_PROGR_LEV_PER_GAME

typedef struct {
    WORD    PGID;           // 0x0001 ... 0xfffe
    union {
        WORD    LFLG;
        struct {
            WORD lev:3;     // 0...7
            WORD res:13;
            } bits;
        } LFLG;
    char    LNAME[QCOM_EXTJIP_MAX_LNAME];  // NULL term and padded with 0x00
    } qc_extjipretype;

typedef struct {
    ULONG   RES;
    WORD    RTP;    // %RTP x QCOM_EXTJIP_RTP_MULT
    union {
        WORD    FLG;
        struct {
            WORD    num:4;         // 0...8
            WORD    res1:3;
            WORD    display:1;
            WORD    res2:8;
            } bits;
        struct {
            BYTE    LSB;
            BYTE    MSB;
            } bytes;
        } FLG;
    BYTE    SIZ;
    qc_extjipretype re[QCOM_REMAX_EXTJIP];
    } qc_extjiptype;

// Macro for building/accessing repeated EXTJIP repeated entries with customised SIZ's
// Pre-conditions: qcpt must be defined & SIZ valid
#define QCOM_EXTJIPRE(i) ((qc_extjipretype *)((BYTE *)qcpt->Data.extjip.re + (i)*qcpt->Data.extjip.SIZ))
// Usage e.g. QCOM_EXTJIPRE(0)->PGID

//  ...................................................................
//  Program Hash Request Poll QCOM_PHRP_FC

typedef struct {
    BYTE    seed[QCOM_MAX_PHA_SH_LENGTH];  // LSB first
    } qc_phrp2seedtype; // QCOM v1.6

typedef struct {
    union {
        BYTE    RES;
        struct {
            BITS res:6;
            BITS seed:1;
            BITS MEF:1;
            } bits;
        } RES;
    qc_phrp2seedtype SEED;
    } qc_phrptype2; // QCOM v1.6

typedef struct {
    ULONG   SEEDL;
    ULONG   SEEDH;
    } qc_psrpseedtype; // QCOM v1.5

typedef struct {
    union {
        BYTE    RES;
        struct {
            BITS res:6;
            BITS seed:1;
            BITS MEF:1;
            } bits;
        } RES;
    qc_psrpseedtype SEED;
    } qc_psrptype; // QCOM v1.5


//  ...................................................................
//  System Lockup Request Poll  QCOM_SALRP_FC

#define QCOM_SALRP_TEXT_SIZE    80

typedef struct {
    union {
        BYTE    FLG;
        struct {
            BITS res:3;
            BITS resetkeydisable:1;
            BITS continu:1;
            BITS question:1;
            BITS lamptest:1;
            BITS fanfare:1;
            } bits;
        } FLG;
    BYTE    LEN;
    char    TEXT[QCOM_SALRP_TEXT_SIZE];
    } qc_salrptype;

//  ...................................................................
//  Ticket Out Request Acknowledgement Poll QCOM_TORACKP_FC

#define QCOM_TORAP_MAX_CTEXT    80

typedef struct {
    union {
        BYTE    FLG;
        struct {
            BITS    res:6;
            BITS    onfail:1;
            BITS    success:1;
            } bits;
        } flg;
    ULONG   RES;
    WORD    TSER;
    ULONG   TAMT;                   // cents
    qc_timedatetype     timedate;
    qc_titoauthnotype   authno;
    ULONG   RES1;
    BYTE    CLEN;
    char    CTEXT[QCOM_TORAP_MAX_CTEXT];
    } qc_torackptype;

//  ...................................................................
//  Ticket In Request Acknowledgement Poll QCOM_TIRACKP_FC

enum QCOM_TIR_FCODES {
    QCOM_TIR_FOCDE_SUCCESS,
    QCOM_TIR_FOCDE_SYS_UNAVAIL,
    QCOM_TIR_FOCDE_EXP,
    QCOM_TIR_FOCDE_TOO_LARGE,
    QCOM_TIR_FOCDE_INVALID,
    QCOM_TIR_FOCDE_NOT_FOUND,
    QCOM_TIR_FOCDE_REDEEMED
    };

typedef struct {
    ULONG   FLG;
    BYTE    FCODE;                  // see enum above
    ULONG   TAMT;                   // cents
    qc_titoauthnotype   authno;
    } qc_tirackptype;

//  ...................................................................
//  ECT Lockup Reset Poll  QCOM_ECTLRP_FC
typedef struct {
    union {
        BYTE    CFUNC;
        struct {
            BITS    success:1;
            BITS    res:7;
            } bits;
        } CFUNC;
    } qc_ectlrptype;


//  ...................................................................
//  ECT to EGM Poll     QCOM_ECTEGMP_FC

#define QCOM_ECTEGMP_CAMT_SIZE  3           // bytes
#define QCOM_ECTEGMP_MAX        999999l     // Max ECT to EGM amount in cents for QCOM v1.5.x

typedef struct {
    BYTE    PSN;
    union {
        BYTE    FLG;
        struct {
            BITS    res:7;
            BITS   cashlessmode:1;
            } bits;
        } FLG;
    BYTE    CAMT[QCOM_ECTEGMP_CAMT_SIZE];   // BCD
    } qc_ectegmptype;                       // QCOM v1.5

typedef struct {
    BYTE    PSN;
    union {
        BYTE    FLG;
        struct {
            BITS   sourceid:3;
            BITS   res:4;
            BITS   cashlessmode:1;
            } bits;
        } FLG;
    BYTE    OCAMT[QCOM_ECTEGMP_CAMT_SIZE];  // BCD v1.5 only
    ULONG CAMT;                             // cents hex
    } qc_ectegmptype2;                      // QCOM v1.6 version

//  ...................................................................
//  EGM General Maintenance Poll  QCOM_EGMGMP_FC
typedef struct {
    BYTE    BLK;
    union {
        BYTE    FLG;
        struct {
            BITS res:6;
            BITS nasr:1;    // QCOM v1.6 Request Note Acceptor Status Response
            BITS MEF:1;
            } bits;
        } FLG;
    BYTE    NUM;
    BYTE    SIZ;
    WORD    GVN;
    BYTE    VAR;
    union {
        BYTE    GFLG;
        struct {
            BITS qPCMR:1;       // Request the Player Choice Meters Response
            BITS qBMR:1;        // Request the Bet Meters Response
            BITS qPCR:1;        // Request the Progressive Configuration Response
            BITS qGmeCfg:1;     // Request EGM Game Configuration bit
            BITS qLP:1;         // Request LP meters bit
            BITS qSAP:1;
            BITS qmgmeters:1;   // Request multi-game meters bit
            BITS GEF:1;
            } bits;
        } GFLG;
    } qc_egmgmptype;

//  ...................................................................
//  Purge events Poll  QCOM_PEP_FC
typedef struct {
    BYTE    PSN;
    BYTE    FLG;
    BYTE    EVTNO;
    } qc_peptype;


//  ...................................................................
//  DLSA/CRE Lockup Acknowledgement Poll   QCOM_DLSALAP_FC

#define QCOM_DLSALAP_TEXT_SIZE  80      // QCOM v1.6 up from 40

typedef struct {
    BYTE    LEN;
    char    TEXT[QCOM_DLSALAP_TEXT_SIZE];
    } qc_dlsalaptype;


//  ...................................................................
//  DLSA/CRE Configuration Poll  QCOM_DLSACP_FC

#define QCOM_REMAX_DLSACP   QCOM_MAX_DLSA

typedef struct {
    union {
        BYTE    LEV;
        struct {
            BITS    level:3;
            BITS    res:2;
            BITS    lockup:1;
            BITS    res1:1;
            BITS    enable:1;
            } bits;
        } LEV;
    ULONG   RATE;
} qc_dlsacpretype;  // DLSA/CRE Configuration Poll repeated entry type

typedef struct {
    union {
        BYTE    NUM;
        struct {
            BITS    NUM:4;
            BITS    res:3;
            BITS    queueresponse:1;
            } bits;
        } NUM;
    BYTE    SIZ;
    qc_dlsacpretype re[QCOM_REMAX_DLSACP];
    } qc_dlsacptype;

// Macro for building/accessing repeated DLSACP repeated entries with customised SIZ's
// Pre-conditions: qcpt must be defined & SIZ valid
#define QCOM_DLSACPRE(i) ((qc_dlsacpretype *)((BYTE *)qcpt->Data.dlsacp.re + (i)*qcpt->Data.dlsacp.SIZ))
// Usage e.g. QCOM_DLSACPRE(0)->RATE

//  ...................................................................
//  Note Acceptor Maint Poll    QCOM_NAMP_FC
typedef struct {
    union {
        WORD    NAFLG;
        struct {
            BYTE LSB;
            BYTE MSB;
            } bytes;
        struct {
            BYTE LSB;
            BITS five:1;
            BITS ten:1;
            BITS twenty:1;
            BITS fifty:1;
            BITS hundred:1;
            BITS res:3;
            } bits;
        } NAFLG;
    } qc_namptype;

//  ...................................................................
//  Hopper/Ticker Printer Maint Poll   QCOM_HTPMP_FC
typedef struct {        // QCOM v1.5
    ULONG   REFILL;     // cents
    ULONG   COLLIM;     // cents
    ULONG   TICKET;     // cents
    } qc_htpmptype;

typedef struct {        // QCOM v1.6
    ULONG   REFILL;     // cents
    ULONG   COLLIM;     // cents
    ULONG   TICKET;     // cents
    union {
        WORD    FLG;
        struct {
            BYTE lsb;
            BYTE msb;
            } bytes;
        struct {
            WORD    res:15;
            WORD    PrintTestTicket:1;
            } bits;
        } FLG;
    BYTE RES;
    ULONG   DOREFILL;       // cents new for v1.6
    } qc_htpmptype2;

//  ...................................................................
//  EGM General Reset Poll     QCOM_EGMGRP_FC

typedef struct { // QCOMv1.5
    union {
        BYTE    FLG;
        struct {
            BITS    fault:1;
            BITS    lockup:1;
            BITS    res:6;
            } bits;
        } FLG;
    } qc_egmgrptype;

typedef struct { // QCOMv1.6
    union {
        BYTE    FLG;
        struct {
            BITS    fault:1;
            BITS    lockup:1;
            BITS    res:6;
            } bits;
        } FLG;
    BYTE    STATE;
    } qc_egmgrptype2;


//  ...................................................................
//  Specific Promotional Message poll  QCOM_SPAMA_FC QCOM_SPAMB_FC

#define QCOM_SPAMP_TEXT_SIZE        80        // QCOM v1.6
#define QCOM_SPAMP_TEXT_SIZE_V1_5   40        // QCOM v1.5 was only 40

typedef struct {
    union {
        BYTE    FMT;
        struct {
            BITS res:6;
            BITS prom:1;    // SPAMA prominence flag v1.6.1 only
            BITS fanfare:1;
            } bits;
        } FMT;
    BYTE    LEN;
    char    TEXT[QCOM_SPAMP_TEXT_SIZE];
    } qc_spamptype;

//  ...................................................................
//  Tower Light Maintenance  QCOM_EGMTLMP_FC

typedef struct {
    union {
//      BYTE    FLG[2];
        WORD    FLGS;
        struct {
            struct {
                BITS lowest:1;
                BITS middle:1;
                BITS top:1;
                BITS res:5;
                } on;
            struct {
                BITS lowest:1;
                BITS middle:1;
                BITS top:1;
                BITS res:5;
                } flash;
            } bits;
        struct {
            BYTE    on;
            BYTE    flash;
            } bytes;
        } FLG;
    } qc_egmtlmptype;


//  ...................................................................
//  Extended Broadcast - Linked Progressive Current Amounts QCOM_BMLPCA_FC

#define QCOM_REMAX_BMLPCA   QCOM_MAX_PROGR_LEV_PER_GAME
#define QCOM_LPLVL_MASK     (0xF8)

typedef struct {            // repeated entry for LPCA
    WORD    PGID;
    union {
        BYTE    PLVL;
        struct {
            BITS level:3;   // ID of level
            BITS res:5;     // Also see QCOM_LPLVL_MASK
            } bits;
        } PLVL;
    ULONG   CAMT;           // cents
    } qc_lpcaretype;        // repeated entry for LPCA

typedef struct {
    BYTE    EFUNC;      // extended broadcast function code
    union {
        BYTE    NUM;
        struct {
            BITS levels:3; // 0...7  (minus one)
            BITS res:5;    // Also see QCOM_LPLVL_MASK
            } bits;
        } NUM;
    qc_lpcaretype re[QCOM_REMAX_BMLPCA];
    } qc_lpcatype;


//  ...................................................................
//  Extended Broadcast - General Promotional Message  QCOM_BMGPM_FC

#define QCOM_BMGPM_TEXT_SIZE    80

typedef struct {
    BYTE    EFUNC;  // extended broadcast function code
    union {
        BYTE    FMT;
        struct {
            BITS res:7;
            BITS chime:1;
            } bits;
        } FMT;
    BYTE    LEN;
    char    TEXT[QCOM_BMGPM_TEXT_SIZE];
    } qc_gpmtype;


//  ...................................................................
//  Extended Broadcast - EGM Poll Address Configuration   QCOM_BMEGMPAC_FC

#define QCOM_REMAX_BMEGMPAC 16

typedef struct {
    qc_serialnotype SN;
    BYTE        PADR;
    } qc_egmpacretype;  // repeated entry for EGM PA Config

typedef struct {
    BYTE    EFUNC;  // extended broadcast function code
    BYTE    NUM;
    BYTE    ESIZ;
    qc_egmpacretype re[QCOM_REMAX_BMEGMPAC];
    } qc_egmpactype;

// Macro for building/accessing customised SIZ'ed PAC broadcasts
// Pre-conditions: qcpt must be defined & ESIZ valid
#define QCOM_PACBRE(i) ((qc_egmpacretype *)((BYTE *)qcpt->Data.Broadcast.extd.EXTD.egmpac.re + (i)*qcpt->Data.Broadcast.extd.EXTD.egmpac.ESIZ))
// Usage e.g. QCOM_PACBRE(0)->PADR

//  ...................................................................
//  Extended Broadcast - Site Details  QCOM_BMSD_FC

#define QCOM_BMSD_SLEN      40  // Name of licenced Gaming Venue
#define QCOM_BMSD_LLEN      80  // QCOM v1.6 (was 15 in QCOM v1.5) - Address of licenced gaming venue
#define QCOM_BMSD_LLEN_V1_5 15

typedef struct {
    BYTE    EFUNC;  // extended broadcast function code
    BYTE    FMT;
    BYTE    SLEN;
    BYTE    LLEN;
    char    TEXT[QCOM_BMSD_SLEN+QCOM_BMSD_LLEN];  // These are actually listed as two separate fields in the QCOM doc.
    } qc_sdtype;

//  ...................................................................
//  Broadcast Messages  QCOM_BROADCAST_POLL_FC

typedef struct {
    // optional extended broadcast data types
    union {
        BYTE            EFUNC;  //  Extended broadcast function code
        qc_lpcatype     lpca;   //  Linked Progressive Current Amounts
        qc_gpmtype      gpm;    //  General Promotional Message
        qc_egmpactype   egmpac; //  EGM Poll Address Configuration
        qc_sdtype       sd;     //  Site Details
        } EXTD;
    } qc_broadcastextdtype;

typedef union {
        BYTE    FLG;
        struct {
            BITS res:6;
            BITS clock:1;
            BITS SEF:1;
            } bits;
        } qc_broadcastFLGtype;


typedef struct {
    qc_broadcastFLGtype FLG;
    qc_timedatetype TIMEDATE;
    BYTE    ESIZ;

    qc_broadcastextdtype    extd;   // optional extended data

    } qc_broadcastpolltype;

//  ...................................................................
//  FTP Download Data Broadcast Message  QCOM_BROADCAST_FTP_FC

#define QCOM_FTP_MAX_PACKETSIZE 128

typedef struct {
    WORD    DID;
    BYTE    FLG;
    ULONG   OFFSET;         // current file offsett
    WORD    BLEN;           // current length of following data field
    BYTE    DATA[QCOM_FTP_MAX_PACKETSIZE];          // broadcast data starts here
    } qc_broadcastftptype;


//--------------------------------------------------------------------------
//  Responses Message Data Types

typedef union {
    qc_segmbrtype   segmbr;     //  Seek EGM Broadcast Response (QCOM v1.6)
    qc_psrtype      phr;        //  Program Hash Response QCOM v1.5
    qc_phrtype2     psr2;       //  Program Hash Response QCOM v1.6
    qc_egmcrtype    egmcr;      //  EGM Configuration Response QCOM v1.5
    qc_egmcrtype2   egmcr2;     //  EGM Configuration Response QCOM v1.6
    qc_egmgcrtype   egmgcr;     //  EGM Game Config Response QCOM v1.5
    qc_ertype       er;         //  Event Response
    qc_ectacktype   ectack;     //  ECT to EGM Ack. Response QCOM v1.6
    qc_pepacktype   pepar;      //  Purge Event Poll Acknowledgement Response
    qc_mgcrtype     mgcr;       //  Meter Group/Contribution Response
    qc_mgvmrtype    mgvmr;      //  Multi-Game/Variation Meters Response
    qc_bmrtype      bmr;        //  Bet Meters Response QCOM v1.6
    qc_pcmrtype     pcmr;       //  Player Choice Meters Response QCOM v1.6
    qc_pcrtype      pcr;        //  Progressive Configuration Response QCOM v1.6
    qc_pmrtype      pmr;        //  Progressive Meters Response QCOM v1.5
    qc_pmrtype2     pmr2;       //  Progressive Meters Response QCOM v1.6
    qc_dlsasrtype   dlsasr;     //  DLSA/CRE Status Response
    qc_ftpdsrtype   ftpdsr;     //  FTP Download Status Response
//  qc_dllsrtype    dllsr;      //  DLL Status Response QCOM v1.6
    qc_nasrtype     nasr;       //  Note Acceptor Status Response QCOM v1.6
    qc_gsrtype      gsr;        //  General Status Response

    } QC_ResponsesType;

//-------------------------------------------------------------------------
//
// Response Message Root Data Type
//
typedef struct {
    qc_dlltype          DLL;    // Data Link Layer
    QC_ResponsesType    Data;

    } QCOM_RespMsgType;

typedef QCOM_RespMsgType    *qcom_respmsg;

#define QCOMRMSG(msgptr)    ((QCOM_RespMsgType *)msgptr)    // Generic cast into poll messages
#define qcrt                ((QCOM_RespMsgType *)RespMsg)   // Generic cast into reponse messages (RespMsg must be a defined pointer to the start of a qcom message)


//-------------------------------------------------------------------------
//  QCOM Poll Message Types

typedef union {
//  qc_ftpaldptype  ftpaldp;    //  FTP Activate Last Download Poll
//  qc_ftprdsptype  ftprdsp;    //  FTP Request Download Status Poll
//  qc_ftprp        ftprp;      //  FTP Reset Poll
    qc_ftpcdrptype  ftpcdrp;    //  FTP Commence Download Request Poll
    qc_egmcrptype   egmcrp;     //  EGM Configuration Request Poll
    qc_egmcptype    egmcp;      //  EGM Configuration Poll v1.5
    qc_egmcptype2   egmcp2;     //  EGM Configuration Poll (QCOM v1.6)
    qc_egmgcptype   egmgcp;     //  EGM Game Configuration Poll
    qc_egmvcptype   egmvcp;     //  EGM Variation Change Poll
    qc_egmpptype    egmpp;      //  EGM Parameters Poll v1.5
    qc_egmpptype2   egmpp2;     //  EGM Parameters Poll (QCOM v1.6)
    qc_pcptype      pcp;        //  Progressive Configuration Poll (QCOM v1.6 only) QCOM_PCP_FC
    qc_extjiptype   extjip;     //  External Jackpot Information Poll (QCOM v1.6 only) QCOM_EXTJIP_FC
    qc_psrptype     psrp;       //  Program Sign Request Poll v1.5
    qc_phrptype2    phrp2;      //  Program Hash Request Poll (QCOM v1.6)
    qc_salrptype    salrp;      //  System Lockup Request Poll
    qc_torackptype  torackp;    //  Ticket Out Request Acknowledgement Poll (QCOM v1.6)
    qc_tirackptype  tirackp;    //  Ticket In  Request Acknowledgement Poll (QCOM v1.6)
    qc_ectlrptype   ectlrp;     //  ECT Lockup Reset Poll
                                //  ECT from EGM Lockup Request Poll
    qc_ectegmptype  ectegmp;    //  ECT to EGM Poll v1.5
    qc_ectegmptype2 ectegmp2;   //  ECT to EGM Poll (QCOM v1.6)
                                //  Cancel Credit Lockup Request Poll
    qc_egmgmptype   egmgmp;     //  EGM General Maintenance Poll
                                //  Request All Logged Events Poll
    qc_peptype      pep;        //  Purge Events Poll
    qc_dlsalaptype  dlsalap;    //  DLSA/CRE Lockup Acknowledgement Poll
    qc_dlsacptype   dlsacp;     //  DLSA/CRE Configuration Poll
    qc_namptype     namp;       //  Note Acceptor Maint Poll
    qc_htpmptype    htpmp;      //  Hopper/Ticker Printer Maint Poll v1.5
    qc_htpmptype2   htpmp2;     //  Hopper/Ticker Printer Maint Poll (QCOM v1.6)
                                //  Linked Progressive Award Acknowledged Poll
    qc_egmgrptype   egmgrp;     //  EGM General Reset Poll v1.5
    qc_egmgrptype2  egmgrp2;    //  EGM General Reset Poll (QCOM v1.6)
    qc_spamptype    spamp;      //  Specific Promotional Message poll
    qc_egmtlmptype  egmtlmp;    //  Tower Light Maintenance
                                //  General Status Poll

    qc_broadcastpolltype    Broadcast;
    qc_broadcastftptype     ftpbroadcast;   // Download Data Broadcast Message

    } QC_PollType;

//  ...................................................................
//  Poll Message Root Data Type

typedef struct {
    qc_dlltype  DLL;        // Data Link Layer
    QC_PollType Data;

} QCOM_PollMsgType;

typedef QCOM_PollMsgType *qcom_pollmsg;


#define QCOMPMSG(msgptr)    ((QCOM_PollMsgType *)msgptr)    // Generic cast into poll messages
#define qcpt                ((QCOM_PollMsgType *)PollMsg)   // Generic cast into poll messages (PollMsg must be a defined pointer to the start of a qcom message)

//---------------------------------------------------------------------------
#if defined(__WIN32__) || defined(WIN32) || defined(_WIN32)
#pragma pack(pop)      // return (force) to dword alignment
#endif
#ifdef __GNUC__
#pragma pack(pop)
#endif
//--------------------------------------------------------------------------
//  QCOM Misc Data

extern  char QCOMReservedString[];  // "Reserved"
extern  char *QCOMMeterStrings[];   // returns meter msg string else returns QCOMReservedString for an undefined meter
extern  char *QCOMmgMeterStrings[]; // Multi-game meter label strings
extern  int   QCOMmgMeterUnitID[];

//--------------------------------------------------------------------------
//  Function Prototypes
//
//  The following function returns formatted static strings
//  Refer qcom.c for details

char    *QCOM_DecodePoll(unsigned char *PollMsg, int AddCRLF);
char    *QCOM_DecodeResponse(unsigned char *RespMsg, int addCRLF);

char    *QCOM_MIDstring(unsigned char ID);
char    *QCOM_DecodeGSRState(unsigned char State);          // General Status Response State
char    *QCOM_DecodeGSRFlags(qc_gsrtype *gsr);
char    *QCOM_GSRStateExtstring(unsigned char State);
char    *QCOM_FTPStateString(qcom_ftp_enum_states state);
char    *QCOM_DecodeMeterID(BYTE MeterID);
char    *QCOM_MeterUnitsString(BYTE MeterID);
char    *QCOM_TIRFCodeString(BYTE FCode);

char    *QCOM_DecodePollFC(BYTE fc);                        // Decodes QCOM poll function code
char    *QCOM_DecodePollFCshort(BYTE fc);                   // As bove but returns an abbreviation
char    *QCOM_DecodeResponseFC(BYTE fc);                    // Decodes QCOM response function code
char    *QCOM_DecodeResponseFCshort(BYTE fc);               // As bove but returns an abbreviation
char    *QCOM_DecodeExtdBroadcastFC(BYTE fc);               // Decodes QCOM poll extended broadcast function code

char    *QCOM_EventCodeString(WORD EventCode);
char    *QCOM_ExtEventDataString(WORD EventCode, BYTE ExtEvtDataSize, BYTE *ExDatab);
BYTE     QCOM_ExtEventDataSize(WORD EventCode);

char    *QCOM_Decodetimedate(qc_timedatetype *td);          // Decodes date and time from a QCOM message

int     QCOM_ValidateResponseLength(BYTE *RespMsg);
int     QCOM_ValidatePollLength(BYTE *PollMsg, BYTE DLLVER);

//-----------------------------------------------------------------------------
typedef struct {
    unsigned int DispGbl:1;
    unsigned int DispHex:1;
    unsigned int DispTxt:1;
    unsigned int DispPoll:1;

    unsigned int DispResp:1;
    unsigned int DispAll:1;             // Toggles display of General Status Polls/Resp
    unsigned int Spare:1;
    unsigned int Garbage:1;
    } qcom_decodeMessageFlags_type;

typedef enum {
    QCOM_DT_GARB_ENUM,        // Denotes Garbage
    QCOM_DT_POLL_ENUM,      // Denotes Poll Data
    QCOM_DT_RESP_ENUM,        // Response Data
    QCOM_DT_GLBL_ENUM,        // Global Message
        } qcom_messageTypeDecode_enum;

char    *QCOM_DecodeMessage(
        unsigned char *message,                 // QCOM message to decode
        short length,                           // Length of message in bytes
        qcom_messageTypeDecode_enum DataType,
        qcom_decodeMessageFlags_type dws        // type of data to display. ie. hex, text etc..
        );

#ifdef __cplusplus
}
#endif

#endif
