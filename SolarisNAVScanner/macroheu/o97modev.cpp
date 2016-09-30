//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/O97MODEV.CPv   1.2   09 Nov 1998 13:44:02   DCHI  $
//
// Description:
//  Module environment creation and initialization.
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/O97MODEV.CPv  $
// 
//    Rev 1.2   09 Nov 1998 13:44:02   DCHI
// Added #pragma data_seg("FAR_DATA_0") for global FAR data.
// 
//    Rev 1.1   12 Oct 1998 13:25:30   DCHI
// Added O97ModuleGetOpcode() to provide easy O97, Mac O98, and O9
// compatibility without having to change much else.
// 
//    Rev 1.0   15 Aug 1997 13:29:32   DCHI
// Initial revision.
// 
//************************************************************************

#include "o97api.h"
#include "o97env.h"

//*************************************************************************
//
// BOOL O97ModuleGetOpcode()
//
// Parameters:
//  lpstModule          Ptr to module structure
//  dwOffset            Offset of opcode
//  lpabyOpcode         Ptr to two-byte buffer for opcode
//
// Description:
//  The function reads the two bytes from the given offset of the
//  given module.
//
//  The opcode is properly endianized and converted to an O97 opcode.
//
// Returns:
//  TRUE                On success
//  FALSE               On failure
//
//*************************************************************************

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyXOpO98toO97[O98_OP_MAX+1] =
{
    0x00,       // 0x00 // Imp
    0x01,       // 0x01 // Eqv
    0x02,       // 0x02 // Xor
    0x03,       // 0x03 // Or
    0x04,       // 0x04 // And
    0x05,       // 0x05 // Eq
    0x06,       // 0x06 // Ne
    0x07,       // 0x07 // Le
    0x08,       // 0x08 // Ge
    0x09,       // 0x09 // Lt
    0x0A,       // 0x0A // Gt
    0x0B,       // 0x0B // Add
    0x0C,       // 0x0C // Sub
    0x0D,       // 0x0D // Mod
    0x0E,       // 0x0E // IDv
    0x0F,       // 0x0F // Mul
    0x10,       // 0x10 // Div
    0x11,       // 0x11 // Concat
    0x12,       // 0x12 // Like
    0x13,       // 0x13 // Pwr
    0x14,       // 0x14 // Is
    0x15,       // 0x15 // Not
    0x16,       // 0x16 // UMi
    0x17,       // 0x17 // FnAbs
    0x18,       // 0x18 // FnFix
    0x19,       // 0x19 // FnInt
    0x1A,       // 0x1A // FnSgn
    0x1B,       // 0x1B // FnLen
    0x1C,       // 0x1C // FnLenB
    0x1D,       // 0x1D // Paren
    0x1E,       // 0x1E // Sharp
    0x1F,       // 0x1F // LdLHS
    0x20,       // 0x20 // Ld
    0x21,       // 0x21 // MemLd
    0x22,       // 0x22 // DictLd
    0x23,       // 0x23 // IndexLd
    0x24,       // 0x24 // ArgsLd
    0x25,       // 0x25 // ArgsMemLd
    0x26,       // 0x26 // ArgsDictLd
    0x27,       // 0x27 // St
    0x28,       // 0x28 // MemSt
    0x29,       // 0x29 // DictSt
    0x2A,       // 0x2A // IndexSt
    0x2B,       // 0x2B // ArgsSt
    0x2C,       // 0x2C // ArgsMemSt
    0x2D,       // 0x2D // ArgsDictSt
    0x2E,       // 0x2E // Set
    0x2F,       // 0x2F // MemSet
    0x30,       // 0x30 // DictSet
    0x31,       // 0x31 // IndexSet
    0x32,       // 0x32 // ArgsSet
    0x33,       // 0x33 // ArgsMemSet
    0x34,       // 0x34 // ArgsDictSet
    0x35,       // 0x35 // MemLdWith
    0x36,       // 0x36 // DictLdWith
    0x37,       // 0x37 // ArgsMemLdWith
    0x38,       // 0x38 // ArgsDictLdWith
    0x39,       // 0x39 // MemStWith
    0x3A,       // 0x3A // DictStWith
    0x3B,       // 0x3B // ArgsMemStWith
    0x3C,       // 0x3C // ArgsDictStWith
    0x3D,       // 0x3D // MemSetWith
    0x3E,       // 0x3E // DictSetWith
    0x3F,       // 0x3F // ArgsMemSetWith
    0x40,       // 0x40 // ArgsDictSetWith
    0x41,       // 0x41 // ArgsCall
    0x42,       // 0x42 // ArgsMemCall
    0x43,       // 0x43 // ArgsMemCallWith
    0x44,       // 0x44 // ArgsArray
    O97_OP_MAX, // 0x45 // *Assert
    0x45,       // 0x46 // Bos
    0x46,       // 0x47 // BosImplicit
    0x47,       // 0x48 // Bol
    0xA4,       // 0x49 // *LdAddressOf
    0xA4,       // 0x4A // *MemAddressOf
    0x48,       // 0x4B // Case
    0x49,       // 0x4C // CaseTo
    0x4A,       // 0x4D // CaseGt
    0x4B,       // 0x4E // CaseLt
    0x4C,       // 0x4F // CaseGe
    0x4D,       // 0x50 // CaseLe
    0x4E,       // 0x51 // CaseNe
    0x4F,       // 0x52 // CaseEq
    0x50,       // 0x53 // CaseElse
    0x51,       // 0x54 // CaseDone
    0x52,       // 0x55 // Circle
    0x53,       // 0x56 // Close
    0x54,       // 0x57 // CloseAll
    0x55,       // 0x58 // Coerce
    0x56,       // 0x59 // CoerceVar
    0x57,       // 0x5A // Context
    0x58,       // 0x5B // Debug
    0x59,       // 0x5C // Deftype
    0x5A,       // 0x5D // Dim
    0x5B,       // 0x5E // DimImplicit
    0x5C,       // 0x5F // Do
    0x5D,       // 0x60 // DoEvents
    0x5E,       // 0x61 // DoUntil
    0x5F,       // 0x62 // DoWhile
    0x60,       // 0x63 // Else
    0x61,       // 0x64 // ElseBlock
    0x62,       // 0x65 // ElseIfBlock
    0x63,       // 0x66 // ElseIfTypeBlock
    0x64,       // 0x67 // End
    0x65,       // 0x68 // EndContext
    0x66,       // 0x69 // EndFunc
    0x67,       // 0x6A // EndIf
    0x68,       // 0x6B // EndIfBlock
    0x69,       // 0x6C // EndImmediate
    0x6A,       // 0x6D // EndProp
    0x6B,       // 0x6E // EndSelect
    0x6C,       // 0x6F // EndSub
    0x6D,       // 0x70 // EndType
    0x6E,       // 0x71 // EndWith
    0x6F,       // 0x72 // Erase
    0x70,       // 0x73 // Error
    0xA5,       // 0x74 // *EventDecl
    0xA5,       // 0x75 // *RaiseEvent
    0xA5,       // 0x76 // *ArgsMemRaiseEvent
    0xA5,       // 0x77 // *ArgsMemRaiseEventWith
    0x71,       // 0x78 // ExitDo
    0x72,       // 0x79 // ExitFor
    0x73,       // 0x7A // ExitFunc
    0x74,       // 0x7B // ExitProp
    0x75,       // 0x7C // ExitSub
    0x76,       // 0x7D // FnCurDir
    0x77,       // 0x7E // FnDir
    0x78,       // 0x7F // Empty0
    0x79,       // 0x80 // Empty1
    0x7A,       // 0x81 // FnError
    0x7B,       // 0x82 // FnFormat
    0x7C,       // 0x83 // FnFreeFile
    0x7D,       // 0x84 // FnInStr
    0x7E,       // 0x85 // FnInStr3
    0x7F,       // 0x86 // FnInStr4
    0x80,       // 0x87 // FnInStrB
    0x81,       // 0x88 // FnInStrB3
    0x82,       // 0x89 // FnInStrB4
    0x83,       // 0x8A // FnLBound
    0x84,       // 0x8B // FnMid
    0x85,       // 0x8C // FnMidB
    0x86,       // 0x8D // FnStrComp
    0x87,       // 0x8E // FnStrComp3
    0x88,       // 0x8F // FnStringVar
    0x89,       // 0x90 // FnStringStr
    0x8A,       // 0x91 // FnUBound
    0x8B,       // 0x92 // For
    0x8C,       // 0x93 // ForEach
    0x8D,       // 0x94 // ForEachAs
    0x8E,       // 0x95 // ForStep
    0x8F,       // 0x96 // FuncDefn
    0x90,       // 0x97 // FuncDefnSave
    0x91,       // 0x98 // GetRec
    0x92,       // 0x99 // Gosub
    0x93,       // 0x9A // Goto
    0x94,       // 0x9B // If
    0x95,       // 0x9C // IfBlock
    0x96,       // 0x9D // TypeOf
    0x97,       // 0x9E // IfTypeBlock
    0xA4,       // 0x9F // *Implements
    0x98,       // 0xA0 // Input
    0x99,       // 0xA1 // InputDone
    0x9A,       // 0xA2 // InputItem
    0x9B,       // 0xA3 // Label
    0x9C,       // 0xA4 // Let
    0x9D,       // 0xA5 // Line
    0x9E,       // 0xA6 // LineCont
    0x9F,       // 0xA7 // LineInput
    0xA0,       // 0xA8 // LineNum
    0xA1,       // 0xA9 // LitCy
    0xA2,       // 0xAA // LitDate
    0xA3,       // 0xAB // LitDefault
    0xA4,       // 0xAC // LitDI2
    0xA5,       // 0xAD // LitDI4
    0xA6,       // 0xAE // LitHI2
    0xA7,       // 0xAF // LitHI4
    0xA8,       // 0xB0 // LitNothing
    0xA9,       // 0xB1 // LitOI2
    0xAA,       // 0xB2 // LitOI4
    0xAB,       // 0xB3 // LitR4
    0xAC,       // 0xB4 // LitR8
    0xAD,       // 0xB5 // LitSmallI2
    0xAE,       // 0xB6 // LitStr
    0xAF,       // 0xB7 // LitVarSpecial
    0xB0,       // 0xB8 // Lock
    0xB1,       // 0xB9 // Loop
    0xB2,       // 0xBA // LoopUntil
    0xB3,       // 0xBB // LoopWhile
    0xB4,       // 0xBC // Lset
    0xB5,       // 0xBD // Me
    0xB6,       // 0xBE // MeImplicit
    0xB7,       // 0xBF // MemRedim
    0xB8,       // 0xC0 // MemRedimWith
    0xB9,       // 0xC1 // MemRedimAs
    0xBA,       // 0xC2 // MemRedimAsWith
    0xBB,       // 0xC3 // Mid
    0xBC,       // 0xC4 // MidB
    0xBD,       // 0xC5 // Name
    0xBE,       // 0xC6 // New
    0xBF,       // 0xC7 // Next
    0xC0,       // 0xC8 // NextVar
    0xC1,       // 0xC9 // OnError
    0xC2,       // 0xCA // OnGosub
    0xC3,       // 0xCB // OnGoto
    0xC4,       // 0xCC // Open
    0xC5,       // 0xCD // Option
    0xC6,       // 0xCE // OptionBase
    0xC7,       // 0xCF // ParamByval
    0xC8,       // 0xD0 // ParamOmitted
    0xC9,       // 0xD1 // ParamNamed
    0xCA,       // 0xD2 // PrintChan
    0xCB,       // 0xD3 // PrintComma
    0xCC,       // 0xD4 // PrintEos
    0xCD,       // 0xD5 // PrintItemComma
    0xCE,       // 0xD6 // PrintItemNL
    0xCF,       // 0xD7 // PrintItemSemi
    0xD0,       // 0xD8 // PrintNL
    0xD1,       // 0xD9 // PrintObj
    0xD2,       // 0xDA // PrintSemi
    0xD3,       // 0xDB // PrintSpc
    0xD4,       // 0xDC // PrintTab
    0xD5,       // 0xDD // PrintTabComma
    0xD6,       // 0xDE // Pset
    0xD7,       // 0xDF // PutRec
    0xD8,       // 0xE0 // QuoteRem
    0xD9,       // 0xE1 // Redim
    0xDA,       // 0xE2 // RedimAs
    0xDB,       // 0xE3 // Reparse
    0xDC,       // 0xE4 // Rem
    0xDD,       // 0xE5 // Resume
    0xDE,       // 0xE6 // Return
    0xDF,       // 0xE7 // Rset
    0xE0,       // 0xE8 // Scale
    0xE1,       // 0xE9 // Seek
    0xE2,       // 0xEA // SelectCase
    0xE3,       // 0xEB // SelectIs
    0xE4,       // 0xEC // SelectType
    0xE5,       // 0xED // SetStmt
    0xE6,       // 0xEE // Stack
    0xE7,       // 0xEF // Stop
    0xE8,       // 0xF0 // Type
    0xE9,       // 0xF1 // UnLock
    0xEA,       // 0xF2 // VarDefn
    0xEB,       // 0xF3 // Wend
    0xEC,       // 0xF4 // While
    0xED,       // 0xF5 // With
    0xEE,       // 0xF6 // WriteChan
    0xEF,       // 0xF7 // ConstFuncExpr
    0xF0,       // 0xF8 // LbConst
    0xF1,       // 0xF9 // LbIf
    0xF2,       // 0xFA // LbElse
    0xF3,       // 0xFB // LbElseif
    0xF4,       // 0xFC // LbEndif
    0xF5,       // 0xFD // LbMark
    0xF6,       // 0xFE // EndForVariable
    0xF7,       // 0xFF // StartForVariable
    0xF8,       // 0x100 // NewRedim
    0xF9,       // 0x101 // StartWithExpr
    0xFA,       // 0x102 // SetOrSt
    0xFB,       // 0x103 // EndEnum
    0xFC        // 0x104 // Illegal
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

BOOL O97ModuleGetOpcode
(
    LPO97_MODULE        lpstModule,
    DWORD               dwOffset,
    LPBYTE              lpabyOpcode
)
{
    if (O97GetModuleBinary(lpstModule,
                           dwOffset,
                           lpabyOpcode,
                           2) == FALSE)
        return(FALSE);

    // Endianize

    if (lpstModule->bLittleEndian == FALSE)
    {
        BYTE    byTemp;

        byTemp = lpabyOpcode[0];
        lpabyOpcode[0] = lpabyOpcode[1];
        lpabyOpcode[1] = byTemp;
    }

    // Convert to O97 opcode

    if (lpstModule->nVersion != O97_VERSION_O97)
    {
        int     nOpcode;

        // Translate O98 opcodes to O97

        nOpcode = lpabyOpcode[0] |
            ((lpabyOpcode[1] & 0x01) << 8);

        if (nOpcode > O98_OP_MAX)
            nOpcode = O98_OP_MAX;

        lpabyOpcode[0] = gabyXOpO98toO97[nOpcode];
        lpabyOpcode[1] &= 0xFC;
    }

    return(TRUE);
}


//*************************************************************************
//
// LPO97_MODULE_ENV O97ModuleEnvCreate()
//
// Parameters:
//  lpstEnv             Ptr to environment structure
//
// Description:
//  Allocates the following:
//      - Module environment structure
//      - Stream access structure
//      - Module access structure
//
// Returns:
//  LPO97_MODULE_ENV    On success
//  NULL                On failure
//
//*************************************************************************

LPO97_MODULE_ENV O97ModuleEnvCreate
(
    LPO97_ENV           lpstEnv
)
{
    LPO97_MODULE_ENV    lpstModEnv;

    // Allocate environment structure

    if (SSMemoryAlloc(lpstEnv->lpstRoot->lpvRootCookie,
                      sizeof(O97_MODULE_ENV_T),
                      (LPLPVOID)&lpstModEnv) != SS_STATUS_OK)
        return(NULL);

    // Create a stream structure for accessing the module

    if (SSAllocStreamStruct(lpstEnv->lpstRoot,
                            &lpstModEnv->lpstStream,
                            SS_STREAM_FLAG_DEF_BAT_CACHE) != SS_STATUS_OK)
    {
        SSMemoryFree(lpstEnv->lpstRoot->lpvRootCookie,lpstModEnv);
        return(NULL);
    }

    // Create a module access structure

    if (O97CreateModuleAccessStruct(lpstEnv->lpstRoot->lpvRootCookie,
                                    &lpstModEnv->lpstModule) == FALSE)
    {
        SSFreeStreamStruct(lpstModEnv->lpstStream);

        SSMemoryFree(lpstEnv->lpstRoot->lpvRootCookie,lpstModEnv);
        return(NULL);
    }

    lpstModEnv->nMaxCachedFunctions = O97_MOD_ENV_MAX_CACHED_FUNCTIONS;
    lpstModEnv->nMaxCachedSubs = O97_MOD_ENV_MAX_CACHED_SUBS;

    return(lpstModEnv);
}


//*************************************************************************
//
// BOOL O97ModuleEnvDestroy()
//
// Parameters:
//  lpstEnv             Ptr to environment structure
//  lpstModEnv          Ptr to module environment
//
// Description:
//  Frees the following:
//      - Module access structure
//      - Stream access structure
//      - Module environment structure
//
// Returns:
//  TRUE                On success
//  FALSE               On failure
//
//*************************************************************************

BOOL O97ModuleEnvDestroy
(
    LPO97_ENV           lpstEnv,
    LPO97_MODULE_ENV    lpstModEnv
)
{
    O97DestroyModuleAccessStruct(lpstEnv->lpstRoot->lpvRootCookie,
                                 lpstModEnv->lpstModule);

    SSFreeStreamStruct(lpstModEnv->lpstStream);

    SSMemoryFree(lpstEnv->lpstRoot->lpvRootCookie,
                 lpstModEnv);

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97ModuleScan()
//
// Parameters:
//  lpstModEnv          Ptr to module environment
//
// Description:
//  Scans a module for global variables, functions, and subroutines.
//
// Returns:
//  TRUE                On success
//  FALSE               On failure
//
//*************************************************************************

BOOL O97ModuleScan
(
    LPO97_MODULE_ENV    lpstModEnv
)
{
    DWORD               dwLine;
    O97_LINE_ENTRY_T    stLine;
    DWORD               dwOffset;
    DWORD               dwLimit;
    BYTE                abyOpcode[2];
    EO97_OP_T           eOpcode;
    LPO97_MODULE        lpstModule;
    BOOL                bStateOutside;
    WORD                wTemp;
    DWORD               dwTemp;

    // Search for all globals, functions, and subs

    bStateOutside = TRUE;
    lpstModule = lpstModEnv->lpstModule;
    for (dwLine=0;dwLine<lpstModule->stLineTable.dwNumLines;dwLine++)
    {
        if (O97GetLineEntry(lpstModule,
                            dwLine,
                            &stLine) == FALSE)
        {
            return(FALSE);
        }

        dwOffset = stLine.dwOffset;
        dwLimit = dwOffset + stLine.wSize;
        while (dwOffset < dwLimit)
        {
            // Get the opcode

            if (O97ModuleGetOpcode(lpstModule,
                                   dwOffset,
                                   abyOpcode) == FALSE)
                return(FALSE);

            eOpcode = (EO97_OP_T)abyOpcode[0];
            switch (eOpcode)
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
                case eO97_OP_A4_WORD:
                case eO97_OP_A6_HEX_WORD:
                case eO97_OP_DD_RESUME:
                case eO97_OP_BE_NEW:
                case eO97_OP_C1_ON_ERROR_GOTO:
                case eO97_OP_C9_PARAM_ASSIGN:
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
                case eO97_OP_59_DEFTYPE:
                case eO97_OP_A5_DWORD:
                case eO97_OP_A7_HEX_DWORD:
                case eO97_OP_E8_TYPE:
                    dwOffset += 2 + 4;
                    break;

                case eO97_OP_5A_DECLARATION:
                    dwOffset += 2;
                    switch (abyOpcode[1])
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
                    bStateOutside = TRUE;
                    break;

                case eO97_OP_8F_SUB_FUNCTION:
                    dwOffset += 2;
                    bStateOutside = FALSE;

                    // Get the type table record offset

                    if (O97GetModuleDWORD(lpstModule,
                                          dwOffset,
                                          &dwTemp) == FALSE)
                        return(FALSE);

                    // Get the identifier ID

                    if (O97TypeTableGetRecordIdent(lpstModule,
                                                   dwTemp,
                                                   &wTemp) == FALSE)
                        return(FALSE);

                    // Set the hash bit

                    switch (abyOpcode[1])
                    {
                        case 0x04:
                        case 0x14:
                            // Sub

                            lpstModEnv->
                                abySubHash[(wTemp & 0xFF) >> 3] |=
                                    (1 << (wTemp & 7));

                            if (lpstModEnv->nNumCachedSubs <
                                lpstModEnv->nMaxCachedSubs)
                            {
                                lpstModEnv->
                                    wCachedSubsID[lpstModEnv->
                                        nNumCachedSubs] = wTemp;
                                lpstModEnv->
                                    astCachedSubsIP[lpstModEnv->
                                        nNumCachedSubs].wLine =
                                            (WORD)dwLine;
                                lpstModEnv->
                                    astCachedSubsIP[lpstModEnv->
                                        nNumCachedSubs++].wOffset =
                                            (WORD)(dwOffset - stLine.dwOffset - 2);
                            }
                            break;

                        case 0x08:
                        case 0x18:
                            // Function

                            lpstModEnv->
                                abyFunctionHash[(wTemp & 0xFF) >> 3] |=
                                    (1 << (wTemp & 7));

                            if (lpstModEnv->nNumCachedFunctions <
                                lpstModEnv->nMaxCachedFunctions)
                            {
                                lpstModEnv->
                                    wCachedFunctionsID[lpstModEnv->
                                        nNumCachedFunctions] = wTemp;
                                lpstModEnv->
                                    astCachedFunctionsIP[lpstModEnv->
                                        nNumCachedFunctions].wLine =
                                            (WORD)dwLine;
                                lpstModEnv->
                                    astCachedFunctionsIP[lpstModEnv->
                                        nNumCachedFunctions++].wOffset =
                                            (WORD)(dwOffset - stLine.dwOffset - 2);
                            }
                            break;

                        default:
                            break;
                    }
                    dwOffset += 4;
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
                    dwOffset += 2 + 8;
                    break;

                case eO97_OP_D8_COMMENT:
                    dwOffset += 4;
                    if (O97GetModuleWORD(lpstModule,
                                         dwOffset,
                                         &wTemp) == FALSE)
                        return(FALSE);
                    dwOffset += 2 + wTemp;
                    if (wTemp & 1)
                        ++dwOffset;
                    break;

                case eO97_OP_D9_REDIM:
                case eO97_OP_DA_REDIM_PRESERVE:
                    dwOffset += 2 + 8;
                    break;

                case eO97_OP_AE_STRING:
                case eO97_OP_DB_LITERAL:
                case eO97_OP_DC_REM:
                    dwOffset += 2;
                    if (O97GetModuleWORD(lpstModule,
                                         dwOffset,
                                         &wTemp) == FALSE)
                        return(FALSE);
                    dwOffset += 2 + wTemp;
                    if (wTemp & 1)
                        ++dwOffset;
                    break;

                case eO97_OP_EA_SYMBOL_DECL:
                    dwOffset += 2;
                    if (bStateOutside == TRUE)
                    {
                        // Global identifier

                        // Get the type table record offset

                        if (O97GetModuleDWORD(lpstModule,
                                              dwOffset,
                                              &dwTemp) == FALSE)
                            return(FALSE);

                        // Get the identifier ID

                        if (O97TypeTableGetRecordIdent(lpstModule,
                                                       dwTemp,
                                                       &wTemp) == FALSE)
                            return(FALSE);

                        // Set the hash bit

                        lpstModEnv->
                            abyGlobalVarHash[(wTemp & 0xFF) >> 3] |=
                                (1 << (wTemp & 7));
                    }
                    if (abyOpcode[1] & 0x40)
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
                case eO97_OP_53:
                case eO97_OP_54:
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
                case eO97_OP_A3:
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
                case eO97_OP_C4:
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
                case eO97_OP_EE:
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
        }
    }

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97ModuleScan()
//
// Parameters:
//  lpstModEnv          Ptr to module environment
//  dwModuleIndex       Index of module
//  dwModuleEntry       Module entry number
//
// Description:
//  Assumes that lpstModEnv->lpstStream has been initialized for the
//  module stream to initialize.
//
//  Initializes the module access structure.
//
// Returns:
//  TRUE                On success
//  FALSE               On failure
//
//*************************************************************************

BOOL O97ModuleEnvInit
(
    LPO97_MODULE_ENV    lpstModEnv,
    DWORD               dwModuleIndex,
    DWORD               dwModuleEntry
)
{
    int                 i;

    // Initialize module access structure

    if (O97InitModuleAccess(lpstModEnv->lpstStream,
                            lpstModEnv->lpstModule) == FALSE)
        return(FALSE);

    lpstModEnv->dwModuleIndex = dwModuleIndex;
    lpstModEnv->dwModuleEntry = dwModuleEntry;

    // Indicate no waiting dimension indices

    lpstModEnv->wNumIndices = 0;

    // Indicate no cached functions nor subs

    lpstModEnv->nNumCachedFunctions = 0;
    lpstModEnv->nNumCachedSubs = 0;

    // Zero out the hash arrays

    for (i=0;i<O97_MOD_ENV_HASH_ARRAY_SIZE;i++)
    {
        lpstModEnv->abyGlobalVarHash[i] = 0;
        lpstModEnv->abyFunctionHash[i] = 0;
        lpstModEnv->abySubHash[i] = 0;
    }

    // Scan

    if (O97ModuleScan(lpstModEnv) == FALSE)
        return(FALSE);

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97ModuleScan()
//
// Parameters:
//  lpstModEnv          Ptr to module environment
//  lpstIP              Starting IP to search, returns with next sub IP
//
// Description:
//  Initialize *lpstIP with 0xFFFF the first time.
//  The caller should skip 6 after each return.
//
//  The function only returns subs that have zero parameters
//
// Returns:
//  TRUE                On success
//  FALSE               On failure
//
//*************************************************************************

BOOL O97ModuleEnvFindNextSub
(
    LPO97_MODULE_ENV    lpstModEnv,
    LPO97_IP            lpstIP
)
{
    DWORD               dwLine;
    O97_LINE_ENTRY_T    stLine;
    DWORD               dwOffset;
    DWORD               dwLimit;
    BYTE                abyOpcode[2];
    EO97_OP_T           eOpcode;
    LPO97_MODULE        lpstModule;
    WORD                wTemp;

    lpstModule = lpstModEnv->lpstModule;
    if (lpstIP->wLine == 0xFFFF && lpstIP->wOffset == 0xFFFF)
    {
        dwLine = 0;
        dwOffset = 0;
    }
    else
    {
        dwLine = lpstIP->wLine;
        dwOffset = lpstIP->wOffset;
    }
    for (;dwLine<lpstModule->stLineTable.dwNumLines;dwLine++)
    {
        if (O97GetLineEntry(lpstModule,
                            dwLine,
                            &stLine) == FALSE)
        {
            return(FALSE);
        }

        dwOffset = stLine.dwOffset + dwOffset;
        dwLimit = dwOffset + stLine.wSize;
        while (dwOffset < dwLimit)
        {
            // Get the opcode

            if (O97ModuleGetOpcode(lpstModule,
                                   dwOffset,
                                   abyOpcode) == FALSE)
                return(FALSE);

            eOpcode = (EO97_OP_T)abyOpcode[0];
            switch (eOpcode)
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
                case eO97_OP_A4_WORD:
                case eO97_OP_A6_HEX_WORD:
                case eO97_OP_DD_RESUME:
                case eO97_OP_BE_NEW:
                case eO97_OP_C1_ON_ERROR_GOTO:
                case eO97_OP_C4_OPEN:
                case eO97_OP_C9_PARAM_ASSIGN:
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
                case eO97_OP_59_DEFTYPE:
                case eO97_OP_A5_DWORD:
                case eO97_OP_A7_HEX_DWORD:
                case eO97_OP_E8_TYPE:
                    dwOffset += 2 + 4;
                    break;


                case eO97_OP_8F_SUB_FUNCTION:
                    switch (abyOpcode[1])
                    {
                        case 0x04:
                        case 0x14:
                        {
                            DWORD       dwRecordOffset;
                            BYTE        byNumParams;
                            BYTE        abyFlagBytes[2];

                            // Sub

                            lpstIP->wLine = (WORD)dwLine;
                            lpstIP->wOffset =
                                (WORD)(dwOffset - stLine.dwOffset);


                            if (O97GetModuleDWORD(lpstModule,
                                                  dwOffset + 2,
                                                  &dwRecordOffset) == FALSE)
                                return(FALSE);

                            if (O97TypeTableGetRoutineInfo(lpstModule,
                                                           dwRecordOffset,
                                                           abyFlagBytes,
                                                           NULL,
                                                           &byNumParams,
                                                           NULL) == FALSE)
                                return(FALSE);

                            if (byNumParams == 0 &&
                                (abyFlagBytes[0] & 0x08) != 0)
                            {
                                // Only return it if it takes zero params
                                // and is not a private function

                                return(TRUE);
                            }

                            break;
                        }

                        case 0x08:
                        case 0x18:
                            break;

                        default:
                            break;
                    }
                    dwOffset += 2;
                    dwOffset += 4;
                    break;

                case eO97_OP_9E_BROKEN_LINE:
                    dwOffset += 2;
                    if (O97GetModuleWORD(lpstModule,
                                         dwOffset,
                                         &wTemp) == FALSE)
                        return(FALSE);
                    dwOffset += 2 + wTemp;
                    break;

                case eO97_OP_D8_COMMENT:
                    dwOffset += 4;
                    if (O97GetModuleWORD(lpstModule,
                                         dwOffset,
                                         &wTemp) == FALSE)
                        return(FALSE);
                    dwOffset += 2 + wTemp;
                    if (wTemp & 1)
                        ++dwOffset;
                    break;

                case eO97_OP_AC_DOUBLE:
                case eO97_OP_D9_REDIM:
                case eO97_OP_DA_REDIM_PRESERVE:
                    dwOffset += 2 + 8;
                    break;

                case eO97_OP_AE_STRING:
                case eO97_OP_DB_LITERAL:
                case eO97_OP_DC_REM:
                    dwOffset += 2;
                    if (O97GetModuleWORD(lpstModule,
                                         dwOffset,
                                         &wTemp) == FALSE)
                        return(FALSE);
                    dwOffset += 2 + wTemp;
                    if (wTemp & 1)
                        ++dwOffset;
                    break;

                case eO97_OP_EA_SYMBOL_DECL:
                    dwOffset += 2;
                    if (abyOpcode[1] & 0x40)
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
                case eO97_OP_53:
                case eO97_OP_54:
                case eO97_OP_55_TYPE_CONVERSION:
                case eO97_OP_56:
                case eO97_OP_57:
                case eO97_OP_58:
                case eO97_OP_5A_DECLARATION:
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
                case eO97_OP_66_END_FUNCTION:
                case eO97_OP_67_END_SINGLE_IF:
                case eO97_OP_68_END_IF:
                case eO97_OP_69:
                case eO97_OP_6A_END_PROPERTY:
                case eO97_OP_6B_END_SELECT:
                case eO97_OP_6C_END_SUB:
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
                case eO97_OP_A3:
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
                case eO97_OP_EE:
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
        }

        // Reset dwOffset so that we start at the beginning
        //  of the line

        dwOffset = 0;
    }

    // No more subs

    lpstIP->wLine = lpstIP->wOffset = 0xFFFF;

    return(TRUE);
}


//*************************************************************************
//
// BOOL O97ModEnvVarIsGlobal()
//
// Parameters:
//  lpstModEnv          Ptr to module environment
//  wID                 ID of variable to check
//
// Description:
//  Determines whether an ID is that of a global variable or not.
//
// Returns:
//  TRUE                Variable is global
//  FALSE               Variable is not global
//
//*************************************************************************

BOOL O97ModEnvVarIsGlobal
(
    LPO97_ENV       lpstEnv,
    WORD            wID
)
{
    // Check the hash bit

    if (lpstEnv->lpstModEnv->
        abyGlobalVarHash[(wID & 0xFF) >> 3] & (1 << (wID & 7)))
        return(TRUE);

    return(FALSE);
}


//*************************************************************************
//
// BOOL O97ModEnvGetFunctionIP()
//
// Parameters:
//  lpstModEnv          Ptr to module environment
//  wFuncID             ID of the function
//  lpwIPLine           Ptr to WORD for IP line
//  lpwIPOffset         Ptr to WORD for IP offset
//
// Description:
//  Gets the IP for a function.
//
// Returns:
//  TRUE                On success
//  FALSE               On failure
//
//*************************************************************************

BOOL O97ModEnvGetFunctionIP
(
    LPO97_ENV           lpstEnv,
    WORD                wFuncID,
    LPWORD              lpwIPLine,
    LPWORD              lpwIPOffset
)
{
    LPO97_MODULE_ENV    lpstModEnv;
    int                 n;

    lpstModEnv = lpstEnv->lpstModEnv;

    for (n=0;n<lpstModEnv->nNumCachedFunctions;n++)
    {
        if (lpstModEnv->wCachedFunctionsID[n] == wFuncID)
        {
            *lpwIPLine = lpstModEnv->astCachedFunctionsIP[n].wLine;
            *lpwIPOffset = lpstModEnv->astCachedFunctionsIP[n].wOffset;
            return(TRUE);
        }
    }

    return(FALSE);
}


//*************************************************************************
//
// BOOL O97ModEnvGetSubIP()
//
// Parameters:
//  lpstModEnv          Ptr to module environment
//  wSubID              ID of the function
//  lpwIPLine           Ptr to WORD for IP line
//  lpwIPOffset         Ptr to WORD for IP offset
//
// Description:
//  Gets the IP for a subroutine.
//
// Returns:
//  TRUE                On success
//  FALSE               On failure
//
//*************************************************************************

BOOL O97ModEnvGetSubIP
(
    LPO97_ENV           lpstEnv,
    WORD                wSubID,
    LPWORD              lpwIPLine,
    LPWORD              lpwIPOffset
)
{
    LPO97_MODULE_ENV    lpstModEnv;
    int                 n;

    lpstModEnv = lpstEnv->lpstModEnv;

    for (n=0;n<lpstModEnv->nNumCachedSubs;n++)
    {
        if (lpstModEnv->wCachedSubsID[n] == wSubID)
        {
            *lpwIPLine = lpstModEnv->astCachedSubsIP[n].wLine;
            *lpwIPOffset = lpstModEnv->astCachedSubsIP[n].wOffset;
            return(TRUE);
        }
    }

    return(FALSE);
}




