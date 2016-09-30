//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/STATE.CPv   1.1   09 Nov 1998 13:44:00   DCHI  $
//
// Description:
//  Macro emulation environment state array definition.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/STATE.CPv  $
// 
//    Rev 1.1   09 Nov 1998 13:44:00   DCHI
// Added #pragma data_seg("FAR_DATA_0") for global FAR data.
// 
//    Rev 1.0   30 Jun 1997 16:15:44   DCHI
// Initial revision.
// 
//************************************************************************

#include "wd7api.h"
#include "wd7env.h"

void StateERROR(LPENV lpstEnv);
void StateEXPR(LPENV lpstEnv);
void StateEXPR_IDENTIFIER(LPENV lpstEnv);
void StateEXPR_BUILTIN(LPENV lpstEnv);
void StateEXPR_EXTERNAL(LPENV lpstEnv);
void StateSTATEMENT(LPENV lpstEnv);
void StateSTATEMENT_DLG_FIELD(LPENV lpstEnv);
void StateSTATEMENT_IDENTIFIER(LPENV lpstEnv);
void StateSTATEMENT_BUILTIN(LPENV lpstEnv);
void StateSTATEMENT_EXTERNAL(LPENV lpstEnv);
void StateFUNCTION_CALL(LPENV lpstEnv);
void StateSUB_CALL(LPENV lpstEnv);
void StateBUILTIN_FUNCTION_CALL(LPENV lpstEnv);
void StateBUILTIN_SUB_CALL(LPENV lpstEnv);
void StateEXTERNAL_FUNCTION_CALL(LPENV lpstEnv);
void StateEXTERNAL_SUB_CALL(LPENV lpstEnv);
void StateEXTERNAL_CLEANUP(LPENV lpstEnv);
void StateCALL_GET_PARAM(LPENV lpstEnv);
void StateGET_DIALOG_FIELD_PARAM(LPENV lpstEnv);
void StateCALL_PUSH_PARAM(LPENV lpstEnv);
void StateCALL_PASS_BY_REF(LPENV lpstEnv);
void StateCALL_FUNCTION(LPENV lpstEnv);
void StateCALL_SUB(LPENV lpstEnv);
void StateCALL_GET_ARGUMENTS(LPENV lpstEnv);
void StateCALL_BODY(LPENV lpstEnv);
void StateCALL_END(LPENV lpstEnv);
void StateON(LPENV lpstEnv);
void StateON_ERROR(LPENV lpstEnv);
void StateON_ERROR_GOTO(LPENV lpstEnv);
void StateON_ERROR_RESUME(LPENV lpstEnv);
void StateDLG_FIELD_ASSIGN(LPENV lpstEnv);
void StateASSIGN(LPENV lpstEnv);
void StateIF_COND(LPENV lpstEnv);
void StateIF_THEN(LPENV lpstEnv);
void StateTHEN_SINGLE(LPENV lpstEnv);
void StateTHEN_MULTIPLE(LPENV lpstEnv);
void StateELSE_SINGLE(LPENV lpstEnv);
void StateELSE_MULTIPLE(LPENV lpstEnv);
void StateIF_THEN_END(LPENV lpstEnv);
void StateFOR_IDENTIFIER(LPENV lpstEnv);
void StateFOR_EQUAL(LPENV lpstEnv);
void StateFOR_START(LPENV lpstEnv);
void StateFOR_END(LPENV lpstEnv);
void StateFOR_STEP(LPENV lpstEnv);
void StateFOR_BODY(LPENV lpstEnv);
void StateFOR_NEXT(LPENV lpstEnv);
void StateWHILE(LPENV lpstEnv);
void StateWHILE_COND(LPENV lpstEnv);
void StateWHILE_BODY(LPENV lpstEnv);
void StateWHILE_WEND(LPENV lpstEnv);
void StateDIM(LPENV lpstEnv);
void StateDIM_IDENTIFIER(LPENV lpstEnv);
void StateDIM_IDENTIFIER_AS(LPENV lpstEnv);
void StateGET_CUR_VALUES(LPENV lpstEnv);
void StateEXPR_DIALOG(LPENV lpstEnv);
void StateDIALOG_SUB_CALL(LPENV lpstEnv);
void StateDIALOG_FUNCTION_CALL(LPENV lpstEnv);
void StateGOTO(LPENV lpstEnv);
void StateGOTO_IDENTIFIER0(LPENV lpstEnv);
void StateGOTO_IDENTIFIER1(LPENV lpstEnv);
void StateGOTO_WORD0(LPENV lpstEnv);
void StateGOTO_WORD1(LPENV lpstEnv);
void StateGOTO_CHECK_FOR_SPECIAL(LPENV lpstEnv);
void StateBEGIN_DIALOG(LPENV lpstEnv);
void StateBEGIN_DIALOG_END(LPENV lpstEnv);
void StateIGNORE_EXPR_RESULT(LPENV lpstEnv);
void StateSELECT(LPENV lpstEnv);
void StateSELECT_CASE(LPENV lpstEnv);
void StateSELECT_BODY(LPENV lpstEnv);
void StateSELECT_END(LPENV lpstEnv);
void StateCASE(LPENV lpstEnv);
void StateCASE_EXPR(LPENV lpstEnv);
void StateCASE_TO(LPENV lpstEnv);
void StateCASE_IS(LPENV lpstEnv);
void StateCASE_IS_EXPR(LPENV lpstEnv);
void StateCASE_BODY(LPENV lpstEnv);
void StateCASE_FOUND_MATCH(LPENV lpstEnv);
void StateSKIP_CASE_LINE(LPENV lpstEnv);
void StateSKIP_LINE(LPENV lpstEnv);

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

LPFNSTATE FAR gapfnState[] =
{
    NULL,
    NULL,
    StateERROR,
    StateEXPR,
    StateEXPR_IDENTIFIER,
    StateEXPR_BUILTIN,
    StateEXPR_EXTERNAL,
    StateSTATEMENT,
    StateSTATEMENT_DLG_FIELD,
    StateSTATEMENT_IDENTIFIER,
    StateSTATEMENT_BUILTIN,
    StateSTATEMENT_EXTERNAL,
    StateFUNCTION_CALL,
    StateSUB_CALL,
    StateBUILTIN_FUNCTION_CALL,
    StateBUILTIN_SUB_CALL,
    StateEXTERNAL_FUNCTION_CALL,
    StateEXTERNAL_SUB_CALL,
    StateEXTERNAL_CLEANUP,
    StateCALL_GET_PARAM,
    StateGET_DIALOG_FIELD_PARAM,
    StateCALL_PUSH_PARAM,
    StateCALL_PASS_BY_REF,
    StateCALL_FUNCTION,
    StateCALL_SUB,
    StateCALL_GET_ARGUMENTS,
    StateCALL_BODY,
    StateCALL_END,
    StateON,
    StateON_ERROR,
    StateON_ERROR_GOTO,
    StateON_ERROR_RESUME,
    StateDLG_FIELD_ASSIGN,
    StateASSIGN,
    StateIF_COND,
    StateIF_THEN,
    StateTHEN_SINGLE,
    StateTHEN_MULTIPLE,
    StateELSE_SINGLE,
    StateELSE_MULTIPLE,
    StateIF_THEN_END,
    StateFOR_IDENTIFIER,
    StateFOR_EQUAL,
    StateFOR_START,
    StateFOR_END,
    StateFOR_STEP,
    StateFOR_BODY,
    StateFOR_NEXT,
    StateWHILE,
    StateWHILE_COND,
    StateWHILE_BODY,
    StateWHILE_WEND,
    StateDIM,
    StateDIM_IDENTIFIER,
    StateDIM_IDENTIFIER_AS,
    StateGET_CUR_VALUES,
    StateEXPR_DIALOG,
    StateDIALOG_SUB_CALL,
    StateDIALOG_FUNCTION_CALL,
    StateGOTO,
    StateGOTO_IDENTIFIER0,
    StateGOTO_IDENTIFIER1,
    StateGOTO_WORD0,
    StateGOTO_WORD1,
    StateGOTO_CHECK_FOR_SPECIAL,
    StateBEGIN_DIALOG,
    StateBEGIN_DIALOG_END,
    StateIGNORE_EXPR_RESULT,
    StateSELECT,
    StateSELECT_CASE,
    StateSELECT_BODY,
    StateSELECT_END,
    StateCASE,
    StateCASE_EXPR,
    StateCASE_TO,
    StateCASE_IS,
    StateCASE_IS_EXPR,
    StateCASE_BODY,
    StateCASE_FOUND_MATCH,
    StateSKIP_CASE_LINE,
    StateSKIP_LINE,
    NULL
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

