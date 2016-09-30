//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/O97EXEC.CPv   1.1   12 Oct 1998 13:25:08   DCHI  $
//
// Description:
//  Top-level VBA 5 execution function.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/O97EXEC.CPv  $
// 
//    Rev 1.1   12 Oct 1998 13:25:08   DCHI
// Made the following modifications in O97Execute().
// - Changed O97GetModuleBinary() call to O97ModuleGetOpcode().
// - Simplied handling of eO97_OP_EA_SYMBOL_DECL opcode.
// 
//    Rev 1.0   15 Aug 1997 13:29:30   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "o97api.h"
#include "o97env.h"

// The IP should be at the 0x8F of the sub.

BOOL O97Execute
(
    LPO97_ENV           lpstEnv,
    LPO97_MODULE_ENV    lpstModEnv,
    LPO97_IP            lpstStartIP
)
{
    WORD                wTemp;
    LPO97_CALL          lpstCall;
    BOOL                bResult;
    LPO97_MODULE_ENV    lpstNextModEnv;
    DWORD               dwNumIter;
    O97_LINE_ENTRY_T    stLine;
    LPO97_MODULE        lpstModule;
    DWORD               dwOffset;

    if (O97EnvExecuteInit(lpstEnv) == FALSE)
        return(FALSE);

    lpstEnv->lpstModEnv = lpstModEnv;
    lpstModEnv->lpstPrev = NULL;

    if (O97VarPushGlobalFrame(lpstEnv) == FALSE)
        return(FALSE);

    // No params

    if (O97ParamInitList(lpstEnv) == FALSE)
        return(FALSE);

    /////////////////////////////////////////////////////////////
    // Push CALL_SUB state

    O97StatePush(lpstEnv,esO97_CALL_SUB);

    if (O97ControlCreateControl(lpstEnv,ectO97_CALL) == FALSE)
        return(FALSE);

    lpstCall = (LPO97_CALL)O97ControlTop(lpstEnv);

    lpstCall->stReturnIP.wLine = (WORD)lpstModEnv->lpstModule->stLineTable.dwNumLines;
    lpstCall->stReturnIP.wOffset = 0xFFFF;

    /////////////////////////////////////////////////////////////

    lpstEnv->dwIPLine = lpstStartIP->wLine;
    lpstEnv->dwIPOffset = lpstStartIP->wOffset;
    lpstEnv->stRoutineStartIP = *lpstStartIP;
    lpstEnv->bRedoToken = FALSE;
    lpstEnv->bNewIP = FALSE;
    lpstEnv->bIsStringCall = FALSE;
    lpstEnv->eError = eO97_ERROR_NO_ERROR;
    lpstEnv->lErr = 0;
    bResult = TRUE;
    dwNumIter = 0;

    while (O97StateTop(lpstEnv) != esO97_INIT &&
           lpstEnv->eError == eO97_ERROR_NO_ERROR &&
           lpstEnv->dwIPLine <
           lpstEnv->lpstModEnv->lpstModule->stLineTable.dwNumLines &&
           dwNumIter++ < 30000)
    {
        lpstModule = lpstEnv->lpstModEnv->lpstModule;
        if (O97GetLineEntry(lpstModule,
                            lpstEnv->dwIPLine,
                            &stLine) == FALSE)
        {
            // Error getting line

            break;
        }

        while (dwNumIter++ < 30000)
        {
#ifdef SYM_NLM
            if ((dwNumIter & 0x1FF) == 0)
            {
                // Relinquish control every 512 iterations

                SSProgress(lpstEnv->lpvRootCookie);
            }
#endif // #ifdef SYM_NLM

            if (lpstEnv->dwIPOffset >= stLine.wSize)
            {
                lpstEnv->dwIPLine++;
                lpstEnv->dwIPOffset = 0;
                break;
            }

            // Get the opcode

            dwOffset = stLine.dwOffset + lpstEnv->dwIPOffset;
            if (O97ModuleGetOpcode(lpstModule,
                                   dwOffset,
                                   lpstEnv->abyOpcode) == FALSE)
            {
                lpstEnv->eError = eO97_ERROR_GENERIC;
                break;
            }

            switch ((EO97_OP_T)lpstEnv->abyOpcode[0])
            {
                case eO97_OP_20_EVAL:
                case eO97_OP_21_DOT_EVAL:
                case eO97_OP_27_ASSIGN:
                case eO97_OP_28_DOT_ASSIGN:
                case eO97_OP_2E_ASSIGN_USING_SET:
                case eO97_OP_2F_ASSIGN_DOTTED_SET:
                case eO97_OP_35_ROOT_DOT_EVAL:
                case eO97_OP_39_ASSIGN_ROOT_DOT:
                case eO97_OP_45_COLON:
                case eO97_OP_93_GOTO:
                case eO97_OP_9B_LABEL:
                case eO97_OP_DD_RESUME:
                case eO97_OP_BE_NEW:
                case eO97_OP_C1_ON_ERROR_GOTO:
                case eO97_OP_C9_PARAM_ASSIGN:
                    if (O97GetModuleWORD(lpstModule,
                                         dwOffset + 2,
                                         &lpstEnv->wOpID) == FALSE)
                    {
                        lpstEnv->eError = eO97_ERROR_GENERIC;
                        break;
                    }

                    // Make sure bit 0 is off

                    lpstEnv->wOpID &= 0xFFFE;

                    // No operands

                    lpstEnv->wOpCount = 0;

                    dwOffset += 2 + 2;
                    break;

                case eO97_OP_53_CLOSE:
                case eO97_OP_A4_WORD:
                case eO97_OP_A6_HEX_WORD:
                    if (O97GetModuleWORD(lpstModule,
                                         dwOffset + 2,
                                         &lpstEnv->wOpValue) == FALSE)
                    {
                        lpstEnv->eError = eO97_ERROR_GENERIC;
                        break;
                    }
                    dwOffset += 2 + 2;
                    break;

                case eO97_OP_24_FUNCTION_EVAL:
                case eO97_OP_25_DOT_FUNCTION_EVAL:
                case eO97_OP_2B_ASSIGN_FUNCTION:
                case eO97_OP_2C_ASSIGN_DOT_FUNCTION:
                case eO97_OP_37_ROOT_DOT_FUNCTION_EVAL:
                case eO97_OP_3B_ASSIGN_ROOT_DOT_FUNCTION:
                case eO97_OP_41_CALL:
                case eO97_OP_42_DOT_CALL:
                case eO97_OP_43_ROOT_DOT_CALL:
                    if (O97GetModuleWORD(lpstModule,
                                         dwOffset + 2,
                                         &lpstEnv->wOpID) == FALSE)
                    {
                        lpstEnv->eError = eO97_ERROR_GENERIC;
                        break;
                    }

                    // Make sure bit 0 is off

                    lpstEnv->wOpID &= 0xFFFE;

                    if (O97GetModuleWORD(lpstModule,
                                         dwOffset + 4,
                                         &lpstEnv->wOpCount) == FALSE)
                    {
                        lpstEnv->eError = eO97_ERROR_GENERIC;
                        break;
                    }
                    dwOffset += 2 + 4;
                    break;

                case eO97_OP_59_DEFTYPE:
                case eO97_OP_A5_DWORD:
                case eO97_OP_A7_HEX_DWORD:
                case eO97_OP_E8_TYPE:
                    if (O97GetModuleDWORD(lpstModule,
                                          dwOffset + 2,
                                          &lpstEnv->dwOpValue) == FALSE)
                    {
                        lpstEnv->eError = eO97_ERROR_GENERIC;
                        break;
                    }
                    dwOffset += 2 + 4;
                    break;

                case eO97_OP_5A_DECLARATION:
                    dwOffset += 2;
                    switch (lpstEnv->abyOpcode[1])
                    {
                        case 0x00:
                        case 0x04:
                        case 0x10:
                        case 0x14:
                        case 0x20:
                        case 0x24:
                        case 0x40:
                        case 0x44:
                            break;

                        default:
                            break;
                    }
                    break;

                case eO97_OP_66_END_FUNCTION:
                case eO97_OP_6C_END_SUB:
                    dwOffset += 2;
                    break;

                case eO97_OP_8F_SUB_FUNCTION:
                    if (O97GetModuleDWORD(lpstModule,
                                          dwOffset + 2,
                                          &lpstEnv->dwOpTypeOffset) == FALSE)
                    {
                        lpstEnv->eError = eO97_ERROR_GENERIC;
                        break;
                    }
                    dwOffset += 2 + 4;
                    break;

                case eO97_OP_9E_BROKEN_LINE:
                    dwOffset += 2;
                    if (O97GetModuleWORD(lpstModule,
                                         dwOffset,
                                         &wTemp) == FALSE)
                        return(FALSE);
                    dwOffset += 2 + wTemp;
                    break;

                case eO97_OP_AC_DOUBLE:
                    if (O97GetModuleBinary(lpstModule,
                                           dwOffset + 2,
                                           (LPBYTE)&lpstEnv->lfOpValue,
                                           8) == FALSE)
                    {
                        lpstEnv->eError = eO97_ERROR_GENERIC;
                        break;
                    }
                    dwOffset += 2 + 8;
                    break;

                case eO97_OP_D8_COMMENT:
                    dwOffset += 4;
                    if (O97GetModuleWORD(lpstModule,
                                         dwOffset,
                                         &wTemp) == FALSE)
                    {
                        lpstEnv->eError = eO97_ERROR_GENERIC;
                        break;
                    }
                    dwOffset += 2 + wTemp;
                    if (wTemp & 1)
                        ++dwOffset;
                    break;

                case eO97_OP_D9_REDIM:
                case eO97_OP_DA_REDIM_PRESERVE:
                    dwOffset += 2 + 8;
                    break;

                case eO97_OP_AE_STRING:
                    if (O97GetModuleWORD(lpstModule,
                                         dwOffset + 2,
                                         &wTemp) == FALSE)
                    {
                        lpstEnv->eError = eO97_ERROR_GENERIC;
                        break;
                    }
                    if (wTemp > 255)
                        lpstEnv->abyOpString[0] = 255;
                    else
                        lpstEnv->abyOpString[0] = (BYTE)wTemp;

                    if (O97GetModuleBinary(lpstModule,
                                           dwOffset + 4,
                                           lpstEnv->abyOpString + 1,
                                           lpstEnv->abyOpString[0]) == FALSE)
                    {
                        lpstEnv->eError = eO97_ERROR_GENERIC;
                        break;
                    }
                    dwOffset += 2 + 2 + wTemp;
                    if (wTemp & 1)
                        ++dwOffset;
                    break;

                case eO97_OP_C4_OPEN:
                    dwOffset += 2 + 2;
                    break;

                case eO97_OP_DB_LITERAL:
                case eO97_OP_DC_REM:
                    dwOffset += 2;
                    if (O97GetModuleWORD(lpstModule,
                                         dwOffset,
                                         &wTemp) == FALSE)
                    {
                        lpstEnv->eError = eO97_ERROR_GENERIC;
                        break;
                    }
                    dwOffset += 2 + wTemp;
                    if (wTemp & 1)
                        ++dwOffset;
                    break;

                case eO97_OP_EA_SYMBOL_DECL:
                    dwOffset += 2;
                    if (lpstEnv->abyOpcode[1] & 0x40)
                        dwOffset += 6;
                    else
                        dwOffset += 4;
                    break;

/*********************************************************************
                case eO97_OP_00_IMP:
                case eO97_OP_01_EQV:
                case eO97_OP_02_XOR:
                case eO97_OP_03_OR:
                case eO97_OP_04_AND:
                case eO97_OP_05_EQ:
                case eO97_OP_06_NE:
                case eO97_OP_07_LE:
                case eO97_OP_08_GE:
                case eO97_OP_09_LT:
                case eO97_OP_0A_GT:
                case eO97_OP_0B_PLUS:
                case eO97_OP_0C_MINUS:
                case eO97_OP_0D_MOD:
                case eO97_OP_0E_INT_DIV:
                case eO97_OP_0F_ASTERISK:
                case eO97_OP_10_SLASH:
                case eO97_OP_11_AMPERSAND:
                case eO97_OP_12_LIKE:
                case eO97_OP_13_CARAT:
                case eO97_OP_14_IS:
                case eO97_OP_15_NOT:
                case eO97_OP_16_NEGATE:
                case eO97_OP_17_ABS:
                case eO97_OP_18_FIX:
                case eO97_OP_19_INT:
                case eO97_OP_1A_SGN:
                case eO97_OP_1B_LEN:
                case eO97_OP_1C_LENB:
                case eO97_OP_1D_PARENTHESIZE:
                case eO97_OP_1E:
                case eO97_OP_1F:
                case eO97_OP_22:
                case eO97_OP_23:
                case eO97_OP_26:
                case eO97_OP_29:
                case eO97_OP_2A:
                case eO97_OP_2D:
                case eO97_OP_30:
                case eO97_OP_31:
                case eO97_OP_32:
                case eO97_OP_33:
                case eO97_OP_34:
                case eO97_OP_36:
                case eO97_OP_38:
                case eO97_OP_3A:
                case eO97_OP_3C:
                case eO97_OP_3D:
                case eO97_OP_3E:
                case eO97_OP_3F:
                case eO97_OP_40:
                case eO97_OP_44:
                case eO97_OP_46_THEN_ELSE_SINGLE:
                case eO97_OP_47:
                case eO97_OP_48_CASE:
                case eO97_OP_49_CASE_TO:
                case eO97_OP_4A_CASE_GT:
                case eO97_OP_4B_CASE_LT:
                case eO97_OP_4C_CASE_GE:
                case eO97_OP_4D_CASE_LE:
                case eO97_OP_4E_CASE_NE:
                case eO97_OP_4F_CASE_EQ:
                case eO97_OP_50_CASE_ELSE:
                case eO97_OP_51_CASE_EOL:
                case eO97_OP_52:
                case eO97_OP_54_CLOSE_ALL:
                case eO97_OP_55_TYPE_CONVERSION:
                case eO97_OP_56:
                case eO97_OP_57:
                case eO97_OP_58:
                case eO97_OP_5B_TYPE_ELEMENT:
                case eO97_OP_5C_DO:
                case eO97_OP_5D:
                case eO97_OP_5E_DO_UNTIL:
                case eO97_OP_5F_DO_WHILE:
                case eO97_OP_60_ELSE_SINGLE:
                case eO97_OP_61_ELSE_MULTIPLE:
                case eO97_OP_62_ELSEIF:
                case eO97_OP_63:
                case eO97_OP_64_END:
                case eO97_OP_65:
                case eO97_OP_67_END_SINGLE_IF:
                case eO97_OP_68_END_IF:
                case eO97_OP_69:
                case eO97_OP_6A_END_PROPERTY:
                case eO97_OP_6B_END_SELECT:
                case eO97_OP_6D_END_TYPE:
                case eO97_OP_6E_END_WITH:
                case eO97_OP_6F:
                case eO97_OP_70:
                case eO97_OP_71_EXIT_DO:
                case eO97_OP_72_EXIT_FOR:
                case eO97_OP_73_EXIT_FUNCTION:
                case eO97_OP_74_EXIT_PROPERTY:
                case eO97_OP_75_EXIT_SUB:
                case eO97_OP_76:
                case eO97_OP_77:
                case eO97_OP_78:
                case eO97_OP_79:
                case eO97_OP_7A:
                case eO97_OP_7B:
                case eO97_OP_7C:
                case eO97_OP_7D_INSTR_TWO_PARAMS:
                case eO97_OP_7E_INSTR_THREE_PARAMS:
                case eO97_OP_7F_INSTR_FOUR_PARAMS:
                case eO97_OP_80:
                case eO97_OP_81:
                case eO97_OP_82:
                case eO97_OP_83:
                case eO97_OP_84:
                case eO97_OP_85:
                case eO97_OP_86_STRCOMP_A_B:
                case eO97_OP_87_STRCOMP_A_B_C:
                case eO97_OP_88:
                case eO97_OP_89:
                case eO97_OP_8A:
                case eO97_OP_8B_B_TO_C:
                case eO97_OP_8C_EACH_B:
                case eO97_OP_8D:
                case eO97_OP_8E_B_TO_C_STEP_D:
                case eO97_OP_90:
                case eO97_OP_91:
                case eO97_OP_92:
                case eO97_OP_94_IF_THEN_SINGLE:
                case eO97_OP_95_IF_THEN_MULTIPLE:
                case eO97_OP_96:
                case eO97_OP_97:
                case eO97_OP_98:
                case eO97_OP_99:
                case eO97_OP_9A:
                case eO97_OP_9C:
                case eO97_OP_9D:
                case eO97_OP_9F:
                case eO97_OP_A0:
                case eO97_OP_A1:
                case eO97_OP_A2:
                case eO97_OP_A3_NO_LEN:
                case eO97_OP_A8_NOTHING:
                case eO97_OP_A9:
                case eO97_OP_AA:
                case eO97_OP_AB:
                case eO97_OP_AD:
                case eO97_OP_AF_TRUE_OR_FALSE:
                case eO97_OP_B0:
                case eO97_OP_B1_LOOP:
                case eO97_OP_B2_LOOP_UNTIL:
                case eO97_OP_B3_LOOP_WHILE:
                case eO97_OP_B4:
                case eO97_OP_B5:
                case eO97_OP_B6:
                case eO97_OP_B7:
                case eO97_OP_B8:
                case eO97_OP_B9:
                case eO97_OP_BA:
                case eO97_OP_BB:
                case eO97_OP_BC:
                case eO97_OP_BD:
                case eO97_OP_BF_NEXT:
                case eO97_OP_C0_NEXT_A:
                case eO97_OP_C2:
                case eO97_OP_C3:
                case eO97_OP_C5_OPTION:
                case eO97_OP_C6_ARRAY_SIZE:
                case eO97_OP_C7:
                case eO97_OP_C8:
                case eO97_OP_CA:
                case eO97_OP_CB:
                case eO97_OP_CC:
                case eO97_OP_CD:
                case eO97_OP_CE:
                case eO97_OP_CF:
                case eO97_OP_D0:
                case eO97_OP_D1:
                case eO97_OP_D2:
                case eO97_OP_D3:
                case eO97_OP_D4:
                case eO97_OP_D5:
                case eO97_OP_D6:
                case eO97_OP_D7:
                case eO97_OP_DE:
                case eO97_OP_DF:
                case eO97_OP_E0:
                case eO97_OP_E1:
                case eO97_OP_E2_SELECT_CASE:
                case eO97_OP_E3:
                case eO97_OP_E4:
                case eO97_OP_E5_SET:
                case eO97_OP_E6:
                case eO97_OP_E7:
                case eO97_OP_E9:
                case eO97_OP_EB_WEND:
                case eO97_OP_EC_WHILE:
                case eO97_OP_ED_WITH_A:
                case eO97_OP_EE_WRITEN_BEGIN:
                case eO97_OP_EF:
                case eO97_OP_F0:
                case eO97_OP_F1:
                case eO97_OP_F2:
                case eO97_OP_F3:
                case eO97_OP_F4:
                case eO97_OP_F5:
                case eO97_OP_F6_FOR_NEXT_A:
                case eO97_OP_F7_FOR_NEXT:
                case eO97_OP_F8:
                case eO97_OP_F9_WITH:
                case eO97_OP_FA:
                case eO97_OP_FB:
                case eO97_OP_FC:
                case eO97_OP_FD:
                case eO97_OP_FE:
                case eO97_OP_FF:
*********************************************************************/
                default:
                    dwOffset += 2;
                    break;
            }

            if (lpstEnv->eError != eO97_ERROR_NO_ERROR)
                break;

            // Advance past the token (set next ip)

            lpstEnv->dwNextIPOffset = dwOffset - stLine.dwOffset;

            if (gapfnO97_OP[lpstEnv->abyOpcode[0]](lpstEnv) == FALSE)
            {
                lpstEnv->eError = eO97_ERROR_GENERIC;
                break;
            }

            if (lpstEnv->bNewIP != FALSE)
            {
                lpstEnv->bNewIP = FALSE;
                break;
            }

            if (lpstEnv->bRedoToken == FALSE)
            {
                lpstEnv->dwIPOffset = lpstEnv->dwNextIPOffset;
            }
        }
    }

//    printf("dwNumIter=%lu\n",dwNumIter);

    if (O97VarPopGlobalFrame(lpstEnv) == FALSE)
        bResult = FALSE;

    while (lpstEnv->lpstModEnv->lpstPrev != NULL)
    {
        lpstNextModEnv = lpstEnv->lpstModEnv->lpstPrev;
        O97ModuleEnvDestroy(lpstEnv,lpstEnv->lpstModEnv);
        lpstEnv->lpstModEnv = lpstNextModEnv;
    }

    return(bResult);
}


