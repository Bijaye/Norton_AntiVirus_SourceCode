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





#include "bochs.h"



INLINE void bx_FCOMP_STi() {bx_panic("FCOMP?:\n");}

/* NDP ESC 0 */
INLINE void bx_FADD_Es() {bx_panic("FADD?:\n");}
INLINE void bx_FMUL_Es() {bx_panic("FMUL?:\n");}
INLINE void bx_FCOM_Es() {bx_panic("FCOM?:\n");}
INLINE void bx_FCOMP_Es() {bx_panic("FCOMP?:\n");}
INLINE void bx_FSUB_Es() {bx_panic("FSUB?:\n");}
INLINE void bx_FSUBR_Es() {bx_panic("FSUBR?:\n");}
INLINE void bx_FDIV_Es() {bx_panic("FDIV?:\n");}
INLINE void bx_FDIVR_Es() {bx_panic("FDIVR?:\n");}

INLINE void bx_FADD_ST_STi() {bx_panic("FADD?:\n");}
INLINE void bx_FMUL_ST_STi() {bx_panic("FMUL?:\n");}
INLINE void bx_FCOM_ST_STi() {bx_panic("FCOM?:\n");}
INLINE void bx_FCOMP_ST_STi() {bx_panic("FCOMP?:\n");}
INLINE void bx_FSUB_ST_STi() {bx_panic("FSUB?:\n");}
INLINE void bx_FSUBR_ST_STi() {bx_panic("FSUBR?:\n");}
INLINE void bx_FDIV_ST_STi() {bx_panic("FDIV?:\n");}
INLINE void bx_FDIVR_ST_STi() {bx_panic("FDIVR?:\n");}

/* NDP ESC 1 */
INLINE void bx_FLD_STi() {bx_panic("FLD?:\n");}
INLINE void bx_FXCH_STi() {bx_panic("FXCH?:\n");}

INLINE void bx_FLD_Es() {bx_panic("FLD?:\n");}
/* NULL, INVALID */
INLINE void bx_FST_Es() {bx_panic("FST?:\n");}
INLINE void bx_FSTP_Es() {bx_panic("FSTP?:\n");}
INLINE void bx_FLDENV_Ea() {bx_panic("FLDENV?:\n");}
INLINE void bx_FLDCW_Ew() {bx_panic("FLDCW?:\n");}
INLINE void bx_FSTENV_Ea() {bx_panic("FSTENV?:\n");}

  INLINE void
bx_FSTCW_Ew()
{
  Bit32u mem_addr;
  unsigned unused, mem_type;
  bx_segment_reg_t *mem_seg_reg;

  Bit16u control_word;

  bx_decode_exgx(&unused, &mem_addr, &mem_type, &mem_seg_reg);
  BX_HANDLE_EXCEPTION()

  control_word = 0;
  if (mem_type == BX_REGISTER_REF) {
    bx_panic("fstcw_ew: Ew is register\n");
    }
  else {
    bx_write_virtual_word(mem_seg_reg, mem_addr, &control_word);
    BX_HANDLE_EXCEPTION()
    }

  if (bx_cpu.cr0.ts) {
    bx_panic("FSTCW_Ew: TS set\n");
    }
}



INLINE void bx_FNOP() {bx_panic("FNOP?:\n");}
/* NULL, INVALID */
INLINE void bx_FCHS() {bx_panic("FCHS?:\n");}
INLINE void bx_FLD1() {bx_panic("FLD1?:\n");}
INLINE void bx_F2XM1() {bx_panic("F2XM1?:\n");}
INLINE void bx_FPREM() {bx_panic("FPREM?:\n");}


/* NULL, INVALID */
/* NULL, INVALID */
INLINE void bx_FABS() {bx_panic("FABS?:\n");}
INLINE void bx_FLDL2T() {bx_panic("FLDL2T?:\n");}
INLINE void bx_FYL2X() {bx_panic("FYL2X?:\n");}
INLINE void bx_FYL2XP1() {bx_panic("FYL2XP1?:\n");}


/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
INLINE void bx_FLDL2E() {bx_panic("FLDL2E?:\n");}
INLINE void bx_FPTAN() {bx_panic("FPTAN?:\n");}
INLINE void bx_FSQRT() {bx_panic("FSQRT?:\n");}

INLINE void bx_FXCH_3() {bx_panic("FXCH?:\n");}
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
INLINE void bx_FLDPI() {bx_panic("FLDPI?:\n");}
INLINE void bx_FPATAN() {bx_panic("FPATAN?:\n");}
INLINE void bx_FSINCOS() {bx_panic("FSINCOS?:\n");}


/* NULL, INVALID */
/* NULL, INVALID */
INLINE void bx_FTST() {bx_panic("FTST?:\n");}
INLINE void bx_FLDLG2() {bx_panic("FLDLG2?:\n");}
INLINE void bx_FXTRACT() {bx_panic("FXTRACT?:\n");}
INLINE void bx_FRNDINT() {bx_panic("FRNDINT?:\n");}


/* NULL, INVALID */
/* NULL, INVALID */
INLINE void bx_FXAM() {bx_panic("FXAM?:\n");}
INLINE void bx_FLDLN2() {bx_panic("FLDLN2?:\n");}
INLINE void bx_FPREM1() {bx_panic("FPREM1?:\n");}
INLINE void bx_FSCALE() {bx_panic("FSCALE?:\n");}


/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
INLINE void bx_FLDZ() {bx_panic("FLDZ?:\n");}
INLINE void bx_FDECSTP() {bx_panic("FDECSTP?:\n");}
INLINE void bx_FSIN() {bx_panic("FSIN?:\n");}

/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
INLINE void bx_FINCSTP() {bx_panic("FINCSTP?:\n");}
INLINE void bx_FCOS() {bx_panic("FCOS?:\n");}

/* NDP ESC 2 */
INLINE void bx_FIADD_Ew() {bx_panic("FIADD?:\n");}
INLINE void bx_FIMUL_Ew() {bx_panic("FIMUL?:\n");}
INLINE void bx_FICOM_Ew() {bx_panic("FICOM?:\n");}
INLINE void bx_FICOMP_Ew() {bx_panic("FICOMP?:\n");}
INLINE void bx_FISUB_Ew() {bx_panic("FISUB?:\n");}
INLINE void bx_FISUBR_Ew() {bx_panic("FISUBR?:\n");}
INLINE void bx_FIDIV_Ew() {bx_panic("FIDIV?:\n");}
INLINE void bx_FIDIVR_Ew() {bx_panic("FIDIVR?:\n");}

/* NULL, INVALID */
INLINE void bx_FUCOMPP() {bx_panic("FUCOMPP?:\n");}
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */

/* NDP ESC 3 */
INLINE void bx_FILD_Ew() {bx_panic("FILD?:\n");}
/* NULL, INVALID */
INLINE void bx_FIST_Ew() {bx_panic("FIST?:\n");}
INLINE void bx_FISTP_Ew() {bx_panic("FISTP?:\n");}
INLINE void bx_FLD_Et() {bx_panic("FLD?:\n");}
/* NULL, INVALID */
INLINE void bx_FSTP_Et() {bx_panic("FSTP?:\n");}
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
INLINE void bx_FENI() {bx_panic("FENI?:\n");}
INLINE void bx_FDISI() {bx_panic("FDISI?:\n");}
INLINE void bx_FCLEX() {bx_panic("FCLEX?:\n");}

  INLINE void
bx_FINIT()
{
  bx_printf("FINIT: called, but no action taken.\n");
}

  INLINE void
bx_FSETPM()
{
#if BX_CPU > 2
  bx_panic("ndp.c: not implemented for 386\n");
#endif

#if BX_CPU >= 2
  if (bx_protected_mode())
    bx_panic(
      "not yet supported for protected mode\n");
#endif

  bx_panic("FSETPM not supported!  Continuing anyway!\n");
}

/* NULL, INVALID */
/* NULL, INVALID */
/* NULL, INVALID */

/* NDP ESC 4 */
INLINE void bx_FADD_El() {bx_panic("FADD?:\n");}
INLINE void bx_FMUL_El() {bx_panic("FMUL?:\n");}
INLINE void bx_FCOM_El() {bx_panic("FCOM?:\n");}
INLINE void bx_FCOMP_El() {bx_panic("FCOMP?:\n");}
INLINE void bx_FSUB_El() {bx_panic("FSUB?:\n");}
INLINE void bx_FSUBR_El() {bx_panic("FSUBR?:\n");}
INLINE void bx_FDIV_El() {bx_panic("FDIV?:\n");}
INLINE void bx_FDIVR_El() {bx_panic("FDIVR?:\n");}

INLINE void bx_FADD_STi_ST() {bx_panic("FADD?:\n");}
INLINE void bx_FMUL_STi_ST() {bx_panic("FMUL?:\n");}
INLINE void bx_FCOM_STi_ST() {bx_panic("FCOM?:\n");}
INLINE void bx_FCOMP_STi_ST() {bx_panic("FCOMP?:\n");}
INLINE void bx_FSUB_STi_ST() {bx_panic("FSUB?:\n");}
INLINE void bx_FSUBR_STi_ST() {bx_panic("FSUBR?:\n");}
INLINE void bx_FDIV_STi_ST() {bx_panic("FDIV?:\n");}
INLINE void bx_FDIVR_STi_ST() {bx_panic("FDIVR:\n");}

/* NDP ESC 5 */
INLINE void bx_FLD_El() {bx_panic("FLD?:\n");}
/* NULL, INVALID */
INLINE void bx_FST_El() {bx_panic("FST?:\n");}
INLINE void bx_FSTP_El() {bx_panic("FSTP?:\n");}
INLINE void bx_FRSTOR_Ea() {bx_panic("FRSTOR?:\n");}
/* NULL, INVALID */
INLINE void bx_FSAVE_Ea() {bx_panic("FSAVE?:\n");}

  INLINE void
bx_FSTSW_Ew()
{
  Bit32u mem_addr;
  unsigned  unused, mem_type;
  bx_segment_reg_t *mem_seg_reg;

  bx_printf("FSTSW_Ew: called but no action taken\n");

  bx_decode_exgx(&unused, &mem_addr, &mem_type, &mem_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* do nothing: in particular, don't write any value to memory at Ew */

  if (bx_cpu.cr0.ts) {
    bx_panic("FSTSW_Ew: TS set\n");
    }
}


INLINE void bx_FFREE_STi() {bx_panic("FFREE?:\n");}
/* NULL, INVALID */
INLINE void bx_FST_STi() {bx_panic("FST?:\n");}
INLINE void bx_FSTP_STi() {bx_panic("FSTP?:\n");}
INLINE void bx_FUCOM_STi_ST() {bx_panic("FUCOM?:\n");}
INLINE void bx_FUCOMP_STi() {bx_panic("FUCOMP?:\n");}
/* NULL, INVALID */
/* NULL, INVALID */

/* NDP ESC 6 */
INLINE void bx_FIADD_Ed() {bx_panic("FIADD?:\n");}
INLINE void bx_FIMUL_Ed() {bx_panic("FIMUL?:\n");}
INLINE void bx_FICOM_Ed() {bx_panic("FICOM?:\n");}
INLINE void bx_FICOMP_Ed() {bx_panic("FICOMP?:\n");}
INLINE void bx_FISUB_Ed() {bx_panic("FISUB?:\n");}
INLINE void bx_FISUBR_Ed() {bx_panic("FISUBR?:\n");}
INLINE void bx_FIDIV_Ed() {bx_panic("FIDIV?:\n");}
INLINE void bx_FIDIVR_Ed() {bx_panic("FIDIVR?:\n");}

INLINE void bx_FADDP_STi_ST() {bx_panic("FADDP?:\n");}
INLINE void bx_FMULP_STi_ST() {bx_panic("FMULP?:\n");}
/* NULL, INVALID */
INLINE void bx_FCOMPP() {bx_panic("FCOMPP?:\n");}
INLINE void bx_FSUBP_STi_ST() {bx_panic("FSUBP?:\n");}
INLINE void bx_FSUBRP_STi_ST() {bx_panic("FSUBRP?:\n");}
INLINE void bx_FDIVP_STi_ST() {bx_panic("FDIV?:\n");}
INLINE void bx_FDIVRP_STi_ST() {bx_panic("FDIVRP?:\n");}

/* NDP ESC 7 */
INLINE void bx_FILD_Ed()
{
#if BX_CPU > 2
  bx_panic("ndp.c: not implemented for 386\n");
#endif

#if BX_CPU >= 2
  if (bx_protected_mode())
    bx_panic(
      "not yet supported for protected mode\n");
#endif

bx_panic("FILD:\n");
}

  INLINE void
bx_FIST_Ed()
{
#if BX_CPU > 2
  bx_panic("ndp.c: not implemented for 386\n");
#endif

#if BX_CPU >= 2
  if (bx_protected_mode())
    bx_panic(
      "not yet supported for protected mode\n");
#endif

bx_panic("FIST:\n");
}

INLINE void bx_FISTP_Ed() {bx_panic("FISTP?:\n");}
INLINE void bx_FBLD_Eb() {bx_panic("FBLD?:\n");}
INLINE void bx_FILD_Eq() {bx_panic("FILD?:\n");}
INLINE void bx_FBSTP_Eb() {bx_panic("FBSTP?:\n");}
INLINE void bx_FISTP_Eq() {bx_panic("FISTP?:\n");}

INLINE void bx_FSTSW_AX() {bx_panic("FSTSW?:\n");}
