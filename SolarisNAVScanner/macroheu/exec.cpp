//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/EXEC.CPv   1.2   12 Oct 1998 13:18:22   DCHI  $
//
// Description:
//  Macro emulation environment main execution function.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/EXEC.CPv  $
// 
//    Rev 1.2   12 Oct 1998 13:18:22   DCHI
// Added NLM relinquish control call to WD7Execute().
// 
//    Rev 1.1   15 Jul 1997 19:15:32   DCHI
// Changed interpretation of 0x7C to UPASCAL_COMMENT.
// 
//    Rev 1.0   30 Jun 1997 16:15:26   DCHI
// Initial revision.
// 
//************************************************************************

#include <assert.h>

#include "wd7api.h"
#include "wd7env.h"
#include "wdop.h"

#define LITTLE_ENDIAN       0
#define BIG_ENDIAN          1

ETOKEN_T gWDOP_TOKEN_map[] =
{
    eTOKEN_SKIP,        // WDOP_00                       0x00    // "?0x00?"
    eTOKEN_SKIP,        // WDOP_01                       0x01    // "?0x01?"
    eTOKEN_NOT,         // WDOP_NOT                      0x02    // "Not"
    eTOKEN_AND,         // WDOP_AND                      0x03    // "And"
    eTOKEN_OR,          // WDOP_OR                       0x04    // "Or"
    eTOKEN_OPEN_PAREN,  // WDOP_OPEN_PAREN               0x05    // "("
    eTOKEN_CLOSE_PAREN, // WDOP_CLOSE_PAREN              0x06    // ")"
    eTOKEN_PLUS,        // WDOP_PLUS                     0x07    // "+"
    eTOKEN_MINUS,       // WDOP_MINUS                    0x08    // "-"
    eTOKEN_SLASH,       // WDOP_SLASH                    0x09    // "/"
    eTOKEN_ASTERISK,    // WDOP_ASTERISK                 0x0A    // "*"
    eTOKEN_MOD,         // WDOP_MOD                      0x0B    // "Mod"
    eTOKEN_EQ,          // WDOP_EQUAL                    0x0C    // "="
    eTOKEN_NE,          // WDOP_NOT_EQUAL                0x0D    // "<>"
    eTOKEN_LT,          // WDOP_LESS_THAN                0x0E    // "<"
    eTOKEN_GT,          // WDOP_GREATER_THAN             0x0F    // ">"
    eTOKEN_LE,          // WDOP_LESS_THAN_EQUAL          0x10    // "<="
    eTOKEN_GE,          // WDOP_GREATER_THAN_EQUAL       0x11    // ">="
    eTOKEN_COMMA,       // WDOP_COMMA                    0x12    // ","
    eTOKEN_SKIP,        // WDOP_13                       0x13    // "?0x13?"
    eTOKEN_SKIP,        // WDOP_14                       0x14    // "?0x14?"
    eTOKEN_SKIP,        // WDOP_15                       0x15    // "?0x15?"
    eTOKEN_SKIP,        // WDOP_16                       0x16    // "?0x16?"
    eTOKEN_SKIP,        // WDOP_17                       0x17    // "?0x17?"
    eTOKEN_RESUME,      // WDOP_RESUME                   0x18    // "Resume"
    eTOKEN_COLON,       // WDOP_COLON                    0x19    // ":"
    eTOKEN_END,         // WDOP_END                      0x1A    // "End"
    eTOKEN_SUB,         // WDOP_SUB                      0x1B    // "Sub"
    eTOKEN_FUNCTION,    // WDOP_FUNCTION                 0x1C    // "Function"
    eTOKEN_IF,          // WDOP_IF                       0x1D    // "If"
    eTOKEN_THEN,        // WDOP_THEN                     0x1E    // "Then"
    eTOKEN_ELSE_IF,     // WDOP_ELSE_IF                  0x1F    // "ElseIf"
    eTOKEN_ELSE,        // WDOP_ELSE                     0x20    // "Else"
    eTOKEN_WHILE,       // WDOP_WHILE                    0x21    // "While"
    eTOKEN_WEND,        // WDOP_WEND                     0x22    // "Wend"
    eTOKEN_FOR,         // WDOP_FOR                      0x23    // "For"
    eTOKEN_TO,          // WDOP_TO                       0x24    // "To"
    eTOKEN_STEP,        // WDOP_STEP                     0x25    // "Step"
    eTOKEN_NEXT,        // WDOP_NEXT                     0x26    // "Next"
    eTOKEN_SKIP,        // WDOP_27                       0x27    // "?0x27?"
    eTOKEN_SEMICOLON,   // WDOP_SEMICOLON                0x28    // ";"
    eTOKEN_CALL,        // WDOP_CALL                     0x29    // "Call"
    eTOKEN_GOTO,        // WDOP_GOTO                     0x2A    // "Goto"
    eTOKEN_SKIP,        // WDOP_2B                       0x2B    // "?0x2B?"
    eTOKEN_ON,          // WDOP_ON                       0x2C    // "On"
    eTOKEN_ERROR,       // WDOP_ERROR                    0x2D    // "Error"
    eTOKEN_LET,         // WDOP_LET                      0x2E    // "Let"
    eTOKEN_DIM,         // WDOP_DIM                      0x2F    // "Dim"
    eTOKEN_SHARED,      // WDOP_SHARED                   0x30    // "Shared"
    eTOKEN_SELECT,      // WDOP_SELECT                   0x31    // "Select"
    eTOKEN_IS,          // WDOP_IS                       0x32    // "Is"
    eTOKEN_CASE,        // WDOP_CASE                     0x33    // "Case"
    eTOKEN_AS,          // WDOP_AS                       0x34    // "As"
    eTOKEN_REDIM,       // WDOP_REDIM                    0x35    // "Redim"
    eTOKEN_PRINT,       // WDOP_PRINT                    0x36    // "Print"
    eTOKEN_INPUT,       // WDOP_INPUT                    0x37    // "Input"
    eTOKEN_LINE,        // WDOP_LINE                     0x38    // "Line"
    eTOKEN_WRITE,       // WDOP_WRITE                    0x39    // "Write"
    eTOKEN_NAME,        // WDOP_NAME                     0x3A    // "Name"
    eTOKEN_OUTPUT,      // WDOP_OUTPUT                   0x3B    // "Output"
    eTOKEN_APPEND,      // WDOP_APPEND                   0x3C    // "Append"
    eTOKEN_OPEN,        // WDOP_OPEN                     0x3D    // "Open"
    eTOKEN_GET_CUR_VALUES,  // WDOP_GET_CUR_VALUES           0x3E    // "GetCurValues"
    eTOKEN_DIALOG,      // WDOP_DIALOG                   0x3F    // "Dialog"
    eTOKEN_SUPER,       // WDOP_SUPER                    0x40    // "Super"
    eTOKEN_DECLARE,     // WDOP_DECLARE                  0x41    // "Declare"
    eTOKEN_DOUBLE,      // WDOP_DOUBLE                   0x42    // "Double"
    eTOKEN_INTEGER,     // WDOP_INTEGER                  0x43    // "Integer"
    eTOKEN_LONG,        // WDOP_LONG                     0x44    // "Long"
    eTOKEN_SINGLE,      // WDOP_SINGLE                   0x45    // "Single"
    eTOKEN_STRING,      // WDOP_STRING                   0x46    // "String"
    eTOKEN_CDECL,       // WDOP_CDECL                    0x47    // "Cdecl"
    eTOKEN_ALIAS,       // WDOP_ALIAS                    0x48    // "Alias"
    eTOKEN_ANY,         // WDOP_ANY                      0x49    // "Any"
    eTOKEN_TOOLS_GET_SPELLING,  // WDOP_TOOLS_GET_SPELLING       0x4A    // "ToolsGetSpelling"
    eTOKEN_TOOLS_GET_SYNONYMS,  // WDOP_TOOLS_GET_SYNONYMS       0x4B    // "ToolsGetSynonyms"
    eTOKEN_CLOSE,       // WDOP_CLOSE                    0x4C    // "Close"
    eTOKEN_BEGIN,       // WDOP_BEGIN                    0x4D    // "Begin"
    eTOKEN_LIB,         // WDOP_LIB                      0x4E    // "Lib"
    eTOKEN_READ,        // WDOP_READ                     0x4F    // "Read"
    eTOKEN_CHECK_DIALOG,// WDOP_CHECK_DIALOG             0x50    // "CheckDialog"
    eTOKEN_SKIP,        // WDOP_SPACE                    0x51    // " "
    eTOKEN_SKIP,        // WDOP_TAB                      0x52    // "    "
    eTOKEN_SKIP,        // WDOP_53                       0x53    // "?0x53?"
    eTOKEN_END_IF,      // WDOP_END_IF                   0x54    // "EndIf"
    eTOKEN_SKIP,        // WDOP_55                       0x55    // "?0x55?"
    eTOKEN_SKIP,        // WDOP_56                       0x56    // "?0x56?"
    eTOKEN_SKIP,        // WDOP_57                       0x57    // "?0x57?"
    eTOKEN_SKIP,        // WDOP_58                       0x58    // "?0x58?"
    eTOKEN_SKIP,        // WDOP_59                       0x59    // "?0x59?"
    eTOKEN_SKIP,        // WDOP_5A                       0x5A    // "?0x5A?"
    eTOKEN_SKIP,        // WDOP_5B                       0x5B    // "?0x5B?"
    eTOKEN_SKIP,        // WDOP_5C                       0x5C    // "?0x5C?"
    eTOKEN_SKIP,        // WDOP_5D                       0x5D    // "?0x5D?"
    eTOKEN_SKIP,        // WDOP_5E                       0x5E    // "?0x5E?"
    eTOKEN_SKIP,        // WDOP_5F                       0x5F    // "?0x5F?"
    eTOKEN_SKIP,        // WDOP_60                       0x60    // "?0x60?"
    eTOKEN_SKIP,        // WDOP_61                       0x61    // "?0x61?"
    eTOKEN_SKIP,        // WDOP_62                       0x62    // "?0x62?"
    eTOKEN_SKIP,        // WDOP_63                       0x63    // "?0x63?"
    eTOKEN_NEWLINE,     // WDOP_NEWLINE                  0x64    // "\n"
    eTOKEN_LABEL,       // WDOP_LABEL                    0x65    // "LABEL"
    eTOKEN_WORD_LABEL,  // WDOP_WORD_VALUE_LABEL         0x66    // "WORD_VALUE_LABEL"
    eTOKEN_FUNCTION_VALUE,  // WDOP_FUNCTION_VALUE           0x67    // "FUNCTION"
    eTOKEN_DOUBLE_VALUE,    // WDOP_DOUBLE_VALUE             0x68    // "DOUBLE_VALUE"
    eTOKEN_IDENTIFIER,      // WDOP_PASCAL_STRING            0x69    // "PASCAL_STRING"
    eTOKEN_QUOTED_STRING,   // WDOP_QUOTED_PASCAL_STRING     0x6A    // "QUOTED_PASCAL_STRING"
    eTOKEN_SKIP,        // WDOP_COMMENT_PASCAL_STRING    0x6B    // "COMMENT_PASCAL_STRING"
    eTOKEN_WORD_VALUE,  // WDOP_UNSIGNED_WORD_VALUE      0x6C    // "UNSIGNED_WORD_VALUE"
    eTOKEN_ASCII_CHAR,  // WDOP_ASCII_CHARACTER          0x6D    // "ASCII_CHARACTER"
    eTOKEN_SKIP,            // WDOP_MULTIPLE_SPACES          0x6E    // "MULTIPLE_SPACES"
    eTOKEN_SKIP,            // WDOP_MULTIPLE_TABS            0x6F    // "MULTIPLE_TABS"
    eTOKEN_SKIP,        // WDOP_REM                      0x70    // "REM"
    eTOKEN_SKIP,        // WDOP_POUND                    0x71    // "#"
    eTOKEN_SKIP,        // WDOP_BACKSLASH                0x72    // "\\"
    eTOKEN_DIALOG_FIELD,// WDOP_DIALOG_FIELD             0x73    // "DIALOG_FIELD"
    eTOKEN_SKIP,        // WDOP_74                       0x74    // "?0x74?"
    eTOKEN_SKIP,        // WDOP_75                       0x75    // "?0x75?"
    eTOKEN_EXTERNAL_MACRO,  // WDOP_EXTERNAL_MACRO           0x76    // "EXTERNAL_MACRO"
    eTOKEN_EXTERNAL_MACRO,  // WDOP_IDENTIFIER               0x77    // "IDENTIFIER"
    eTOKEN_SKIP,        // WDOP_78                       0x78    // "?0x78?"
    eTOKEN_QUOTED_USTRING,  // WDOP_QUOTED_UPASCAL_STRING    0x79    // "QUOTED_UPASCAL_STRING"
    eTOKEN_SKIP,        // WDOP_7A
    eTOKEN_UIDENTIFIER, // WDOP_UPASCAL_STRING_7B
    eTOKEN_SKIP,        // WDOP_UPASCAL_COMMENT
};

BOOL WD7Execute
(
    LPENV               lpstEnv,
    WORD                wMacroIndex,
    DWORD               dwMacroOffset,
    DWORD               dwMacroSize,
    BYTE                byMacroEncryptByte
)
{
    BYTE                byCurToken;
    WORD                wTemp;
    BYTE                byLen;
    BYTE                abyTempBuf[4];
    BYTE                abyBuf[256];
    LPCALL              lpstCall;
    LPCALL_GET_ARGS     lpstCallGetArgs;
    BOOL                bResult;
    LPMODULE_ENV        lpstNextModEnv;
    BOOL                bContinueLine;
    DWORD               dwNumIter;

    lpstEnv->lpstModEnv = ModuleEnvCreate(lpstEnv,
                                          lpstEnv->lpstKey,
                                          wMacroIndex,
                                          dwMacroSize,
                                          dwMacroOffset,
                                          byMacroEncryptByte);

    if (lpstEnv->lpstModEnv == NULL)
        return(FALSE);

    if (VarPushFrame(lpstEnv) == FALSE ||
        VarPushGlobalFrame(lpstEnv) == FALSE)
        return(FALSE);

    if (ModuleScan(lpstEnv->lpstModEnv) == FALSE)
    {
        ModuleEnvDestroy(lpstEnv,lpstEnv->lpstModEnv);
        return(FALSE);
    }

    /////////////////////////////////////////////////////////////
    // Push CALL_SUB state

    StatePush(lpstEnv,esCALL_SUB);

    if (ControlCreateControl(lpstEnv,ectCALL) == FALSE)
        return(FALSE);

    lpstCall = (LPCALL)ControlTop(lpstEnv);

    lpstCall->dwReturnIP = lpstEnv->lpstModEnv->dwSize;

    StatePush(lpstEnv,esCALL_GET_ARGUMENTS);

    if (ControlCreateControl(lpstEnv,ectCALL_GET_ARGS) == FALSE)
        return(FALSE);

    lpstCallGetArgs = (LPCALL_GET_ARGS)ControlTop(lpstEnv);
    lpstCallGetArgs->lpstParams = NULL;

    /////////////////////////////////////////////////////////////

    lpstEnv->dwIP = lpstEnv->dwRoutineStartIP =
        lpstEnv->lpstModEnv->dwMainIP;
    lpstEnv->bRedoToken = FALSE;
    lpstEnv->eError = eERROR_NO_ERROR;
    lpstEnv->lErr = 0;
    bResult = TRUE;
    bContinueLine = FALSE;
    dwNumIter = 0;
    while (StateTop(lpstEnv) != esINIT &&
           lpstEnv->eError == eERROR_NO_ERROR &&
           lpstEnv->dwIP < lpstEnv->lpstModEnv->dwSize &&
           dwNumIter++ < 30000)
    {
#ifdef SYM_NLM
            if ((dwNumIter & 0x1FF) == 0)
            {
                // Relinquish control every 512 iterations

                SSProgress(lpstEnv->lpvRootCookie);
            }
#endif // #ifdef SYM_NLM

        if (lpstEnv->bRedoToken == FALSE)
        {
            if (ModuleRead(lpstEnv->lpstModEnv,
                           lpstEnv->dwIP,
                           &byCurToken,
                           sizeof(BYTE)) == FALSE)
            {
                bResult = FALSE;
                break;
            }

            if (byCurToken <= 0x7C)
                lpstEnv->eToken = gWDOP_TOKEN_map[byCurToken];
            else
                lpstEnv->eToken = eTOKEN_SKIP;

            switch (lpstEnv->eToken)
            {
                case eTOKEN_IDENTIFIER:
                    // Read the identifier

                    if (ModuleReadIdentifier(lpstEnv->lpstModEnv,
                                             lpstEnv->dwIP + 1,
                                             &lpstEnv->uTokenData.
                                                 Identifier.byLen,
                                             abyBuf) == FALSE)
                    {
                        bResult = FALSE;
                        break;
                    }

                    lpstEnv->uTokenData.Identifier.lpby = abyBuf;
                    break;

                case eTOKEN_QUOTED_STRING:
                case eTOKEN_EXTERNAL_MACRO:
                case eTOKEN_LABEL:
                    // Read the string

                    if (ModuleRead(lpstEnv->lpstModEnv,
                                   lpstEnv->dwIP + 1,
                                   &byLen,
                                   sizeof(BYTE)) == FALSE)
                    {
                        bResult = FALSE;
                        break;
                    }

                    if (ModuleRead(lpstEnv->lpstModEnv,
                                   lpstEnv->dwIP + 1,
                                   abyBuf,
                                   byLen + 1) == FALSE)
                    {
                        bResult = FALSE;
                        break;
                    }

                    lpstEnv->uTokenData.String.lpby = abyBuf;
                    break;

                case eTOKEN_QUOTED_USTRING:
                    // Convert the double-byte string to a single-byte string

                    // Read the WORD

                    if (ModuleRead(lpstEnv->lpstModEnv,
                                   lpstEnv->dwIP + 1,
                                   abyBuf,
                                   2) == FALSE)
                    {
                        bResult = FALSE;
                        break;
                    }

                    if (lpstEnv->lpstModEnv->bLittleEndian == TRUE)
                    {
                        wTemp =
                            (((WORD)abyBuf[1]) << 8) | (WORD)abyBuf[0];
                    }
                    else
                    {
                        wTemp =
                            (((WORD)abyBuf[0]) << 8) | (WORD)abyBuf[1];
                    }

                    if (wTemp > 255)
                        abyBuf[0] = 255;
                    else
                        abyBuf[0] = (BYTE)wTemp;

                    for (wTemp=1;wTemp<=abyBuf[0];wTemp++)
                        if (ModuleRead(lpstEnv->lpstModEnv,
                                       lpstEnv->dwIP + 1 + 2 * wTemp,
                                       abyBuf + wTemp,
                                       sizeof(BYTE)) == FALSE)
                        {
                            bResult = FALSE;
                            break;
                        }

                    if (bResult == FALSE)
                        break;

                    lpstEnv->uTokenData.String.lpby = abyBuf;
                    lpstEnv->eToken = eTOKEN_QUOTED_STRING;
                    break;

                case eTOKEN_UIDENTIFIER:
                    // Read the identifier

                    if (ModuleReadUIdentifier(lpstEnv->lpstModEnv,
                                              lpstEnv->dwIP + 1,
                                              &lpstEnv->uTokenData.
                                                  Identifier.byLen,
                                              abyBuf) == FALSE)
                    {
                        bResult = FALSE;
                        break;
                    }

                    lpstEnv->uTokenData.Identifier.lpby = abyBuf;
                    lpstEnv->eToken = eTOKEN_IDENTIFIER;
                    break;

                case eTOKEN_WORD_VALUE:
                case eTOKEN_FUNCTION_VALUE:
                case eTOKEN_WORD_LABEL:
                case eTOKEN_DIALOG_FIELD:

                    // Read the WORD

                    if (ModuleRead(lpstEnv->lpstModEnv,
                                   lpstEnv->dwIP + 1,
                                   abyBuf,
                                   2) == FALSE)
                    {
                        bResult = FALSE;
                        break;
                    }

                    if (lpstEnv->lpstModEnv->bLittleEndian == TRUE)
                    {
                        lpstEnv->uTokenData.wValue =
                            (((WORD)abyBuf[1]) << 8) | (WORD)abyBuf[0];
                    }
                    else
                    {
                        lpstEnv->uTokenData.wValue =
                            (((WORD)abyBuf[0]) << 8) | (WORD)abyBuf[1];
                    }

                    switch (lpstEnv->eToken)
                    {
                        case eTOKEN_FUNCTION_VALUE:
                            lpstEnv->uTokenData.eWDCMD =
                                (EWDCMD_T)lpstEnv->uTokenData.wValue;
                            break;

                        case eTOKEN_DIALOG_FIELD:
                            lpstEnv->uTokenData.eWDDLG =
                                (EWDDLG_T)lpstEnv->uTokenData.wValue;
                            break;

                        default:
                            break;
                    }
                    break;

                case eTOKEN_DOUBLE_VALUE:
                    // Just make it a zero

                    lpstEnv->uTokenData.wValue = 0;
                    lpstEnv->eToken = eTOKEN_WORD_VALUE;
                    break;

                case eTOKEN_NEWLINE:
                    if (bContinueLine == TRUE)
                    {
                        bContinueLine = FALSE;
                        lpstEnv->eToken = eTOKEN_SKIP;
                    }
                    break;

                default:
                    break;
            }

            if (bResult == FALSE)
                break;

            // Advance past the token

            switch (byCurToken)
            {
                case WDOP_BACKSLASH:
                    bContinueLine = TRUE;
                    lpstEnv->dwNextIP = lpstEnv->dwIP + 1;
                    break;

                case WDOP_DOUBLE_VALUE:
                    lpstEnv->dwNextIP = lpstEnv->dwIP + 9;
                    break;

                case WDOP_LABEL:
                case WDOP_PASCAL_STRING:
                case WDOP_QUOTED_PASCAL_STRING:
                case WDOP_COMMENT_PASCAL_STRING:
                case WDOP_REM:
                case WDOP_EXTERNAL_MACRO:
                case WDOP_IDENTIFIER:
                    if (ModuleRead(lpstEnv->lpstModEnv,
                                   lpstEnv->dwIP + 1,
                                   &byLen,
                                   sizeof(BYTE)) == FALSE)
                    {
                        bResult = FALSE;
                        break;
                    }
                    lpstEnv->dwNextIP = lpstEnv->dwIP + byLen + 2;
                    break;

                case WDOP_UNSIGNED_WORD_VALUE:
                case WDOP_ASCII_CHARACTER:
                case WDOP_DIALOG_FIELD:
                case WDOP_WORD_VALUE_LABEL:
                case WDOP_FUNCTION_VALUE:
                    lpstEnv->dwNextIP = lpstEnv->dwIP + 3;
                    break;

                case WDOP_MULTIPLE_SPACES:
                case WDOP_MULTIPLE_TABS:
                    lpstEnv->dwNextIP = lpstEnv->dwIP + 2;
                    break;

                case WDOP_QUOTED_UPASCAL_STRING:
                    // Read the WORD

                    if (ModuleRead(lpstEnv->lpstModEnv,
                                   lpstEnv->dwIP + 1,
                                   abyTempBuf,
                                   2) == FALSE)
                    {
                        bResult = FALSE;
                        break;
                    }

                    if (lpstEnv->lpstModEnv->bLittleEndian == TRUE)
                    {
                        wTemp =
                            (((WORD)abyTempBuf[1]) << 8) | (WORD)abyTempBuf[0];
                    }
                    else
                    {
                        wTemp =
                            (((WORD)abyTempBuf[0]) << 8) | (WORD)abyTempBuf[1];
                    }

                    lpstEnv->dwNextIP = lpstEnv->dwIP + 2 * wTemp + 3;
                    break;

                case WDOP_UPASCAL_STRING_7B:
                case WDOP_UPASCAL_STRING:
                    if (ModuleRead(lpstEnv->lpstModEnv,
                                   lpstEnv->dwIP + 1,
                                   &byLen,
                                   sizeof(BYTE)) == FALSE)
                    {
                        bResult = FALSE;
                        break;
                    }
                    lpstEnv->dwNextIP = lpstEnv->dwIP + 2 * byLen + 2;
                    break;

                default:
                    lpstEnv->dwNextIP = lpstEnv->dwIP + 1;
                    break;
            }
        }

        if (bResult == FALSE)
            break;

        // Perform any necessary state transitions

        if (lpstEnv->eToken != eTOKEN_SKIP)
        {
            lpstEnv->bRedoToken = FALSE;

            assert(esBEGIN_INVALID < lpstEnv->lpstTopControl->eState &&
                   lpstEnv->lpstTopControl->eState < esEND_INVALID);

            gapfnState[lpstEnv->lpstTopControl->eState](lpstEnv);
        }

        if (lpstEnv->bRedoToken == FALSE)
        {
            lpstEnv->dwIP = lpstEnv->dwNextIP;
        }
    }

    if (VarPopGlobalFrame(lpstEnv) == FALSE)
        bResult = FALSE;

    while (lpstEnv->lpstModEnv != NULL)
    {
        lpstNextModEnv = lpstEnv->lpstModEnv->lpstPrev;
        ModuleEnvDestroy(lpstEnv,lpstEnv->lpstModEnv);
        lpstEnv->lpstModEnv = lpstNextModEnv;
    }

    return(TRUE);
}


