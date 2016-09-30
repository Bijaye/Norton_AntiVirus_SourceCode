#include "watcher.h"
#include "watch_ins_tbl.h"
#include "..\..\Bochs-96\fetch.h"
#include "..\..\Bochs-96\instr_proto.h"

void code_escape(void)
{
   instructions_escape[bx_fetch_next_byte()]();
}

void no_watch(void)
{
}

void panic_opcode(void)
{
	bx_panic("0xFFFF Panic opcode found!\n");
}

void illegal(void)
{
	bx_exception(6, 0, 0);
}

void nop(void) {}

void setalc(void)
{
   AL = bx_get_CF()?0xff:0x00;
}

void icebp(void)
{
   // check for dr7 bit 12: implement later
   bx_interrupt(1, 1, 0);
}

void group1EbIb(void)
{
   Bit8u mod_rm_byte, mod, opcode, rm;
   mod_rm_byte = bx_peek_next_byte();
   BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
   switch(opcode)
   {
   case 0x00: bx_ADD_EbIb(); break;
   case 0x01: bx_OR_EbIb();  break;
   case 0x02: bx_ADC_EbIb(); break;
   case 0x03: bx_SBB_EbIb(); break;
   case 0x04: bx_AND_EbIb(); break;
   case 0x05: bx_SUB_EbIb(); break;
   case 0x06: bx_XOR_EbIb(); break;
   case 0x07: bx_CMP_EbIb(); break;
   default: bx_panic("decode: error in bx_decode()!\n"); break;
   }
}

void group1EvIv(void)
{
   Bit8u mod_rm_byte, mod, opcode, rm;
   mod_rm_byte = bx_peek_next_byte();
   BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
   switch(opcode)
   {
   case 0x00: bx_ADD_EvIv(); break;
   case 0x01: bx_OR_EvIv(); break;
   case 0x02: bx_ADC_EvIv(); break;
   case 0x03: bx_SBB_EvIv(); break;
   case 0x04: bx_AND_EvIv(); break;
   case 0x05: bx_SUB_EvIv(); break;
   case 0x06: bx_XOR_EvIv(); break;
   case 0x07: bx_CMP_EvIv(); break;
   default: bx_panic("decode: error in bx_decode()!\n"); break;
   }
}

void group1EvIb(void)
{
   Bit8u mod_rm_byte, mod, opcode, rm;
   mod_rm_byte = bx_peek_next_byte();
   BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
   switch(opcode)
   {
   case 0x00: bx_ADD_EvIb(); break;
   case 0x01: bx_OR_EvIb(); break;
   case 0x02: bx_ADC_EvIb(); break;
   case 0x03: bx_SBB_EvIb(); break;
   case 0x04: bx_AND_EvIb(); break;
   case 0x05: bx_SUB_EvIb(); break;
   case 0x06: bx_XOR_EvIb(); break;
   case 0x07: bx_CMP_EvIb(); break;
   default: bx_panic("decode: error in bx_decode()!\n"); break;
   }
}

void group2EbIb(void)
{
   Bit8u mod_rm_byte, mod, opcode, rm;
	mod_rm_byte = bx_peek_next_byte();
	BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
	switch(opcode)
	{
	case 0x00: bx_ROL_EbIb(); break;
	case 0x01: bx_ROR_EbIb(); break;
	case 0x02: bx_RCL_EbIb(); break;
	case 0x03: bx_RCR_EbIb(); break;
	case 0x04: bx_SHL_EbIb(); break;
	case 0x05: bx_SHR_EbIb(); break;
	case 0x06: bx_SHL_EbIb(); break;
	case 0x07: bx_SAR_EbIb(); break;
	default: bx_panic("decode: error in bx_decode()!\n"); break;
   }
}

void group2Eb1(void)
{
   Bit8u mod_rm_byte, mod, opcode, rm;
	mod_rm_byte = bx_peek_next_byte();
	BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
	switch(opcode)
	{
	case 0x00: bx_ROL_Eb1(); break;
	case 0x01: bx_ROR_Eb1(); break;
	case 0x02: bx_RCL_Eb1(); break;
	case 0x03: bx_RCR_Eb1(); break;
	case 0x04: bx_SHL_Eb1(); break;
	case 0x05: bx_SHR_Eb1(); break;
	case 0x06: bx_SHL_Eb1(); break;
	case 0x07: bx_SAR_Eb1(); break;
	default: bx_panic("decode: error in bx_decode()!\n"); break;
	}
}

void group2EbCL(void)
{
   Bit8u mod_rm_byte, mod, opcode, rm;
	mod_rm_byte = bx_peek_next_byte();
	BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
	switch(opcode)
	{
	case 0x00: bx_ROL_EbCL(); break;
	case 0x01: bx_ROR_EbCL(); break;
	case 0x02: bx_RCL_EbCL(); break;
	case 0x03: bx_RCR_EbCL(); break;
	case 0x04: bx_SHL_EbCL(); break;
	case 0x05: bx_SHR_EbCL(); break;
	case 0x06: bx_SHL_EbCL(); break;
	case 0x07: bx_SAR_EbCL(); break;
	default: bx_panic("decode: error in bx_decode()!\n"); break;
	}
}

void group2EvIb(void)
{
   Bit8u mod_rm_byte, mod, opcode, rm;
	mod_rm_byte = bx_peek_next_byte();
	BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
	switch(opcode)
	{
	case 0x00: bx_ROL_EvIb(); break;
	case 0x01: bx_ROR_EvIb(); break;
	case 0x02: bx_RCL_EvIb(); break;
	case 0x03: bx_RCR_EvIb(); break;
	case 0x04: bx_SHL_EvIb(); break;
	case 0x05: bx_SHR_EvIb(); break;
	case 0x06: bx_SHL_EvIb(); break;
	case 0x07: bx_SAR_EvIb(); break;
	default: bx_panic("decode: error in bx_decode()!\n"); break;
   }
}

void group2Ev1(void)
{
   Bit8u mod_rm_byte, mod, opcode, rm;
	mod_rm_byte = bx_peek_next_byte();
	BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
	switch(opcode)
	{
	case 0x00: bx_ROL_Ev1(); break;
	case 0x01: bx_ROR_Ev1(); break;
	case 0x02: bx_RCL_Ev1(); break;
	case 0x03: bx_RCR_Ev1(); break;
	case 0x04: bx_SHL_Ev1(); break;
	case 0x05: bx_SHR_Ev1(); break;
	case 0x06: bx_SHL_Ev1(); break;
	case 0x07: bx_SAR_Ev1(); break;
	default: bx_panic("decode: error in bx_decode()!\n"); break;
	}
}

void group2EvCL(void)
{
   Bit8u mod_rm_byte, mod, opcode, rm;
	mod_rm_byte = bx_peek_next_byte();
	BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
	switch(opcode)
	{
	case 0x00: bx_ROL_EvCL(); break;
	case 0x01: bx_ROR_EvCL(); break;
	case 0x02: bx_RCL_EvCL(); break;
	case 0x03: bx_RCR_EvCL(); break;
	case 0x04: bx_SHL_EvCL(); break;
	case 0x05: bx_SHR_EvCL(); break;
	case 0x06: bx_SHL_EvCL(); break;
	case 0x07: bx_SAR_EvCL(); break;
	default: bx_panic("decode: error in bx_decode()!\n"); break;
	}
}

void group3Eb(void)
{
   Bit8u mod_rm_byte, mod, opcode, rm;
	mod_rm_byte = bx_peek_next_byte();
	BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
	switch(opcode)
	{
	case 0x00: bx_TEST_EbIb(); break;
	case 0x01: bx_panic("decode: F6:TEST_EbIb duplicate\n"); break;
	case 0x02: bx_NOT_Eb(); break;
	case 0x03: bx_NEG_Eb(); break;
	case 0x04: bx_MUL_ALEb(); break;
	case 0x05: bx_IMUL_ALEb(); break;
	case 0x06: bx_DIV_ALEb(); break;
	case 0x07: bx_IDIV_ALEb(); break;
	default: bx_panic("decode: error in bx_decode()!\n"); break;
	}
}

void group3Ev(void)
{
   Bit8u mod_rm_byte, mod, opcode, rm;
	mod_rm_byte = bx_peek_next_byte();
	BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
	switch(opcode)
	{
	case 0x00: bx_TEST_EvIv(); break;
	case 0x01: bx_panic("decode: F7 01:TEST_EvIv duplicate\n"); break;
	case 0x02: bx_NOT_Ev(); break;
	case 0x03: bx_NEG_Ev(); break;
	case 0x04: bx_MUL_eAXEv(); break;
	case 0x05: bx_IMUL_eAXEv(); break;
	case 0x06: bx_DIV_eAXEv(); break;
	case 0x07: bx_IDIV_eAXEv(); break;
	default: bx_panic("decode: error in bx_decode()!\n"); break;
	}
}

void group4(void)
{
   Bit8u mod_rm_byte, mod, opcode, rm;
	mod_rm_byte = bx_peek_next_byte();
	BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
	switch(opcode)
	{
	case 0x00: bx_INC_Eb(); break;
	case 0x01: bx_DEC_Eb(); break;
	default: bx_panic("decode: FE default:\n"); break;
	}
}

void group5(void)
{
   Bit8u mod_rm_byte, mod, opcode, rm;
	mod_rm_byte = bx_peek_next_byte();
	BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
#if 0 // Fred
	switch(opcode)
	{
	case 0x00: bx_INC_Ev(); break;
	case 0x01: bx_DEC_Ev(); break;
	case 0x02: bx_CALL_Ev(); break;
   case 0x03: bx_CALL_Ep(); break;
	case 0x04: bx_JMP_Ev(); break;
	case 0x05: bx_JMP_Ep(); break;
	case 0x06: bx_PUSH_Ev(); break;
	default: bx_panic("decode: FF default:\n"); break;
	}
#endif
	instructions_group5[opcode]();
}

void group6(void)
{
   Bit8u mod_rm_byte, mod, opcode, rm;
   mod_rm_byte = bx_peek_next_byte();
	BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
	switch(opcode)
	{
	case 0x00: bx_SLDT_Ew(); break;
	case 0x01: bx_STR_Ew(); break;
	case 0x02: bx_LLDT_Ew(); break;
	case 0x03: bx_LTR_Ew(); break;
	case 0x04: bx_VERR_Ew(); break;
	case 0x05: bx_VERW_Ew(); break;
	case 0x06: bx_panic("decode: : Grp6:\n"); break;
	case 0x07: bx_panic("decode: : Grp6:\n"); break;
	default: bx_panic("decode: error in bx_decode()!\n"); break;
	}
}

void group7(void)
{
   Bit8u mod_rm_byte, mod, opcode, rm;
   mod_rm_byte = bx_peek_next_byte();
	BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
	switch(opcode)
	{
	case 0x00: bx_SGDT_Ms(); break;
	case 0x01: bx_SIDT_Ms(); break;
	case 0x02: bx_LGDT_Ms(); break;
	case 0x03: bx_LIDT_Ms(); break;
	case 0x04: bx_SMSW_Ew(); break;
	case 0x05: bx_panic("decode: : Grp7:\n"); break;
	case 0x06: bx_LMSW_Ew(); break;
	case 0x07: bx_panic("decode: : Grp7:\n"); break;
	default: bx_panic("decode: error in bx_decode()!\n"); break;
   }
}

void group8(void)
{
   Bit8u mod_rm_byte, mod, opcode, rm;
	mod_rm_byte = bx_peek_next_byte();
	BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
	switch(opcode)
	{
	case 0x00:
	case 0x01:
	case 0x02:
	case 0x03: bx_panic("decode: Grp8:\n"); break;
	case 0x04: bx_BT_EvIb(); break;
	case 0x05: bx_BTS_EvIb(); break;
	case 0x06: bx_BTR_EvIb(); break;
	case 0x07: bx_BTC_EvIb(); break;
	default: bx_panic("decode: error in bx_decode()!\n"); break;
   }
}


void esc0(void)
{
   Bit8u mod_rm_byte, mod, opcode, rm;
	bx_panic("ESC0:\n");
	mod_rm_byte = bx_peek_next_byte();
	BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
	if(mod != 3)
	{
		switch(opcode)
		{
		case 0x00: bx_FADD_Es(); break;
		case 0x01: bx_FMUL_Es(); break;
		case 0x02: bx_FCOM_Es(); break;
		case 0x03: bx_FCOMP_Es(); break;
		case 0x04: bx_FSUB_Es(); break;
		case 0x05: bx_FSUBR_Es(); break;
		case 0x06: bx_FDIV_Es(); break;
		case 0x07: bx_FDIVR_Es(); break;
		default: bx_panic("decode: error in bx_decode()!\n"); break;
		}
	}
   else
   {
      /* mod == 3 */
		/* used bx_peek_next_byte().  do I need to read forward another byte? */
		switch(opcode)
		{
		case 0x00: bx_FADD_ST_STi(); break;
		case 0x01: bx_FMUL_ST_STi(); break;
		case 0x02: bx_FCOM_ST_STi(); break;
		case 0x03: bx_FCOMP_ST_STi(); break;
		case 0x04: bx_FSUB_ST_STi(); break;
		case 0x05: bx_FSUBR_ST_STi(); break;
		case 0x06: bx_FDIV_ST_STi(); break;
		case 0x07: bx_FDIVR_ST_STi(); break;
		default: bx_panic("decode: error in bx_decode()!\n"); break;
		}
   }
}

void esc1(void)
{
   Bit8u mod_rm_byte, mod, opcode, rm;
   mod_rm_byte = bx_peek_next_byte();
	BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
	bx_printf("decode: ESC1 sequence found, mod = %d, opcode = %d\n", (int) mod, (int) opcode);
	if(mod != 3)
	{
		switch(opcode)
		{
		case 0x00: bx_FLD_Es(); break;
		case 0x01: bx_panic("decode: ESC1:\n"); break;
      case 0x02: bx_FST_Es(); break;
      case 0x03: bx_FSTP_Es(); break;
      case 0x04: bx_FLDENV_Ea(); break;
      case 0x05: bx_FLDCW_Ew(); break;
      case 0x06: bx_FSTENV_Ea(); break;
      case 0x07: bx_FSTCW_Ew(); break;
      default: bx_panic("decode: error in bx_decode()!\n"); break;
		}
   }
   else
   { /* mod == 3 */
      /* used bx_peek_next_byte().  do I need to read forward another byte? */
      switch(opcode)
      {
      case 0x00:
         bx_FLD_STi(); break;
      case 0x01:
         bx_FXCH_STi(); break;
      case 0x02:
         if(rm == 0)
         {
            bx_FNOP(); break;
         }
         else
         {
            bx_panic("decode: ESC9:\n"); break;
         }
      case 0x03:
         bx_FSTP_STi(); break;

      case 0x04:
         switch(rm)
         {
         case 0x00: bx_FCHS(); break;
         case 0x01: bx_FABS(); break;
         case 0x02:
         case 0x03: bx_panic("decode: ESC9:\n"); break;
         case 0x04: bx_FTST(); break;
         case 0x05: bx_FXAM(); break;
         case 0x06:
         case 0x07: bx_panic("decode: ESC9:\n"); break;
         }
         break;
      case 0x05:
         switch(rm)
         {
         case 0x00: bx_FLD1(); break;
         case 0x01: bx_FLDL2T(); break;
         case 0x02: bx_FLDL2E(); break;
         case 0x03: bx_FLDPI(); break;
         case 0x04: bx_FLDLG2(); break;
         case 0x05: bx_FLDLN2(); break;
         case 0x06: bx_FLDZ(); break;
         case 0x07: bx_panic("decode: ESC9:\n"); break;
         }
         break;
      case 0x06:
         switch(rm)
         {
         case 0x00: bx_F2XM1(); break;
         case 0x01: bx_FYL2X(); break;
         case 0x02: bx_FPTAN(); break;
         case 0x03: bx_FPATAN(); break;
         case 0x04: bx_FXTRACT(); break;
         case 0x05: bx_FPREM1(); break;
         case 0x06: bx_FDECSTP(); break;
         case 0x07: bx_FINCSTP(); break;
         }
         break;
      case 0x07:
         switch(rm)
         {
         case 0x00: bx_FPREM(); break;
         case 0x01: bx_FYL2XP1(); break;
         case 0x02: bx_FSQRT(); break;
         case 0x03: bx_FSINCOS(); break;
         case 0x04: bx_FRNDINT(); break;
         case 0x05: bx_FSCALE(); break;
         case 0x06: bx_FSIN(); break;
         case 0x07: bx_FCOS(); break;
         }
         break;
      default: bx_panic("decode: error in bx_decode()!\n"); break;
      }
   }
}

void esc2(void)
{
   Bit8u mod_rm_byte, mod, opcode, rm;
//   bx_panic("ESC2:\n");
   mod_rm_byte = bx_peek_next_byte();
	BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
	if(mod != 3)
	{
		switch(opcode)
		{
		case 0x00: bx_FIADD_Ed(); break;
		case 0x01: bx_FIMUL_Ed(); break;
		case 0x02: bx_FICOM_Ed(); break;
		case 0x03: bx_FICOMP_Ed(); break;
		case 0x04: bx_FISUB_Ed(); break;
		case 0x05: bx_FISUBR_Ed(); break;
		case 0x06: bx_FIDIV_Ed(); break;
		case 0x07: bx_FIDIVR_Ed(); break;
		default: bx_panic("decode: error in bx_decode()!\n"); break;
		}
	}
   else
   { /* mod == 3 */
		/* used bx_peek_next_byte().  do I need to read forward another byte? */
		switch(opcode)
      {
      case 0x05:
         if(rm == 1)
         {
            bx_FUCOMPP(); break;
         }
         else
         {
            bx_panic("decode: ESC2:\n"); break;
         }
      default: bx_panic("decode: ESC2:\n"); break;
      }
   }
}

void esc3(void)
{
   Bit8u mod_rm_byte, mod, opcode, rm;
   bx_printf("ESC3:\n");
   if(bx_cpu.cr0.em)
      bx_exception(BX_NM_EXCEPTION, 0, 0);
   mod_rm_byte = bx_peek_next_byte();
   BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
	if(mod != 3)
		switch(opcode)
      {
      case 0x00: bx_FILD_Ed(); break;
      case 0x01: bx_panic("decode: ESC3:\n"); break;
      case 0x02: bx_FIST_Ed(); break;
      case 0x03: bx_FISTP_Ed(); break;
      case 0x04: bx_panic("decode: ESC3:\n"); break;
      case 0x05: bx_FLD_Et(); break;
      case 0x06: bx_panic("decode: ESC3:\n"); break;
      case 0x07: bx_FSTP_Et(); break;
      default: bx_panic("decode: error in bx_decode()!\n"); break;
      }
   else
   { /* mod == 3 */
		(void) bx_fetch_next_byte();
      if(bx_cpu.errno) return;	/* exception occurred */
		switch(mod_rm_byte)
		{
      case 0xe0: bx_FENI(); break;
      case 0xe1: bx_FDISI(); break;
      case 0xe2: bx_FCLEX(); break;
      case 0xe3: bx_FINIT(); break;
      case 0xe4: bx_FSETPM(); break;
      default:
         bx_panic("decode: ESC3: reserved opcode\n"); break;
      }
   }
}

// TO DO: finish the escapes and groups
void esc4(void)
{
   Bit8u mod_rm_byte, mod, opcode, rm;
   bx_printf("ESC4:\n");
	mod_rm_byte = bx_peek_next_byte();
	// used bx_peek_next_byte().  do I need to read forward another byte? * /
	BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
	if(mod != 3)
	{
		switch(opcode)
		{
		case 0x00: bx_FADD_El(); break;
		case 0x01: bx_FMUL_El(); break;
		case 0x02: bx_FCOM_El(); break;
		case 0x03: bx_FCOMP_El(); break;
		case 0x04: bx_FSUB_El(); break;
		case 0x05: bx_FSUBR_El(); break;
		case 0x06: bx_FDIV_El(); break;
		case 0x07: bx_FDIVR_El(); break;
		default: bx_panic("decode: error in bx_decode()!\n"); break;
		}
   }
   else
   { /* mod == 3 */
	/* used bx_peek_next_byte().  do I need to read forward another byte? */
	   switch(opcode)
		{
		case 0x00: bx_FADD_STi_ST(); break;
		case 0x01: bx_FMUL_STi_ST(); break;
		case 0x02: bx_FCOM_STi_ST(); break;
		case 0x03: bx_FCOMP_STi_ST(); break;
		case 0x04: bx_FSUBR_STi_ST(); break;
		case 0x05: bx_FSUB_STi_ST(); break;
		case 0x06: bx_FDIVR_STi_ST(); break;
		case 0x07: bx_FDIV_STi_ST(); break;
		default: bx_panic("decode: ESC4:\n"); break;
		}
	}
}

void esc5(void)
{
   Bit8u mod_rm_byte, mod, opcode, rm;
   bx_printf("ESC5:\n");
	if(bx_cpu.cr0.em)
	{
		bx_exception(BX_NM_EXCEPTION, 0, 0);
		return;
	}
	mod_rm_byte = bx_peek_next_byte();
	BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
	if(mod != 3)
	{
		switch(opcode)
		{
		case 0x00: bx_FLD_El(); break;
		case 0x01: bx_panic("decode: ESC5:\n"); break;
		case 0x02: bx_FST_El(); break;
		case 0x03: bx_FSTP_El(); break;
		case 0x04: bx_FRSTOR_Ea(); break;
		case 0x05: bx_panic("decode: ESC5:\n"); break;
		case 0x06: bx_FSAVE_Ea(); break;
		case 0x07: bx_FSTSW_Ew(); break;
		default: bx_panic("decode: error in bx_decode()!\n"); break;
		}
	}
	else
   { /* mod == 3 */
		bx_printf("ESC5: recode\n");
	   /* used bx_peek_next_byte().  do I need to read forward another byte? */
		switch(opcode)
		{
		case 0x00: bx_FFREE_STi(); break;
		case 0x01: bx_FXCH_STi(); break;
		case 0x02: bx_FST_STi(); break;
		case 0x03: bx_FSTP_STi(); break;
		case 0x04: bx_FUCOM_STi_ST(); break;
		case 0x05: bx_FUCOMP_STi(); break;
		case 0x06: bx_panic("decode: ESC5:\n"); break;
		case 0x07: bx_panic("decode: ESC5:\n"); break;
		default: bx_panic("decode: ESC5:\n"); break;
		}
	}
}

void esc6(void)
{
   Bit8u mod_rm_byte, mod, opcode, rm;
   bx_printf("ESC6:\n");
   mod_rm_byte = bx_peek_next_byte();
	/* used bx_peek_next_byte().  do I need to read forward another byte? */
	BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
	if(mod != 3)
	{
		switch(opcode)
		{
		case 0x00: bx_FIADD_Ew(); break;
		case 0x01: bx_FIMUL_Ew(); break;
		case 0x02: bx_FICOM_Ew(); break;
		case 0x03: bx_FICOMP_Ew(); break;
		case 0x04: bx_FISUB_Ew(); break;
		case 0x05: bx_FISUBR_Ew(); break;
		case 0x06: bx_FIDIV_Ew(); break;
		case 0x07: bx_FIDIVR_Ew(); break;
		default: bx_panic("decode: error in bx_decode()!\n"); break;
		}
	}
	else
	{ /* mod == 3 */
      /* used bx_peek_next_byte().  do I need to read forward another byte? */
		switch(opcode)
		{
		case 0x00: bx_FADDP_STi_ST(); break;
		case 0x01: bx_FMULP_STi_ST(); break;
		case 0x02: bx_FCOMP_STi(); break;
		case 0x03:
		   switch(rm)
		   {
			   case 0x01: bx_FCOMPP(); break;
			   default: bx_panic("decode: ESC6:\n"); break;
		   }
         break;
		case 0x04: bx_FSUBRP_STi_ST(); break;
		case 0x05: bx_FSUBP_STi_ST(); break;
		case 0x06: bx_FDIVRP_STi_ST(); break;
		case 0x07: bx_FDIVP_STi_ST(); break;
		default: bx_panic("decode: ESC6:\n"); break;
		}
	}
}
            
void esc7(void)
{
   Bit8u mod_rm_byte, mod, opcode, rm;
   bx_printf("ESC7:\n");
	mod_rm_byte = bx_peek_next_byte();
	/* used bx_peek_next_byte().  do I need to read forward another byte? */
	BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
	if(mod != 3)
	{
		switch(opcode)
		{
		case 0x00: bx_FILD_Ew(); break;
		case 0x01: bx_panic("decode: ESC7:\n"); break;
		case 0x02: bx_FIST_Ew(); break;
		case 0x03: bx_FISTP_Ew(); break;
		case 0x04: bx_FBLD_Eb(); break;
		case 0x05: bx_FILD_Eq(); break;
		case 0x06: bx_FBSTP_Eb(); break;
		case 0x07: bx_FISTP_Eq(); break;
		default: bx_panic("decode: error in bx_decode()!\n"); break;
		}
	}
	else
	{ /* mod == 3 */
		/* used bx_peek_next_byte().  do I need to read forward another byte? */
		switch(opcode)
		{
		case 0x00: bx_FFREE_STi(); break;
		case 0x01: bx_FXCH_STi(); break;
		case 0x02: bx_FST_STi(); break;
		case 0x03: bx_FSTP_STi(); break;
		case 0x04:
		   switch(rm)
		   {
		   case 0x01: bx_FSTSW_AX(); break;
		   default: bx_panic("decode: ESC7:\n"); break;
		   }
			break;
      default: bx_panic("decode: ESC7:\n"); break;
		}
	}
}


              
               
void es_override(void)
{
   bx_cpu.segment_override = &bx_cpu.es;
   instructions[bx_fetch_next_byte()]();
}

void cs_override(void)
{
   bx_cpu.segment_override = &bx_cpu.cs;
   instructions[bx_fetch_next_byte()]();
}

void ss_override(void)
{
   bx_cpu.segment_override = &bx_cpu.ss;
   instructions[bx_fetch_next_byte()]();
}

void ds_override(void)
{
   bx_cpu.segment_override = &bx_cpu.ds;
   instructions[bx_fetch_next_byte()]();
}

void fs_override(void)
{
   bx_cpu.segment_override = &bx_cpu.fs;
   instructions[bx_fetch_next_byte()]();
}

void gs_override(void)
{
   bx_cpu.segment_override = &bx_cpu.gs;
   instructions[bx_fetch_next_byte()]();
}

void opsize_prefix(void)
{
	bx_cpu.is_32bit_opsize   = !bx_cpu.cs.cache.u.segment.d_b;
   instructions[bx_fetch_next_byte()]();
}

void addrsize_prefix(void)
{
   bx_cpu.is_32bit_addrsize = !bx_cpu.cs.cache.u.segment.d_b;
   instructions[bx_fetch_next_byte()]();
}

void repne(void)
{
   bx_rep_prefix = BX_REPNE_PREFIX;
   instructions[bx_fetch_next_byte()]();
}

void rep(void)
{
   bx_rep_prefix = BX_REPE_PREFIX;
   instructions[bx_fetch_next_byte()]();
}

void insb(void)
{
   if(bx_rep_prefix)
   {
      if(bx_cpu.is_32bit_addrsize)
         while(ECX != 0)
         {
            bx_INSB_YbDX();
				if(bx_cpu.errno) break;
				ECX = ECX - 1;
         }
      else
         while(CX != 0)
         {
            bx_INSB_YbDX();
            if(bx_cpu.errno) break;
            CX = CX - 1;
		   }
   }
   else
      bx_INSB_YbDX();
}

void insw(void)
{
   if(bx_rep_prefix)
   {
      if(bx_cpu.is_32bit_addrsize)
         while(ECX != 0)
			{
				bx_INSW_YvDX();
				if(bx_cpu.errno) break;
				ECX = ECX - 1;
			}
		else
         while(CX != 0)
         {
            bx_INSW_YvDX();
				if(bx_cpu.errno) break;
				CX = CX - 1;
			}
   }
   else
      bx_INSW_YvDX();
}

void outsb(void)
{
   if(bx_rep_prefix)
   {
		if(bx_cpu.is_32bit_addrsize)
         while(ECX != 0)
			{
				bx_OUTSB_DXXb();
				if(bx_cpu.errno) break;
				ECX = ECX - 1;
			}
		else
         while(CX != 0)
			{
			bx_OUTSB_DXXb();
			if(bx_cpu.errno) break;
         CX = CX - 1;
			}
   }
   else
      bx_OUTSB_DXXb();
}

void outsw(void)
{
   if(bx_rep_prefix)
   {
      if(bx_cpu.is_32bit_addrsize)
			while(ECX != 0)
			{
				bx_OUTSW_DXXv();
				if(bx_cpu.errno) break;
				ECX = ECX - 1;
			}
		else
         while(CX != 0)
			{
				bx_OUTSW_DXXv();
   			if(bx_cpu.errno) break;
				CX = CX - 1;
   		}
   }
   else
      bx_OUTSW_DXXv();
}

// Problem with this implementation: this is not exactly equivalent to a real processor since no IRQ can interrupt
// the MOVS in the middle, but this is probably OK

void movsb(void)
{
   if(bx_rep_prefix)
   {
		if(bx_cpu.is_32bit_addrsize)
         
         while(ECX != 0)
         {
				bx_MOVSB_XbYb();
				if(bx_cpu.errno) break;
				ECX = ECX - 1;
			}
		else
         while(CX != 0)
			{
            bx_MOVSB_XbYb();
            if(bx_cpu.errno) break;
				CX = CX - 1;
         }
	}
   else
      bx_MOVSB_XbYb();
}

void movsw(void)
{
   if(bx_rep_prefix)
	{
      if(bx_cpu.is_32bit_addrsize)
         while(ECX != 0)
         {
            bx_MOVSW_XvYv();
            if(bx_cpu.errno) break;
            ECX = ECX - 1;
         }
      else
         while(CX != 0)
         {
            bx_MOVSW_XvYv();
            if(bx_cpu.errno) break;
            CX = CX - 1;
         }
   }
   else
      bx_MOVSW_XvYv();
}

void cmpsb(void)
{
   if(bx_rep_prefix)
   {
      if(bx_cpu.is_32bit_addrsize)
         while(ECX != 0)
         {
            bx_CMPSB_XbYb();
            if(bx_cpu.errno) break;
            ECX = ECX - 1;
            if(bx_rep_prefix==BX_REPE_PREFIX && bx_get_ZF()==0) break;
            if(bx_rep_prefix==BX_REPNE_PREFIX && bx_get_ZF()!=0) break;
         }
		else
         while(CX != 0)
         {
            bx_CMPSB_XbYb();
            if(bx_cpu.errno) break;
            CX = CX - 1;
            if(bx_rep_prefix==BX_REPE_PREFIX && bx_get_ZF()==0) break;
            if(bx_rep_prefix==BX_REPNE_PREFIX && bx_get_ZF()!=0) break;
         }
   }
   else
      bx_CMPSB_XbYb();
}

void cmpsw(void)
{
   if(bx_rep_prefix)
   {
      if(bx_cpu.is_32bit_addrsize)
         while(ECX != 0)
         {
            bx_CMPSW_XvYv();
            if(bx_cpu.errno) break;
            ECX = ECX - 1;
            if(bx_rep_prefix==BX_REPE_PREFIX && bx_get_ZF()==0) break;
            if(bx_rep_prefix==BX_REPNE_PREFIX && bx_get_ZF()!=0) break;
         }
       else
         while(CX != 0)
         {
            bx_CMPSW_XvYv();
            if(bx_cpu.errno) break;
            CX = CX - 1;
            if(bx_rep_prefix==BX_REPE_PREFIX && bx_get_ZF()==0) break;
            if(bx_rep_prefix==BX_REPNE_PREFIX && bx_get_ZF()!=0) break;
         }
   }
   else
      bx_CMPSW_XvYv();
}

void stosb(void)
{
   if(bx_rep_prefix)
   {
      if(bx_cpu.is_32bit_addrsize)
         while(ECX != 0)
         {
            bx_STOSB_YbAL();
            if(bx_cpu.errno) break;
            ECX = ECX - 1;
         }
      else
         while(CX != 0)
         {
            bx_STOSB_YbAL();
            if(bx_cpu.errno) break;
            CX = CX - 1;
         }
   }
   else
      bx_STOSB_YbAL();
}

void stosw(void)
{
   if(bx_rep_prefix)
   {
      if(bx_cpu.is_32bit_addrsize)
         while(ECX != 0)
         {
            bx_STOSW_YveAX();
            if(bx_cpu.errno) break;
            ECX = ECX - 1;
         }
      else
         while(CX != 0)
         {
            bx_STOSW_YveAX();
            if(bx_cpu.errno) break;
            CX = CX - 1;
         }
   }
   else
      bx_STOSW_YveAX();
}

void lodsb(void)
{
   if(bx_rep_prefix)
   {
      if(bx_cpu.is_32bit_addrsize)
         while(ECX != 0)
         {
            bx_LODSB_ALXb();
            if(bx_cpu.errno) break;
            ECX = ECX - 1;
         }
      else
         while(CX != 0)
         {
            bx_LODSB_ALXb();
            if(bx_cpu.errno) break;
            CX = CX - 1;
         }
   }
   else
      bx_LODSB_ALXb();
}

void lodsw(void)
{
   if(bx_rep_prefix)
   {
      if(bx_cpu.is_32bit_addrsize)
         while(ECX != 0)
         {
            bx_LODSW_eAXXv();
            if(bx_cpu.errno) break;
            ECX = ECX - 1;
         }
      else
         while(CX != 0)
         {
            bx_LODSW_eAXXv();
            if(bx_cpu.errno) break;
            CX = CX - 1;
         }
   }
   else
      bx_LODSW_eAXXv();
}

void scasb(void)
{
   if(bx_rep_prefix)
   {
      if(bx_cpu.is_32bit_addrsize)
         while(ECX != 0)
         {
            bx_SCASB_ALXb();
            if(bx_cpu.errno) break;
            ECX = ECX - 1;
            if(bx_rep_prefix==BX_REPE_PREFIX && bx_get_ZF()==0) break;
            if(bx_rep_prefix==BX_REPNE_PREFIX && bx_get_ZF()!=0) break;
         }
      else
         while(CX != 0)
         {
            bx_SCASB_ALXb();
            if(bx_cpu.errno) break;
            CX = CX - 1;
            if(bx_rep_prefix==BX_REPE_PREFIX && bx_get_ZF()==0) break;
            if(bx_rep_prefix==BX_REPNE_PREFIX && bx_get_ZF()!=0) break;
         }
   }
   else
      bx_SCASB_ALXb();
}

void scasw(void)
{
   if(bx_rep_prefix)
   {
      if(bx_cpu.is_32bit_addrsize)
         while(ECX != 0)
         {
            bx_SCASW_eAXXv();
            if(bx_cpu.errno) break;
            ECX = ECX - 1;
            if(bx_rep_prefix==BX_REPE_PREFIX && bx_get_ZF()==0) break;
            if(bx_rep_prefix==BX_REPNE_PREFIX && bx_get_ZF()!=0) break;
         }
      else
         while(CX != 0)
         {
            bx_SCASW_eAXXv();
				if(bx_cpu.errno) break;
            CX = CX - 1;
            if(bx_rep_prefix==BX_REPE_PREFIX && bx_get_ZF()==0) break;
            if(bx_rep_prefix==BX_REPNE_PREFIX && bx_get_ZF()!=0) break;
         }
   }
   else
      bx_SCASW_eAXXv();
}

void lock(void)
{
   /* This is not a multiprocessor system, no need */
}

void loadalld(void)
{
   PANIC("watch_ins_tbl.c - LOADALLD not yet implemented");
}

void rdtsc(void)
{ /* 0f 31: read time stamp counter - Pentium undocumented */

}

void rdmsr(void)
{ /* 0f 32: read model specific register - Pentium undocumented */

}

Instr instructions[0x100] = {
   bx_ADD_EbGb, /* 00 */
   bx_ADD_EvGv, /* 01 */
   bx_ADD_GbEb, /* 02 */
   bx_ADD_GvEv, /* 03 */
	bx_ADD_ALIb, /* 04 */
	bx_ADD_eAXIv, /* 05 */
   bx_PUSH_ES, /* 06 */
	bx_POP_ES, /* 07 */
	bx_OR_EbGb, /* 08 */
	bx_OR_EvGv, /* 09 */
	bx_OR_GbEb, /* 0a */
	bx_OR_GvEv, /* 0b */
	bx_OR_ALIb, /* 0c */
	bx_OR_eAXIv, /* 0d */
	bx_PUSH_CS, /* 0e */
   code_escape, /***** 0f */

   bx_ADC_EbGb, /* 10 */
   bx_ADC_EvGv, /* 11 */
   bx_ADC_GbEb, /* 12 */
   bx_ADC_GvEv, /* 13 */
   bx_ADC_ALIb, /* 14 */
   bx_ADC_eAXIv, /* 15 */
   bx_PUSH_SS, /* 16 */
   bx_POP_SS, /* 17 */
   bx_SBB_EbGb, /* 18 */
   bx_SBB_EvGv, /* 19 */
   bx_SBB_GbEb, /* 1a */
   bx_SBB_GvEv, /* 1b */
   bx_SBB_ALIb, /* 1c */
   bx_SBB_eAXIv, /* 1d */
   bx_PUSH_DS, /* 1e */
   bx_POP_DS, /* 1f */           
              
   bx_AND_EbGb, /* 20 */
   bx_AND_EvGv, /* 21 */
   bx_AND_GbEb, /* 22 */
   bx_AND_GvEv, /* 23 */
   bx_AND_ALIb, /* 24 */
   bx_AND_eAXIv, /* 25 */
   es_override, /* 26: ES segment override */
   bx_DAA, /* 27 */
   bx_SUB_EbGb, /* 28 */
   bx_SUB_EvGv, /* 29 */
   bx_SUB_GbEb, /* 2a */
   bx_SUB_GvEv, /* 2b */
   bx_SUB_ALIb, /* 2c */
   bx_SUB_eAXIv, /* 2d */
   cs_override, /* 2e: CS segment override */
   bx_DAS, /* 2f */
              
   bx_XOR_EbGb, /* 30 */
   bx_XOR_EvGv, /* 31 */
   bx_XOR_GbEb, /* 32 */
   bx_XOR_GvEv, /* 33 */
   bx_XOR_ALIb, /* 34 */
   bx_XOR_eAXIv, /* 35 */
   ss_override, /* 36: SS segment override */
   bx_AAA, /* 37 */
   bx_CMP_EbGb, /* 38 */
   bx_CMP_EvGv, /* 39 */
   bx_CMP_GbEb, /* 3a */
   bx_CMP_GvEv, /* 3b */
   bx_CMP_ALIb, /* 3c */
   bx_CMP_eAXIv, /* 3d */
   ds_override, /* 3e: DS segment override */
   bx_AAS, /* 3f */
              
   bx_INC_eAX, /* 40 */
   bx_INC_eCX, /* 41 */
   bx_INC_eDX, /* 42 */
   bx_INC_eBX, /* 43 */
   bx_INC_eSP, /* 44 */
   bx_INC_eBP, /* 45 */
   bx_INC_eSI, /* 46 */
   bx_INC_eDI, /* 47 */
   bx_DEC_eAX, /* 48 */
   bx_DEC_eCX, /* 49 */
   bx_DEC_eDX, /* 4a */
   bx_DEC_eBX, /* 4b */
   bx_DEC_eSP, /* 4c */
   bx_DEC_eBP, /* 4d */
   bx_DEC_eSI, /* 4e */
   bx_DEC_eDI, /* 4f */
              
   bx_PUSH_eAX, /* 50 */
   bx_PUSH_eCX, /* 51 */
   bx_PUSH_eDX, /* 52 */
   bx_PUSH_eBX, /* 53 */
   bx_PUSH_eSP, /* 54 */
   bx_PUSH_eBP, /* 55 */
   bx_PUSH_eSI, /* 56 */
   bx_PUSH_eDI, /* 57 */
   bx_POP_eAX, /* 58 */
   bx_POP_eCX, /* 59 */
   bx_POP_eDX, /* 5a */
   bx_POP_eBX, /* 5b */
   bx_POP_eSP, /* 5c */
   bx_POP_eBP, /* 5d */
   bx_POP_eSI, /* 5e */
   bx_POP_eDI, /* 5f */
              
   bx_PUSHAD, /* 60 */
   bx_POPAD, /* 61 */
   bx_BOUND_GvMa, /* 62 */
   bx_ARPL_EwGw, /* 63 */
   fs_override, /* 64: FS segment override */
   gs_override, /* 65: GS segment override */
   opsize_prefix, /* 66: 16-32 bit operand size prefix */
   addrsize_prefix, /* 67: 16-32 bit address size prefix */
   bx_PUSH_Iv, /* 68 */
   bx_IMUL_GvEvIv, /* 69 */
   bx_PUSH_Ib, /* 6a */
   bx_IMUL_GvEvIb, /* 6b */
   insb, /* 6c: INSB instruction */
   insw, /* 6d: INSW or INSD instruction */
   outsb, /* 6e: OUTSB instruction */
   outsw, /* 6f: OUTSW or OUTSD instruction */

   bx_JO_Jb, /* 70 */
   bx_JNO_Jb, /* 71 */
   bx_JB_Jb, /* 72 */
   bx_JNB_Jb, /* 73 */
   bx_JZ_Jb, /* 74 */
   bx_JNZ_Jb, /* 75 */
   bx_JBE_Jb, /* 76 */
   bx_JNBE_Jb, /* 77 */
   bx_JS_Jb, /* 78 */
   bx_JNS_Jb, /* 79 */
   bx_JP_Jb, /* 7a */
   bx_JNP_Jb, /* 7b */
   bx_JL_Jb, /* 7c */
   bx_JNL_Jb, /* 7d */
   bx_JLE_Jb, /* 7e */
   bx_JNLE_Jb, /* 7f */

   group1EbIb, /* 80: group 1 Eb,Ib */
   group1EvIv, /* 81: group 1 Ev,Iv */
   group1EbIb, /* 82: same as 80 */
   group1EvIb, /* 83: group 1 Ev,Ib */
   bx_TEST_EbGb, /* 84 */
   bx_TEST_EvGv, /* 85 */
   bx_XCHG_EbGb, /* 86 */
   bx_XCHG_EvGv, /* 87 */
   bx_MOV_EbGb, /* 88 */
   bx_MOV_EvGv, /* 89 */
   bx_MOV_GbEb, /* 8a */
   bx_MOV_GvEv, /* 8b */
   bx_MOV_EwSw, /* 8c */
   bx_LEA_GvM, /* 8d */
   bx_MOV_SwEw, /* 8e */
   bx_POP_Ev, /* 8f */

   nop, /* 90 */
   bx_XCHG_eCXeAX, /* 91 */
   bx_XCHG_eDXeAX, /* 92 */
   bx_XCHG_eBXeAX, /* 93 */
   bx_XCHG_eSPeAX, /* 94 */
   bx_XCHG_eBPeAX, /* 95 */
   bx_XCHG_eSIeAX, /* 96 */
   bx_XCHG_eDIeAX, /* 97 */
   bx_CBW, /* 98 */
   bx_CWD, /* 99 */
   bx_CALL_Ap, /* 9a */
   bx_WAIT, /* 9b */
   bx_PUSHF_Fv, /* 9c */
   bx_POPF_Fv, /* 9d */
   bx_SAHF, /* 9e */
   bx_LAHF, /* 9f */

   bx_MOV_ALOb, /* a0 */
   bx_MOV_eAXOv, /* a1 */
   bx_MOV_ObAL, /* a2 */
   bx_MOV_OveAX, /* a3 */
   movsb, /* a4: MOVSB instruction */
   movsw, /* a5: MOVSW or MOVSD instruction */
   cmpsb, /* a6: CMPSB instruction */
   cmpsw, /* a7: CMPSW or CMPSD instruction */
   bx_TEST_ALIb, /* a8 */
   bx_TEST_eAXIv, /* a9 */
   stosb, /* aa: STOSB instruction */
   stosw, /* ab: STOSW or STOSD instruction */
   lodsb, /* ac: LODSB instruction */
   lodsw, /* ad: LODSW or LODSD instruction */
   scasb, /* ae: SCASB instruction */
   scasw, /* af: SCASW or SCASD instruction */

   bx_MOV_ALIb, /* b0 */
   bx_MOV_CLIb, /* b1 */
   bx_MOV_DLIb, /* b2 */
   bx_MOV_BLIb, /* b3 */
   bx_MOV_AHIb, /* b4 */
   bx_MOV_CHIb, /* b5 */
   bx_MOV_DHIb, /* b6 */
   bx_MOV_BHIb, /* b7 */
   bx_MOV_eAXIv, /* b8 */
   bx_MOV_eCXIv, /* b9 */
   bx_MOV_eDXIv, /* ba */
   bx_MOV_eBXIv, /* bb */
   bx_MOV_eSPIv, /* bc */
   bx_MOV_eBPIv, /* bd */
   bx_MOV_eSIIv, /* be */
   bx_MOV_eDIIv, /* bf */

   group2EbIb, /* c0: group 2 Eb,Ib */
   group2EvIb, /* c1: group 2 Ev,Ib */
   bx_RETnear_Iw, /* c2 */
   bx_RETnear, /* c3 */
   bx_LES_GvMp, /* c4 */
   bx_LDS_GvMp, /* c5 */
   bx_MOV_EbIb, /* c6 */
   bx_MOV_EvIv, /* c7 */
   bx_ENTER_IwIb, /* c8 */
   bx_LEAVE, /* c9 */
   bx_RETfar_Iw, /* ca */
   bx_RETfar, /* cb */
   bx_INT3, /* cc */
   bx_INT_Ib, /* cd */
   bx_INTO, /* ce */
   bx_IRET, /* cf */

   group2Eb1, /* d0: group 2 Eb,1 */
   group2Ev1, /* d1: group 2 Ev,1 */
   group2EbCL, /* d2: group 2 Eb,CL */
   group2EvCL, /* d3: group 2 Ev,CL */
   bx_AAM, /* d4 */
   bx_AAD, /* d5 */
   setalc, /* d6: SETALC instruction - undocumented */
   bx_XLAT, /* d7 */
   esc0, /* d8: escape to coprcessor instruction set */
   esc1, /* d9: escape to coprcessor instruction set */
   esc2, /* da: escape to coprcessor instruction set */
   esc3, /* db: escape to coprcessor instruction set */
   esc4, /* dc: escape to coprcessor instruction set */
   esc5, /* dd: escape to coprcessor instruction set */
   esc6, /* de: escape to coprcessor instruction set */
   esc7, /* df: escape to coprcessor instruction set */
   
   bx_LOOPNE_Jb, /* e0 */
   bx_LOOPE_Jb, /* e1 */
   bx_LOOP_Jb, /* e2 */
   bx_JCXZ_Jb, /* e3 */
   bx_IN_ALIb, /* e4 */
   bx_IN_eAXIb, /* e5 */
   bx_OUT_IbAL, /* e6 */
   bx_OUT_IbeAX, /* e7 */
   bx_CALL_Av, /* e8 */
   bx_JMP_Jv, /* e9 */
   bx_JMP_Ap, /* ea */
   bx_JMP_Jb, /* eb */
   bx_IN_ALDX, /* ec */
   bx_IN_eAXDX, /* ed */
   bx_OUT_DXAL, /* ee */
   bx_OUT_DXeAX, /* ef */

   lock, /* f0: LOCK prefix */
   icebp, /***** f1: ICEBP breakpoint - undocumented */
   repne, /* f2: REPNE prefix */
   rep, /* f3: REP/REPE prefix */
   bx_HLT, /* f4 */
   bx_CMC, /* f5 */
   group3Eb, /* f6: group 3 Eb */
   group3Ev, /* f7: group 3 Ev */
   bx_CLC, /* f8 */
   bx_STC, /* f9 */
   bx_CLI, /* fa */
   bx_STI, /* fb */
   bx_CLD, /* fc */
   bx_STD, /* fd */
   group4, /* fe: group 4 */
   group5, /* ff: group 5 */
};


Instr instructions_escape [0x100] = {
   group6, /* 0f 00: group 6 */
   group7, /* 0f 01: group 7 */
   bx_LAR_GvEw, /* 0f 02 */
   bx_LSL_GvEw, /* 0f 03 */
	illegal, /* 0f 04 */
	bx_LOADALL, /* 0f 05 */
	bx_CLTS, /* 0f 06 */
	loadalld, /* 0f 07: LOADALL for 386+ */
	bx_INVD, /* 0f 08 */
	bx_WBINVD, /* 0f 09 */
   illegal, /* 0f 0a */
   illegal, /* 0f 0b */
   illegal, /* 0f 0c */
   illegal, /* 0f 0d */
   illegal, /* 0f 0e */
   illegal, /* 0f 0f */

   illegal, /* 0f 10 */
   illegal, /* 0f 11 */
   illegal, /* 0f 12 */
   illegal, /* 0f 13 */
   illegal, /* 0f 14 */
   illegal, /* 0f 15 */
   illegal, /* 0f 16 */
   illegal, /* 0f 17 */
   illegal, /* 0f 18 */
   illegal, /* 0f 19 */
   illegal, /* 0f 1a */
   illegal, /* 0f 1b */
   illegal, /* 0f 1c */
   illegal, /* 0f 1d */
   illegal, /* 0f 1e */
   bx_PSEUDO_INT_Ib, /* 0f 1f: Bochs' pseudo-int */

   bx_MOV_RdCd, /* 0f 20 */
   bx_MOV_RdDd, /* 0f 21 */
   bx_MOV_CdRd, /* 0f 22 */
   bx_MOV_DdRd, /* 0f 23 */
   bx_MOV_RdTd, /* 0f 24 */
   illegal, /* 0f 25 */
   bx_MOV_TdRd, /* 0f 26 */
   illegal, /* 0f 27 */
   illegal, /* 0f 28 */
   illegal, /* 0f 29 */
   illegal, /* 0f 2a */
   illegal, /* 0f 2b */
   illegal, /* 0f 2c */
   illegal, /* 0f 2d */
   illegal, /* 0f 2e */
   illegal, /* 0f 2f */

   illegal, /* 0f 30 */
   rdtsc, /* 0f 31: read time stamp counter - Pentium undocumented */
   rdmsr, /* 0f 32: read model specific register - Pentium undocumented */
   illegal, /* 0f 33 */
   illegal, /* 0f 34 */
   illegal, /* 0f 35 */
   illegal, /* 0f 36 */
   illegal, /* 0f 37 */
   illegal, /* 0f 38 */
   illegal, /* 0f 39 */
   illegal, /* 0f 3a */
   illegal, /* 0f 3b */
   illegal, /* 0f 3c */
   illegal, /* 0f 3d */
   illegal, /* 0f 3e */
   illegal, /* 0f 3f */

   illegal, /* 0f 40 */
   illegal, /* 0f 41 */
   illegal, /* 0f 42 */
   illegal, /* 0f 43 */
   illegal, /* 0f 44 */
   illegal, /* 0f 45 */
   illegal, /* 0f 46 */
   illegal, /* 0f 47 */
   illegal, /* 0f 48 */
   illegal, /* 0f 49 */
   illegal, /* 0f 4a */
   illegal, /* 0f 4b */
   illegal, /* 0f 4c */
   illegal, /* 0f 4d */
   illegal, /* 0f 4e */
   illegal, /* 0f 4f */

   illegal, /* 0f 50 */
   illegal, /* 0f 51 */
   illegal, /* 0f 52 */
   illegal, /* 0f 53 */
   illegal, /* 0f 54 */
   illegal, /* 0f 55 */
   illegal, /* 0f 56 */
   illegal, /* 0f 57 */
   illegal, /* 0f 58 */
   illegal, /* 0f 59 */
   illegal, /* 0f 5a */
   illegal, /* 0f 5b */
   illegal, /* 0f 5c */
   illegal, /* 0f 5d */
   illegal, /* 0f 5e */
   illegal, /* 0f 5f */

   illegal, /* 0f 60 */
   illegal, /* 0f 61 */
   illegal, /* 0f 62 */
   illegal, /* 0f 63 */
   illegal, /* 0f 64 */
   illegal, /* 0f 65 */
   illegal, /* 0f 66 */
   illegal, /* 0f 67 */
   illegal, /* 0f 68 */
   illegal, /* 0f 69 */
   illegal, /* 0f 6a */
   illegal, /* 0f 6b */
   illegal, /* 0f 6c */
   illegal, /* 0f 6d */
   illegal, /* 0f 6e */
   illegal, /* 0f 6f */

   illegal, /* 0f 70 */
   illegal, /* 0f 71 */
   illegal, /* 0f 72 */
   illegal, /* 0f 73 */
   illegal, /* 0f 74 */
   illegal, /* 0f 75 */
   illegal, /* 0f 76 */
   illegal, /* 0f 77 */
   illegal, /* 0f 78 */
   illegal, /* 0f 79 */
   illegal, /* 0f 7a */
   illegal, /* 0f 7b */
   illegal, /* 0f 7c */
   illegal, /* 0f 7d */
   illegal, /* 0f 7e */
   illegal, /* 0f 7f */

   bx_JO_Jv, /* 0f 80 */
   bx_JNO_Jv, /* 0f 81 */
   bx_JB_Jv, /* 0f 82 */
   bx_JNB_Jv, /* 0f 83 */
   bx_JZ_Jv, /* 0f 84 */
   bx_JNZ_Jv, /* 0f 85 */
   bx_JBE_Jv, /* 0f 86 */
   bx_JNBE_Jv, /* 0f 87 */
   bx_JS_Jv, /* 0f 88 */
   bx_JNS_Jv, /* 0f 89 */
   bx_JP_Jv, /* 0f 8a */
   bx_JNP_Jv, /* 0f 8b */
   bx_JL_Jv, /* 0f 8c */
   bx_JNL_Jv, /* 0f 8d */
   bx_JLE_Jv, /* 0f 8e */
   bx_JNLE_Jv, /* 0f 8f */
                 
   bx_SETO_Eb, /* 0f 90 */
   bx_SETNO_Eb, /* 0f 91 */
   bx_SETB_Eb, /* 0f 92 */
   bx_SETNB_Eb, /* 0f 93 */
   bx_SETZ_Eb, /* 0f 94 */
   bx_SETNZ_Eb, /* 0f 95 */
   bx_SETBE_Eb, /* 0f 96 */
   bx_SETNBE_Eb, /* 0f 97 */
   bx_SETS_Eb, /* 0f 98 */
   bx_SETNS_Eb, /* 0f 99 */
   bx_SETP_Eb, /* 0f 9a */
   bx_SETNP_Eb, /* 0f 9b */
   bx_SETL_Eb, /* 0f 9c */
   bx_SETNL_Eb, /* 0f 9d */
   bx_SETLE_Eb, /* 0f 9e */
   bx_SETNLE_Eb, /* 0f 9f */
                 
   bx_PUSH_FS, /* 0f a0 */
   bx_POP_FS, /* 0f a1 */
   bx_CPUID, /* 0f a2 */
   bx_BT_EvGv, /* 0f a3 */
   bx_SHLD_EvGvIb, /* 0f a4 */
   bx_SHLD_EvGvCL, /* 0f a5 */
   bx_CMPXCHG_XBTS, /* 0f a6 */
   bx_CMPXCHG_IBTS, /* 0f a7 */
   bx_PUSH_GS, /* 0f a8 */
   bx_POP_GS, /* 0f a9 */
   illegal, /* 0f aa */
   bx_BTS_EvGv, /* 0f ab */
   bx_SHRD_EvGvIb, /* 0f ac */
   bx_SHRD_EvGvCL, /* 0f ad */
   illegal, /* 0f ae */
   bx_IMUL_GvEv, /* 0f af */
                 
   bx_CMPXCHG_EbGb, /* 0f b0 */
   bx_CMPXCHG_EvGv, /* 0f b1 */
   bx_LSS_GvMp, /* 0f b2 */
   bx_BTR_EvGv, /* 0f b3 */
   bx_LFS_GvMp, /* 0f b4 */
   bx_LGS_GvMp, /* 0f b5 */
   bx_MOVZX_GvEb, /* 0f b6 */
   bx_MOVZX_GvEw, /* 0f b7 */
   illegal, /* 0f b8 */
   illegal, /* 0f b9 */
   group8, /* 0f ba: group 8 */
   bx_BTC_EvGv, /* 0f bb */
   bx_BSF_GvEv, /* 0f bc */
   bx_BSR_GvEv, /* 0f bd */
   bx_MOVSX_GvEb, /* 0f be */
   bx_MOVSX_GvEw, /* 0f bf */
                
   bx_XADD_EbGb, /* 0f c0 */
   bx_XADD_EvGv, /* 0f c1 */
   illegal, /* 0f c2 */
   illegal, /* 0f c3 */
   illegal, /* 0f c4 */
   illegal, /* 0f c5 */
   illegal, /* 0f c6 */
   illegal, /* 0f c7 */
   bx_BSWAP_EAX, /* 0f c8 */
   bx_BSWAP_ECX, /* 0f c9 */
   bx_BSWAP_EDX, /* 0f ca */
   bx_BSWAP_EBX, /* 0f cb */
   bx_BSWAP_ESP, /* 0f cc */
   bx_BSWAP_EBP, /* 0f cd */
   bx_BSWAP_ESI, /* 0f ce */
   bx_BSWAP_EDI, /* 0f cf */

   illegal, /* 0f d0 */
   illegal, /* 0f d1 */
   illegal, /* 0f d2 */
   illegal, /* 0f d3 */
   illegal, /* 0f d4 */
   illegal, /* 0f d5 */
   illegal, /* 0f d6 */
   illegal, /* 0f d7 */
   illegal, /* 0f d8 */
   illegal, /* 0f d9 */
   illegal, /* 0f da */
   illegal, /* 0f db */
   illegal, /* 0f dc */
   illegal, /* 0f dd */
   illegal, /* 0f de */
   illegal, /* 0f df */

   illegal, /* 0f e0 */
   illegal, /* 0f e1 */
   illegal, /* 0f e2 */
   illegal, /* 0f e3 */
   illegal, /* 0f e4 */
   illegal, /* 0f e5 */
   illegal, /* 0f e6 */
   illegal, /* 0f e7 */
   illegal, /* 0f e8 */
   illegal, /* 0f e9 */
   illegal, /* 0f ea */
   illegal, /* 0f eb */
   illegal, /* 0f ec */
   illegal, /* 0f ed */
   illegal, /* 0f ee */
   illegal, /* 0f ef */

   illegal, /* 0f f0 */
   illegal, /* 0f f1 */
   illegal, /* 0f f2 */
   illegal, /* 0f f3 */
   illegal, /* 0f f4 */
   illegal, /* 0f f5 */
   illegal, /* 0f f6 */
   illegal, /* 0f f7 */
   illegal, /* 0f f8 */
   illegal, /* 0f f9 */
   illegal, /* 0f fa */
   illegal, /* 0f fb */
   illegal, /* 0f fc */
   illegal, /* 0f fd */
   illegal, /* 0f fe */
   illegal /* 0f ff */
};

Instr instructions_group5 [0x08] = {
	bx_INC_Ev,		// FF mod-0x00-r/m
	bx_DEC_Ev,		// FF mod-0x01-r/m
	bx_CALL_Ev,		// FF mod-0x02-r/m
	bx_CALL_Ep,		// FF mod-0x03-r/m
	bx_JMP_Ev,		// FF mod-0x04-r/m
	bx_JMP_Ep,		// FF mod-0x05-r/m
	bx_PUSH_Ev,		// FF mod-0x06-r/m
	//illegal			// FF mod-0x07-r/m
	panic_opcode	// FF mod-0x07-r/m
};

Instr watching_instructions_group5 [0x08] = {
   no_watch,
   no_watch,
   no_watch,
   CALL_Ep_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
};

Instr watching_instructions [0x100] = {

   no_watch, /* 00 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   code_escape, /* there is a separate escape code watch array */

   no_watch, /* 10 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,

   no_watch, /* 20 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,

   no_watch, /* 30 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,

   no_watch, /* 40 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,

   no_watch, /* 50 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,

   no_watch, /* 60 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   insb_watch,
   insw_watch,
   outsb_watch,
   outsw_watch,

   no_watch, /* 70 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,

   no_watch, /* 80 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,

   no_watch, /* 90 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   CALL_Ap_watch, // CALL FAR xxxx:xxxx
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,

   no_watch, /* a0 */
   no_watch,
   no_watch,
   no_watch,
   movsb_watch, /* a4 */
   movsw_watch, /* a5 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,

   no_watch, /* b0 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,

   no_watch, /* c0 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   RETfar_Iw_watch,
   RETfar_watch,
   INT3_watch,
   INT_Ib_watch,
   no_watch,
   IRET_watch,

   no_watch, /* d0 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,

   no_watch, /* e0 */
   no_watch,
   no_watch,
   no_watch,
   inalib_watch, /* e4 */
   ineaxib_watch, /* e5 */
   outibal_watch, /* e6 */
   outibeax_watch, /* e7 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   inaldx_watch, /* ec */
   ineaxdx_watch, /* ed */
   outdxal_watch, /* ee */
   outdxeax_watch, /* ef */

   no_watch, /* f0 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch

};


Instr watching_instructions_escape [0x100] = {

   no_watch, /* 00 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,

   no_watch, /* 10 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,

   no_watch, /* 20 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,

   no_watch, /* 30 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,

   no_watch, /* 40 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,

   no_watch, /* 50 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,

   no_watch, /* 60 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,

   no_watch, /* 70 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,

   no_watch, /* 80 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,

   no_watch, /* 90 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,

   no_watch, /* a0 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,

   no_watch, /* b0 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,

   no_watch, /* c0 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,

   no_watch, /* d0 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,

   no_watch, /* e0 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,

   no_watch, /* f0 */
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch,
   no_watch

};
