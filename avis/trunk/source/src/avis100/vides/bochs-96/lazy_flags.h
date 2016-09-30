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



#if BX_EXTERNAL_ENVIRONMENT==0

#define BX_INSTR_ADD8   1
#define BX_INSTR_ADD16  2
#define BX_INSTR_ADD32  3

#define BX_INSTR_SUB8   4
#define BX_INSTR_SUB16  5
#define BX_INSTR_SUB32  6

#define BX_INSTR_ADC8   7
#define BX_INSTR_ADC16  8
#define BX_INSTR_ADC32  9

#define BX_INSTR_SBB8   10
#define BX_INSTR_SBB16  11
#define BX_INSTR_SBB32  12

#define BX_INSTR_CMP8   13
#define BX_INSTR_CMP16  14
#define BX_INSTR_CMP32  15

#define BX_INSTR_INC8   16
#define BX_INSTR_INC16  17
#define BX_INSTR_INC32  18

#define BX_INSTR_DEC8   19
#define BX_INSTR_DEC16  20
#define BX_INSTR_DEC32  21

#define BX_INSTR_NEG8   22
#define BX_INSTR_NEG16  23
#define BX_INSTR_NEG32  24

#define BX_INSTR_XADD8  25
#define BX_INSTR_XADD16 26
#define BX_INSTR_XADD32 27

#define BX_INSTR_OR8    28
#define BX_INSTR_OR16   29
#define BX_INSTR_OR32   30

#define BX_INSTR_AND8   31
#define BX_INSTR_AND16  32
#define BX_INSTR_AND32  33

#define BX_INSTR_TEST8   34
#define BX_INSTR_TEST16  35
#define BX_INSTR_TEST32  36

#define BX_INSTR_XOR8    37
#define BX_INSTR_XOR16   38
#define BX_INSTR_XOR32   39

#define BX_INSTR_CMPS8   40
#define BX_INSTR_CMPS16  41
#define BX_INSTR_CMPS32  42

#define BX_INSTR_SCAS8   43
#define BX_INSTR_SCAS16  44
#define BX_INSTR_SCAS32  45

#define BX_INSTR_SHR8    46
#define BX_INSTR_SHR16   47
#define BX_INSTR_SHR32   48

#define BX_INSTR_SHL8    49
#define BX_INSTR_SHL16   50
#define BX_INSTR_SHL32   51



#define BX_LF_INDEX_KNOWN   0
#define BX_LF_INDEX_OSZAPC  1
#define BX_LF_INDEX_OSZAP   2
#define BX_LF_INDEX_P       3

#define BX_LF_MASK_OSZAPC 0x111111
#define BX_LF_MASK_OSZAP  0x222220
#define BX_LF_MASK_P      0x000030

#ifdef WIN32
  #ifdef inline
    #undef inline
  #endif
  #define inline 
#endif

typedef struct {
  Bit8u op1_8;
  Bit8u op2_8;
  Bit8u result_8;

  Bit16u op1_16;
  Bit16u op2_16;
  Bit16u result_16;

  Bit32u op1_32;
  Bit32u op2_32;
  Bit32u result_32;

  Boolean prev_CF;
  unsigned instr;
  } bx_lf_flags_entry;

extern bx_lf_flags_entry bx_oszapc;
extern bx_lf_flags_entry bx_oszap;

extern Bit32u bx_lf_flags_status;
extern Boolean bx_lf_pf;


  static inline void
BX_SET_FLAGS_OSZAPC_8(Bit8u op1, Bit8u op2, Bit8u result, unsigned instr)
{
  bx_oszapc.op1_8 = op1;
  bx_oszapc.op2_8 = op2;
  bx_oszapc.result_8 = result;
  bx_oszapc.instr = instr;
  bx_lf_flags_status = BX_LF_MASK_OSZAPC;
}

  static inline void
BX_SET_FLAGS_OSZAPC_8_CF(Bit8u op1, Bit8u op2, Bit8u result, unsigned instr,
                         Boolean prev_CF)
{
  bx_oszapc.op1_8 = op1;
  bx_oszapc.op2_8 = op2;
  bx_oszapc.result_8 = result;
  bx_oszapc.instr = instr;
  bx_oszapc.prev_CF = prev_CF;
  bx_lf_flags_status = BX_LF_MASK_OSZAPC;
}

  static inline void
BX_SET_FLAGS_OSZAPC_16(Bit16u op1, Bit16u op2, Bit16u result, unsigned instr)
{
  bx_oszapc.op1_16 = op1;
  bx_oszapc.op2_16 = op2;
  bx_oszapc.result_16 = result;
  bx_oszapc.instr = instr;
  bx_lf_flags_status = BX_LF_MASK_OSZAPC;
}

  static inline void
BX_SET_FLAGS_OSZAPC_16_CF(Bit16u op1, Bit16u op2, Bit16u result, unsigned instr,
                          Boolean prev_CF)
{
  bx_oszapc.op1_16 = op1;
  bx_oszapc.op2_16 = op2;
  bx_oszapc.result_16 = result;
  bx_oszapc.instr = instr;
  bx_oszapc.prev_CF = prev_CF;
  bx_lf_flags_status = BX_LF_MASK_OSZAPC;
}

  static inline void
BX_SET_FLAGS_OSZAPC_32(Bit32u op1, Bit32u op2, Bit32u result, unsigned instr)
{
  bx_oszapc.op1_32 = op1;
  bx_oszapc.op2_32 = op2;
  bx_oszapc.result_32 = result;
  bx_oszapc.instr = instr;
  bx_lf_flags_status = BX_LF_MASK_OSZAPC;
}

  static inline void
BX_SET_FLAGS_OSZAPC_32_CF(Bit32u op1, Bit32u op2, Bit32u result, unsigned instr,
                          Boolean prev_CF)
{
  bx_oszapc.op1_32 = op1;
  bx_oszapc.op2_32 = op2;
  bx_oszapc.result_32 = result;
  bx_oszapc.instr = instr;
  bx_oszapc.prev_CF = prev_CF;
  bx_lf_flags_status = BX_LF_MASK_OSZAPC;
}


  static inline void
BX_SET_FLAGS_OSZAP_8(Bit8u op1, Bit8u op2, Bit8u result, unsigned instr)
{
  bx_oszap.op1_8 = op1;
  bx_oszap.op2_8 = op2;
  bx_oszap.result_8 = result;
  bx_oszap.instr = instr;
  bx_lf_flags_status = (bx_lf_flags_status & 0x00000f) | BX_LF_MASK_OSZAP;
}

  static inline void
BX_SET_FLAGS_OSZAP_16(Bit16u op1, Bit16u op2, Bit16u result, unsigned instr)
{
  bx_oszap.op1_16 = op1;
  bx_oszap.op2_16 = op2;
  bx_oszap.result_16 = result;
  bx_oszap.instr = instr;
  bx_lf_flags_status = (bx_lf_flags_status & 0x00000f) | BX_LF_MASK_OSZAP;
}

  static inline void
BX_SET_FLAGS_OSZAP_32(Bit32u op1, Bit32u op2, Bit32u result, unsigned instr)
{
  bx_oszap.op1_32 = op1;
  bx_oszap.op2_32 = op2;
  bx_oszap.result_32 = result;
  bx_oszap.instr = instr;
  bx_lf_flags_status = (bx_lf_flags_status & 0x00000f) | BX_LF_MASK_OSZAP;
}

  static inline void
BX_SET_OxxxxC(Boolean new_of, Boolean new_cf)
{
  bx_cpu.eflags.of = new_of;
  bx_cpu.eflags.cf = new_cf;
  bx_lf_flags_status &= 0x0ffff0;
  /* ??? could also mark other bits undefined here */
}

  static inline void
bx_set_CF(Boolean val)
{
  bx_lf_flags_status &= 0xfffff0;
  bx_cpu.eflags.cf = val;
}

  static inline void
bx_set_AF(Boolean val)
{
  bx_lf_flags_status &= 0xfff0ff;
  bx_cpu.eflags.af = val;
}

  static inline void
bx_set_ZF(Boolean val)
{
  bx_lf_flags_status &= 0xff0fff;
  bx_cpu.eflags.zf = val;
}

  static inline void
bx_set_SF(Boolean val)
{
  bx_lf_flags_status &= 0xf0ffff;
  bx_cpu.eflags.sf = val;
}


  static inline void
bx_set_OF(Boolean val)
{
  bx_lf_flags_status &= 0x0fffff;
  bx_cpu.eflags.of = val;
}

  static inline void
bx_set_PF(Boolean val)
{
  bx_lf_flags_status &= 0xffff0f;
  bx_lf_pf = val;
}

  static inline void
bx_set_PF_base(Bit8u val)
{
  bx_cpu.eflags.pf_byte = val;
  bx_lf_flags_status = (bx_lf_flags_status & 0xffff0f) | BX_LF_MASK_P;
}





Boolean bx_get_CF(void);
Boolean bx_get_AF(void);
Boolean bx_get_ZF(void);
Boolean bx_get_SF(void);
Boolean bx_get_OF(void);
Boolean bx_get_PF(void);

void    bx_resolve_flags(void);

#endif /* BX_EXTERNAL_ENVIRONMENT==0 */
