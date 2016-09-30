/*
Copyright Notice
================
BOCHS is Copyright 1994,1995,1996 by Kevin P. Lawton.

BOCHS is commercial software.

For more information, read the file 'LICENSE' included in the bochs
distribution.  If you don't have access to this file, or have questions
regarding the licensing policy, the author may be contacted via:

    US Mail:  Kevin Lawton
              528 Lexington St.
              Waltham, MA 02154

    EMail:    bochs@world.std.com
*/





/* ------ One byte opcode stuff ------ */
/* 0x00 - 0x0F */

#ifndef _INSTR_PROTO_H
#define _INSTR_PROTO_H

void bx_ADD_EbGb(void);
void bx_ADD_EvGv(void);
void bx_ADD_GbEb(void);
void bx_ADD_GvEv(void);
void bx_ADD_ALIb(void);
void bx_ADD_eAXIv(void);
void bx_PUSH_ES(void);
void bx_POP_ES(void);
void bx_OR_EbGb(void);
void bx_OR_EvGv(void);
void bx_OR_GbEb(void);
void bx_OR_GvEv(void);
void bx_OR_ALIb(void);
void bx_OR_eAXIv(void);
void bx_PUSH_CS(void);

/* 0x10 - 0x1F */
void bx_ADC_EbGb(void);
void bx_ADC_EvGv(void);
void bx_ADC_GbEb(void);
void bx_ADC_GvEv(void);
void bx_ADC_ALIb(void);
void bx_ADC_eAXIv(void);
void bx_PUSH_SS(void);
void bx_POP_SS(void);
void bx_SBB_EbGb(void);
void bx_SBB_EvGv(void);
void bx_SBB_GbEb(void);
void bx_SBB_GvEv(void);
void bx_SBB_ALIb(void);
void bx_SBB_eAXIv(void);
void bx_PUSH_DS(void);
void bx_POP_DS(void);

/* 0x20 - 0x2F */
void bx_AND_EbGb(void);
void bx_AND_EvGv(void);
void bx_AND_GbEb(void);
void bx_AND_GvEv(void);
void bx_AND_ALIb(void);
void bx_AND_eAXIv(void);
void bx_DAA(void);
void bx_SUB_EbGb(void);
void bx_SUB_EvGv(void);
void bx_SUB_GbEb(void);
void bx_SUB_GvEv(void);
void bx_SUB_ALIb(void);
void bx_SUB_eAXIv(void);
void bx_DAS(void);

/* 0x30 - 0x3F */
void bx_XOR_EbGb(void);
void bx_XOR_EvGv(void);
void bx_XOR_GbEb(void);
void bx_XOR_GvEv(void);
void bx_XOR_ALIb(void);
void bx_XOR_eAXIv(void);
void bx_AAA(void);
void bx_CMP_EbGb(void);
void bx_CMP_EvGv(void);
void bx_CMP_GbEb(void);
void bx_CMP_GvEv(void);
void bx_CMP_ALIb(void);
void bx_CMP_eAXIv(void);
void bx_AAS(void);

/* 0x40 - 0x4F */
void bx_INC_eAX(void);
void bx_INC_eCX(void);
void bx_INC_eDX(void);
void bx_INC_eBX(void);
void bx_INC_eSP(void);
void bx_INC_eBP(void);
void bx_INC_eSI(void);
void bx_INC_eDI(void);
void bx_DEC_eAX(void);
void bx_DEC_eCX(void);
void bx_DEC_eDX(void);
void bx_DEC_eBX(void);
void bx_DEC_eSP(void);
void bx_DEC_eBP(void);
void bx_DEC_eSI(void);
void bx_DEC_eDI(void);

/* 0x50 - 0x5F */
void bx_PUSH_eAX(void);
void bx_PUSH_eCX(void);
void bx_PUSH_eDX(void);
void bx_PUSH_eBX(void);
void bx_PUSH_eSP(void);
void bx_PUSH_eBP(void);
void bx_PUSH_eSI(void);
void bx_PUSH_eDI(void);
void bx_POP_eAX(void);
void bx_POP_eCX(void);
void bx_POP_eDX(void);
void bx_POP_eBX(void);
void bx_POP_eSP(void);
void bx_POP_eBP(void);
void bx_POP_eSI(void);
void bx_POP_eDI(void);

/* 0x60 - 0x6F */
void bx_PUSHAD(void);
void bx_POPAD(void);
void bx_BOUND_GvMa(void);
void bx_ARPL_EwGw(void);
void bx_PUSH_Iv(void);
void bx_IMUL_GvEvIv(void);
void bx_PUSH_Ib(void);
void bx_IMUL_GvEvIb(void);
void bx_INSB_YbDX(void);
void bx_INSW_YvDX(void);
void bx_OUTSB_DXXb(void);
void bx_OUTSW_DXXv(void);

/* 0x70 - 0x7F */
void bx_JO_Jb(void);
void bx_JNO_Jb(void);
void bx_JB_Jb(void);
void bx_JNB_Jb(void);
void bx_JZ_Jb(void);
void bx_JNZ_Jb(void);
void bx_JBE_Jb(void);
void bx_JNBE_Jb(void);
void bx_JS_Jb(void);
void bx_JNS_Jb(void);
void bx_JP_Jb(void);
void bx_JNP_Jb(void);
void bx_JL_Jb(void);
void bx_JNL_Jb(void);
void bx_JLE_Jb(void);
void bx_JNLE_Jb(void);

/* 0x80 - 0x8F */
/* NULL, GROUP1 EbIb */
/* NULL, GROUP1 EvIv */
/* NULL, INVALID */
/* NULL, GROUP1 EvIb */
void bx_TEST_EbGb(void);
void bx_TEST_EvGv(void);
void bx_XCHG_EbGb(void);
void bx_XCHG_EvGv(void);
void bx_MOV_EbGb(void);
void bx_MOV_EvGv(void);
void bx_MOV_GbEb(void);
void bx_MOV_GvEv(void);
void bx_MOV_EwSw(void);
void bx_LEA_GvM(void);
void bx_MOV_SwEw(void);
void bx_POP_Ev(void);

/* 0x90 - 0x9F */
/* NULL, NOP */
void bx_XCHG_eDXeAX(void);
void bx_XCHG_eBXeAX(void);
void bx_XCHG_eSPeAX(void);
void bx_XCHG_eBPeAX(void);
void bx_XCHG_eSIeAX(void);
void bx_XCHG_eDIeAX(void);
void bx_CBW(void);
void bx_CWD(void);
void bx_CALL_Ap(void);
void bx_WAIT(void);
void bx_PUSHF_Fv(void);
void bx_POPF_Fv(void);
void bx_SAHF(void);
void bx_LAHF(void);

/* 0xA0 - 0xAF */
void bx_MOV_ALOb(void);
void bx_MOV_eAXOv(void);
void bx_MOV_ObAL(void);
void bx_MOV_OveAX(void);
void bx_MOVSB_XbYb(void);
void bx_MOVSW_XvYv(void);
void bx_CMPSB_XbYb(void);
void bx_CMPSW_XvYv(void);
void bx_TEST_ALIb(void);
void bx_TEST_eAXIv(void);
void bx_STOSB_YbAL(void);
void bx_STOSW_YveAX(void);
void bx_LODSB_ALXb(void);
void bx_LODSW_eAXXv(void);
void bx_SCASB_ALXb(void);
void bx_SCASW_eAXXv(void);

/* 0xB0 - 0xBF */
void bx_MOV_ALIb(void);
void bx_MOV_CLIb(void);
void bx_MOV_DLIb(void);
void bx_MOV_BLIb(void);
void bx_MOV_AHIb(void);
void bx_MOV_CHIb(void);
void bx_MOV_DHIb(void);
void bx_MOV_BHIb(void);
void bx_MOV_eAXIv(void);
void bx_MOV_eCXIv(void);
void bx_MOV_eDXIv(void);
void bx_MOV_eBXIv(void);
void bx_MOV_eSPIv(void);
void bx_MOV_eBPIv(void);
void bx_MOV_eSIIv(void);
void bx_MOV_eDIIv(void);

/* 0xC0 - 0xCF */
/* NULL, group 2 */
/* NULL, group 2 */
void bx_RETnear(void);
void bx_LES_GvMp(void);
void bx_LDS_GvMp(void);
void bx_MOV_EbIb(void);
void bx_MOV_EvIv(void);
void bx_ENTER_IwIb(void);
void bx_LEAVE(void);
void bx_RETfar(void);
void bx_INT3(void);
void bx_INT_Ib(void);
void bx_INTO(void);
void bx_IRET(void);

/* 0xD0 - 0xDF */
/* group2 Eb1 */
/* group2 Ev1 */
/* group2 EbCL */
/* group2 EvCL */
void bx_AAM(void);
void bx_AAD(void);
/* NULL, INVALID */
void bx_XLAT(void);
/* NULL, ESC0 */
/* NULL, ESC1 */
/* NULL, ESC2 */
/* NULL, ESC3 */
/* NULL, ESC4 */
/* NULL, ESC5 */
/* NULL, ESC6 */
/* NULL, ESC7 */

/* 0xE0 - 0xEF */
void bx_LOOPNE_Jb(void);
void bx_LOOPE_Jb(void);
void bx_LOOP_Jb(void);
void bx_JCXZ_Jb(void);
void bx_IN_ALIb(void);
void bx_IN_eAXIb(void);
void bx_OUT_IbAL(void);
void bx_OUT_IbeAX(void);
void bx_CALL_Av(void);
void bx_JMP_Jv(void);
void bx_JMP_Ap(void);
void bx_JMP_Jb(void);
void bx_IN_ALDX(void);
void bx_IN_eAXDX(void);
void bx_OUT_DXAL(void);
void bx_OUT_DXeAX(void);

/* 0xF0 - 0xFF */
/* NULL, LOCK */
/* NULL, INVALID */
/* NULL, REPNE */
/* NULL, REP */
void bx_HLT(void);
void bx_CMC(void);
/* NULL, GROUP3 Eb */
/* NULL, GROUP3 Ev */
void bx_CLC(void);
void bx_STC(void);
void bx_CLI(void);
void bx_STI(void);
void bx_CLD(void);
void bx_STD(void);
/* NULL, GROUP4 */
/* NULL, GROUP5 */


/* ------ Two byte opcode stuff ------ */
/* 0x00 - 0x0F */
/* NULL, GROUP6 */
/* NULL, GROUP7 */
void bx_LAR_GvEw(void);
void bx_LSL_GvEw(void);
/* NULL, INVALID */
/* NULL, INVALID */
void bx_CLTS(void);
/* NULL, INVALID */
void bx_INVD(void);
void bx_WBINVD(void);
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */

/* 0x10 - 0x1F */
void bx_INVLPG_Ea(void);
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */

/* 0x20 - 0x2F */
void bx_MOV_CdRd(void);
void bx_MOV_DdRd(void);
void bx_MOV_RdCd(void);
void bx_MOV_RdDd(void);
void bx_MOV_TdRd(void);
/* NULL, INVALID */
void bx_MOV_RdTd(void);
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */

/* 0x30 - 0x3F */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */

/* 0x40 - Ox4F */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */

/* 0x50 - 0x5F */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */

/* 0x60 - 0x6F */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */

/* 0x70 - 0x7F */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */

/* 0x80 - 0x8F */
void bx_JO_Jv(void);
void bx_JNO_Jv(void);
void bx_JB_Jv(void);
void bx_JNB_Jv(void);
void bx_JZ_Jv(void);
void bx_JNZ_Jv(void);
void bx_JBE_Jv(void);
void bx_JNBE_Jv(void);
void bx_JS_Jv(void);
void bx_JNS_Jv(void);
void bx_JP_Jv(void);
void bx_JNP_Jv(void);
void bx_JL_Jv(void);
void bx_JNL_Jv(void);
void bx_JLE_Jv(void);
void bx_JNLE_Jv(void);

/* 0x90 - 0x9F */
void bx_SETO_Eb(void);
void bx_SETNO_Eb(void);
void bx_SETB_Eb(void);
void bx_SETNB_Eb(void);
void bx_SETZ_Eb(void);
void bx_SETNZ_Eb(void);
void bx_SETBE_Eb(void);
void bx_SETNBE_Eb(void);
void bx_SETS_Eb(void);
void bx_SETNS_Eb(void);
void bx_SETP_Eb(void);
void bx_SETNP_Eb(void);
void bx_SETL_Eb(void);
void bx_SETNL_Eb(void);
void bx_SETLE_Eb(void);
void bx_SETNLE_Eb(void);

/* A0 - AF */
void bx_PUSH_FS(void);
void bx_POP_FS(void);
void bx_CPUID(void);
void bx_BT_EvGv(void);
void bx_SHLD_EvGvIb(void);
void bx_SHLD_EvGvCL(void);


void bx_PUSH_GS(void);
void bx_POP_GS(void);
/* NULL, INVALID */
void bx_BTS_EvGv(void);
void bx_SHRD_EvGvIb(void);
void bx_SHRD_EvGvCL(void);
/* NULL, INVALID */
void bx_IMUL_GvEv(void);

/* B0 - BF */
void bx_LSS_GvMp(void);
void bx_BTR_EvGv(void);
void bx_LFS_GvMp(void);
void bx_LGS_GvMp(void);
void bx_MOVZX_GvEb(void);
void bx_MOVZX_GvEw(void);
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, GROUP8 EvIb */
void bx_BTC_EvGv(void);
void bx_BSF_GvEv(void);
void bx_BSR_GvEv(void);
void bx_MOVSX_GvEb(void);
void bx_MOVSX_GvEw(void);

/* C0 - CF */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
void bx_BSWAP_EAX(void);
void bx_BSWAP_ECX(void);
void bx_BSWAP_EDX(void);
void bx_BSWAP_EBX(void);
void bx_BSWAP_ESP(void);
void bx_BSWAP_EBP(void);
void bx_BSWAP_ESI(void);
void bx_BSWAP_EDI(void);

/* D0 - DF */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */

/* E0 - EF */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */

/* F0 - FF */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */


/* --------- MOD R/M OPCODE STUFF -------- */

/* GROUP1[EbIb] */
void bx_ADD_EbIb(void);
void bx_OR_EbIb(void);
void bx_ADC_EbIb(void);
void bx_SBB_EbIb(void);
void bx_AND_EbIb(void);
void bx_SUB_EbIb(void);
void bx_XOR_EbIb(void);
void bx_CMP_EbIb(void);

/* GROUP1[EvIv] */
void bx_ADD_EvIv(void);
void bx_OR_EvIv(void);
void bx_ADC_EvIv(void);
void bx_SBB_EvIv(void);
void bx_AND_EvIv(void);
void bx_SUB_EvIv(void);
void bx_XOR_EvIv(void);
void bx_CMP_EvIv(void);

/* GROUP1[EvIb] */
void bx_ADD_EvIb(void);
void bx_OR_EvIb(void);
void bx_ADC_EvIb(void);
void bx_SBB_EvIb(void);
void bx_AND_EvIb(void);
void bx_SUB_EvIb(void);
void bx_XOR_EvIb(void);
void bx_CMP_EvIb(void);

/* GROUP2[EbIb] */
void bx_ROL_EbIb(void);
void bx_ROR_EbIb(void);
void bx_RCL_EbIb(void);
void bx_RCR_EbIb(void);
void bx_SHL_EbIb(void);
void bx_SHR_EbIb(void);
/* NULL, INVALID */
void bx_SAR_EbIb(void);

/* GROUP2[EvIb] */
void bx_ROL_EvIb(void);
void bx_ROR_EvIb(void);
void bx_RCL_EvIb(void);
void bx_RCR_EvIb(void);
void bx_SHL_EvIb(void);
void bx_SHR_EvIb(void);
/* NULL, INVALID */
void bx_SAR_EvIb(void);

/* GROUP2[Eb1] */
void bx_ROL_Eb1(void);
void bx_ROR_Eb1(void);
void bx_RCL_Eb1(void);
void bx_RCR_Eb1(void);
void bx_SHL_Eb1(void);
void bx_SHR_Eb1(void);
/* NULL, INVALID */
void bx_SAR_Eb1(void);

/* GROUP2[Ev1] */
void bx_ROL_Ev1(void);
void bx_ROR_Ev1(void);
void bx_RCL_Ev1(void);
void bx_RCR_Ev1(void);
void bx_SHL_Ev1(void);
void bx_SHR_Ev1(void);
/* NULL, INVALID */
void bx_SAR_Ev1(void);

/* GROUP2[EbCL] */
void bx_ROL_EbCL(void);
void bx_ROR_EbCL(void);
void bx_RCL_EbCL(void);
void bx_RCR_EbCL(void);
void bx_SHL_EbCL(void);
void bx_SHR_EbCL(void);
/* NULL, INVALID */
void bx_SAR_EbCL(void);

/* GROUP2[EvCL] */
void bx_ROL_EvCL(void);
void bx_ROR_EvCL(void);
void bx_RCL_EvCL(void);
void bx_RCR_EvCL(void);
void bx_SHL_EvCL(void);
void bx_SHR_EvCL(void);
/* NULL, INVALID */
void bx_SAR_EvCL(void);

/* GROUP3[Eb] */
void bx_TEST_EbIb(void);
/* NULL, INVALID */
void bx_NOT_Eb(void);
void bx_NEG_Eb(void);
void bx_MUL_ALEb(void);
void bx_IMUL_ALEb(void);
void bx_DIV_ALEb(void);
void bx_IDIV_ALEb(void);

/* GROUP3[Ev] */
void bx_TEST_EvIv(void);
/* NULL, INVALID */
void bx_NOT_Ev(void);
void bx_NEG_Ev(void);
void bx_MUL_eAXEv(void);
void bx_IMUL_eAXEv(void);
void bx_DIV_eAXEv(void);
void bx_IDIV_eAXEv(void);

/* GROUP4[] */
void bx_INC_Eb(void);
void bx_DEC_Eb(void);
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */

/* GROUP5[] */
void bx_INC_Ev(void);
void bx_DEC_Ev(void);
void bx_CALL_Ev(void);
void bx_CALL_Ep(void);
void bx_JMP_Ev(void);
void bx_JMP_Ep(void);
void bx_PUSH_Ev(void);
/* NULL, INVALID */

/* GROUP6[] */
void bx_SLDT_Ew(void);
void bx_STR_Ew(void);
void bx_LLDT_Ew(void);
void bx_LTR_Ew(void);
void bx_VERR_Ew(void);
void bx_VERW_Ew(void);
/* NULL, INVALID */
/* NULL, INVALID */

/* GROUP7[] */
void bx_SGDT_Ms(void);
void bx_SIDT_Ms(void);
void bx_LGDT_Ms(void);
void bx_LIDT_Ms(void);
void bx_SMSW_Ew(void);
/* NULL, INVALID */
void bx_LMSW_Ew(void);
/* NULL, INVALID */


/* GROUP8[EvIb] */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
void bx_BT_EvIb(void);
void bx_BTS_EvIb(void);
void bx_BTR_EvIb(void);
void bx_BTC_EvIb(void);

/* ------ NDP ESCAPES 0-7 --------- */

/* NDP ESC 0 */
void bx_FADD_Es(void);
void bx_FMUL_Es(void);
void bx_FCOM_Es(void);
void bx_FCOMP_Es(void);
void bx_FSUB_Es(void);
void bx_FSUBR_Es(void);
void bx_FDIV_Es(void);
void bx_FDIVR_Es(void);

void bx_FADD_ST_STi(void);
void bx_FMUL_ST_STi(void);
void bx_FCOM_ST_STi(void);
void bx_FCOMP_ST_STi(void);
void bx_FSUB_ST_STi(void);
void bx_FSUBR_ST_STi(void);
void bx_FDIV_ST_STi(void);
void bx_FDIVR_ST_STi(void);

/* NDP ESC 1 */
void bx_FLD_Es(void);
/* NULL, INVALID */
void bx_FST_Es(void);
void bx_FSTP_Es(void);
void bx_FLDENV_Ea(void);
void bx_FLDCW_Ew(void);
void bx_FSTENV_Ea(void);
void bx_FSTCW_Ew(void);

void bx_FNOP(void);
/* NULL, INVALID */
void bx_FCHS(void);
void bx_FLD1(void);
void bx_F2XM1(void);
void bx_FPREM(void);

/* NULL, INVALID */
/* NULL, INVALID */
void bx_FABS(void);
void bx_FLDL2T(void);
void bx_FYL2X(void);
void bx_FYL2XP1(void);

/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
void bx_FLDL2E(void);
void bx_FPTAN(void);
void bx_FSQRT(void);

void bx_FXCH_3(void);
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
void bx_FLDPI(void);
void bx_FPATAN(void);
void bx_FSINCOS(void);

/* NULL, INVALID */
/* NULL, INVALID */
void bx_FTST(void);
void bx_FLDLG2(void);
void bx_FXTRACT(void);
void bx_FRNDINT(void);

/* NULL, INVALID */
/* NULL, INVALID */
void bx_FXAM(void);
void bx_FSCALE(void);

/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
void bx_FLDZ(void);
void bx_FDECSTP(void);
void bx_FSIN(void);
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
void bx_FINCSTP(void);
void bx_FCOS(void);

/* NDP ESC 2 */
void bx_FIADD_Ew(void);
void bx_FIMUL_Ew(void);
void bx_FICOM_Ew(void);
void bx_FICOMP_Ew(void);
void bx_FISUB_Ew(void);
void bx_FISUBR_Ew(void);
void bx_FIDIV_Ew(void);
void bx_FIDIVR_Ew(void);

/* NULL, INVALID */
void bx_FUCOMPP(void);
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */

/* NDP ESC 3 */
void bx_FILD_Ew(void);
/* NULL, INVALID */
void bx_FIST_Ew(void);
void bx_FISTP_Ew(void);
void bx_FLD_Et(void);
/* NULL, INVALID */
void bx_FSTP_Et(void);
/* NULL, INVALID */

/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, GROUP3A */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */

/* NDP ESC 3, GROUP3a */
void bx_FENI(void);
void bx_FDISI(void);
void bx_FCLEX(void);
void bx_FINIT(void);
void bx_FSETPM(void);
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */

/* NDP ESC 4 */
void bx_FADD_El(void);
void bx_FMUL_El(void);
void bx_FCOM_El(void);
void bx_FCOMP_El(void);
void bx_FSUB_El(void);
void bx_FSUBR_El(void);
void bx_FDIV_El(void);
void bx_FDIVR_El(void);

void bx_FADD_STi_ST(void);
void bx_FMUL_STi_ST(void);
void bx_FCOM_STi_ST(void);
void bx_FCOMP_STi_ST(void);
void bx_FSUB_STi_ST(void);
void bx_FSUBR_STi_ST(void);
void bx_FDIV_STi_ST(void);
void bx_FDIVR_STi_ST(void);

/* NDP ESC 5 */
void bx_FLD_El(void);
/* NULL, INVALID */
void bx_FST_El(void);
void bx_FSTP_El(void);
void bx_FRSTOR_Ea(void);
/* NULL, INVALID */
void bx_FSAVE_Ea(void);
void bx_FSTSW_Ew(void);

void bx_FFREE_STi(void);
/* NULL, INVALID */
void bx_FST_STi(void);
void bx_FSTP_STi(void);
void bx_FUCOMP_STi(void);
/* NULL, INVALID */
/* NULL, INVALID */

/* NDP ESC 6 */
void bx_FIADD_Ed(void);
void bx_FIMUL_Ed(void);
void bx_FICOM_Ed(void);
void bx_FICOMP_Ed(void);
void bx_FISUB_Ed(void);
void bx_FISUBR_Ed(void);
void bx_FIDIV_Ed(void);
void bx_FIDIVR_Ed(void);

void bx_FADDP_STi_ST(void);
void bx_FMULP_STi_ST(void);
/* NULL, INVALID */
void bx_FCOMPP(void);
void bx_FSUBP_STi_ST(void);
void bx_FSUBRP_STi_ST(void);
void bx_FDIVP_STi_ST(void);
void bx_FDIVRP_STi_ST(void);

/* NDP ESC 7 */
void bx_FILD_Ed(void);
/* NULL, INVALID */
void bx_FIST_Ed(void);
void bx_FISTP_Ed(void);
void bx_FBLD_Eb(void);
void bx_FILD_Eq(void);
void bx_FBSTP_Eb(void);
void bx_FISTP_Eq(void);

void bx_FSTSW_AX(void);
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */



void bx_CMPXCHG_XBTS(void);
void bx_CMPXCHG_IBTS(void);
void bx_CMPXCHG_EbGb(void);
void bx_CMPXCHG_EvGv(void);
void bx_XADD_EbGb(void);
void bx_XADD_EvGv(void);
void bx_XCHG_eCXeAX(void);
void bx_RETnear_Iw(void);
void bx_RETfar_Iw(void);
void bx_FLD_STi(void);
void bx_FXCH_STi(void);
void bx_FLDLN2(void);
void bx_FPREM1(void);
void bx_FUCOM_STi_ST(void);
void bx_FCOMP_STi(void);

void bx_PSEUDO_INT_Ib(void);

void bx_LOADALL(void);


#endif /* _INSTR_PROTO_H */
