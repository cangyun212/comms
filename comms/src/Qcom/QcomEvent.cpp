#include "Core.hpp"
#include "Utils.hpp"

#include "Qcom/QcomEvent.hpp"
#include "Qcom/qogr/qogr_crc.h"


namespace sg
{
    namespace
    {
        bool qcom_log_event(qc_ertype *d, uint32_t sermid, u32 sec, u32 min, u32 hour, u32 day, u32 month, u32 year)
        {
            bool err_bcd = false;
            bool err_size = false;
            const char * ev = nullptr;
            std::string ext;

            switch (d->ECOD)
            {
            case QC_EC_COIN_IN_FAULT:
                ev = "EGM Coin-in Fault";
                break;
            case QC_EC_EXCESS_COIN_REJ:
                ev = "EGM Excessive Coin Rejects Fault";
                break;
            case QC_EC_YO_YO:
                ev = "EGM Coin-In Yo-Yo";
                break;
            case QC_EC_DIVERTER_FAULT:
                ev = "EGM Cash Box Optic/Diverter Fault";
                break;
            case QC_EC_HOPPER_LEVEL_MISMATCH:
                ev = "EGM Hopper Level Mismatch";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.HLM))
                {
                    ext = (boost::format("SL-HLM = $%||") % d->EXTD.HLM.SML).str();
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EC_HOPPER_EMPTY:
                ev = "EGM Hopper Empty";
                break;
            case QC_EC_HOPPER_JAMMED:
                ev = "EGM Hopper Jammed";
                break;
            case QC_EC_HOPPER_RUNAWAY:
                ev = "EGM Hopper Runaway/Overpay";
                break;
            case QC_EC_HOPPER_DISCONNECTED:
                ev = "EGM Hopper Disconnected";
                break;
            case QC_EC_MECH_METERS_DISC:
                ev = "EGM Mechanical Meters Disconnected";
                break;
            case QC_EC_PROGR_CONTR_FAULT:
                ev = "EGM Progressive Contr. Fault";
                break;
            case QC_EC_IO_CONTROLLER_FAULT:
                ev = "EGM I/O Controller Fault";
                break;
            case QC_EC_TOUCH_SCREEN_FAULT:
                ev = "EGM Touch Screen Fault";
                break;
            case QC_EC_MAN_SPEC_FAULT_A:
                ev = "EGM Manufacturer Specific Fault A";
                break;
            case QC_EC_MAN_SPEC_FAULT_B:
                ev = "EGM Manufacturer Specific Fault B";
                break;
            case QC_EC_MAN_SPEC_FAULT_EXT:
                ev = "EGM Manufacturer Specific";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.MSF))
                {
                    ext = (boost::format("Reason: %||") % d->EXTD.MSF.REASON).str();
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EC_STEPPER_REEL_FAULT:
                ev = "EGM Stepper Reel Fault";
                break;
            case QC_EC_BONUS_DEVICE_FAULT:
                ev = "EGM Bonus Device Fault";
                break;
            case QC_EC_MECH_METER_DISC_FAULT:
                ev = "EGM Mech Meter Disconnected Fault";
                break;
            case QC_EC_NOTE_STACKER_FULL:
                ev = "EGM Note Stacker Full Fault";
                break;
            case QC_EC_NOTE_ACCEPTOR_JAMMED:
                ev = "EGM Note Acceptor Jammed";
                break;
            case QC_EC_NOTE_ACCEPTOR_DISCONNECTED:
                ev = "EGM Note Acceptor Disconnected";
                break;
            case QC_EC_NOTE_ACCEPTOR_FAULT:
                ev = "EGM Note Acceptor Fault";
                break;
            case QC_EC_EXCESS_NOTE_REJ:
                ev = "EGM Excessive Note/Ticket Acceptor Rejects Fault";
                break;
            case QC_EC_NOTE_ACCEPTOR_YOYO:
                ev = "EGM Note Acceptor Yo-Yo";
                break;
            case QC_EC_LOW_NV_RAM_BATTERY:
                ev = "EGM Low NV-RAM Battery";
                break;
            case QC_EC_LOW_PF_DOOR_DET_BATT:
                ev = "EGM Low PF Door Detection Batt";
                break;
            case QC_EC_EEPROM_FAULT:
                ev = "EGM EEPROM Error/Fault";
                break;
            case QC_EXT_EC_EGM_RAM_CLEARED:
                ev = "EGM NV-RAM Cleared";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.RC))
                {
                    u32 ser, mid;
                    if (_QComGetBCD(&ser, &(d->EXTD.RC.SN.SerialNo.SER[0]), sizeof(d->EXTD.RC.SN.SerialNo.SER)))
                    {
                        if (_QComGetBCD(&mid, &(d->EXTD.RC.SN.SerialNo.MID), sizeof(d->EXTD.RC.SN.SerialNo.MID)))
                        {
                            ext = (boost::format("Serial No: %||%||") % mid % ser).str();
                        }
                        else
                        {
                            err_bcd = true;
                        }
                    }
                    else
                    {
                        err_bcd = true;
                    }
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EC_TICKET_PAPER_OUT:
                ev = "EGM Ticket Printer Paper Out";
                break;
            case QC_EC_TICKET_PAPER_JAM:
                ev = "EGM Ticket Printer Paper Jam";
                break;
            case QC_EC_TICKET_GENERAL_FAULT:
                ev = "EGM Ticket Printer General Fault";
                break;
            case QC_EXT_EC_TICKET_PRINT_FAIL:
                ev = "EGM Cash Ticket Out Print Failure";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.TOPFAIL))
                {
                    ext = (boost::format("Ticket Serial: %|| Amount $%||") % d->EXTD.TOPFAIL.TSER % d->EXTD.TOPFAIL.TAMT).str();
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EC_LOW_MEMORY:
                ev = "EGM Low Memory";
                break;
            case QC_EC_BAD_POWER_DOWN:
                ev = "EGM Power Down Incomplete";
                break;
            case QC_EC_AUX_DISP_FAILURE:
                ev = "EGM Auxiliary Display Device Failure";
                break;
            case QC_EC_PRI_DISP_FAILURE:
                ev = "EGM Primary Display Device Failure";
                break;
            case QC_EC_TER_DISP_FAILURE:
                ev = "EGM Tertiary Display Device Failure";
                break;
            case QC_EC_LIC_KEY_FAIL:
                ev = "EGM License Key Misssing/Failure";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.KEYID))
                {
                    ext = (boost::format("KeyID: %|02X|\
                                                 %|02X|\
                                                 %|02X|\
                                                 %|02X|\
                                                 %|02X|\
                                                 %|02X|\
                                                 %|02X|\
                                                 %|02X|") %
                        d->EXTD.KEYID.KEYID[0] %
                        d->EXTD.KEYID.KEYID[1] %
                        d->EXTD.KEYID.KEYID[2] %
                        d->EXTD.KEYID.KEYID[3] %
                        d->EXTD.KEYID.KEYID[4] %
                        d->EXTD.KEYID.KEYID[5] %
                        d->EXTD.KEYID.KEYID[6] %
                        d->EXTD.KEYID.KEYID[7]).str();
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EXT_EC_LP_AWARD: //
                ev = "EGM Linked Progressive Award";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.LP))
                {
                    u32 var;
                    if (_QComGetBCD(&var, &(d->EXTD.LP.VAR), sizeof(d->EXTD.LP.VAR)))
                    {
                        ext = (boost::format("Gem:0x%|04X| Var:%|02d| Group:0x%|4X| Lev: %|d| $%|d|") %
                            d->EXTD.LP.GVN % var % d->EXTD.LP.PGID % d->EXTD.LP.PLVL.PLVL % d->EXTD.LP.PAMT).str();
                    }
                    else
                    {
                        err_bcd = true;
                    }
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EXT_EC_SAP_AWARD:
                ev = "EGM SA Progressive Award";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.SAP))
                {
                    u32 var;
                    if (_QComGetBCD(&var, &(d->EXTD.SAP.VAR), sizeof(d->EXTD.SAP.VAR)))
                    {
                        ext = (boost::format("Gem:0x%|04X| Var:%|02d| Lev: %|d| $%|d|") %
                            d->EXTD.SAP.GVN % var % d->EXTD.SAP.PLVL.PLVL % d->EXTD.SAP.PAMT).str();
                    }
                    else
                    {
                        err_bcd = true;
                    }
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EXT_EC_LARGE_WIN:
                ev = "EGM Large Win";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.LW))
                {
                    u32 var;
                    if (_QComGetBCD(&var, &(d->EXTD.LW.VAR), sizeof(d->EXTD.LW.VAR)))
                    {
                        ext = (boost::format("Gem:0x%|04X| Var:%|02d| $%|d|") %
                            d->EXTD.LW.GVN % var % d->EXTD.LW.WAMT).str();
                    }
                    else
                    {
                        err_bcd = true;
                    }
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EXT_EC_CANCEL_CREDIT:
                ev = "EGM Cancel Credit";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.CC))
                {
                    ext = (boost::format("$%|d|") % d->EXTD.CC.CC).str();
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EC_ALL_FAULTS_CLEAR: //
                ev = "EGM All Faults Cleared";
                break;
            case QC_EC_LOCKUP_CLEAR:
                ev = "EGM Lockup Cleared";
                break;
            case QC_EC_CANCEL_CREDIT_CANCELLED:
                ev = "EGM Cancel Credit Cancelled";
                break;
            case QC_EC_RESERVED_QSIM:
                ev = "Reserved for the QSIM Protocol Simulator";
                break;
            case QC_EC_EGM_EVENT_QUEUE_FULL:
                ev = "EGM Event Queue Full";
                break;
            case QC_EC_COMM_TIMEOUT:
                ev = "EGM Communications Time-Out";
                break;
            case QC_EXT_EC_VAR_ENABLED:
                ev = "EGM Game Variation Enabled";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.GVE))
                {
                    u32 var;
                    if (_QComGetBCD(&var, &(d->EXTD.GVE.VAR), sizeof(d->EXTD.GVE.VAR)))
                    {
                        ext = (boost::format("Gem:0x%|04X| Var:%|02d|") %
                            d->EXTD.GVE.GVN % var).str();
                    }
                    else
                    {
                        err_bcd = true;
                    }
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EC_EGM_RTC_REFRESHED:
                ev = "EGM RTC Refreshed";
                break;
            case QC_EXT_EC_REFILL_RECORDED:
                ev = "EGM Hopper Refill Recorded";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.R))
                {
                    ext = (boost::format("$%|d|") % d->EXTD.R.AMT).str();
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EC_EGM_POWER_UP:
                ev = "EGM Power Up";
                break;
            case QC_EC_EGM_POWER_DOWN:
                ev = "EGM Power Down";
                break;
            case QC_EC_EGM_MAIN_DOOR_OPENED:
                ev = "EGM Main Door Opened";
                break;
            case QC_EC_EGM_MAIN_DOOR_CLOSED:
                ev = "EGM Main Door Closed";
                break;
            case QC_EC_CASH_BOX_DOOR_OPENED:
                ev = "EGM Cash Box Door Opened";
                break;
            case QC_EC_CASH_BOX_DOOR_CLOSED:
                ev = "EGM Cash Box Door Closed";
                break;
            case QC_EC_PROCESSOR_DOOR_OPENED:
                ev = "EGM Processor Door Opened";
                break;
            case QC_EC_PROCESSOR_DOOR_CLOSED:
                ev = "EGM Processor Door Closed";
                break;
            case QC_EC_BELLY_PANEL_DOOR_OPENED:
                ev = "EGM Belly Panel Door Opened";
                break;
            case QC_EC_BELLY_PANEL_DOOR_CLOSED:
                ev = "EGM Belly Panel Door Closed";
                break;
            case QC_EC_NOTE_STACKER_HIGH_LEVEL_WARN:
                break;
            case QC_EC_NOTE_ACCEPTOR_DOOR_OPENED:
                ev = "EGM Note Acceptor Door Opened";
                break;
            case QC_EC_NOTE_ACCEPTOR_DOOR_CLOSED:
                ev = "EGM Note Acceptor Door Closed";
                break;
            case QC_EC_NOTE_ACCEPTOR_STACKER_REMOVED:
                ev = "EGM Note Acceptor Stacker Removed";
                break;
            case QC_EC_NOTE_ACCEPTOR_STACKER_RETURNED:
                ev = "EGM Note Acceptor Stacker Returned";
                break;
            case QC_EC_NOTE_STACKER_CLEARED:
                ev = "EGM Note Stacker Cleared";
                break;
            case QC_EC_PWR_OFF_PROCESSOR_DOOR_ACCESS:
                ev = "EGM Pwr Off Processor Door Access";
                break;
            case QC_EC_PWR_OFF_CASH_DOOR_ACCESS:
                ev = "EGM Pwr Off Cash Door Access";
                break;
            case QC_EC_PWR_OFF_MAIN_DOOR_ACCESS:
                ev = "EGM Pwr Off Main Door Access";
                break;
            case QC_EC_PWR_OFF_NOTE_ACCPTR_DOOR_ACCESS:
                ev = "EGM Pwr Off Note Acceptor Door Access";
                break;
            case QC_EC_PWR_OFF_MECH_METER_DOOR_ACCESS:
                ev = "EGM Pwr Off Mechanical Meter Door Access";
                break;
            case QC_EC_CASHBOX_CLEARED:
                ev = "EGM Cash Box Cleared";
                break;
            case QC_EXT_EC_TICKET_PRINTED:
                ev = "EGM Cash Ticket Printed";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.TP))
                {
                    ext = (boost::format("Ser:%|d| TAC:%|d| $%|d|") %
                        d->EXTD.TP.SN.SER %
                        d->EXTD.TP.TAC %
                        d->EXTD.TP.AMT).str();
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EC_TICKET_INK_LOW:
                ev = "EGM Ticket Printer Ink Low";
                break;
            case QC_EC_NOTE_STACKER_FULL_NOTICE:
                ev = "EGM Note Stacker Full - Note Acceptance Disabled";
                break;
            case QC_EC_STACKER_HIGH_LEVEL:
                ev = "EGM Note Stacker High Level Warning";
                break;
            case QC_EC_TICKET_PAPER_LOW:
                ev = "EGM Ticket Printer Paper Low";
                break;
            case QC_EC_INVALID_EGM_CONFIGURATION:
                ev = "EGM Invalid EGM Configuration";
                break;
            case QC_EC_INVALID_GAME_CONFIGURATION:
                ev = "EGM Invalid Game Configuration";
                break;
            case QC_EXT_INVALID_PROGR_CONFIGURATION:
                ev = "EGM Invalid Progressive Configuration";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.GVE))
                {
                    u32 var;
                    if (_QComGetBCD(&var, &(d->EXTD.GVE.VAR), sizeof(d->EXTD.GVE.VAR)))
                    {
                        ext = (boost::format("Gem:0x%|04X| Var:%|02d|") %
                            d->EXTD.GVE.GVN % var).str();
                    }
                    else
                    {
                        err_bcd = true;
                    }
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EC_PROCESSOR_OVER_TEMP:
                ev = "EGM Processor Over-Temperature";
                break;
            case QC_EC_COOLING_FAN_FAILURE:
                ev = "EGM Cooling Fan Low RPM";
                break;
            case QC_EC_CALL_SERVICE_TECHNICIAN:
                ev = "EGM Call Service Technician";
                break;
            case QC_EC_MECH_METER_DOOR_OPENED:
                ev = "EGM Mechanical Meter Door Opened";
                break;
            case QC_EC_MECH_METER_DOOR_CLOSED:
                ev = "EGM Mechanical Meter Door Closed";
                break;
            case QC_EC_AUX_DOOR_OPENED:
                ev = "EGM Top Box/Aux Door Opened";
                break;
            case QC_EC_AUX_DOOR_CLOSED:
                ev = "EGM Top Box/Aux Door Closed";
                break;
            case QC_EC_RECOVERABLE_RAM_CORRUPTION:
                ev = "EGM Recoverable RAM Corruption";
                break;
            case QC_EC_PID_SESSION_STARTED:
                ev = "EGM New PID Session Started";
                break;
            case QC_EC_DENOM_CHANGED:
                ev = "EGM Denomination Enabled/Changed";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.EGMDC))
                {
                    ext = (boost::format("Denom $%|d|") % d->EXTD.EGMDC.DEN).str();
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EC_INVALID_DENOMINATION:
                ev = "EGM Invalid Denomination";
                break;
            case QC_EC_HOPPER_CALIBRATED:
                ev = "EGM Hopper Calibrated";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.HP)) // TODO : ? I can't find the correct ext data type
                {
                    ext = (boost::format("Level $%|d|") % d->EXTD.HP.AMT).str();
                }
                else
                {
                    err_size = true;
                }

                break;
            case QC_EC_PRGSSVE_CFG_CHANGED:
                ev = "EGM Progressive Configuration Changed";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.PRGCC))
                {
                    u32 var;
                    if (_QComGetBCD(&var, &(d->EXTD.PRGCC.VAR), sizeof(d->EXTD.PRGCC.VAR)))
                    {
                        ext = (boost::format("Gem:0x%|04X| Var:%|02d| RTP %|.4f|%%") %
                            d->EXTD.PRGCC.GVN % var % (d->EXTD.PRGCC.PRTP / 1000.0f)).str();
                    }
                    else
                    {
                        err_bcd = true;
                    }
                }
                else
                {
                    err_size = true;
                }

                break;
            case QC_EC_NP_TOP_PRIZE_HIT:
                ev = "EGM Top NP Prize Hit";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.GVE))
                {
                    u32 var;
                    if (_QComGetBCD(&var, &(d->EXTD.GVE.VAR), sizeof(d->EXTD.GVE.VAR)))
                    {
                        ext = (boost::format("Gem:0x%|04X| Var:%|02d|") %
                            d->EXTD.GVE.GVN % var).str();
                    }
                    else
                    {
                        err_bcd = true;
                    }
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EC_INVALID_TICKET_ACK:
                ev = "EGM Invalid Ticket Out Acknowledgement";
                break;
            case QC_EC_PERIOD_METERS_RESET:
                ev = "EGM Period Meters Reset";
                break;
            case QC_EC_ICON_DISP_ENABLED:
                ev = "EGM EXTJIP Icon Display Enabled";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.MSF)) // TODO : ? I can't find the correct ext data type
                {
                    ext = (boost::format("%||") % d->EXTD.MSF.REASON).str();
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EC_HOPPER_RUNAWAY_OVERPAY:
                ev = "EGM Hopper Overpay Amount";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.HP)) // TODO : ? I can't find the correct ext data type
                {
                    ext = (boost::format("$%|11d|") % d->EXTD.HP.AMT).str();
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EC_LIC_KEY_DETECTED:
                ev = "EGM License Key Detected";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.KEYID))
                {
                    ext = (boost::format("KeyID: %|02X|\
                                                 %|02X|\
                                                 %|02X|\
                                                 %|02X|\
                                                 %|02X|\
                                                 %|02X|\
                                                 %|02X|\
                                                 %|02X|") %
                        d->EXTD.KEYID.KEYID[0] %
                        d->EXTD.KEYID.KEYID[1] %
                        d->EXTD.KEYID.KEYID[2] %
                        d->EXTD.KEYID.KEYID[3] %
                        d->EXTD.KEYID.KEYID[4] %
                        d->EXTD.KEYID.KEYID[5] %
                        d->EXTD.KEYID.KEYID[6] %
                        d->EXTD.KEYID.KEYID[7]).str();
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EC_NON_PROD_LIC_KEY_DETECTED:
                ev = "EGM Non-Production License key Detected";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.KEYID))
                {
                    ext = (boost::format("KeyID: %|02X|\
                                                 %|02X|\
                                                 %|02X|\
                                                 %|02X|\
                                                 %|02X|\
                                                 %|02X|\
                                                 %|02X|\
                                                 %|02X|") %
                        d->EXTD.KEYID.KEYID[0] %
                        d->EXTD.KEYID.KEYID[1] %
                        d->EXTD.KEYID.KEYID[2] %
                        d->EXTD.KEYID.KEYID[3] %
                        d->EXTD.KEYID.KEYID[4] %
                        d->EXTD.KEYID.KEYID[5] %
                        d->EXTD.KEYID.KEYID[6] %
                        d->EXTD.KEYID.KEYID[7]).str();
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EC_TICKET_IN_TIMEOUT:
                ev = "EGM Ticket-In Timeout";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.TIR)) 
                {
                    ext = (boost::format("Auth No: 0x%|02X|%|02X|%|02X|%|02X|%|02X|%|02X|%|02X|%|02X|\
                                         %|02X|%|02X|%|02X|%|02X|%|02X|%|02X|%|02X|%|02X|") %
                        d->EXTD.TIR.authno.AUTHNO[7] %
                        d->EXTD.TIR.authno.AUTHNO[6] %
                        d->EXTD.TIR.authno.AUTHNO[5] %
                        d->EXTD.TIR.authno.AUTHNO[4] %
                        d->EXTD.TIR.authno.AUTHNO[3] %
                        d->EXTD.TIR.authno.AUTHNO[2] %
                        d->EXTD.TIR.authno.AUTHNO[1] %
                        d->EXTD.TIR.authno.AUTHNO[0] %
                        d->EXTD.TIR.authno.AUTHNO[15] %
                        d->EXTD.TIR.authno.AUTHNO[14] %
                        d->EXTD.TIR.authno.AUTHNO[13] %
                        d->EXTD.TIR.authno.AUTHNO[12] %
                        d->EXTD.TIR.authno.AUTHNO[11] %
                        d->EXTD.TIR.authno.AUTHNO[10] %
                        d->EXTD.TIR.authno.AUTHNO[9] %
                        d->EXTD.TIR.authno.AUTHNO[8]).str();
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EC_TICKET_IN_ABORTED:
                ev = "EGM Ticket-In Timeout";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.TIR)) 
                {
                    ext = (boost::format("Auth No: Ox%|02X|%|02X|%|02X|%|02X|%|02X|%|02X|%|02X|%|02X|\
                                         %|02X|%|02X|%|02X|%|02X|%|02X|%|02X|%|02X|%|02X|") %
                        d->EXTD.TIR.authno.AUTHNO[7] %
                        d->EXTD.TIR.authno.AUTHNO[6] %
                        d->EXTD.TIR.authno.AUTHNO[5] %
                        d->EXTD.TIR.authno.AUTHNO[4] %
                        d->EXTD.TIR.authno.AUTHNO[3] %
                        d->EXTD.TIR.authno.AUTHNO[2] %
                        d->EXTD.TIR.authno.AUTHNO[1] %
                        d->EXTD.TIR.authno.AUTHNO[0] %
                        d->EXTD.TIR.authno.AUTHNO[15] %
                        d->EXTD.TIR.authno.AUTHNO[14] %
                        d->EXTD.TIR.authno.AUTHNO[13] %
                        d->EXTD.TIR.authno.AUTHNO[12] %
                        d->EXTD.TIR.authno.AUTHNO[11] %
                        d->EXTD.TIR.authno.AUTHNO[10] %
                        d->EXTD.TIR.authno.AUTHNO[9] %
                        d->EXTD.TIR.authno.AUTHNO[8]).str();
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EC_TRANSACTION_DENIED:
                ev = "EGM Transaction Denied - Credit Limit Reached";
                break;
            case QC_EXT_EC_HOPPER_PAYOUT: //
                ev = "EGM Hopper Payout";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.HP))
                {
                    ext = (boost::format("$%|d|") % d->EXTD.HP.AMT).str();
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EXT_EC_RES_CC_LOCKUP:
                ev = "EGM Residual Cancel Credit Lock Up";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.RCCL))
                {
                    ext = (boost::format("%|d|") % d->EXTD.RCCL.CC).str();
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EC_EGM_TEMP_EVENT_QUEUE_FULL:
                ev = "EGM Secondary Event Queue Full";
                break;
            case QC_EXT_EC_NEW_GAME_SELECTED:
                ev = "EGM New Game Selected";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.GVE))
                {
                    u32 var;
                    if (_QComGetBCD(&var, &(d->EXTD.GVE.VAR), sizeof(d->EXTD.GVE.VAR)))
                    {
                        ext = (boost::format("Gem:0x%|04X| Var:%|02d|") %
                            d->EXTD.GVE.GVN % var).str();
                    }
                    else
                    {
                        err_bcd = true;
                    }
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EXT_EC_ECT_FROM_EGM:
                ev = "EGM ECT from EGM";
                break;
            case QC_EXT_EC_DLSA:
                ev = "EGM CRanE Hit";
                break;
            case QC_EXT_EC_SLUR:
                ev = "EGM System Lockup User Response";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.SLUR))
                {
                    ext = (boost::format("Ans: %||") % (d->EXTD.SLUR.YESNO.YESNO ? "Yes" : "No")).str();
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EXT_EC_SAP_UN_V2:
                ev = "EGM SAP Award";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.SAP))
                {
                    u32 var;
                    if (_QComGetBCD(&var, &(d->EXTD.SAP.VAR), sizeof(d->EXTD.SAP.VAR)))
                    {
                        ext = (boost::format("Gem:0x%|04X| Var:%|02d| Lev: %|d| $%|d|") %
                            d->EXTD.SAP.GVN % var % d->EXTD.SAP.PLVL.PLVL % d->EXTD.SAP.PAMT).str();
                    }
                    else
                    {
                        err_bcd = true;
                    }
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EXT_EC_ECT_FROM_EGM_V2:
                ev = "EGM ECT From EGM";
                break;
            case QC_EXT_EC_RES_CC_LOCKUP_V2:
                ev = "EGM Residual Cancel Credit Lock Up";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.RCCL))
                {
                    ext = (boost::format("%|d|") % d->EXTD.RCCL.CC).str();
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EXT_EC_TICKET_OUT_LOCKUP:
                ev = "EGM Cash Ticket Out Request";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.TOR))
                {
                    ext = (boost::format("Ticket Serial: %|d| Amount $%|d|") % d->EXTD.TOR.TSER % d->EXTD.TOR.TAMT).str();
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EXT_EC_TO_PRINT_OK:
                ev = "EGM Cash Ticket Out Print Successful";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.TOPOK))
                {
                    ext = (boost::format("Ticket Serial: %|d| Amount $%|d|") % d->EXTD.TOPOK.TSER % d->EXTD.TOPOK.TAMT).str();
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EXT_EC_TICKET_IN_REQUEST:
                ev = "EGM Cash Ticket In Request";
                if (d->ESIZ.ESIZ >= sizeof(d->EXTD.TIR)) 
                {
                    ext = (boost::format("Auth No: Ox%|02X|%|02X|%|02X|%|02X|%|02X|%|02X|%|02X|%|02X|\
                                         %|02X|%|02X|%|02X|%|02X|%|02X|%|02X|%|02X|%|02X|") %
                        d->EXTD.TIR.authno.AUTHNO[7] %
                        d->EXTD.TIR.authno.AUTHNO[6] %
                        d->EXTD.TIR.authno.AUTHNO[5] %
                        d->EXTD.TIR.authno.AUTHNO[4] %
                        d->EXTD.TIR.authno.AUTHNO[3] %
                        d->EXTD.TIR.authno.AUTHNO[2] %
                        d->EXTD.TIR.authno.AUTHNO[1] %
                        d->EXTD.TIR.authno.AUTHNO[0] %
                        d->EXTD.TIR.authno.AUTHNO[15] %
                        d->EXTD.TIR.authno.AUTHNO[14] %
                        d->EXTD.TIR.authno.AUTHNO[13] %
                        d->EXTD.TIR.authno.AUTHNO[12] %
                        d->EXTD.TIR.authno.AUTHNO[11] %
                        d->EXTD.TIR.authno.AUTHNO[10] %
                        d->EXTD.TIR.authno.AUTHNO[9] %
                        d->EXTD.TIR.authno.AUTHNO[8]).str();
                }
                else
                {
                    err_size = true;
                }
                break;
            case QC_EC_NEW_PID_SESSION_START:
                ev = "Rserved - NZ (EGM New PID Session Started)";
                break;
            case QC_EXT_EC_PID_SESSION_STOP:
                ev = "Reserved - NZ (EGM PID Session Stopped";
                break;
            default:
                ev = "Unkown event";
                break;
            }

            if (!err_size && !err_bcd)
            {
                COMMS_LOG(
                    boost::format("%|| %|02d|:%|02d|:%|02d| %|02d|-%|02d|-%|02d| %|| %|| %|| %||\n") %
                    sermid % hour % min % sec % year % month % day % static_cast<u32>(d->SEQ) % d->ECOD %
                    ev % ext, 
                    CLL_Info);
            }
            else
            {
                if (err_size)
                {
                    COMMS_LOG(
                        boost::format("Invalid extend data size %1% of event %2%\n") %
                        static_cast<u32>(d->ESIZ.ESIZ) % ev,
                        CLL_Error);
                }
                else
                {
                    COMMS_LOG(
                        boost::format("Non-BCD value of event %1% received\n") %
                        ev,
                        CLL_Error);
                }

                return false;
            }

            return true;
        }
    }

    uint8_t QcomEvent::RespId() const
    {
        return QCOM_ER_FC;
    }

    bool QcomEvent::Parse(uint8_t buf[], int length)
    {
        SG_UNREF_PARAM(length);

        if (auto it = m_qcom.lock())
        {
            QCOM_RespMsgType *p = (QCOM_RespMsgType*)buf;
            if (p->DLL.Length >= QCOM_GET_PACKET_LENGTH(sizeof(qc_ertype) - sizeof(p->Data.er.EXTD) + p->Data.er.ESIZ.ESIZ))
            {
                QcomDataPtr pd = it->GetEgmData(p->DLL.PollAddress);

                qc_ertype d;
                uint32_t sermid;
                if (pd)
                {
                    d = p->Data.er;
                    std::unique_lock<std::mutex> lock(pd->locker);
                    pd->data.control.last_control ^= (QCOM_ACK_MASK);
                    sermid = pd->data.control.serialMidBCD;
                }
                else
                {
                    return false;
                }

                u32 sec, min, hour, day, month, year;
                if (_QComGetBCD(&sec, &(d.TIMEDATE.seconds), sizeof(d.TIMEDATE.seconds)))
                {
                    if (_QComGetBCD(&min, &(d.TIMEDATE.day), sizeof(d.TIMEDATE.day)))
                    {
                        if (_QComGetBCD(&hour, &(d.TIMEDATE.hours), sizeof(d.TIMEDATE.hours)))
                        {
                            if (_QComGetBCD(&day, &(d.TIMEDATE.day), sizeof(d.TIMEDATE.day)))
                            {
                                if (_QComGetBCD(&month, &(d.TIMEDATE.month), sizeof(d.TIMEDATE.month)))
                                {
                                    if (_QComGetBCD(&year, &(d.TIMEDATE.year), sizeof(d.TIMEDATE.year)))
                                    {
                                        return qcom_log_event(&d, sermid, sec, min, hour, day, month, year);
                                    }
                                }
                            }
                        }
                    }
                }
                
                COMMS_LOG(
                    boost::format("Time/Date [SEC:%1% MIN:%2% HOUR:%3% DAY:%4% MONTH:%5% YEAR:%6%]\
                                  of Event response is not BCD value\n") %
                    static_cast<uint32_t>(d.TIMEDATE.seconds) %
                    static_cast<uint32_t>(d.TIMEDATE.minutes) %
                    static_cast<uint32_t>(d.TIMEDATE.hours) %
                    static_cast<uint32_t>(d.TIMEDATE.day) %
                    static_cast<uint32_t>(d.TIMEDATE.month) %
                    static_cast<uint32_t>(d.TIMEDATE.year), 
                    CLL_Error);
            }
        }

        return false;
    }
}


