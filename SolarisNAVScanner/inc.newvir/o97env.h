//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/o97env.h_v   1.1   04 Aug 1998 15:13:38   DCHI  $
//
// Description:
//  VBA 5 emulation header file.
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/o97env.h_v  $
// 
//    Rev 1.1   04 Aug 1998 15:13:38   DCHI
// Added With state to O97_ENV_T and prototype for O97ModuleGetOpcode().
// 
//    Rev 1.0   15 Aug 1997 13:28:38   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _O97ENV_H_

#define _O97ENV_H_

#include "o97str.h"
#include "gsm.h"

//////////////////////////////////////////////////////////////////////
// IP
//////////////////////////////////////////////////////////////////////

typedef struct tagO97_IP
{
    WORD                    wLine;
    WORD                    wOffset;
} O97_IP_T, FAR *LPO97_IP;


//////////////////////////////////////////////////////////////////////
// VBA 5 Opcodes, etc.
//////////////////////////////////////////////////////////////////////

typedef enum tagEO97_OP
{
    eO97_OP_00_IMP                      = 0x00,
    eO97_OP_01_EQV                      = 0x01,
    eO97_OP_02_XOR                      = 0x02,
    eO97_OP_03_OR                       = 0x03,
    eO97_OP_04_AND                      = 0x04,
    eO97_OP_05_EQ                       = 0x05,
    eO97_OP_06_NE                       = 0x06,
    eO97_OP_07_LE                       = 0x07,
    eO97_OP_08_GE                       = 0x08,
    eO97_OP_09_LT                       = 0x09,
    eO97_OP_0A_GT                       = 0x0A,
    eO97_OP_0B_PLUS                     = 0x0B,
    eO97_OP_0C_MINUS                    = 0x0C,
    eO97_OP_0D_MOD                      = 0x0D,
    eO97_OP_0E_INT_DIV                  = 0x0E,
    eO97_OP_0F_ASTERISK                 = 0x0F,
    eO97_OP_10_SLASH                    = 0x10,
    eO97_OP_11_AMPERSAND                = 0x11,
    eO97_OP_12_LIKE                     = 0x12,
    eO97_OP_13_CARAT                    = 0x13,
    eO97_OP_14_IS                       = 0x14,
    eO97_OP_15_NOT                      = 0x15,
    eO97_OP_16_NEGATE                   = 0x16,
    eO97_OP_17_ABS                      = 0x17,
    eO97_OP_18_FIX                      = 0x18,
    eO97_OP_19_INT                      = 0x19,
    eO97_OP_1A_SGN                      = 0x1A,
    eO97_OP_1B_LEN                      = 0x1B,
    eO97_OP_1C_LENB                     = 0x1C,
    eO97_OP_1D_PARENTHESIZE             = 0x1D,
    eO97_OP_1E                          = 0x1E,
    eO97_OP_1F                          = 0x1F,
    eO97_OP_20_EVAL                     = 0x20,
    eO97_OP_21_DOT_EVAL                 = 0x21,
    eO97_OP_22                          = 0x22,
    eO97_OP_23                          = 0x23,
    eO97_OP_24_FUNCTION_EVAL            = 0x24,
    eO97_OP_25_DOT_FUNCTION_EVAL        = 0x25,
    eO97_OP_26                          = 0x26,
    eO97_OP_27_ASSIGN                   = 0x27,
    eO97_OP_28_DOT_ASSIGN               = 0x28,
    eO97_OP_29                          = 0x29,
    eO97_OP_2A                          = 0x2A,
    eO97_OP_2B_ASSIGN_FUNCTION          = 0x2B,
    eO97_OP_2C_ASSIGN_DOT_FUNCTION      = 0x2C,
    eO97_OP_2D                          = 0x2D,
    eO97_OP_2E_ASSIGN_USING_SET         = 0x2E,
    eO97_OP_2F_ASSIGN_DOTTED_SET        = 0x2F,
    eO97_OP_30                          = 0x30,
    eO97_OP_31                          = 0x31,
    eO97_OP_32                          = 0x32,
    eO97_OP_33                          = 0x33,
    eO97_OP_34                          = 0x34,
    eO97_OP_35_ROOT_DOT_EVAL            = 0x35,
    eO97_OP_36                          = 0x36,
    eO97_OP_37_ROOT_DOT_FUNCTION_EVAL   = 0x37,
    eO97_OP_38                          = 0x38,
    eO97_OP_39_ASSIGN_ROOT_DOT          = 0x39,
    eO97_OP_3A                          = 0x3A,
    eO97_OP_3B_ASSIGN_ROOT_DOT_FUNCTION = 0x3B,
    eO97_OP_3C                          = 0x3C,
    eO97_OP_3D                          = 0x3D,
    eO97_OP_3E                          = 0x3E,
    eO97_OP_3F                          = 0x3F,
    eO97_OP_40                          = 0x40,
    eO97_OP_41_CALL                     = 0x41,
    eO97_OP_42_DOT_CALL                 = 0x42,
    eO97_OP_43_ROOT_DOT_CALL            = 0x43,
    eO97_OP_44                          = 0x44,
    eO97_OP_45_COLON                    = 0x45,
    eO97_OP_46_THEN_ELSE_SINGLE         = 0x46,
    eO97_OP_47                          = 0x47,
    eO97_OP_48_CASE                     = 0x48,
    eO97_OP_49_CASE_TO                  = 0x49,
    eO97_OP_4A_CASE_GT                  = 0x4A,
    eO97_OP_4B_CASE_LT                  = 0x4B,
    eO97_OP_4C_CASE_GE                  = 0x4C,
    eO97_OP_4D_CASE_LE                  = 0x4D,
    eO97_OP_4E_CASE_NE                  = 0x4E,
    eO97_OP_4F_CASE_EQ                  = 0x4F,
    eO97_OP_50_CASE_ELSE                = 0x50,
    eO97_OP_51_CASE_EOL                 = 0x51,
    eO97_OP_52                          = 0x52,
    eO97_OP_53_CLOSE                    = 0x53,
    eO97_OP_54_CLOSE_ALL                = 0x54,
    eO97_OP_55_TYPE_CONVERSION          = 0x55,
    eO97_OP_56                          = 0x56,
    eO97_OP_57                          = 0x57,
    eO97_OP_58                          = 0x58,
    eO97_OP_59_DEFTYPE                  = 0x59,
    eO97_OP_5A_DECLARATION              = 0x5A,
    eO97_OP_5B_TYPE_ELEMENT             = 0x5B,
    eO97_OP_5C_DO                       = 0x5C,
    eO97_OP_5D                          = 0x5D,
    eO97_OP_5E_DO_UNTIL                 = 0x5E,
    eO97_OP_5F_DO_WHILE                 = 0x5F,
    eO97_OP_60_ELSE_SINGLE              = 0x60,
    eO97_OP_61_ELSE_MULTIPLE            = 0x61,
    eO97_OP_62_ELSEIF                   = 0x62,
    eO97_OP_63                          = 0x63,
    eO97_OP_64_END                      = 0x64,
    eO97_OP_65                          = 0x65,
    eO97_OP_66_END_FUNCTION             = 0x66,
    eO97_OP_67_END_SINGLE_IF            = 0x67,
    eO97_OP_68_END_IF                   = 0x68,
    eO97_OP_69                          = 0x69,
    eO97_OP_6A_END_PROPERTY             = 0x6A,
    eO97_OP_6B_END_SELECT               = 0x6B,
    eO97_OP_6C_END_SUB                  = 0x6C,
    eO97_OP_6D_END_TYPE                 = 0x6D,
    eO97_OP_6E_END_WITH                 = 0x6E,
    eO97_OP_6F                          = 0x6F,
    eO97_OP_70                          = 0x70,
    eO97_OP_71_EXIT_DO                  = 0x71,
    eO97_OP_72_EXIT_FOR                 = 0x72,
    eO97_OP_73_EXIT_FUNCTION            = 0x73,
    eO97_OP_74_EXIT_PROPERTY            = 0x74,
    eO97_OP_75_EXIT_SUB                 = 0x75,
    eO97_OP_76                          = 0x76,
    eO97_OP_77                          = 0x77,
    eO97_OP_78                          = 0x78,
    eO97_OP_79                          = 0x79,
    eO97_OP_7A                          = 0x7A,
    eO97_OP_7B                          = 0x7B,
    eO97_OP_7C                          = 0x7C,
    eO97_OP_7D_INSTR_TWO_PARAMS         = 0x7D,
    eO97_OP_7E_INSTR_THREE_PARAMS       = 0x7E,
    eO97_OP_7F_INSTR_FOUR_PARAMS        = 0x7F,
    eO97_OP_80                          = 0x80,
    eO97_OP_81                          = 0x81,
    eO97_OP_82                          = 0x82,
    eO97_OP_83                          = 0x83,
    eO97_OP_84                          = 0x84,
    eO97_OP_85                          = 0x85,
    eO97_OP_86_STRCOMP_A_B              = 0x86,
    eO97_OP_87_STRCOMP_A_B_C            = 0x87,
    eO97_OP_88                          = 0x88,
    eO97_OP_89                          = 0x89,
    eO97_OP_8A                          = 0x8A,
    eO97_OP_8B_B_TO_C                   = 0x8B,
    eO97_OP_8C_EACH_B                   = 0x8C,
    eO97_OP_8D                          = 0x8D,
    eO97_OP_8E_B_TO_C_STEP_D            = 0x8E,
    eO97_OP_8F_SUB_FUNCTION             = 0x8F,
    eO97_OP_90                          = 0x90,
    eO97_OP_91                          = 0x91,
    eO97_OP_92                          = 0x92,
    eO97_OP_93_GOTO                     = 0x93,
    eO97_OP_94_IF_THEN_SINGLE           = 0x94,
    eO97_OP_95_IF_THEN_MULTIPLE         = 0x95,
    eO97_OP_96                          = 0x96,
    eO97_OP_97                          = 0x97,
    eO97_OP_98                          = 0x98,
    eO97_OP_99                          = 0x99,
    eO97_OP_9A                          = 0x9A,
    eO97_OP_9B_LABEL                    = 0x9B,
    eO97_OP_9C                          = 0x9C,
    eO97_OP_9D                          = 0x9D,
    eO97_OP_9E_BROKEN_LINE              = 0x9E,
    eO97_OP_9F                          = 0x9F,
    eO97_OP_A0                          = 0xA0,
    eO97_OP_A1                          = 0xA1,
    eO97_OP_A2                          = 0xA2,
    eO97_OP_A3_NO_LEN                   = 0xA3,
    eO97_OP_A4_WORD                     = 0xA4,
    eO97_OP_A5_DWORD                    = 0xA5,
    eO97_OP_A6_HEX_WORD                 = 0xA6,
    eO97_OP_A7_HEX_DWORD                = 0xA7,
    eO97_OP_A8_NOTHING                  = 0xA8,
    eO97_OP_A9                          = 0xA9,
    eO97_OP_AA                          = 0xAA,
    eO97_OP_AB                          = 0xAB,
    eO97_OP_AC_DOUBLE                   = 0xAC,
    eO97_OP_AD                          = 0xAD,
    eO97_OP_AE_STRING                   = 0xAE,
    eO97_OP_AF_TRUE_OR_FALSE            = 0xAF,
    eO97_OP_B0                          = 0xB0,
    eO97_OP_B1_LOOP                     = 0xB1,
    eO97_OP_B2_LOOP_UNTIL               = 0xB2,
    eO97_OP_B3_LOOP_WHILE               = 0xB3,
    eO97_OP_B4                          = 0xB4,
    eO97_OP_B5                          = 0xB5,
    eO97_OP_B6_PRINT_BEGIN              = 0xB6,
    eO97_OP_B7                          = 0xB7,
    eO97_OP_B8                          = 0xB8,
    eO97_OP_B9                          = 0xB9,
    eO97_OP_BA                          = 0xBA,
    eO97_OP_BB                          = 0xBB,
    eO97_OP_BC                          = 0xBC,
    eO97_OP_BD                          = 0xBD,
    eO97_OP_BE_NEW                      = 0xBE,
    eO97_OP_BF_NEXT                     = 0xBF,
    eO97_OP_C0_NEXT_A                   = 0xC0,
    eO97_OP_C1_ON_ERROR_GOTO            = 0xC1,
    eO97_OP_C2                          = 0xC2,
    eO97_OP_C3                          = 0xC3,
    eO97_OP_C4_OPEN                     = 0xC4,
    eO97_OP_C5_OPTION                   = 0xC5,
    eO97_OP_C6_ARRAY_SIZE               = 0xC6,
    eO97_OP_C7                          = 0xC7,
    eO97_OP_C8                          = 0xC8,
    eO97_OP_C9_PARAM_ASSIGN             = 0xC9,
    eO97_OP_CA_PRINTN_BEGIN             = 0xCA,
    eO97_OP_CB_PRINT_COMMA              = 0xCB,
    eO97_OP_CC_PRINT_END                = 0xCC,
    eO97_OP_CD_PRINT_ARG_COMMA          = 0xCD,
    eO97_OP_CE_PRINT_ARG_END            = 0xCE,
    eO97_OP_CF_PRINT_ARG_SEMICOLON      = 0xCF,
    eO97_OP_D0_PRINT_END_NONE           = 0xD0,
    eO97_OP_D1_PRINT_MID_END_NONE       = 0xD1,
    eO97_OP_D2_PRINT_SEMICOLON          = 0xD2,
    eO97_OP_D3_PRINT_SPC_ARG            = 0xD3,
    eO97_OP_D4_PRINT_TAB_ARG            = 0xD4,
    eO97_OP_D5_PRINT_TAB                = 0xD5,
    eO97_OP_D6                          = 0xD6,
    eO97_OP_D7                          = 0xD7,
    eO97_OP_D8_COMMENT                  = 0xD8,
    eO97_OP_D9_REDIM                    = 0xD9,
    eO97_OP_DA_REDIM_PRESERVE           = 0xDA,
    eO97_OP_DB_LITERAL                  = 0xDB,
    eO97_OP_DC_REM                      = 0xDC,
    eO97_OP_DD_RESUME                   = 0xDD,
    eO97_OP_DE                          = 0xDE,
    eO97_OP_DF                          = 0xDF,
    eO97_OP_E0                          = 0xE0,
    eO97_OP_E1                          = 0xE1,
    eO97_OP_E2_SELECT_CASE              = 0xE2,
    eO97_OP_E3                          = 0xE3,
    eO97_OP_E4                          = 0xE4,
    eO97_OP_E5_SET                      = 0xE5,
    eO97_OP_E6                          = 0xE6,
    eO97_OP_E7                          = 0xE7,
    eO97_OP_E8_TYPE                     = 0xE8,
    eO97_OP_E9                          = 0xE9,
    eO97_OP_EA_SYMBOL_DECL              = 0xEA,
    eO97_OP_EB_WEND                     = 0xEB,
    eO97_OP_EC_WHILE                    = 0xEC,
    eO97_OP_ED_WITH_A                   = 0xED,
    eO97_OP_EE_WRITEN_BEGIN             = 0xEE,
    eO97_OP_EF                          = 0xEF,
    eO97_OP_F0                          = 0xF0,
    eO97_OP_F1                          = 0xF1,
    eO97_OP_F2                          = 0xF2,
    eO97_OP_F3                          = 0xF3,
    eO97_OP_F4                          = 0xF4,
    eO97_OP_F5                          = 0xF5,
    eO97_OP_F6_FOR_NEXT_A               = 0xF6,
    eO97_OP_F7_FOR_NEXT                 = 0xF7,
    eO97_OP_F8                          = 0xF8,
    eO97_OP_F9_WITH                     = 0xF9,
    eO97_OP_FA                          = 0xFA,
    eO97_OP_FB                          = 0xFB,
    eO97_OP_FC                          = 0xFC,
    eO97_OP_FD                          = 0xFD,
    eO97_OP_FE                          = 0xFE,
    eO97_OP_FF                          = 0xFF
} EO97_OP_T, FAR *LPEO97_OP;

typedef enum tagEO97_DEFTYPE
{
    eO97_DEFTYPE_UNKNOWN                = 0x00,
    eO97_DEFTYPE_INT                    = 0x08,
    eO97_DEFTYPE_LNG                    = 0x0C,
    eO97_DEFTYPE_SNG                    = 0x10,
    eO97_DEFTYPE_DBL                    = 0x14,
    eO97_DEFTYPE_CUR                    = 0x18,
    eO97_DEFTYPE_DATE                   = 0x1C,
    eO97_DEFTYPE_STR                    = 0x20,
    eO97_DEFTYPE_OBJ                    = 0x24,
    eO97_DEFTYPE_BOOL                   = 0x2C,
    eO97_DEFTYPE_VAR                    = 0x30,
    eO97_DEFTYPE_BYTE                   = 0x44
} EO97_DEFTYPE_T, FAR *LPEO97_DEFTYPE;

typedef enum tagEO97_FIXED_ID
{
    eO97_FIXED_ID_CALL                  = 0x0028,
    eO97_FIXED_ID_FORMAT                = 0x00AE,
    eO97_FIXED_ID_NAME                  = 0x0108,
    eO97_FIXED_ID_TAB                   = 0x0164
} EO97_FIXED_ID_T;


//////////////////////////////////////////////////////////////////////
// Data Types
//////////////////////////////////////////////////////////////////////

typedef enum tagEO97_TYPE
{
    eO97_TYPE_UNUSED,
    eO97_TYPE_UNKNOWN,
    eO97_TYPE_VAR,
    eO97_TYPE_VAR_ID,
    eO97_TYPE_LONG,
    eO97_TYPE_STRING,
    eO97_TYPE_ARRAY,
    eO97_TYPE_DIALOG,
    eO97_TYPE_DIALOG_ID,
    eO97_TYPE_DIALOG_FIELD,
    eO97_TYPE_MODULE_ID,
    eO97_TYPE_OBJECT_ID,
    eO97_TYPE_O97_OBJECT_ID,
    eO97_TYPE_PARAM_ASSIGN
} EO97_TYPE_T, FAR *LPEO97_TYPE;

typedef enum tagEO97_OPERATOR
{
    eO97_OPERATOR_NEG,
    eO97_OPERATOR_MUL,
    eO97_OPERATOR_DIV,
    eO97_OPERATOR_MOD,
    eO97_OPERATOR_ADD,
    eO97_OPERATOR_SUB,
    eO97_OPERATOR_EQ,
    eO97_OPERATOR_NE,
    eO97_OPERATOR_LT,
    eO97_OPERATOR_GT,
    eO97_OPERATOR_LE,
    eO97_OPERATOR_GE,
    eO97_OPERATOR_NOT,
    eO97_OPERATOR_AND,
    eO97_OPERATOR_OR,
    eO97_OPERATOR_XOR,
    eO97_OPERATOR_IMP,
    eO97_OPERATOR_EQV,
    eO97_OPERATOR_LIKE,
    eO97_OPERATOR_POWER,
    eO97_OPERATOR_IS
} EO97_OPERATOR_T, FAR *LPEO97_OPERATOR;

typedef union tagUO97_DATA
{
    LPVOID          lpvstElement;
    LPVOID          lpvstVar;
    LPBYTE          lpbyStr;
    LPVOID          lpvstDlg;
    long            lValue;
    WORD            wDlgField;
    DWORD           dwDlgID;
    WORD            wVarID;
    DWORD           dwObjectID;
} UO97_DATA_T, FAR *LPUO97_DATA;

typedef struct tagO97_VAR
{
    EO97_TYPE_T     eType;
    UO97_DATA_T     uData;

    WORD            wID;
} O97_VAR_T, FAR *LPO97_VAR;

typedef struct tagO97_ELEMENT
{
    EO97_TYPE_T                 eType;
    UO97_DATA_T                 uData;

    DWORD                       dwIndex;
    struct tagO97_ELEMENT FAR * lpstNext;
} O97_ELEMENT_T, FAR *LPO97_ELEMENT;

typedef struct tagO97_OPERAND
{
    EO97_TYPE_T     eType;
    UO97_DATA_T     uData;
} O97_OPERAND_T, FAR *LPO97_OPERAND;

typedef struct tagO97_VAR_OPERAND
{
    WORD            wID;
    LPO97_VAR       lpstVar;
} O97_VAR_OPERAND_T, FAR *LPO97_VAR_OPERAND;

typedef struct tagO97_PARAM
{
    WORD                    wNamedParamID;
    EO97_TYPE_T             eType;
    UO97_DATA_T             uData;
} O97_PARAM_T, FAR *LPO97_PARAM;


//////////////////////////////////////////////////////////////////////
// WordBasic dialog structures
//////////////////////////////////////////////////////////////////////

typedef struct tagO97_DIALOG
{
    DWORD       dwSize;  // Including dwSize and eWDDLG
    DWORD       dwDlgID;
} O97_DIALOG_T, FAR *LPO97_DIALOG;


//////////////////////////////////////////////////////////////////////
// States and substates
//////////////////////////////////////////////////////////////////////

typedef enum tagEO97_STATE
{
    esO97_INIT,
    esO97_BEGIN_INVALID,
    esO97_ERROR,
    esO97_FUNCTION_CALL,
    esO97_SUB_CALL,
    esO97_EXTERNAL_CLEANUP,
    esO97_CALL_FUNCTION,
    esO97_CALL_SUB,
    esO97_CALL_BODY,
    esO97_IF_COND,
    esO97_THEN_SINGLE,
    esO97_THEN_MULTIPLE,
    esO97_ELSE_SINGLE,
    esO97_ELSE_MULTIPLE,
    esO97_FOR_NEXT,
    esO97_DO_LOOP,
    esO97_WHILE,
    esO97_WHILE_COND,
    esO97_WHILE_BODY,
    esO97_WHILE_WEND,
    esO97_EXPR_DIALOG,
    esO97_BEGIN_DIALOG,
    esO97_BEGIN_DIALOG_END,
    esO97_SELECT,
    esO97_SELECT_BODY
} EO97_STATE_T, FAR *LPEO97_STATE;

typedef enum tagEO97_SUBSTATE
{
    essO97_DO,
    essO97_SKIP,
    essO97_TRUE,
    essO97_FALSE
} EO97_SUBSTATE_T, FAR *LPEO97_SUBSTATE;

typedef struct tagO97_CONTROL
{
    EO97_STATE_T        eState;
    EO97_SUBSTATE_T     eSubState;
    LPVOID              lpvstControl;
} O97_CONTROL_T, FAR *LPO97_CONTROL;

typedef enum tagEO97_CONTROL_TYPE
{
    ectO97_CALL,
    ectO97_CALL_GET_ARGS,
    ectO97_EXPR,
    ectO97_FOR,
    ectO97_DO_LOOP,
    ectO97_WHILE,
    ectO97_DIALOG_CALL,
    ectO97_SELECT
} EO97_CONTROL_TYPE_T, FAR *LPEO97_CONTROL_TYPE;

typedef struct tagO97_CONTROL_HDR
{
    EO97_CONTROL_TYPE_T ecType;
} O97_CONTROL_HDR_T, FAR *LPO97_CONTROL_HDR;

typedef struct tagO97_CALL
{
    EO97_CONTROL_TYPE_T ecType;
    O97_IP_T            stReturnIP;
    O97_IP_T            stRoutineStartIP;
} O97_CALL_T, FAR *LPO97_CALL;

typedef struct tagO97_CALL_GET_ARGS
{
    EO97_CONTROL_TYPE_T ecType;
    LPO97_PARAM         lpstParams;
} O97_CALL_GET_ARGS_T, FAR *LPO97_CALL_GET_ARGS;

typedef struct tagO97_EXPR
{
    EO97_CONTROL_TYPE_T ecType;
    WORD                wParenDepth;
} O97_EXPR_T, FAR *LPO97_EXPR;

typedef struct tagO97_FOR
{
    EO97_CONTROL_TYPE_T ecType;
    O97_IP_T            stBodyIP;
    LPO97_VAR           lpstVar;
    long                lStart;
    long                lEnd;
    long                lStep;
} O97_FOR_T, FAR *LPO97_FOR;

typedef struct tagO97_WHILE
{
    EO97_CONTROL_TYPE_T ecType;
    O97_IP_T            stCondIP;
} O97_WHILE_T, FAR *LPO97_WHILE;

typedef struct tagO97_DO_LOOP
{
    EO97_CONTROL_TYPE_T ecType;
    O97_IP_T            stCondIP;
} O97_DO_LOOP_T, FAR *LPO97_DO_LOOP;

typedef struct tagO97_DIALOG_CALL
{
    EO97_CONTROL_TYPE_T ecType;
    LPO97_PARAM         lpstFirstParam;
} O97_DIALOG_CALL_T, FAR *LPO97_DIALOG_CALL;

typedef enum tagO97_CASE_TYPE
{
    eO97_CASE_TYPE_IS_EQ,
    eO97_CASE_TYPE_IS_NE,
    eO97_CASE_TYPE_IS_LT,
    eO97_CASE_TYPE_IS_GT,
    eO97_CASE_TYPE_IS_LE,
    eO97_CASE_TYPE_IS_GE,
    eO97_CASE_TYPE_TO_CHECK,
    eO97_CASE_TYPE_TO_NO_CHECK
} O97_CASE_TYPE_T, FAR *LPO97_CASE_TYPE;

typedef struct tagO97_SELECT
{
    EO97_CONTROL_TYPE_T ecType;
    O97_CASE_TYPE_T     eCaseType;
    EO97_TYPE_T         eType;
    UO97_DATA_T         uData;
} O97_SELECT_T, FAR *LPO97_SELECT;

typedef struct tagO97_ASSIGN
{
    EO97_CONTROL_TYPE_T ecType;
    LPO97_VAR           lpstVar;
} O97_ASSIGN_T, FAR *LPO97_ASSIGN;

typedef enum EO97_ERROR
{
    eO97_ERROR_NO_ERROR,
    eO97_ERROR_GENERIC,
    eO97_ERROR_STATE_NEW,
    eO97_ERROR_STATE_PUSH,
    eO97_ERROR_STATE_POP,
    eO97_ERROR_STATE_TOP,
    eO97_ERROR_CONTROL_CREATE_IDENTIFIER,
    eO97_ERROR_CONTROL_ALLOC,
    eO97_ERROR_CONTROL_FREE,
} EO97_ERROR_T, FAR *LPEO97_ERROR;


//////////////////////////////////////////////////////////////////////
// Module environment
//////////////////////////////////////////////////////////////////////

#define O97_MOD_ENV_HASH_ARRAY_SIZE         32
#define O97_MOD_ENV_MAX_CACHED_FUNCTIONS    16
#define O97_MOD_ENV_MAX_CACHED_SUBS         16

#define O97_MOD_ENV_MAX_HASHED_GOTOS        16

typedef struct tagO97_MODULE_ENV
{
    struct tagO97_MODULE_ENV FAR * lpstPrev;

    LPSS_STREAM             lpstStream;

    DWORD                   dwModuleIndex;
    DWORD                   dwModuleEntry;

    LPO97_MODULE            lpstModule;

    // Number of waiting dimension indices

    WORD                    wNumIndices;

    // Global variable frame

    int                     nGlobalVarFrameStart;

    // Module size

    DWORD                   dwSize;

    // Global variable hash

    BYTE                    abyGlobalVarHash[O97_MOD_ENV_HASH_ARRAY_SIZE];

    // User function hash

    BYTE                    abyFunctionHash[O97_MOD_ENV_HASH_ARRAY_SIZE];
    int                     nMaxCachedFunctions;
    int                     nNumCachedFunctions;
    WORD                    wCachedFunctionsID[O97_MOD_ENV_MAX_CACHED_FUNCTIONS];
    O97_IP_T                astCachedFunctionsIP[O97_MOD_ENV_MAX_CACHED_FUNCTIONS];

    // User subroutine hash

    BYTE                    abySubHash[O97_MOD_ENV_HASH_ARRAY_SIZE];
    int                     nMaxCachedSubs;
    int                     nNumCachedSubs;
    WORD                    wCachedSubsID[O97_MOD_ENV_MAX_CACHED_SUBS];
    O97_IP_T                astCachedSubsIP[O97_MOD_ENV_MAX_CACHED_SUBS];

    // Goto hash

    WORD                    awGotoLabelID[O97_MOD_ENV_MAX_HASHED_GOTOS];
    WORD                    awGotoLabelCount[O97_MOD_ENV_MAX_HASHED_GOTOS];

} O97_MODULE_ENV_T, FAR *LPO97_MODULE_ENV;


//////////////////////////////////////////////////////////////////////
// Execution environment
//////////////////////////////////////////////////////////////////////

#define O97_ENV_MAX_MODULE_IDENTS           64

typedef struct tagO97_VAR_ENV
{
    int             nMaxNumVars;
    int             nNumVars;
    LPO97_VAR       lpastVars;
} O97_VAR_ENV_T, FAR *LPO97_VAR_ENV;

typedef enum tagEO97_STATE_GOTO
{
    eO97_STATE_GOTO_NONE,
    eO97_STATE_GOTO_0,
    eO97_STATE_GOTO_1
} EO97_STATE_GOTO_T, FAR *LPEO97_STATE_GOTO;

typedef enum tagEO97_STATE_EXIT
{
    eO97_STATE_EXIT_NONE,
    eO97_STATE_EXIT_DO,
    eO97_STATE_EXIT_FOR,
    eO97_STATE_EXIT_FUNCTION,
    eO97_STATE_EXIT_SUB
} EO97_STATE_EXIT_T, FAR *LPEO97_STATE_EXIT;

typedef struct tagO97_ENV FAR *LPO97_ENV;

#define O97_ID_INVALID              0xFFFF
#define O97_OBJECT_UNKNOWN_ID       0xFFFFFFFF

typedef BOOL (*LPFNO97_ObjectEvaluate)
    (
        LPO97_ENV       lpstEnv,
        WORD            wNumParams,
        LPBOOL          lpbKnown
    );

typedef BOOL (*LPFNO97_SubObjectEvaluate)
    (
        LPO97_ENV       lpstEnv,
        WORD            wNumParams,
        EO97_TYPE_T     eParentType,
        LPVOID          lpvParentData,
        LPBOOL          lpbKnown
    );

typedef void (*LPFNO97_GetDialogInfo)
    (
        LPO97_DIALOG    lpstDlg,
        DWORD           dwDlgID
    );

typedef void (*LPFNO97_InitDialogInfo)
    (
        LPO97_ENV       lpstEnv,
        LPO97_DIALOG    lpstDlg
    );

typedef BOOL (*LPFNO97_AssignDialogField)
    (
        LPO97_ENV       lpstEnv,
        LPO97_DIALOG    lpstDlg
    );

typedef struct tagO97_OBJECT_FUNC
{
    LPFNO97_ObjectEvaluate      ObjectEvaluate;
    LPFNO97_SubObjectEvaluate   SubObjectEvaluate;
    LPFNO97_GetDialogInfo       GetDialogInfo;
    LPFNO97_InitDialogInfo      InitDialogInfo;
    LPFNO97_AssignDialogField   AssignDialogField;
} O97_OBJECT_FUNC_T, FAR *LPO97_OBJECT_FUNC;

#define O97_MAX_WITH_DEPTH  8

#define O97_MAX_PARAMS      32

#define O97_UNNAMED_PARAM_ID    0xFFFF

typedef struct tagO97_ENV
{
    LPVOID                  lpvRootCookie;
    LPSS_ROOT               lpstRoot;

    // App specific fields (should be transferred when time permits)

    BOOL                    bNormal;

    DWORD                   dwVBAChildEntry;
    DWORD                   dw_VBA_PROJECTEntry;
    DWORD                   dwIdentTableOffset;
    DWORD                   dwNumIdents;

    EO97_ERROR_T            eError;

    // Module count

    DWORD                   dwNumModules;

    // Current module

    LPO97_MODULE_ENV        lpstModEnv;
    WORD                    wModuleDepth;

    // Current execution location

    BOOL                    bRedoToken;
    BOOL                    bNewIP;

    DWORD                   dwIPLine;
    DWORD                   dwIPOffset;

    DWORD                   dwNextIPOffset;

    O97_IP_T                stRoutineStartIP;

    // String call and regular call

    BOOL                    bIsStringCall;
    WORD                    wCallModuleID;
    WORD                    wCallRoutineID;

    // Instruction stream

    BYTE                    abyOpcode[2];

    WORD                    wOpID;
    WORD                    wOpCount;
    DWORD                   dwOpTypeOffset;
    BYTE                    abyOpString[256];
    WORD                    wOpValue;
    DWORD                   dwOpValue;
    double                  lfOpValue;

    // Module identifiers

    int                     nNumModuleIdents;
    WORD                    awModuleIdent[O97_ENV_MAX_MODULE_IDENTS];
    DWORD                   adwModuleEntry[O97_ENV_MAX_MODULE_IDENTS];
    WORD                    awModuleIndex[O97_ENV_MAX_MODULE_IDENTS];

    // O97 identifiers

    WORD                    awO97Ident[NUM_O97_STR];
    WORD                    awO97IdentEnum[NUM_O97_STR];

    // Known identifiers

    int                     nNumKnownIdents;
    LPWORD                  lpwKnownIdent;
    LPWORD                  lpwIdentEnum;

    // Current application error value

    long                    lErr;

    // Variable name and data storage

    O97_VAR_ENV_T           stGlobalVar;
    O97_VAR_ENV_T           stLocalVar;
    int                     nMaxVarFrames;
    int                     nNumVarFrames;
    LPINT                   lpanVarFrames;
    DWORD                   dwGSMVarSize;
    LPGSM_HDR               lpstGSMVarData;

    // Control stack

    int                     nMaxControls;
    int                     nNumControls;

    LPO97_CONTROL           lpstTopControl;
    LPO97_CONTROL           lpastControlStack;

    DWORD                   dwGSMControlSize;
    LPGSM_HDR               lpstGSMControlData;

    // Goto state

    EO97_STATE_GOTO_T       eStateGoto;
    WORD                    wGotoLabelID;
    DWORD                   dwGotoIPLine;
    DWORD                   dwGotoIPOffset;

    // Exit state

    EO97_STATE_EXIT_T       eStateExit;

    // With state

    int                     nWithDepth;
    DWORD                   adwWithObjIDs[O97_MAX_WITH_DEPTH];

    // Expression evaluation stack

    int                     nMaxOperands;
    int                     nOperandsUsed;
    LPO97_OPERAND           lpastOperands;
    LPO97_VAR_OPERAND       lpastVarOperands;

    int                     nEvalDataStoreSize;
    int                     nEvalDataStoreUsed;
    LPBYTE                  lpbyEvalDataStore;

    // Parameter passing stack

    int                     nNumParams;
    O97_PARAM_T             astParams[O97_MAX_PARAMS];

    DWORD                   dwGSMParamSize;
    LPGSM_HDR               lpstGSMParamData;

    // Object evaluate

    LPO97_OBJECT_FUNC       lpstObjectFunc;

    // Context data

    LPVOID                  lpvContextData;
} O97_ENV_T, FAR *LPO97_ENV;

typedef BOOL (*LPFNO97_OP)
    (
        LPO97_ENV       lpstEnv
    );

extern LPFNO97_OP gapfnO97_OP[];

LPO97_ENV O97EnvCreate
(
    LPVOID          lpvRootCookie,
    int             nNumKnownIdents
);

BOOL O97EnvDestroy
(
    LPVOID          lpvRootCookie,
    LPO97_ENV       lpstEnv
);

BOOL O97EnvInit
(
    LPO97_ENV           lpstEnv,
    LPSS_ROOT           lpstRoot,
    DWORD               dwVBAChildEntry,
    LPO97_OBJECT_FUNC   lpstObjectFunc,
    LPLPBYTE            lplpabyKnownIdentSet
);

BOOL O97EnvExecuteInit
(
    LPO97_ENV       lpstEnv
);

BOOL O97InitKnownIdent
(
    LPO97_ENV       lpstEnv,
    LPLPBYTE        lplpabyKnownIdentSet
);

WORD O97GetO97IdentEnum
(
    LPO97_ENV       lpstEnv,
    WORD            wID
);

WORD O97GetKnownIdentEnum
(
    LPO97_ENV       lpstEnv,
    WORD            wID
);

DWORD O97GetModuleEntryNum
(
    LPO97_ENV       lpstEnv,
    WORD            wID,
    LPWORD          lpwModuleIndex
);

BOOL O97IdentLookup
(
    LPO97_ENV       lpstEnv,
    LPBYTE          lpbyIdent0,
    LPBYTE          lpbyIdent1,
    LPWORD          lpwID0,
    LPWORD          lpwID1
);


/////////////////////////////////////////////////////////////////
// Module environment Functions
/////////////////////////////////////////////////////////////////

BOOL O97ModuleGetOpcode
(
    LPO97_MODULE        lpstModule,
    DWORD               dwOffset,
    LPBYTE              lpabyOpcode
);

LPO97_MODULE_ENV O97ModuleEnvCreate
(
    LPO97_ENV           lpstEnv
);

BOOL O97ModuleEnvDestroy
(
    LPO97_ENV           lpstEnv,
    LPO97_MODULE_ENV    lpstModEnv
);

BOOL O97ModuleEnvInit
(
    LPO97_MODULE_ENV    lpstModEnv,
    DWORD               dwModuleIndex,
    DWORD               dwModuleEntry
);

BOOL O97ModuleEnvFindNextSub
(
    LPO97_MODULE_ENV    lpstModEnv,
    LPO97_IP            lpstIP
);

BOOL O97ModEnvVarIsGlobal
(
    LPO97_ENV       lpstEnv,
    WORD            wID
);

BOOL O97ModEnvGetFunctionIP
(
    LPO97_ENV           lpstEnv,
    WORD                wFuncID,
    LPWORD              lpwIPLine,
    LPWORD              lpwIPOffset
);

BOOL O97ModEnvGetSubIP
(
    LPO97_ENV           lpstEnv,
    WORD                wSubID,
    LPWORD              lpwIPLine,
    LPWORD              lpwIPOffset
);


/////////////////////////////////////////////////////////////////
// Execution Functions
/////////////////////////////////////////////////////////////////

BOOL O97Execute
(
    LPO97_ENV           lpstEnv,
    LPO97_MODULE_ENV    lpstModEnv,
    LPO97_IP            lpstStartIP
);


/////////////////////////////////////////////////////////////////
// Control Functions
/////////////////////////////////////////////////////////////////

BOOL O97ControlCreate
(
    LPO97_ENV       lpstEnv
);

BOOL O97ControlDestroy
(
    LPO97_ENV       lpstEnv
);

BOOL O97ControlInit
(
    LPO97_ENV       lpstEnv
);

#ifndef NDEBUG
void O97StateNew
(
    LPO97_ENV       lpstEnv,
    EO97_STATE_T    eState
);
#else
#define O97StateNew(lpstEnv,eNewState) \
    lpstEnv->lpstTopControl->eState = eNewState
#endif

void O97StatePush
(
    LPO97_ENV       lpstEnv,
    EO97_STATE_T    eState
);

EO97_STATE_T O97StatePop
(
    LPO97_ENV       lpstEnv
);

#ifndef NDEBUG
EO97_STATE_T O97StateTop
(
    LPO97_ENV       lpstEnv
);
#else
#define O97StateTop(lpstEnv) \
    (lpstEnv->lpstTopControl->eState)
#endif

#ifndef NDEBUG
void O97SubStateNew
(
    LPO97_ENV       lpstEnv,
    EO97_SUBSTATE_T eSubState
);
#else
#define O97SubStateNew(lpstEnv,eNewSubState) \
    lpstEnv->lpstTopControl->eSubState = eNewSubState
#endif

#ifndef NDEBUG
EO97_SUBSTATE_T O97SubStateTop
(
    LPO97_ENV       lpstEnv
);
#else
#define O97SubStateTop(lpstEnv) \
    (lpstEnv->lpstTopControl->eSubState)
#endif

#ifndef NDEBUG
void O97ControlSetTopControl
(
    LPO97_ENV       lpstEnv,
    LPVOID      lpvstControl
);
#else
#define O97ControlSetTopControl(lpstEnv,lpvstNewControl) \
    lpstEnv->lpstTopControl->lpvstControl = lpvstNewControl
#endif

void O97ControlDestroyTopControl
(
    LPO97_ENV           lpstEnv
);

BOOL O97ControlCreateControl
(
    LPO97_ENV           lpstEnv,
    EO97_CONTROL_TYPE_T ecType
);

BOOL O97ControlCreateSelectControl
(
    LPO97_ENV           lpstEnv,
    EO97_TYPE_T         eType,
    LPVOID              lpvData
);

#ifndef NDEBUG
LPVOID O97ControlTop
(
    LPO97_ENV           lpstEnv
);
#else
#define O97ControlTop(lpstEnv) \
    lpstEnv->lpstTopControl->lpvstControl
#endif


/////////////////////////////////////////////////////////////////
// Parameter Functions
/////////////////////////////////////////////////////////////////

BOOL O97ParamCreate
(
    LPO97_ENV       lpstEnv
);

BOOL O97ParamDestroy
(
    LPO97_ENV       lpstEnv
);

BOOL O97ParamInitList
(
    LPO97_ENV       lpstEnv
);

BOOL O97ParamPut
(
    LPO97_ENV       lpstEnv,
    WORD            wNamedParamID,
    EO97_TYPE_T     eType,
    LPVOID          lpvData
);

BOOL O97ParamGetNamedParamID
(
    LPO97_ENV       lpstEnv,
    int             nIndex,
    LPWORD          lpwNamedParamID
);

BOOL O97ParamGetValue
(
    LPO97_ENV       lpstEnv,
    int             nIndex,
    LPEO97_TYPE     lpeType,
    LPLPVOID        lplpvData
);

BOOL O97ParamGetLong
(
    LPO97_ENV       lpstEnv,
    int             nIndex,
    LPLONG          lplValue
);

BOOL O97ParamGetString
(
    LPO97_ENV       lpstEnv,
    int             nIndex,
    LPLPBYTE        lplpbyStr
);

BOOL O97ParamGetParams
(
    LPO97_ENV       lpstEnv
);

BOOL O97ParamInvalidateFirst
(
    LPO97_ENV       lpstEnv
);

BOOL O97ParamPopParams
(
    LPO97_ENV       lpstEnv
);


/////////////////////////////////////////////////////////////////
// Variable Functions
/////////////////////////////////////////////////////////////////

BOOL O97VarCreate
(
    LPO97_ENV       lpstEnv
);

BOOL O97VarDestroy
(
    LPO97_ENV       lpstEnv
);

BOOL O97VarInit
(
    LPO97_ENV       lpstEnv
);

LPO97_VAR O97VarAlloc
(
    LPO97_ENV       lpstEnv,
    BOOL            bGlobal,
    BOOL            bArray,
    WORD            wID
);

LPO97_VAR O97VarGet
(
    LPO97_ENV       lpstEnv,
    WORD            wID
);

LPO97_VAR O97VarArrayGet
(
    LPO97_ENV       lpstEnv,
    WORD            wID
);

LPO97_ELEMENT O97VarArrayGetElement
(
    LPO97_VAR       lpstVar,
    WORD            wIndex0,
    WORD            wIndex1
);

LPO97_ELEMENT O97VarArrayElementAlloc
(
    LPO97_ENV       lpstEnv,
    LPO97_VAR       lpstVar,
    WORD            wIndex0,
    WORD            wIndex1
);

BOOL O97VarFree
(
    LPO97_ENV       lpstEnv,
    LPO97_VAR       lpstVar
);

void O97VarGetTypeData
(
    LPO97_VAR       lpstVar,
    LPEO97_TYPE     lpeType,
    LPVOID FAR *    lplpvData
);

LPO97_VAR O97VarSet
(
    LPO97_ENV       lpstEnv,
    LPO97_VAR       lpstVar,
    EO97_TYPE_T     eType,
    LPVOID          lpvData
);

LPO97_VAR O97VarGetFirstFrameVar
(
    LPO97_ENV       lpstEnv,
    LPEO97_TYPE     lpeType,
    LPLPVOID        lplpvData
);

BOOL O97VarPushFrame
(
    LPO97_ENV       lpstEnv
);

BOOL O97VarPopFrame
(
    LPO97_ENV       lpstEnv
);

BOOL O97VarPushGlobalFrame
(
    LPO97_ENV       lpstEnv
);

BOOL O97VarPopGlobalFrame
(
    LPO97_ENV       lpstEnv
);


/////////////////////////////////////////////////////////////////
// Evaluation Functions
/////////////////////////////////////////////////////////////////

BOOL O97EvalStackCreate
(
    LPO97_ENV       lpstEnv
);

BOOL O97EvalStackDestroy
(
    LPO97_ENV       lpstEnv
);

BOOL O97EvalStackInit
(
    LPO97_ENV       lpstEnv
);

BOOL O97EvalPushOperand
(
    LPO97_ENV       lpstEnv,
    EO97_TYPE_T     eType,
    LPVOID          lpvData
);

BOOL O97EvalPushLongOperand
(
    LPO97_ENV       lpstEnv,
    long            l
);

BOOL O97EvalPushSZOperand
(
    LPO97_ENV       lpstEnv,
    LPSTR           lpsz
);

BOOL O97DoOperatorConcatenate
(
    LPO97_ENV       lpstEnv
);

BOOL O97EvalDoOperator
(
    LPO97_ENV       lpstEnv,
    EO97_OPERATOR_T eOperator
);

BOOL O97EvalQueryTopOperand
(
    LPO97_ENV       lpstEnv,
    LPEO97_TYPE     lpeType,
    LPLPVOID        lplpvData
);

BOOL O97EvalGetTopBaseData
(
    LPO97_ENV       lpstEnv,
    LPEO97_TYPE     lpeType,
    LPLPVOID        lplpvData
);

BOOL O97EvalQueryTopOperandAsString
(
    LPO97_ENV       lpstEnv,
    LPLPBYTE        lplpbyStr
);

BOOL O97EvalPopTopOperand
(
    LPO97_ENV       lpstEnv
);

BOOL O97EvalGetAndPopTopAsBoolean
(
    LPO97_ENV       lpstEnv,
    LPBOOL          lpbResult
);

BOOL O97EvalGetAndPopTopAsLong
(
    LPO97_ENV       lpstEnv,
    LPLONG          lplResult
);

BOOL O97EvalGetAndPopTopAsString
(
    LPO97_ENV       lpstEnv,
    LPLPBYTE        lplpbyStr
);

BOOL O97EvalGetAndPopTopAsVarID
(
    LPO97_ENV       lpstEnv,
    LPWORD          lpwVarID
);

BOOL O97EvalPopParameters
(
    LPO97_ENV       lpstEnv,
    WORD            wNumParams
);


/////////////////////////////////////////////////////////////////
// Call functions
/////////////////////////////////////////////////////////////////

BOOL O97CallSetUpParameters
(
    LPO97_ENV       lpstEnv,
    BOOL            bIsSub
);

BOOL O97CallSetup
(
    LPO97_ENV           lpstEnv,
    WORD                wFunctionID,
    WORD                wSubID
);

BOOL O97ExternalCallSetup
(
    LPO97_ENV           lpstEnv,
    DWORD               dwModuleIndex,
    DWORD               dwModuleEntry
);

BOOL O97ExternalCallCleanup
(
    LPO97_ENV           lpstEnv
);

BOOL O97SetupCallUsingString
(
    LPO97_ENV           lpstEnv
);

#endif // #ifndef _O97ENV_H_

