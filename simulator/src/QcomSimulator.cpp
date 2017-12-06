#include "Core.hpp"

#include <string>

#include "Utils.hpp"
#include "Console/ConsoleTable.hpp"
#include "BaseInteger.hpp"
#include "Concatenate.hpp"
#include "Qcom/Qcom.hpp"
#include "Qcom/QcomInline.h"
#include "CmdParser.hpp"
#include "Action.hpp"
#include "QcomAction.hpp"
#include "QcomSimulator.hpp"

#define SG_FLAG_STR_SWITCH(flag, true_str, false_str) ((flag) ? std::string(true_str) : std::string(false_str))

namespace sg 
{
    namespace
    {
        bool AppendEGMInfoToTable(size_t egm, QcomDataPtr const& d, ConsoleTable &t)
        {
            std::unique_lock<std::mutex> lock(d->locker);

            uint32_t serbcd = d->data.control.serialMidBCD & 0x00FFFFFF;
            uint32_t ser = 0;
            _QComGetBCD(&ser, (uint8_t*)(&serbcd), 3);

            uint32_t midbcd = (d->data.control.serialMidBCD & 0xFF000000) >> 6;
            uint32_t mid = 0;
            _QComGetBCD(&mid, (uint8_t*)(&midbcd), 1);

            std::string state;
            if (d->data.control.egm_config_state & QCOM_EGM_CONFIG_READY)
                sg::concatenate(state, "READY");
            if (d->data.control.egm_config_state & QCOM_EGM_HASH_READY)
                sg::concatenate(state, (state.size() ? "|HASH" : "HASH"));
            if ((d->data.control.egm_config_state & QCOM_EGM_CONFIG_SET) ||
                (d->data.control.egm_config_state & QCOM_EGM_NAM_SET))
                sg::concatenate(state, (state.size() ? "|PENDING" : "PENDING"));
            if (!state.size())
                sg::concatenate(state, "NOT CONFIG");

            ConsoleTableItem row[] =
            {
                egm,
                ser,
                mid,
                SG_FLAG_STR_SWITCH(d->data.control.machine_enable, "ENABLED", "DISABLED"),
                state
            };

            t.AddRow(&row);

            return true;
        }

        bool AppendGameInfoToTable(uint8_t game, QcomDataPtr const& d, ConsoleTable &t)
        {
            std::string state;
            if (d->data.control.game_config_state[game] & QCOM_GAME_CONFIG_READY)
                sg::concatenate(state, "READY");
            else if (d->data.control.game_config_state[game] & QCOM_GAME_CONFIG_GVN)
                sg::concatenate(state, "GVN");

            if ((d->data.control.game_config_state[game] & QCOM_GAME_CONFIG_REQ) ||
                (d->data.control.game_config_state[game] & QCOM_GAME_CONFIG_SET) ||
                (d->data.control.game_config_state[game] & QCOM_GAME_PC_CHANGE))
                sg::concatenate(state, (state.empty() ? "PENDING" : "|PENDING"));

            if (state.empty())
                sg::concatenate(state, "NOT CONFIG");

            ConsoleTableItem row[] =
            {
                (uint32_t)(d->data.control.poll_address),
                d->data.games[game].gvn,
                state,
                SG_FLAG_STR_SWITCH(d->data.games[game].config.settings.game_enable, "ENABLED", "DISABLED"),
                d->data.games[game].config.settings.pgid,
                d->data.games[game].config.settings.var,
                SG_FLAG_STR_SWITCH(d->data.control.game_config_state[game] & QCOM_GAME_CONFIG_SET, (d->data.games[game].config.settings.var_lock ? "SET" : "NOT SET"), "N/A"),
                SG_FLAG_STR_SWITCH(d->data.control.game_config_state[game] & QCOM_GAME_CONFIG_READY, (d->data.games[game].var_hot_switching ? "YES" : "NO"), "N/A"),
                SG_FLAG_STR_SWITCH(d->data.control.game_config_state[game] & QCOM_GAME_CONFIG_READY, (d->data.games[game].lp_only ? "YES" : "NO"), "N/A"),
                SG_FLAG_STR_SWITCH(d->data.control.game_config_state[game] & QCOM_GAME_CONFIG_READY, (d->data.games[game].customSAP ? "YES" : "NO"), "N/A")
            };

            t.AddRow(&row);

            return true;
        }

        bool ListEGMInfo(uint8_t egm, uint8_t all, std::vector<QcomDataPtr> const& data)
        {
            ConsoleTable t;
            t.SetStyle("compact");

            ConsoleTableItem header[] =
            {
                std::string("EGM"),
                std::string("SER"),
                std::string("MID"),
                std::string("ENABLED"),
                std::string("STATE"),
            };

            t.SetHeader(&header);

            //t.SetHeaderCellFormat(1, ConsoleTableFormat(" %|#x| ", CTPT_AlignLeft));
            if (all)
            {
                for (size_t i = 0; i < data.size(); ++i)
                {
                    AppendEGMInfoToTable(i + 1, data[i], t);
                }
            }
            else
            {
                AppendEGMInfoToTable(egm, data[egm - 1], t);
            }

            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\n");
            COMMS_PRINT_BLOCK(t);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();

            return true;
        }

        bool ListEGMConfig(uint8_t devices, uint8_t settings, QcomDataPtr const& data)
        {

            if (!devices && !settings)
            {
                ConsoleTable t;
                t.SetStyle("compact");

                ConsoleTableItem header[] =
                {
                    std::string("EGM"),
                    std::string("PROTOCOL"),
                    std::string("BSVN"),
                    std::string("LGVN"),
                    std::string("LVAR"),
                    std::string("GAME NUM"),
                    std::string("SHARED PROGRESSIVE"),
                    std::string("DENOM HOT-SWITCH")
                };

                t.SetHeader(&header);

                if (data->data.control.egm_config_state & QCOM_EGM_CONFIG_READY)
                {
                    t.SetHeaderCellFormat(2, ConsoleTableFormat(std::string(" %|#04X| "), CTPT_AlignLeft));
                    t.SetHeaderCellFormat(3, ConsoleTableFormat(std::string(" %|#04X| "), CTPT_AlignLeft));
                    t.SetHeaderCellFormat(4, ConsoleTableFormat(std::string(" %|02d| "), CTPT_AlignLeft));

                    {
                        std::unique_lock<std::mutex> lock(data->locker);

                        ConsoleTableItem row[] =
                        {
                            (uint32_t)(data->data.control.poll_address),
                            (data->data.control.protocol_ver == 0x01) ? std::string("Qcom 1.6") : std::string("Qcom 1.5"),
                            data->data.config.bsvn,
                            data->data.config.last_gvn,
                            data->data.config.last_var,
                            data->data.config.games_num,
                            SG_FLAG_STR_SWITCH(data->data.config.shared_progressive, "YES", "NO"),
                            SG_FLAG_STR_SWITCH(data->data.config.denom_hot_switching, "YES", "NO")
                        };

                        t.AddRow(&row);
                    }

                    COMMS_START_PRINT_BLOCK();
                    COMMS_PRINT_BLOCK("\n");
                    COMMS_PRINT_BLOCK(t);
                    COMMS_PRINT_BLOCK("\n");
                    COMMS_END_PRINT_BLOCK();
                }
                else
                {
                    ConsoleTableItem row[] =
                    {
                        (uint32_t)(data->data.control.poll_address),
                        std::string("N/A"),
                        std::string("N/A"),
                        std::string("N/A"),
                        std::string("N/A"),
                        std::string("N/A"),
                        std::string("N/A"),
                        std::string("N/A"),
                    };

                    t.AddRow(&row);
                }

            }
            else
            {
                if (devices)
                {
                    ConsoleTable t;
                    t.SetStyle("compact");

                    ConsoleTableItem header[] =
                    {
                        std::string("EGM"),
                        std::string("MECHANICAL METERS"),
                        std::string("NV-RTC"),
                        std::string("PROGRESSIVE DISPLAY"),
                        std::string("TOUCH SCREEN"),
                        std::string("TICKET PRINTER"),
                        std::string("HOPPER"),
                        std::string("NOTE ACCEPTOR"),
                        std::string("AUXILIARY DISPLAY"),
                        std::string("TICKET IN")
                    };

                    t.SetHeader(&header);

                    {
                        std::unique_lock<std::mutex> lock(data->locker);

                        if (data->data.control.egm_config_state & QCOM_EGM_CONFIG_READY)
                        {
                            ConsoleTableItem row[] =
                            {
                                (uint32_t)(data->data.control.poll_address),
                                SG_FLAG_STR_SWITCH(data->data.config.flag_a & (0x1 << 0), "EXPECTED", "NOT EXPECTED"),
                                SG_FLAG_STR_SWITCH(data->data.config.flag_a & (0x1 << 1), "EXPECTED", "NOT EXPECTED"),
                                SG_FLAG_STR_SWITCH(data->data.config.flag_a & (0x1 << 2), "EXPECTED", "NOT EXPECTED"),
                                SG_FLAG_STR_SWITCH(data->data.config.flag_a & (0x1 << 3), "EXPECTED", "NOT EXPECTED"),
                                SG_FLAG_STR_SWITCH(data->data.config.flag_a & (0x1 << 4), "EXPECTED", "NOT EXPECTED"),
                                SG_FLAG_STR_SWITCH(data->data.config.flag_a & (0x1 << 5), "EXPECTED", "NOT EXPECTED"),
                                SG_FLAG_STR_SWITCH(data->data.config.flag_a & (0x1 << 6), "EXPECTED", "NOT EXPECTED"),
                                SG_FLAG_STR_SWITCH(data->data.config.flag_a & (0x1 << 7), "EXPECTED", "NOT EXPECTED"),
                                SG_FLAG_STR_SWITCH(data->data.config.flag_b & (0x1 << 6), "EXPECTED", "NOT EXPECTED"),
                                SG_FLAG_STR_SWITCH(data->data.config.flag_b & (0x1 << 7), "EXPECTED", "NOT EXPECTED"),
                            };

                            t.AddRow(&row);
                        }
                        else
                        {
                            ConsoleTableItem row[] =
                            {
                                (uint32_t)(data->data.control.poll_address),
                                std::string("N/A"),
                                std::string("N/A"),
                                std::string("N/A"),
                                std::string("N/A"),
                                std::string("N/A"),
                                std::string("N/A"),
                                std::string("N/A"),
                                std::string("N/A"),
                                std::string("N/A"),
                                std::string("N/A"),
                            };

                            t.AddRow(&row);
                        }
                    }

                    COMMS_START_PRINT_BLOCK();
                    COMMS_PRINT_BLOCK("\n");
                    COMMS_PRINT_BLOCK(t);
                    COMMS_PRINT_BLOCK("\n");
                    COMMS_END_PRINT_BLOCK();
                }

                if (settings)
                {
                    ConsoleTable t;
                    t.SetStyle("compact");

                    ConsoleTableItem header[] =
                    {
                        std::string("EGM"),
                        std::string("JUR"),
                        std::string("DEN"),
                        std::string("TOK"),
                        std::string("MAX DEN"),
                        std::string("MIN RTP"),
                        std::string("MAX RTP"),
                        std::string("MAX SD"),
                        std::string("MAX LINES"),
                        std::string("MAX BET"),
                        std::string("MAX NPWIN"),
                        std::string("MAX PWIN"),
                        std::string("MAX ECT"),
                    };

                    t.SetHeader(&header);

                    {
                        std::unique_lock<std::mutex> lock(data->locker);

                        std::string jur, den, tok;
                        if (data->data.control.egm_config_state & QCOM_EGM_CONFIG_SET)
                        {
                            switch (data->data.custom.jur)
                            {
                            case 0x00:
                                concatenate(jur, "QLD CLUBS & HOTELS");
                                break;
                            case 0x01:
                                concatenate(jur, "QLD CASINOS");
                                break;
                            case 0x02:
                                concatenate(jur, "New Zealand");
                                break;
                            case 0x03:
                                concatenate(jur, "Victoria");
                                break;
                            case 0x04:
                                concatenate(jur, "South Australia");
                                break;
                            default:
                                concatenate(jur, "Unknown");
                                break;
                            }

                            concatenate(den, data->data.custom.den);
                            concatenate(tok, data->data.custom.tok);
                        }
                        else
                        {
                            concatenate(jur, "N/A");
                            concatenate(den, "N/A");
                            concatenate(tok, "N/A");
                        }

                        ConsoleTableItem row[] =
                        {
                            (uint32_t)(data->data.control.poll_address),
                            jur,
                            den,
                            tok,
                            data->data.custom.maxden,
                            data->data.custom.minrtp,
                            data->data.custom.maxrtp,
                            data->data.custom.maxsd,
                            data->data.custom.maxlines,
                            data->data.custom.maxbet,
                            data->data.custom.maxnpwin,
                            data->data.custom.maxpwin,
                            data->data.custom.maxect
                        };

                        t.AddRow(&row);
                    }

                    COMMS_START_PRINT_BLOCK();
                    COMMS_PRINT_BLOCK("\n");
                    COMMS_PRINT_BLOCK(t);
                    COMMS_PRINT_BLOCK("\n");
                    COMMS_END_PRINT_BLOCK();
                }
            }

            return true;
        }

        bool ListEGMHash(QcomDataPtr const& data)
        {
            ConsoleTable t;
            t.SetStyle("compact");

            ConsoleTableItem header[] =
            {
                std::string("EGM"),
                std::string("HASH")
            };

            t.SetHeader(&header);

            {
                std::unique_lock<std::mutex> lock(data->locker);

                std::string hash;

                if (data->data.control.egm_config_state & QCOM_EGM_HASH_READY)
                {
                    for (uint8_t i = 0; i < QCOM_MAX_PHA_SH_LENGTH; ++i)
                    {
                        concatenate(hash, (boost::format("%|X|") % static_cast<uint32_t>(data->data.control.hash[i])).str());
                    }
                }
                else
                    hash = "N/A";

                ConsoleTableItem row[] =
                {
                    (uint32_t)(data->data.control.poll_address),
                    hash
                };

                t.AddRow(&row);
            }

            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\n");
            COMMS_PRINT_BLOCK(t);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();

            return true;
        }

        bool ListEGMPSN(QcomDataPtr const& data)
        {
            ConsoleTable t;
            t.SetStyle("compact");

            ConsoleTableItem header[] =
            {
                std::string("EGM"),
                std::string("EVENT PSN"),
                std::string("ECT PSN")
            };

            t.SetHeader(&header);

            {
                std::unique_lock<std::mutex> lock(data->locker);

                ConsoleTableItem row[] =
                {
                    (uint32_t)(data->data.control.poll_address),
                    (uint32_t)(data->data.control.psn[Qcom_PSN_Events]),
                    (uint32_t)(data->data.control.psn[Qcom_PSN_ECT])
                };

                t.AddRow(&row);
            }

            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\n");
            COMMS_PRINT_BLOCK(t);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();

            return true;
        }

        bool ListEGMParameters(QcomDataPtr const& data)
        {
            ConsoleTable t;
            t.SetStyle("compact");

            ConsoleTableItem header[] =
            {
                std::string("EGM"),
                std::string("RESERVE FEATURE"),
                std::string("AUTOPLAY"),
                std::string("CRLIMIT MODE"),
                std::string("OPERATOR ID"),
                std::string("LARGE WIN"),
                std::string("CRLIMIT"),
                std::string("MAX GAMBLE"),
                std::string("GAMBLE LIMIT"),
                std::string("TIME ZONE AD"),
                std::string("POWER-SAVE TIME-OUT"),
                std::string("PID"),
                std::string("END OF DAY TIME"),
                std::string("NP WIN PAYOUT THRESHOLD"),
                std::string("SAP WIN PAYOUT THRESHOLD")
            };

            t.SetHeader(&header);

            t.SetHeaderCellFormat(4, ConsoleTableFormat(" %|#02X| ", CTPT_AlignLeft));

            {
                std::unique_lock<std::mutex> lock(data->locker);

                ConsoleTableItem row[] =
                {
                    (uint32_t)(data->data.control.poll_address),
                    SG_FLAG_STR_SWITCH(data->data.param.reserve, "ENABLED", "DISABLED"),
                    SG_FLAG_STR_SWITCH(data->data.param.auto_play, "ENABLED", "DISABLED"),
                    SG_FLAG_STR_SWITCH(data->data.param.crlimit_mode, "YES", "NO"),
                    data->data.param.lwin,
                    data->data.param.crlimit,
                    data->data.param.dulimit,
                    data->data.param.dumax,
                    data->data.param.tzadj,
                    data->data.param.pwrtime,
                    (uint32_t)(data->data.param.pid),
                    data->data.param.eodt,
                    data->data.param.npwinp,
                    data->data.param.sapwinp
                };

                t.AddRow(&row);
            }

            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\n");
            COMMS_PRINT_BLOCK(t);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();

            return true;
        }

        bool ListEGMState(uint8_t concurrent, uint8_t note_acceptor, QcomDataPtr const& data)
        {
            if (!concurrent && !note_acceptor)
            {
                ConsoleTable t;
                t.SetStyle("compact");

                ConsoleTableItem header[] =
                {
                    std::string("EGM"),
                    std::string("STATUS"),
                    std::string("MAIN DOOR"),
                    std::string("CASH BOX DOOR"),
                    std::string("PROCESSOR DOOR"),
                    std::string("BELLY PANEL DOOR"),
                    std::string("NOTE ACCEPTOR DOOR"),
                    std::string("NOTE STACKER"),
                    std::string("MECHANICAL METER DOOR"),
                    std::string("TOP BOX/AUX DOOR"),
                };

                t.SetHeader(&header);

                {
                    std::unique_lock<std::mutex> lock(data->locker);

                    std::string state("Idle Mode");
                    switch (data->data.status.state)
                    {
                    case 0x01:
                        break;
                    case 0x02:
                        state = "Play in Progress";
                        break;
                    case 0x03:
                        state = "Play in Progress - Feature";
                        break;
                    case 0x04:
                        state = "Hopper Collect";
                        break;
                    case 0x05:
                        state = "Residual Credit Removal Feature";
                        break;
                    case 0x06:
                        state = "Residual cancel credit Lockup";
                        break;
                    case 0x07:
                        state = "Cancel Credit Lockup";
                        break;
                    case 0x08:
                        state = "Large Win Lockup";
                        break;
                    case 0x09:
                        state = "Linked Progressive Award Lockup";
                        break;
                    case 0x0A:
                        state = "Unused";
                        break;
                    case 0x0B:
                        state = "System Lockup";
                        break;
                    case 0x0C:
                        state = "ECT from EGM Lockup";
                        break;
                    case 0x0D:
                        state = "CRanE Lockup";
                        break;
                    case 0x0F:
                        state = "Printing Cash Ticket Out";
                        break;
                    case 0x10:
                        state = "Cash Ticket Out Lockup";
                        break;
                    case 0x12:
                        state = "Play in Progress - Double Up/Gamble";
                        break;
                    case 0x13:
                        state = "Reserved";
                        break;
                    case 0x14:
                        state = "Reserved";
                        break;
                    default:
                        state = "Unknown";
                        break;
                    }

                    ConsoleTableItem row[] =
                    {
                        (uint32_t)(data->data.control.poll_address),
                        state,
                        SG_FLAG_STR_SWITCH(data->data.status.flag_a & (0x1 << 0), "Open", "Closed"),
                        SG_FLAG_STR_SWITCH(data->data.status.flag_a & (0x1 << 1), "Open", "Closed"),
                        SG_FLAG_STR_SWITCH(data->data.status.flag_a & (0x1 << 2), "Open", "Closed"),
                        SG_FLAG_STR_SWITCH(data->data.status.flag_a & (0x1 << 3), "Open", "Closed"),
                        SG_FLAG_STR_SWITCH(data->data.status.flag_a & (0x1 << 4), "Open", "Closed"),
                        SG_FLAG_STR_SWITCH(data->data.status.flag_a & (0x1 << 5), "Open", "Closed"),
                        SG_FLAG_STR_SWITCH(data->data.status.flag_a & (0x1 << 6), "Open", "Closed"),
                        SG_FLAG_STR_SWITCH(data->data.status.flag_a & (0x1 << 7), "Open", "Closed"),
                    };

                    t.AddRow(&row);
                }

                COMMS_START_PRINT_BLOCK();
                COMMS_PRINT_BLOCK("\n");
                COMMS_PRINT_BLOCK(t);
                COMMS_PRINT_BLOCK("\n");
                COMMS_END_PRINT_BLOCK();
            }
            else
            {
                if (concurrent)
                {
                    ConsoleTable t;
                    t.SetStyle("compact");

                    ConsoleTableItem header[] =
                    {
                        std::string("EGM"),
                        std::string("FAULT CONDITION"),
                        std::string("AUDIT MODE"),
                        std::string("TEST/SERVICE MODE"),
                        std::string("CASHLESS MODE"),
                        std::string("AWAITING CASH TICKET IN REQUEST ACK"),
                    };

                    t.SetHeader(&header);

                    {
                        std::unique_lock<std::mutex> lock(data->locker);

                        ConsoleTableItem row[] =
                        {
                            (uint32_t)(data->data.control.poll_address),
                            SG_FLAG_STR_SWITCH(data->data.status.flag_b & (0x1 << 0), "Active", "Inactive"),
                            SG_FLAG_STR_SWITCH(data->data.status.flag_b & (0x1 << 1), "Active", "Inactive"),
                            SG_FLAG_STR_SWITCH(data->data.status.flag_b & (0x1 << 2), "Active", "Inactive"),
                            SG_FLAG_STR_SWITCH(data->data.status.flag_b & (0x1 << 3), "Active", "Inactive"),
                            SG_FLAG_STR_SWITCH(data->data.status.flag_b & (0x1 << 4), "Active", "Inactive"),
                        };

                        t.AddRow(&row);
                    }

                    COMMS_START_PRINT_BLOCK();
                    COMMS_PRINT_BLOCK("\n");
                    COMMS_PRINT_BLOCK(t);
                    COMMS_PRINT_BLOCK("\n");
                    COMMS_END_PRINT_BLOCK();
                }

                if (note_acceptor)
                {
                    ConsoleTable t;
                    t.SetStyle("compact");

                    ConsoleTableItem header[] =
                    {
                        std::string("EGM"),
                        std::string("STACKER"),
                        std::string("BANKNOTE DENOM ACCEPTANCE"),
                        std::string("DISCRIPTOR"),
                    };

                    t.SetHeader(&header);

                    {
                        std::unique_lock<std::mutex> lock(data->locker);

                        std::string denom;
                        if (data->data.control.egm_config_state & QCOM_EGM_NAM_SET)
                        {
                            if (data->data.nasr.nam.five)
                                concatenate(denom, "$5");
                            if (data->data.nasr.nam.ten)
                                concatenate(denom, (denom.empty() ? "$10" : " $10"));
                            if (data->data.nasr.nam.twenty)
                                concatenate(denom, (denom.empty() ? "$20" : " $20"));
                            if (data->data.nasr.nam.fifty)
                                concatenate(denom, (denom.empty() ? "$50" : " $50"));
                            if (data->data.nasr.nam.hundred)
                                concatenate(denom, (denom.empty() ? "$100" : " $100"));
                        }
                        else
                            concatenate(denom, "N/A");

                        ConsoleTableItem row[] =
                        {
                            (uint32_t)(data->data.control.poll_address),
                            SG_FLAG_STR_SWITCH(data->data.nasr.full, "Full", "Not Full"),
                            denom,
                            SG_FLAG_STR_SWITCH(data->data.control.egm_config_state & QCOM_EGM_NAM_READY, data->data.nasr.nads, "N/A")
                        };

                        t.AddRow(&row);
                    }

                    COMMS_START_PRINT_BLOCK();
                    COMMS_PRINT_BLOCK("\n");
                    COMMS_PRINT_BLOCK(t);
                    COMMS_PRINT_BLOCK("\n");
                    COMMS_END_PRINT_BLOCK();
                }
            }

            return true;
        }

        bool ListEGMExtJPInfo(uint8_t levels, QcomDataPtr const& data)
        {
            if (!levels)
            {
                ConsoleTable t;
                t.SetStyle("compact");

                ConsoleTableItem header[] =
                {
                    std::string("EGM"),
                    std::string("RTP"),
                    std::string("DISPLAY"),
                    std::string("ICON"),
                    std::string("LEVELS")
                };

                t.SetHeader(&header);

                {
                    std::unique_lock<std::mutex> lock(data->locker);

                    ConsoleTableItem row[] =
                    {
                        (uint32_t)(data->data.control.poll_address),
                        data->data.extjpinfo.rtp,
                        SG_FLAG_STR_SWITCH(data->data.extjpinfo.display, "SET", "NOT SET"),
                        SG_FLAG_STR_SWITCH(data->data.extjpinfo.icon, "SET", "NOT SET"),
                        (uint32_t)(data->data.extjpinfo.levels)
                    };

                    t.AddRow(&row);
                }

                COMMS_START_PRINT_BLOCK();
                COMMS_PRINT_BLOCK("\n");
                COMMS_PRINT_BLOCK(t);
                COMMS_PRINT_BLOCK("\n");
                COMMS_END_PRINT_BLOCK();
            }
            else
            {
                ConsoleTable t;
                t.SetStyle("compact");

                ConsoleTableItem header[] =
                {
                    std::string("EGM"),
                    std::string("LEVEL"),
                    std::string("EPGID"),
                    std::string("UMF"),
                    std::string("NAME")
                };

                t.SetHeader(&header);

                t.SetHeaderCellFormat(2, ConsoleTableFormat(std::string(" %|#04X| "), CTPT_AlignLeft));

                {
                    std::unique_lock<std::mutex> lock(data->locker);

                    for (uint8_t i = 0; i < data->data.extjpinfo.levels; ++i)
                    {
                        ConsoleTableItem row[] =
                        {
                            (uint32_t)(data->data.control.poll_address),
                            (uint32_t)(i),
                            data->data.extjpinfo.epgid[i],
                            SG_FLAG_STR_SWITCH(data->data.extjpinfo.lumf[i], "SET", "NOT SET"),
                            (char*)(data->data.extjpinfo.lname[i])
                        };

                        t.AddRow(&row);
                    }
                }

                COMMS_START_PRINT_BLOCK();
                COMMS_PRINT_BLOCK("\n");
                COMMS_PRINT_BLOCK(t);
                COMMS_PRINT_BLOCK("\n");
                COMMS_END_PRINT_BLOCK();
            }

            return true;
        }

        bool ListEGMGames(uint16_t gvn, QcomDataPtr const& data)
        {
            ConsoleTable t;
            t.SetStyle("compact");

            ConsoleTableItem header[] =
            {
                std::string("EGM"),
                std::string("GVN"),
                std::string("STATE"),
                std::string("ENABLED"),
                std::string("PGID"),
                std::string("VAR"),
                std::string("VAR LOCK"),
                std::string("VAR HOT-SWITCH"),
                std::string("LP ONLY"),
                std::string("CUSTOM SAP")
            };

            t.SetHeader(&header);

            
            t.SetHeaderCellFormat(1, ConsoleTableFormat(std::string(" %|#04X| "), CTPT_AlignLeft));
            t.SetHeaderCellFormat(4, ConsoleTableFormat(std::string(" %|#04X| "), CTPT_AlignLeft));
            t.SetHeaderCellFormat(5, ConsoleTableFormat(std::string(" %|02d| "), CTPT_AlignLeft));

            {
                std::unique_lock<std::mutex> lock(data->locker);

                if (gvn)
                {
                    for (uint8_t game = 0; game < data->data.config.games_num; ++game)
                    {
                        if ((data->data.control.game_config_state[game] & QCOM_GAME_CONFIG_GVN) &&
                            (data->data.games[game].gvn == gvn))
                        {
                            AppendGameInfoToTable(game, data, t);
                            break;
                        }
                    }
                }
                else
                {
                    for (uint8_t game = 0; game < data->data.config.games_num; ++game)
                    {
                        AppendGameInfoToTable(game, data, t);
                    }
                }
            }

            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\n");
            COMMS_PRINT_BLOCK(t);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();

            return true;
        }

        bool ListGameVariations(uint8_t game, QcomDataPtr const& data)
        {
           ConsoleTable t;
            t.SetStyle("compact");

            ConsoleTableItem header[] =
            {
                std::string("EGM"),
                std::string("GVN"),
                std::string("VAR"),
                std::string("RTP"),
            };

            t.SetHeader(&header);

            t.SetHeaderCellFormat(2, ConsoleTableFormat(std::string(" %|02d| "), CTPT_AlignLeft));

            {
                std::unique_lock<std::mutex> lock(data->locker);

                if (data->data.control.game_config_state[game] & QCOM_GAME_CONFIG_READY)
                {
                    for (uint8_t i = 0; i < data->data.games[game].variations.vnum; ++i)
                    {
                        ConsoleTableItem row[] =
                        {
                            (uint32_t)(data->data.control.poll_address),
                            (uint32_t)(data->data.games[game].gvn),
                            (uint32_t)(data->data.games[game].variations.var[i]),
                            data->data.games[game].variations.pret[i]
                        };

                        t.AddRow(&row);
                    }
                }
                else
                {
                    ConsoleTableItem row[] =
                    {
                        (uint32_t)(data->data.control.poll_address),
                        (uint32_t)(data->data.games[game].gvn),
                        std::string("N/A"),
                        std::string("N/A")
                    };

                    t.AddRow(&row);
                }

            }

            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\n");
            COMMS_PRINT_BLOCK(t);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();

            return true;
        }

        bool ListGameProgressive(uint8_t game, QcomDataPtr const& data)
        {
           ConsoleTable t;
            t.SetStyle("compact");

            ConsoleTableItem header[] =
            {
                std::string("EGM"),
                std::string("GVN"),
                std::string("LEVEL"),
                std::string("START UP"),
                std::string("INCREMENT"),
                std::string("CEILING"),
                std::string("AUXRTP"),
                std::string("INITIAL CONTRIBUTION"),
                std::string("TYPE")
            };

            t.SetHeader(&header);

            t.SetHeaderCellFormat(1, ConsoleTableFormat(std::string(" %|#04X| "), CTPT_AlignLeft));

            {
                std::unique_lock<std::mutex> lock(data->locker);

                if (data->data.control.game_config_state[game] & QCOM_GAME_CONFIG_SET)
                {
                    for (uint8_t i = 0; i < data->data.games[game].prog.pnum; ++i)
                    {
                        ConsoleTableItem row[] =
                        {
                            (uint32_t)(data->data.control.poll_address),
                            (uint32_t)(data->data.games[game].gvn),
                            (uint32_t)(i),
                            data->data.games[game].config.progressive.sup[i],
                            data->data.games[game].config.progressive.pinc[i],
                            data->data.games[game].config.progressive.ceil[i],
                            data->data.games[game].config.progressive.auxrtp[i],
                            data->data.games[game].config.progressive.init_contri[i],
                            SG_FLAG_STR_SWITCH(data->data.games[game].config.progressive.flag_p[i], "LP", "SAP")
                        };

                        t.AddRow(&row);
                    }
                }
                else
                {
                    ConsoleTableItem row[] =
                    {
                        (uint32_t)(data->data.control.poll_address),
                        (uint32_t)(data->data.games[game].gvn),
                        std::string("N/A"),
                        std::string("N/A"),
                        std::string("N/A"),
                        std::string("N/A"),
                        std::string("N/A"),
                        std::string("N/A"),
                        std::string("N/A")
                    };

                    t.AddRow(&row);
                }
            }

            COMMS_START_PRINT_BLOCK();
            COMMS_PRINT_BLOCK("\n");
            COMMS_PRINT_BLOCK(t);
            COMMS_PRINT_BLOCK("\n");
            COMMS_END_PRINT_BLOCK();

            return true;
        }

        bool ListGameMeters(uint8_t game,
            uint8_t cmet, 
            uint8_t prog, 
            uint8_t multigamevar, 
            uint8_t player_choice, 
            uint8_t group,
            QcomDataPtr const& data)
        {
            if (!prog && !multigamevar && !player_choice && (group >= 0x10))
            {
                if (!cmet)
                {
                    ConsoleTable t;
                    t.SetStyle("compact");

                    ConsoleTableItem header[] =
                    {
                        std::string("EGM"),
                        std::string("GVN"),
                        std::string("MAX LINE"),
                        std::string("MAX BET"),
                        std::string("LINES/WAYS"),
                        std::string("BETS")
                    };

                    t.SetHeader(&header);

                    t.SetHeaderCellFormat(1, ConsoleTableFormat(std::string(" %|#04X| "), CTPT_AlignLeft));

                    {
                        std::unique_lock<std::mutex> lock(data->locker);

                        ConsoleTableItem row[] =
                        {
                            (uint32_t)(data->data.control.poll_address),
                            (uint32_t)(data->data.games[game].gvn),
                            data->data.games[game].betm.maxl,
                            data->data.games[game].betm.maxb,
                            data->data.games[game].betm.gbfa,
                            data->data.games[game].betm.gbfb
                        };

                        t.AddRow(&row);
                    }

                    COMMS_START_PRINT_BLOCK();
                    COMMS_PRINT_BLOCK("\n");
                    COMMS_PRINT_BLOCK(t);
                    COMMS_PRINT_BLOCK("\n");
                    COMMS_END_PRINT_BLOCK();
                }
                else
                {
                    ConsoleTable t;
                    t.SetStyle("compact");

                    {
                        std::unique_lock<std::mutex> lock(data->locker);

                        if (data->data.games[game].betm.gbfa <= 5)
                        {
                            ConsoleTableItem header[] =
                            {
                                std::string("BETS\\LINES"),
                                std::string("LINE 0"),
                                std::string("LINE 1"),
                                std::string("LINE 2"),
                                std::string("LINE 3"),
                                std::string("LINE 4")
                            };

                            t.SetHeader(&header);
                        }
                        else
                        {
                            ConsoleTableItem header[] =
                            {
                                std::string("BETS\\LINES"),
                                std::string("LINE 0"),
                                std::string("LINE 1"),
                                std::string("LINE 2"),
                                std::string("LINE 3"),
                                std::string("LINE 4"),
                                std::string("...")
                            };

                            t.SetHeader(&header);
                        }

                        for (uint8_t i = 0; i < data->data.games[game].betm.gbfb; ++i)
                        {
                            if (data->data.games[game].betm.gbfa <= 5)
                            {
                                ConsoleTableItem row[] =
                                {
                                    (boost::format("BET %||") % i).str(),
                                    data->data.games[game].betm.cmet[i * 0],
                                    data->data.games[game].betm.cmet[i * 1],
                                    data->data.games[game].betm.cmet[i * 2],
                                    data->data.games[game].betm.cmet[i * 3],
                                    data->data.games[game].betm.cmet[i * 4]
                                };

                                t.AddRow(&row);
                            }
                            else
                            {
                                ConsoleTableItem row[] =
                                {
                                    (boost::format("BET %||") % i).str(),
                                    data->data.games[game].betm.cmet[i * 0],
                                    data->data.games[game].betm.cmet[i * 1],
                                    data->data.games[game].betm.cmet[i * 2],
                                    data->data.games[game].betm.cmet[i * 3],
                                    data->data.games[game].betm.cmet[i * 4],
                                    std::string("...")
                                };

                                t.AddRow(&row);
 
                            }

                        }


                    }

                    COMMS_START_PRINT_BLOCK();
                    COMMS_PRINT_BLOCK("\n");
                    COMMS_PRINT_BLOCK(t);
                    COMMS_PRINT_BLOCK("\n");
                    COMMS_END_PRINT_BLOCK();
                }
            }

            if (prog)
            {
                ConsoleTable t;
                t.SetStyle("compact");

                ConsoleTableItem header[] =
                {
                    std::string("EGM"),
                    std::string("GVN"),
                    std::string("LEVEL"),
                    std::string("CURRENT AMOUNT"),
                    std::string("HITS"),
                    std::string("WINS"),
                    std::string("HIT RATE")
                };

                t.SetHeader(&header);

                t.SetHeaderCellFormat(1, ConsoleTableFormat(std::string(" %|#04X| "), CTPT_AlignLeft));

                {
                    std::unique_lock<std::mutex> lock(data->locker);

                    for (uint8_t level = 0; level < data->data.games[game].prog.pnum; ++level)
                    {
                        ConsoleTableItem row[] =
                        {
                            (uint32_t)(data->data.control.poll_address),
                            (uint32_t)(data->data.games[game].gvn),
                            (uint32_t)(level),
                            data->data.games[game].prog.levels.camt[level],
                            data->data.games[game].prog.levels.hits[level],
                            data->data.games[game].prog.levels.wins[level],
                            data->data.games[game].prog.levels.hrate[level]
                        };

                        t.AddRow(&row);
                    }
                }

                COMMS_START_PRINT_BLOCK();
                COMMS_PRINT_BLOCK("\n");
                COMMS_PRINT_BLOCK(t);
                COMMS_PRINT_BLOCK("\n");
                COMMS_END_PRINT_BLOCK();
            }

            if (multigamevar)
            {
                ConsoleTable t;
                t.SetStyle("compact");

                ConsoleTableItem header[] =
                {
                    std::string("EGM"),
                    std::string("GVN"),
                    std::string("STROKE"),
                    std::string("TURNOVER"),
                    std::string("CREDITS WIN"),
                    std::string("LINKED WINS"),
                    std::string("TOTAL WIN")
                };

                t.SetHeader(&header);

                t.SetHeaderCellFormat(1, ConsoleTableFormat(std::string(" %|#04X| "), CTPT_AlignLeft));

                {
                    std::unique_lock<std::mutex> lock(data->locker);
                    ConsoleTableItem row[] =
                    {
                        (uint32_t)(data->data.control.poll_address),
                        (uint32_t)(data->data.games[game].gvn),
                        data->data.games[game].mgvm.str,
                        data->data.games[game].mgvm.turn,
                        data->data.games[game].mgvm.win,
                        data->data.games[game].mgvm.pwin,
                        data->data.games[game].mgvm.gwin
                    };

                    t.AddRow(&row);
                }

                COMMS_START_PRINT_BLOCK();
                COMMS_PRINT_BLOCK("\n");
                COMMS_PRINT_BLOCK(t);
                COMMS_PRINT_BLOCK("\n");
                COMMS_END_PRINT_BLOCK();
            }

            if (player_choice)
            {
                ConsoleTable t;
                t.SetStyle("compact");

                ConsoleTableItem header[] =
                {
                    std::string("EGM"),
                    std::string("GVN"),
                    std::string("CHOICE 1"),
                    std::string("CHOICE 2"),
                    std::string("CHOICE 3"),
                    std::string("CHOICE 4"),
                    std::string("CHOICE 5"),
                    std::string("CHOICE 6"),
                    std::string("CHOICE 7"),
                    std::string("CHOICE 8"),
                    std::string("CHOICE 9"),
                    std::string("CHOICE 10"),
                    std::string("CHOICE 11"),
                    std::string("CHOICE 12"),
                    std::string("CHOICE 13"),
                    std::string("CHOICE 14"),
                    std::string("CHOICE 15"),
                    std::string("CHOICE 16")
                };

                t.SetHeader(&header);

                t.SetHeaderCellFormat(1, ConsoleTableFormat(std::string(" %|#04X| "), CTPT_AlignLeft));

                {
                    std::unique_lock<std::mutex> lock(data->locker);
                    ConsoleTableItem row[] =
                    {
                        (uint32_t)(data->data.control.poll_address),
                        (uint32_t)(data->data.games[game].gvn),
                        SG_FLAG_STR_SWITCH(0 < data->data.games[game].pcm.num,
                            (boost::format("%||") % data->data.games[game].pcm.pcmet[0]).str(), 
                            std::string("N/A")),
                        SG_FLAG_STR_SWITCH(1 < data->data.games[game].pcm.num, 
                            (boost::format("%||") % data->data.games[game].pcm.pcmet[1]).str(), 
                            std::string("N/A")),
                        SG_FLAG_STR_SWITCH(2 < data->data.games[game].pcm.num, 
                            (boost::format("%||") % data->data.games[game].pcm.pcmet[2]).str(), 
                            std::string("N/A")),
                        SG_FLAG_STR_SWITCH(3 < data->data.games[game].pcm.num, 
                            (boost::format("%||") % data->data.games[game].pcm.pcmet[3]).str(), 
                            std::string("N/A")),
                        SG_FLAG_STR_SWITCH(4 < data->data.games[game].pcm.num, 
                            (boost::format("%||") % data->data.games[game].pcm.pcmet[4]).str(), 
                            std::string("N/A")),
                        SG_FLAG_STR_SWITCH(5 < data->data.games[game].pcm.num, 
                            (boost::format("%||") % data->data.games[game].pcm.pcmet[5]).str(), 
                            std::string("N/A")),
                        SG_FLAG_STR_SWITCH(6 < data->data.games[game].pcm.num, 
                            (boost::format("%||") % data->data.games[game].pcm.pcmet[6]).str(), 
                            std::string("N/A")),
                        SG_FLAG_STR_SWITCH(7 < data->data.games[game].pcm.num, 
                            (boost::format("%||") % data->data.games[game].pcm.pcmet[7]).str(), 
                            std::string("N/A")),
                        SG_FLAG_STR_SWITCH(8 < data->data.games[game].pcm.num, 
                            (boost::format("%||") % data->data.games[game].pcm.pcmet[8]).str(), 
                            std::string("N/A")),
                        SG_FLAG_STR_SWITCH(9 < data->data.games[game].pcm.num, 
                            (boost::format("%||") % data->data.games[game].pcm.pcmet[9]).str(), 
                            std::string("N/A")),
                        SG_FLAG_STR_SWITCH(10 < data->data.games[game].pcm.num, 
                            (boost::format("%||") % data->data.games[game].pcm.pcmet[10]).str(), 
                            std::string("N/A")),
                        SG_FLAG_STR_SWITCH(11 < data->data.games[game].pcm.num, 
                            (boost::format("%||") % data->data.games[game].pcm.pcmet[11]).str(), 
                            std::string("N/A")),
                        SG_FLAG_STR_SWITCH(12 < data->data.games[game].pcm.num, 
                            (boost::format("%||") % data->data.games[game].pcm.pcmet[12]).str(), 
                            std::string("N/A")),
                        SG_FLAG_STR_SWITCH(13 < data->data.games[game].pcm.num, 
                            (boost::format("%||") % data->data.games[game].pcm.pcmet[13]).str(), 
                            std::string("N/A")),
                        SG_FLAG_STR_SWITCH(14 < data->data.games[game].pcm.num, 
                            (boost::format("%||") % data->data.games[game].pcm.pcmet[14]).str(), 
                            std::string("N/A")),
                        SG_FLAG_STR_SWITCH(15 < data->data.games[game].pcm.num, 
                            (boost::format("%||") % data->data.games[game].pcm.pcmet[15]).str(), 
                            std::string("N/A"))
                    };

                    t.AddRow(&row);
                }

                COMMS_START_PRINT_BLOCK();
                COMMS_PRINT_BLOCK("\n");
                COMMS_PRINT_BLOCK(t);
                COMMS_PRINT_BLOCK("\n");
                COMMS_END_PRINT_BLOCK();
            }

            if (group < 0x10)
            {
                ConsoleTable t;
                t.SetStyle("compact");

                if (group == 0)
                {
                    ConsoleTableItem header[] =
                    {
                        std::string("EGM"),
                        std::string("GVN"),
                        std::string("S"), // EGM Stroke
                        std::string("T"), // EGM Turnover
                        std::string("W"), // EGM Wins
                        std::string("CC"),// EGM Cancel Credit
                        std::string("CTO"),// EGM Cash Ticket Out
                        std::string("CI"), // EGM Cents In
                        std::string("CO"), // EGM Cents Out
                        std::string("LP"), // EGM Linked Progressive Wins
                        std::string("CTI"),// EGM Cash Ticket In
                        std::string("EGM PID ACCESSED"),
                        std::string("GWON"),// EGM Games Won
                        std::string("SAP"), // EGM SAP Wins
                    };

                    t.SetHeader(&header);

                    t.SetHeaderCellFormat(1, ConsoleTableFormat(std::string(" %|#04X| "), CTPT_AlignLeft));

                    {
                        std::unique_lock<std::mutex> lock(data->locker);
                        ConsoleTableItem row[] =
                        {
                            (uint32_t)(data->data.control.poll_address),
                            (uint32_t)(data->data.games[game].gvn),
                            data->data.games[game].mgc.groups[0].met[0],
                            data->data.games[game].mgc.groups[0].met[1],
                            data->data.games[game].mgc.groups[0].met[2],
                            data->data.games[game].mgc.groups[0].met[3],
                            data->data.games[game].mgc.groups[0].met[4],
                            data->data.games[game].mgc.groups[0].met[5],
                            data->data.games[game].mgc.groups[0].met[6],
                            data->data.games[game].mgc.groups[0].met[8],
                            data->data.games[game].mgc.groups[0].met[9],
                            data->data.games[game].mgc.groups[0].met[10],
                            data->data.games[game].mgc.groups[0].met[11],
                            data->data.games[game].mgc.groups[0].met[12]
                        };

                        t.AddRow(&row);
                    }

                    COMMS_START_PRINT_BLOCK();
                    COMMS_PRINT_BLOCK("\n");
                    COMMS_PRINT_BLOCK(t);
                    COMMS_PRINT_BLOCK("\n");
                    COMMS_END_PRINT_BLOCK();

                }
                else if (group == 1)
                {
                    ConsoleTableItem header[] =
                    {
                        std::string("EGM"),
                        std::string("GVN"),
                        std::string("R"), // EGM Hopper Refills
                        std::string("COI"), // EGM Coins/Tokens in
                        std::string("COO"), // EGM Coins/Tokens out
                        std::string("CB"), // EGM Coins/Tokens to Cash box
                        std::string("CSI"), // EGM Cashless Credit In
                        std::string("CSO"), // EGM Cashless Credit Out
                        std::string("NI"), // EGM Note Acceptor Cents In
                        std::string("NIC"), // EGM Notes In Count
                        std::string("RCRT"), // Residual Credit Removal Turnover
                        std::string("RCRW"), // Residual Credit Removal Wins
                        std::string("Rejected Enabled Notes"),
                        std::string("GT"), // EGM Gamble Turnover
                        std::string("GW"), // EGM Gamble Wins
                        std::string("EGM COINS/TOKENS CLEARED"),
                        std::string("EGM NOTES CLEARED")
                    };

                    t.SetHeader(&header);

                    t.SetHeaderCellFormat(1, ConsoleTableFormat(std::string(" %|#04X| "), CTPT_AlignLeft));

                    {
                        std::unique_lock<std::mutex> lock(data->locker);
                        ConsoleTableItem row[] =
                        {
                            (uint32_t)(data->data.control.poll_address),
                            (uint32_t)(data->data.games[game].gvn),
                            data->data.games[game].mgc.groups[1].met[0],
                            data->data.games[game].mgc.groups[1].met[1],
                            data->data.games[game].mgc.groups[1].met[2],
                            data->data.games[game].mgc.groups[1].met[3],
                            data->data.games[game].mgc.groups[1].met[4],
                            data->data.games[game].mgc.groups[1].met[5],
                            data->data.games[game].mgc.groups[1].met[6],
                            data->data.games[game].mgc.groups[1].met[8],
                            data->data.games[game].mgc.groups[1].met[9],
                            data->data.games[game].mgc.groups[1].met[10],
                            data->data.games[game].mgc.groups[1].met[11],
                            data->data.games[game].mgc.groups[1].met[12],
                            data->data.games[game].mgc.groups[1].met[13],
                            data->data.games[game].mgc.groups[1].met[14]
                        };

                        t.AddRow(&row);
                    }

                    COMMS_START_PRINT_BLOCK();
                    COMMS_PRINT_BLOCK("\n");
                    COMMS_PRINT_BLOCK(t);
                    COMMS_PRINT_BLOCK("\n");
                    COMMS_END_PRINT_BLOCK();
                }
                else if (group == 2)
                {
                    ConsoleTableItem header[] =
                    {
                        std::string("EGM"),
                        std::string("GVN"),
                        std::string("$5 NOTES IN"),
                        std::string("$10 NOTES IN"),
                        std::string("$20 NOTES IN"),
                        std::string("$50 NOTES IN"),
                        std::string("$100 NOTES IN")
                    };

                    t.SetHeader(&header);

                    t.SetHeaderCellFormat(1, ConsoleTableFormat(std::string(" %|#04X| "), CTPT_AlignLeft));

                    {
                        std::unique_lock<std::mutex> lock(data->locker);
                        ConsoleTableItem row[] =
                        {
                            (uint32_t)(data->data.control.poll_address),
                            (uint32_t)(data->data.games[game].gvn),
                            data->data.games[game].mgc.groups[2].met[0],
                            data->data.games[game].mgc.groups[2].met[1],
                            data->data.games[game].mgc.groups[2].met[2],
                            data->data.games[game].mgc.groups[2].met[3],
                            data->data.games[game].mgc.groups[2].met[4]
                        };

                        t.AddRow(&row);
                    }

                    COMMS_START_PRINT_BLOCK();
                    COMMS_PRINT_BLOCK("\n");
                    COMMS_PRINT_BLOCK(t);
                    COMMS_PRINT_BLOCK("\n");
                    COMMS_END_PRINT_BLOCK();
                }
            }

            return true;
        }

    }

    QcomSim::QcomSim()
        : m_curr_egm(0)
    {

    }

    QcomSim::~QcomSim()
    {

    }

    void QcomSim::DoInit()
    {
        std::string dev = CmdParser::Instance().GetDevPath();
        if (dev.empty())
        {
#ifdef SG_PLATFORM_LINUX
            dev = "/dev/ttyS0";
#else
            dev = "COM1";
#endif // SG_PLATFORM_LINUX
        }

        m_qcom = MakeSharedPtr<CommsQcom>(dev);
        m_qcom->Init();
        m_qcom->Start();
    }

    void QcomSim::SeekEGM(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);
        SG_UNREF_PARAM(action);

        m_qcom->SeekEGM();

        COMMS_LOG("Seek EGM Done [Note: EGM will not always response the seek command if it has responsed one]\n", CLL_Info);
    }

    bool QcomSim::Pick(uint8_t target)
    {
        size_t num = m_qcom->GetEgmNum();

        if (target > 0 && target <= num)
        {
            m_curr_egm = target;
            COMMS_LOG(boost::format("Pick EGM %d\n") % static_cast<uint32_t>(m_curr_egm), CLL_Info);
            return true;
        }
        else if (target == 0 && num == 1)
        {
            m_curr_egm = 1;
            COMMS_LOG(boost::format("Auto pick EGM %d\n") % static_cast<uint32_t>(m_curr_egm), CLL_Info);
            return true;
        }

        return false;
    }

    void QcomSim::PickEGM(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);

        PickEGMActionPtr p = std::static_pointer_cast<PickEGMAction>(action);

        this->Pick(p->Target());
    }

    void QcomSim::ListEGM(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);

        std::vector<QcomDataPtr> data;
        m_qcom->CaptureEGMData(data);

        QcomListActionPtr p = std::static_pointer_cast<QcomListAction>(action);

        uint8_t egm = p->EGM();
        uint8_t all = p->All();

        if (all)
        {
            ListEGMInfo(egm, all, data);
            return;
        }

        if (!egm)
        {
            if (m_curr_egm == 0)
            {
                if (!this->Pick(0))
                {
                    COMMS_LOG("No EGM information avaiable now\n", CLL_Info);
                    return;
                }
            }

            egm = m_curr_egm;
        }
        else
        {
            if (!this->Pick(egm))
            {
                COMMS_LOG(boost::format("Can't find EGM %||\n") % egm, CLL_Error);
                return;
            }
        }

        if (p->NoOpt())
        {
            ListEGMInfo(egm, 0, data);
            return;
        }

        QcomDataPtr const& d = data[egm - 1];

        if (p->Config())
            ListEGMConfig(p->Devices(), p->Settings(), d);

        if (p->Hash())
            ListEGMHash(d);

        if (p->PSN())
            ListEGMPSN(d);

        if (p->Parameters())
            ListEGMParameters(d);

        if (p->State())
            ListEGMState(p->Concurrents(), p->NoteAcceptor(), d);

        if (p->ExtJPInfo())
            ListEGMExtJPInfo(p->Levels(), d);

        if (p->Games())
        {
            ListEGMGames(0, d);
        }
        else if (!p->Variations() && !p->Progressives() && !p->Meters())
        {
            ListEGMGames(p->GVN(), d);
            return;
        }

        if (p->GVN())
        {
            uint8_t game = 0;
            {
                std::unique_lock<std::mutex> lock(d->locker);
                for (; game < d->data.config.games_num; ++game)
                {
                    if ((d->data.control.game_config_state[game] & QCOM_GAME_CONFIG_GVN) &&
                        (p->GVN() == d->data.games[game].gvn))
                        break;
                }

                if (game >= d->data.config.games_num)
                {
                    COMMS_LOG(boost::format("No game found for GVN 0x%|04X|\n") % p->GVN(), CLL_Error);
                    return;
                }
            }

            if (p->Variations())
                ListGameVariations(game, d);

            if (p->Progressives())
                ListGameProgressive(game, d);

            if (p->Meters())
                ListGameMeters(game, p->CMET(), p->PROG(), p->MultiGameVar(), p->PlayerChoice(), p->Group(), d);
        }
    }

    void QcomSim::EGMPollAddConf(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);
        SG_UNREF_PARAM(action);

        m_qcom->PollAddress(m_curr_egm);
        m_qcom->GeneralStatus(m_curr_egm);

        if (m_curr_egm)
        {
            COMMS_LOG(boost::format("Config the poll address of EGM %1%\n") % static_cast<uint32_t>(m_curr_egm), CLL_Info);
        }
        else
        {
            COMMS_LOG("Config the poll address for all EGM\n", CLL_Info);
        }
    }

    void QcomSim::TimeDate(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);
        SG_UNREF_PARAM(action);

        m_qcom->TimeData();
    }

    void QcomSim::LPCurrentAmount(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        QcomLPCurrentAmountActionPtr p = std::static_pointer_cast<QcomLPCurrentAmountAction>(action);

        QcomLinkedProgressiveData data;

        data.pnum = p->LPData(data.lpamt, data.pgid, data.plvl);

        m_qcom->LinkJPCurrentAmount(data);
    }

    void QcomSim::GeneralPromotional(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        QcomGeneralPromotionalActionPtr p = std::static_pointer_cast<QcomGeneralPromotionalAction>(action);

        m_qcom->GeneralPromotional(p->Text());
    }

    void QcomSim::SiteDetail(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        QcomSiteDetailActionPtr p = std::static_pointer_cast<QcomSiteDetailAction>(action);

        m_qcom->SiteDetail(p->SText(), p->LText());
    }

    void QcomSim::EGMConfRequest(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomEGMConfRequestActionPtr p = std::static_pointer_cast<QcomEGMConfRequestAction>(action);

            QcomEGMControlPollData data{ p->MEF(), p->GCR(), p->PSN() };

            m_qcom->EGMConfRequest(m_curr_egm, data);
        }
    }

    void QcomSim::EGMConfiguration(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomEGMConfActionPtr p = std::static_pointer_cast<QcomEGMConfAction>(action);

            QcomEGMConfigPollData data
            {
                p->DEN(),
                p->TOK(),
                p->MaxDEN(),
                p->MinRTP(),
                p->MaxRTP(),
                p->MaxSD(),
                p->MaxLines(),
                p->MaxBet(),
                p->MaxNPWin(),
                p->MaxPWin(),
                p->MaxECT(),
                p->JUR()
            };

            m_qcom->EGMConfiguration(m_curr_egm, data);
        }
    }

    void QcomSim::GameConfiguration(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomGameConfigurationActionPtr p = std::static_pointer_cast<QcomGameConfigurationAction>(action);

            QcomGameConfigData data;
            std::memset(&data, 0, sizeof(data));

            data.settings.pgid = p->PGID();
            data.settings.var = p->VAR();
            data.settings.var_lock = p->VARLock();
            data.settings.game_enable = p->GameEnable();
            uint8_t pnum = p->ProgressiveConfig(data.progressive.flag_p, data.progressive.init_contri);

            m_qcom->GameConfiguration(m_curr_egm, p->GVN(), pnum, data);
        }
    }

    void QcomSim::GameConfigurationChange(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomGameConfigurationChangeActionPtr p = std::static_pointer_cast<QcomGameConfigurationChangeAction>(action);

            QcomGameSettingData data{ p->PGID(), p->VAR(), 0, p->GEF() };

            m_qcom->GameConfigurationChange(m_curr_egm, p->GVN(), data);
        }
    }

    void QcomSim::EGMParameters(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomEGMParametersActionPtr p = std::static_pointer_cast<QcomEGMParametersAction>(action);

            QcomEGMParametersData data 
            {
                p->Reserve(), 
                p->AutoPlay(), 
                p->CRLimitMode(), 
                p->OPR(), 
                p->LWin(), 
                p->CRLimit(), 
                p->DULimit(), 
                p->DUMax(),
                p->TZADJ(), 
                p->PID(), 
                p->PWRTime(), 
                p->NPWinP(), 
                p->SAPWinP(),
                p->EODT()
            };

            m_qcom->EGMParameters(m_curr_egm, data);
        }
    }

    void QcomSim::PurgeEvents(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomPurgeEventsActionPtr p = std::static_pointer_cast<QcomPurgeEventsAction>(action);

            m_qcom->PurgeEvents(m_curr_egm, p->EVTNO());
        }
    }

    void QcomSim::GeneralStatus(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);
        SG_UNREF_PARAM(action);

        m_qcom->GeneralStatus(m_curr_egm);

    }

    void QcomSim::ProgressiveChange(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomProgressiveConfigActionPtr p = std::static_pointer_cast<QcomProgressiveConfigAction>(action);

            QcomProgressiveConfigData data;
            uint8_t pnum = p->ProgChangeData(data.sup, data.pinc, data.ceil, data.auxrtp);
            
            m_qcom->ProgressiveChange(m_curr_egm, p->GVN(), pnum, data);
        }
    }

    void QcomSim::ExtJPInfo(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomExtJPInfoActionPtr p = std::static_pointer_cast<QcomExtJPInfoAction>(action);

            QcomExtJPInfoData data;
            data.levels = p->ExtJPData(data.epgid, data.lumf, data.lname);

            data.rtp = p->ExtJPRTP();
            data.display = p->ExtJPDisplay();
            data.icon = p->ExtJPIcon();

            m_qcom->ExtJPInfo(m_curr_egm, data);
        }
    }

    void QcomSim::ProgHashRequest(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomProgHashRequestActionPtr p = std::static_pointer_cast<QcomProgHashRequestAction>(action);

            QcomProgHashRequestData data;
            data.mef = p->MEF();
            data.new_seed = p->Seed(data.seed, sizeof(data.seed));

            m_qcom->ProgHashRequest(m_curr_egm, data);
        }
    }

    void QcomSim::SysLockupRequest(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomSysLockupRequestActionPtr p = std::static_pointer_cast<QcomSysLockupRequestAction>(action);

            QcomSysLockupRequestData data;

            std::string text = p->Text();
            data.len = static_cast<uint8_t>(text.size());
            data.len = data.len <= QCOM_SALRP_TEXT_SIZE ? data.len : QCOM_SALRP_TEXT_SIZE;
            std::memcpy(data.text, text.c_str(), data.len);

            data.no_resetkey = p->NoResetKey();
            data.continue_style = p->ContinueStyle();
            data.question_style = p->QuestionStyle();
            data.lamp_test = p->LampTest();
            data.fanfare = p->Fanfare();

            m_qcom->SystemLockup(m_curr_egm, data);
        }
    }

    void QcomSim::CashTicketOutAck(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomCashTicketOutAckActionPtr p = std::static_pointer_cast<QcomCashTicketOutAckAction>(action);

            QcomCashTicketOutRequestAckPollData data;

            std::string certification = p->CertificationMessage();
            data.clen = static_cast<uint8_t>(certification.size());
            data.clen = data.clen <= QCOM_TORAP_MAX_CTEXT ? data.clen : QCOM_TORAP_MAX_CTEXT;
            std::memcpy(data.certification, certification.c_str(), data.clen);

            data.amount = p->Amount();
            data.serial = p->Serial();

            data.flag = 0;
            if (p->Approved())
                data.flag &= QCOM_CTO_APPROVE;
            if (p->Canceled())
                data.flag &= QCOM_CTO_CANCEL;

            BaseDecimal dec;
            DecimalInteger authno(p->AuthorisationNumber(), dec);
            HexInteger hex(authno);

            uint8_t digit = 0;
            size_t i = 0;
            for (; i < hex.GetCounts() && ((i / 2) < QCOM_AUTHNO_BYTE_NUM); ++i)
            {
                if (i % 2) // high bit
                {
                    digit &= (hex.GetDigit(i) << 4);
                    data.authno[i / 2] = digit;
                    digit = 0;
                }
                else
                {
                    digit = hex.GetDigit(i);
                }
            }

            if (i % 2 && ((i / 2) < QCOM_AUTHNO_BYTE_NUM)) // hex counts is odd
            {
                data.authno[i / 2] = digit;
            }

            for (size_t j = (i / 2); j < QCOM_AUTHNO_BYTE_NUM; ++j)
            {
                data.authno[j] = 0;
            }

            m_qcom->CashTicketOutAck(m_curr_egm, data);
        }
    }

    void QcomSim::CashTicketInAck(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomCashTicketInAckActionPtr p = std::static_pointer_cast<QcomCashTicketInAckAction>(action);

            QcomCashTicketInRequestAckPollData data;

            data.amount = p->Amount();
            data.fcode = p->FCode();

            BaseDecimal dec;
            DecimalInteger authno(p->AuthorisationNumber(), dec);
            HexInteger hex(authno);

            uint8_t digit = 0;
            size_t i = 0;
            for (; i < hex.GetCounts() && ((i / 2) < QCOM_AUTHNO_BYTE_NUM); ++i)
            {
                if (i % 2) // high bit
                {
                    digit &= (hex.GetDigit(i) << 4);
                    data.authno[i / 2] = digit;
                    digit = 0;
                }
                else
                {
                    digit = hex.GetDigit(i);
                }
            }

            if (i % 2 && ((i / 2) < QCOM_AUTHNO_BYTE_NUM)) // hex counts is odd
            {
                data.authno[i / 2] = digit;
            }

            for (size_t j = (i / 2); j < QCOM_AUTHNO_BYTE_NUM; ++j)
            {
                data.authno[j] = 0;
            }

            m_qcom->CashTicketInAck(m_curr_egm, data);
        }
    }

    void QcomSim::CashTicketOutRequest(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);
        SG_UNREF_PARAM(action);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            m_qcom->CashTicketOutReqeust(m_curr_egm);
        }
    }

    void QcomSim::EGMGeneralMaintenance(const ActionCenter & sender, const ActionPtr & action)
    {

        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomEGMGeneralMaintenanceActionPtr p = std::static_pointer_cast<QcomEGMGeneralMaintenanceAction>(action);

            QcomEGMGeneralMaintenancePollData data;

            data.meter_group_0 = p->MetersGroupFlag(0);
            data.meter_group_1 = p->MetersGroupFlag(1);
            data.meter_group_2 = p->MetersGroupFlag(2);
            data.note_acceptor_status = p->Qnasr();
            data.mef = p->MEF();
            data.var = p->VAR();
            data.player_choice_meter = p->Qpcmr();
            data.bet_meters = p->Qbmr();
            data.progconfig = p->Qprogcfg();
            data.gameconfig = p->Qgmecfg();
            data.progmeters = p->Qprogmeters();
            data.multigame = p->Qmultigame();
            data.gef = p->GEF();

            m_qcom->EGMGeneralMaintenance(m_curr_egm, p->GVN(), data);
        }
    }

    void QcomSim::RequestAllLoggedEvents(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);
        SG_UNREF_PARAM(action);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            m_qcom->RequestAllLoggedEvents(m_curr_egm);
        }
    }

    void QcomSim::NoteAcceptorMaintenance(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomNoteAcceptorMaintenanceActionPtr p = std::static_pointer_cast<QcomNoteAcceptorMaintenanceAction>(action);

            QcomNoteAcceptorMaintenanceData data;
            data.five = p->GetDenomFlag(5);
            data.ten = p->GetDenomFlag(10);
            data.twenty = p->GetDenomFlag(20);
            data.fifty = p->GetDenomFlag(50);
            data.hundred = p->GetDenomFlag(100);

            m_qcom->NoteAcceptorMaintenance(m_curr_egm, data);
        }
    }

    void QcomSim::HopperTicketPrinterMaintenance(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomHopperTicketPrinterMaintenanceActionPtr p = std::static_pointer_cast<QcomHopperTicketPrinterMaintenanceAction>(action);

            QcomHopperTicketPrinterData data;
            data.refill = p->Refill();
            data.collim = p->COLLIM();
            data.ticket = p->Ticket();
            data.dorefill = p->DoRefill();

            m_qcom->HopperTicketPrinterMaintenance(m_curr_egm, p->Test(), data);
        }
    }

    void QcomSim::LPAwardAck(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);
        SG_UNREF_PARAM(action);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            m_qcom->LPAwardAck(m_curr_egm);
        }
    }

    void QcomSim::GeneralReset(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomGeneralResetActionPtr p = std::static_pointer_cast<QcomGeneralResetAction>(action);

            QcomGeneralResetPollData data;
            data.fault = p->Fault();
            data.lockup = p->Lockup();
            data.state = p->State();

            m_qcom->GeneralReset(m_curr_egm, data);
        }
    }

    void QcomSim::SPAM(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomSPAMActionPtr p = std::static_pointer_cast<QcomSPAMAction>(action);

            QcomSPAMPollData data;

            std::string text = p->Text();
            data.len = static_cast<uint8_t>(text.size());
            data.len = data.len < QCOM_SPAMP_TEXT_SIZE ? data.len : QCOM_SALRP_TEXT_SIZE;

            std::memcpy(data.text, text.c_str(), sizeof(char) * data.len);

            data.prominence = p->Prom();
            data.fanfare = p->Fanfare();

            m_qcom->SPAM(m_curr_egm, p->Type(), data);
        }

    }

    void QcomSim::TowerLightMaintenance(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomTowerLightMaintenanceActionPtr p = std::static_pointer_cast<QcomTowerLightMaintenanceAction>(action);

            QcomTowerLightMaintenancePollData data;

            data.yellow_on = p->YellowOn();
            data.blue_on = p->BlueOn();
            data.red_on = p->RedOn();
            data.yellow_flash = p->YellowFlash();
            data.blue_flash = p->BlueFlash();
            data.red_flash = p->RedFlash();

            m_qcom->TowerLightMaintenance(m_curr_egm, data);
        }
    }

    void QcomSim::ECTToEGM(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomECTToEGMActionPtr p = std::static_pointer_cast<QcomECTToEGMAction>(action);

            QcomECTToEGMPollData data;

            data.cashless = p->Cashless();
            data.id = p->ID();
            data.eamt = p->EAMT();

            m_qcom->ECTToEGM(m_curr_egm, data);
        }
    }

    void QcomSim::ECTFromEGMLockupRequest(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);
        SG_UNREF_PARAM(action);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            m_qcom->ECTFromEGMRequest(m_curr_egm);
        }
    }

    void QcomSim::ECTLockupReset(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        if (m_curr_egm == 0)
            Pick(0);

        if (m_curr_egm > 0)
        {
            QcomECTLockupResetActionPtr p = std::static_pointer_cast<QcomECTLockupResetAction>(action);

            m_qcom->ECTLockupReset(m_curr_egm, p->Denied());
        }
    }

    void QcomSim::PendingPoll(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);

        QcomPendingActionPtr p = std::static_pointer_cast<QcomPendingAction>(action);

        m_qcom->PendingPoll(p->PollNum());
    }

    void QcomSim::SendPoll(const ActionCenter & sender, const ActionPtr & action)
    {
        SG_UNREF_PARAM(sender);
        SG_UNREF_PARAM(action);

        m_qcom->SendPoll();
    }

    void QcomSim::ChangeDev(const ActionCenter &sender, const ActionPtr &action)
    {
        SG_UNREF_PARAM(sender);

        ResetDevActionPtr a = std::static_pointer_cast<ResetDevAction>(action);
        m_qcom->ChangeDev(a->GetDev());
    }
}


