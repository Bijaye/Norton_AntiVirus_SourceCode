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




void bx_debug_EbGb(void);
void bx_debug_EvGv(void);
void bx_debug_GbEb(void);
void bx_debug_GvEv(void);
void bx_debug_ALIb(void);
void bx_debug_eAXIv(void);
void bx_debug_ES(void);
void bx_debug_Ew(void);
void bx_debug_Ms(void);
void bx_debug_GvEw(void);
void bx_debug_RdCd(void);
void bx_debug_RdDd(void);
void bx_debug_CdRd(void);
void bx_debug_DdRd(void);
void bx_debug_RdTd(void);
void bx_debug_TdRd(void);
void bx_debug_Jv(void);

void bx_debug_Eb(void);
void bx_debug_FS(void);
void bx_debug_EvGvIb(void);
void bx_debug_EvGvCL(void);
void bx_debug_XBTS(void);
void bx_debug_IBTS(void);
void bx_debug_GS(void);
void bx_debug_Mp(void);
void bx_debug_EvIb(void);
void bx_debug_GvEb(void);
void bx_debug_eAX(void);
void bx_debug_eCX(void);
void bx_debug_eDX(void);
void bx_debug_eBX(void);
void bx_debug_eSP(void);
void bx_debug_eBP(void);
void bx_debug_eSI(void);
void bx_debug_eDI(void);
void bx_debug_SS(void);
void bx_debug_DS(void);

void bx_debug_eSP(void);
void bx_debug_eBP(void);
void bx_debug_eSI(void);
void bx_debug_eDI(void);
void bx_debug_GvMa(void);
void bx_debug_EwRw(void);
void bx_debug_Iv(void);
void bx_debug_GvEvIv(void);
void bx_debug_Ib(void);
void bx_debug_GvEvIb(void);
void bx_debug_YbDX(void);
void bx_debug_YvDX(void);
void bx_debug_DXXb(void);
void bx_debug_DXXv(void);
void bx_debug_Jb(void);

void bx_debug_EbIb(void);
void bx_debug_EvIv(void);
void bx_debug_EvIb(void);
void bx_debug_EwSw(void);
void bx_debug_GvM(void);
void bx_debug_SwEw(void);
void bx_debug_Ev(void);
void bx_debug_eCXeAX(void);
void bx_debug_eDXeAX(void);
void bx_debug_eBXeAX(void);
void bx_debug_eSPeAX(void);
void bx_debug_eBPeAX(void);
void bx_debug_eDIeAX(void);
void bx_debug_Ap(void);
void bx_debug_Fv(void);
void bx_debug_ALOb(void);
void bx_debug_eAXOv(void);
void bx_debug_ObAL(void);
void bx_debug_OveAX(void);
void bx_debug_XbYb(void);
void bx_debug_XvYv(void);
void bx_debug_YbAL(void);
void bx_debug_YveAX(void);
void bx_debug_ALXb(void);
void bx_debug_eAXXv(void);

void bx_debug_eSIeAX(void);
void bx_debug_CLIb(void);
void bx_debug_DLIb(void);
void bx_debug_BLIb(void);
void bx_debug_AHIb(void);
void bx_debug_CHIb(void);
void bx_debug_DHIb(void);
void bx_debug_BHIb(void);
void bx_debug_eCXIv(void);
void bx_debug_eDXIv(void);
void bx_debug_eSPIv(void);
void bx_debug_eSIIv(void);
void bx_debug_eDIIv(void);

void bx_debug_eBxIv(void);
void bx_debug_eBPIv(void);
void bx_debug_Iw(void);
void bx_debug_GvMp(void);
void bx_debug_IwIb(void);
void bx_debug_Eb1(void);
void bx_debug_Ev1(void);
void bx_debug_EbCL(void);
void bx_debug_EvCL(void);
void bx_debug_Es(void);

void bx_debug_CLIb(void);
void bx_debug_CLIb(void);
void bx_debug_BLIb(void);
void bx_debug_AHIb(void);
void bx_debug_CHIb(void);
void bx_debug_DHIb(void);
void bx_debug_BSIb(void);
void bx_debug_eCXIv(void);
void bx_debug_eDXIv(void);
void bx_debug_eBXIv(void);
void bx_debug_eSIIv(void);
void bx_debug_eDIIv(void);

void bx_debug_eSIeAX(void);
void bx_debug_DLIb(void);
void bx_debug_BHIb(void);
void bx_debug_eSPIv(void);
void bx_debug_eBPIv(void);
void bx_debug_Iw(void);
void bx_debug_GvMp(void);
void bx_debug_IwIb(void);
void bx_debug_Eb1(void);
void bx_debug_Ev1(void);
void bx_debug_EbCL(void);
void bx_debug_EvCL(void);
void bx_debug_Es(void);
void bx_debug_ST_STi(void);

void bx_debug_CS(void);
void bx_debug_Ea(void);
void bx_debug_STi(void);
void bx_debug_Et(void);
void bx_debug_Ed(void);
void bx_debug_El(void);
void bx_debug_AX(void);
void bx_debug_eAXIb(void);
void bx_debug_IbAL(void);
void bx_debug_IbeAX(void);

void bx_debug_STi_ST(void);
void bx_debug_Eq(void);
void bx_debug_Av(void);
void bx_debug_ALDX(void);
void bx_debug_eAXDX(void);
void bx_debug_DXAL(void);
void bx_debug_DXeAX(void);
void bx_debug_ALEb(void);
void bx_debug_eAXEv(void);
void bx_debug_Ep(void);


Bit8u  bx_debug_next_byte(void);
Bit8u  bx_debug_peek_next_byte(void);
Bit16u bx_debug_next_word(void);
Bit32u bx_debug_next_dword(void);
void  debug_invalid_opcode(void);

void bx_debug_decode_exgx(int reg_type, int modrm_reg_type);


#define BX_SEGMENT_REG       10
#define BX_GENERAL_8BIT_REG  11
#define BX_GENERAL_16BIT_REG 12
#define BX_GENERAL_32BIT_REG 13
#define BX_NO_REG_TYPE       14
