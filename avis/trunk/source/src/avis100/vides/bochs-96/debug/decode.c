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







#include <stdio.h>

#include "bochs.h"
#include "debug.h"




/* end prototypes */



extern Bit8u *bx_8bit_gen_reg[8];
extern Bit16u *bx_16bit_gen_reg[8];
extern Bit32u *bx_32bit_gen_reg[8];

Boolean db_32bit_opsize;
Boolean db_32bit_addrsize;
static Boolean db_rep_prefix;
static Boolean db_repne_prefix;


bx_segment_reg_t *bx_debug_seg_override;





static Bit32u debug_offset;
static bx_segment_reg_t *debug_segptr;


  void
bx_debug_decode(bx_segment_reg_t *seg, Bit32u offset)
{
  int byte_count;
  Bit8u next_byte;
  Bit8u mod_rm_byte, mod, opcode, rm;

  db_32bit_opsize = bx_cpu.cs.cache.u.segment.d_b;
  db_32bit_addrsize = bx_cpu.cs.cache.u.segment.d_b;
  bx_debug_seg_override = NULL;
  db_rep_prefix = 0;
  db_repne_prefix = 0;


  debug_offset = offset;
  debug_segptr = seg;

  /* On the 386 and above, instructions must be a maximum of 15 bytes long.
   * this means redundant prefix codes can put the byte count over 15 and
   * cause an illegal instruction.
   */
  for (byte_count=0; byte_count<15; byte_count++) {
    next_byte = bx_debug_next_byte(); 
    switch (next_byte) {

      case 0x00: bx_printf("add_ebgb "); bx_debug_EbGb(); return;
      case 0x01: bx_printf("add_evgv "); bx_debug_EvGv(); return;
      case 0x02: bx_printf("add_gbeb "); bx_debug_GbEb(); return;
      case 0x03: bx_printf("add_gvev "); bx_debug_GvEv(); return;
      case 0x04: bx_printf("add_alib "); bx_debug_ALIb(); return;
      case 0x05: bx_printf("add_eaxiv "); bx_debug_eAXIv(); return;
      case 0x06: bx_printf("push_es "); bx_debug_ES(); return;
      case 0x07: bx_printf("pop_es "); bx_debug_ES(); return;
      case 0x08: bx_printf("or_ebgb "); bx_debug_EbGb(); return;
      case 0x09: bx_printf("or_evgv "); bx_debug_EvGv(); return;
      case 0x0A: bx_printf("or_gbeb "); bx_debug_GbEb(); return;
      case 0x0B: bx_printf("or_gvev "); bx_debug_GvEv(); return;
      case 0x0C: bx_printf("or_alib "); bx_debug_ALIb(); return;
      case 0x0D: bx_printf("or_eaxiv "); bx_debug_eAXIv(); return;
      case 0x0E: bx_printf("push_cs "); bx_debug_CS(); return;
      case 0x0F: /* 2-byte escape */
        next_byte = bx_debug_next_byte(); 
        if ( ++byte_count >= 15 ) {
          debug_invalid_opcode();
          return;
	  }
        switch (next_byte) {
          case 0x00: /* Group 6 */
            mod_rm_byte = bx_debug_peek_next_byte();
            BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
            switch (opcode) {
              case 0x00: bx_printf("sldt_ew "); bx_debug_Ew(); return;
              case 0x01: bx_printf("str_ew "); bx_debug_Ew(); return;
              case 0x02: bx_printf("lldt_ew "); bx_debug_Ew(); return;
              case 0x03: bx_printf("ltr_ew "); bx_debug_Ew(); return;
              case 0x04: bx_printf("verr_ew "); bx_debug_Ew(); return;
              case 0x05: bx_printf("verw_ew "); bx_debug_Ew(); return;
              case 0x06: debug_invalid_opcode(); return;
              case 0x07: debug_invalid_opcode(); return;
              default: bx_panic("debugger: invalid opcode\n"); return;
              }
            return;
          case 0x01: /* Group 7 */
            mod_rm_byte = bx_debug_peek_next_byte();
            BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
            switch (opcode) {
              case 0x00: bx_printf("sgdt_ms "); bx_debug_Ms(); return;
              case 0x01: bx_printf("sidt_ms "); bx_debug_Ms(); return;
              case 0x02: bx_printf("lgdt_ms "); bx_debug_Ms(); return;
              case 0x03: bx_printf("lidt_ms "); bx_debug_Ms(); return;
              case 0x04: bx_printf("smsw_ew "); bx_debug_Ew(); return;
              case 0x05: debug_invalid_opcode(); return;
              case 0x06: bx_printf("lmsw_ew "); bx_debug_Ew(); return;
              case 0x07: debug_invalid_opcode(); return;
              default: bx_panic("debugger: invalid opcode\n"); return;
              }
            return;

          case 0x02: bx_printf("lar_gvew "); bx_debug_GvEw(); return;
          case 0x03: bx_printf("lsl_gvew "); bx_debug_GvEw(); return;
          case 0x04: debug_invalid_opcode(); return;
          case 0x05: debug_invalid_opcode(); return;
          case 0x06: bx_printf("clts "); return;
          case 0x07: debug_invalid_opcode(); return;
          case 0x08: bx_printf("invd "); return;
          case 0x09: bx_printf("wbinvd "); return;
	  case 0x0A:
	  case 0x0B:
	  case 0x0C:
	  case 0x0D:
	  case 0x0E:
	  case 0x0F: debug_invalid_opcode(); return;
  
          case 0x10:
	  case 0x11:
	  case 0x12:
	  case 0x13:
	  case 0x14:
	  case 0x15:
	  case 0x16:
	  case 0x17:
	  case 0x18:
	  case 0x19:
	  case 0x1A:
	  case 0x1B:
	  case 0x1C:
	  case 0x1D:
	  case 0x1E: debug_invalid_opcode(); return;
	  case 0x1F:
            bx_printf("pseudo-int ");
            bx_debug_Ib();
            return;

          case 0x20: bx_printf("mov_rdcd "); bx_debug_RdCd(); return;
          case 0x21: bx_printf("mov_rddd "); bx_debug_RdDd(); return;
          case 0x22: bx_printf("mov_cdrd "); bx_debug_CdRd(); return;
          case 0x23: bx_printf("mov_ddrd "); bx_debug_DdRd(); return;
          case 0x24: bx_printf("mov_rdtd "); bx_debug_RdTd(); return;
          case 0x25: debug_invalid_opcode(); return;
          case 0x26: bx_printf("mov_tdrd "); bx_debug_TdRd(); return;
          case 0x27:
          case 0x28:
          case 0x29:
          case 0x2A:
          case 0x2B:
          case 0x2C:
          case 0x2D:
          case 0x2E:
          case 0x2F: debug_invalid_opcode(); return;

          case 0x30:
	  case 0x31:
	  case 0x32:
	  case 0x33:
	  case 0x34:
	  case 0x35:
	  case 0x36:
	  case 0x37:
	  case 0x38:
	  case 0x39:
	  case 0x3A:
	  case 0x3B:
	  case 0x3C:
	  case 0x3D:
	  case 0x3E:
	  case 0x3F: debug_invalid_opcode(); return;

          case 0x40:
	  case 0x41:
	  case 0x42:
	  case 0x43:
	  case 0x44:
	  case 0x45:
	  case 0x46:
	  case 0x47:
	  case 0x48:
	  case 0x49:
	  case 0x4A:
	  case 0x4B:
	  case 0x4C:
	  case 0x4D:
	  case 0x4E:
	  case 0x4F: debug_invalid_opcode(); return;

          case 0x50:
	  case 0x51:
	  case 0x52:
	  case 0x53:
	  case 0x54:
	  case 0x55:
	  case 0x56:
	  case 0x57:
	  case 0x58:
	  case 0x59:
	  case 0x5A:
	  case 0x5B:
	  case 0x5C:
	  case 0x5D:
	  case 0x5E:
	  case 0x5F: debug_invalid_opcode(); return;

          case 0x60:
	  case 0x61:
	  case 0x62:
	  case 0x63:
	  case 0x64:
	  case 0x65:
	  case 0x66:
	  case 0x67:
	  case 0x68:
	  case 0x69:
	  case 0x6A:
	  case 0x6B:
	  case 0x6C:
	  case 0x6D:
	  case 0x6E:
	  case 0x6F: debug_invalid_opcode(); return;

          case 0x70:
	  case 0x71:
	  case 0x72:
	  case 0x73:
	  case 0x74:
	  case 0x75:
	  case 0x76:
	  case 0x77:
	  case 0x78:
	  case 0x79:
	  case 0x7A:
	  case 0x7B:
	  case 0x7C:
	  case 0x7D:
	  case 0x7E:
	  case 0x7F: debug_invalid_opcode(); return;

          case 0x80: bx_printf("jo_jv "); bx_debug_Jv(); return;
          case 0x81: bx_printf("jno_jv "); bx_debug_Jv(); return;
          case 0x82: bx_printf("jb_jv "); bx_debug_Jv(); return;
          case 0x83: bx_printf("jnb_jv "); bx_debug_Jv(); return;
          case 0x84: bx_printf("jz_jv "); bx_debug_Jv(); return;
          case 0x85: bx_printf("jnz_jv "); bx_debug_Jv(); return;
          case 0x86: bx_printf("jbe_jv "); bx_debug_Jv(); return;
          case 0x87: bx_printf("jnbe_jv "); bx_debug_Jv(); return;
          case 0x88: bx_printf("js_jv "); bx_debug_Jv(); return;
          case 0x89: bx_printf("jns_jv "); bx_debug_Jv(); return;
          case 0x8A: bx_printf("jp_jv "); bx_debug_Jv(); return;
          case 0x8B: bx_printf("jnp_jv "); bx_debug_Jv(); return;
          case 0x8C: bx_printf("jl_jv "); bx_debug_Jv(); return;
          case 0x8D: bx_printf("jnl_jv "); bx_debug_Jv(); return;
          case 0x8E: bx_printf("jle_jv "); bx_debug_Jv(); return;
          case 0x8F: bx_printf("jnle_jv "); bx_debug_Jv(); return;

          case 0x90: bx_printf("seto_eb "); bx_debug_Eb(); return;
          case 0x91: bx_printf("setno_eb "); bx_debug_Eb(); return;
          case 0x92: bx_printf("setb_eb "); bx_debug_Eb(); return;
          case 0x93: bx_printf("setnb_eb "); bx_debug_Eb(); return;
          case 0x94: bx_printf("setz_eb "); bx_debug_Eb(); return;
          case 0x95: bx_printf("setnz_eb "); bx_debug_Eb(); return;
          case 0x96: bx_printf("setbe_eb "); bx_debug_Eb(); return;
          case 0x97: bx_printf("setnbe_eb "); bx_debug_Eb(); return;
          case 0x98: bx_printf("sets_eb "); bx_debug_Eb(); return;
          case 0x99: bx_printf("setns_eb "); bx_debug_Eb(); return;
          case 0x9A: bx_printf("setp_eb "); bx_debug_Eb(); return;
          case 0x9B: bx_printf("setnp_eb "); bx_debug_Eb(); return;
          case 0x9C: bx_printf("setl_eb "); bx_debug_Eb(); return;
          case 0x9D: bx_printf("setnl_eb "); bx_debug_Eb(); return;
          case 0x9E: bx_printf("setle_eb "); bx_debug_Eb(); return;
          case 0x9F: bx_printf("setnle_eb "); bx_debug_Eb(); return;

          case 0xA0: bx_printf("push_fs "); bx_debug_FS(); return;
          case 0xA1: bx_printf("pop_fs "); bx_debug_FS(); return;
          case 0xA2: debug_invalid_opcode(); return;
          case 0xA3: bx_printf("bt_evgv "); bx_debug_EvGv(); return;
          case 0xA4: bx_printf("shld_evgvib "); bx_debug_EvGvIb(); return;
          case 0xA5: bx_printf("shld_evgvcl "); bx_debug_EvGvCL(); return;
          case 0xA6: bx_printf("cmpxchg_xbts "); bx_debug_XBTS(); return;
          case 0xA7: bx_printf("cmpxchg_ibts "); bx_debug_IBTS(); return;
          case 0xA8: bx_printf("push_gs "); bx_debug_GS(); return;
          case 0xA9: bx_printf("pop_gs "); bx_debug_GS(); return;
          case 0xAA: debug_invalid_opcode(); return;
          case 0xAB: bx_printf("bts_evgv "); bx_debug_EvGv(); return;
          case 0xAC: bx_printf("shrd_evgvib "); bx_debug_EvGvIb(); return;
          case 0xAD: bx_printf("shrd_evgvcl "); bx_debug_EvGvCL(); return;
          case 0xAE: debug_invalid_opcode(); return;
          case 0xAF: bx_printf("imul_gvev "); bx_debug_GvEv(); return;

          case 0xB0: bx_printf("cmpxchg_ebgb "); bx_debug_EbGb(); return;
          case 0xB1: bx_printf("cmpxchg_evgv "); bx_debug_EvGv(); return;
          case 0xB2: bx_printf("lss_mp "); bx_debug_Mp(); return;
          case 0xB3: bx_printf("btr_evgv "); bx_debug_EvGv(); return;
          case 0xB4: bx_printf("lfs_mp "); bx_debug_Mp(); return;
          case 0xB5: bx_printf("lgs_mp "); bx_debug_Mp(); return;
          case 0xB6: bx_printf("movzx_gveb "); bx_debug_GvEb(); return;
          case 0xB7: bx_printf("movzx_gvew "); bx_debug_GvEw(); return;
          case 0xB8: debug_invalid_opcode(); return;
          case 0xB9: debug_invalid_opcode(); return;
          case 0xBA: /* Group 8 Ev,Ib */
            mod_rm_byte = bx_debug_peek_next_byte();
            BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
            switch (opcode) {
              case 0x00:
              case 0x01:
              case 0x02:
              case 0x03: debug_invalid_opcode(); return;
              case 0x04: bx_printf("bt_evib "); bx_debug_EvIb(); return;
              case 0x05: bx_printf("bts_evib "); bx_debug_EvIb(); return;
              case 0x06: bx_printf("btr_evib "); bx_debug_EvIb(); return;
              case 0x07: bx_printf("btc_evib "); bx_debug_EvIb(); return;
              default: bx_panic("debugger: invalid opcode\n"); return;
              }
            return;

          case 0xBB: bx_printf("btc_evgv "); bx_debug_EvGv(); return;
          case 0xBC: bx_printf("bsf_gvev "); bx_debug_GvEv(); return;
          case 0xBD: bx_printf("bsr_gvev "); bx_debug_GvEv(); return;
          case 0xBE: bx_printf("movsx_gveb "); bx_debug_GvEb(); return;
          case 0xBF: bx_printf("movsx_gvew "); bx_debug_GvEw(); return;

          case 0xC0: bx_printf("xadd_ebgb "); bx_debug_EbGb(); return;
          case 0xC1: bx_printf("xadd_evgv "); bx_debug_EvGv(); return;
          case 0xC2: 
          case 0xC3: 
          case 0xC4: 
          case 0xC5: 
          case 0xC6: 
          case 0xC7: debug_invalid_opcode(); return;
          case 0xC8: bx_printf("bswap_eax "); bx_debug_eAX(); return;
          case 0xC9: bx_printf("bswap_ecx "); bx_debug_eCX(); return;
          case 0xCA: bx_printf("bswap_edx "); bx_debug_eDX(); return;
          case 0xCB: bx_printf("bswap_ebx "); bx_debug_eBX(); return;
          case 0xCC: bx_printf("bswap_esp "); bx_debug_eSP(); return;
          case 0xCD: bx_printf("bswap_ebp "); bx_debug_eBP(); return;
          case 0xCE: bx_printf("bswap_esi "); bx_debug_eSI(); return;
          case 0xCF: bx_printf("bswap_edi "); bx_debug_eDI(); return;
       
          case 0xD0:
	  case 0xD1:
	  case 0xD2:
	  case 0xD3:
	  case 0xD4:
	  case 0xD5:
	  case 0xD6:
	  case 0xD7:
	  case 0xD8:
	  case 0xD9:
	  case 0xDA:
	  case 0xDB:
	  case 0xDC:
	  case 0xDD:
	  case 0xDE:
	  case 0xDF: debug_invalid_opcode(); return;

          case 0xE0:
	  case 0xE1:
	  case 0xE2:
	  case 0xE3:
	  case 0xE4:
	  case 0xE5:
	  case 0xE6:
	  case 0xE7:
	  case 0xE8:
	  case 0xE9:
	  case 0xEA:
	  case 0xEB:
	  case 0xEC:
	  case 0xED:
	  case 0xEE:
	  case 0xEF: debug_invalid_opcode(); return;

          case 0xF0:
	  case 0xF1:
	  case 0xF2:
	  case 0xF3:
	  case 0xF4:
	  case 0xF5:
	  case 0xF6:
	  case 0xF7:
	  case 0xF8:
	  case 0xF9:
	  case 0xFA:
	  case 0xFB:
	  case 0xFC:
	  case 0xFD:
	  case 0xFE:
	  case 0xFF: debug_invalid_opcode(); return;

          default: bx_panic("debugger: invalid opcode\n"); return;
          }
        return;

      case 0x10: bx_printf("adc_ebgb "); bx_debug_EbGb(); return;
      case 0x11: bx_printf("adc_evgv "); bx_debug_EvGv(); return;
      case 0x12: bx_printf("adc_gbeb "); bx_debug_GbEb(); return;
      case 0x13: bx_printf("adc_gvev "); bx_debug_GvEv(); return;
      case 0x14: bx_printf("adc_alib "); bx_debug_ALIb(); return;
      case 0x15: bx_printf("adc_eaxiv "); bx_debug_eAXIv(); return;
      case 0x16: bx_printf("push_ss "); bx_debug_SS(); return;
      case 0x17: bx_printf("pop_ss "); bx_debug_SS(); return;
      case 0x18: bx_printf("sbb_ebgb "); bx_debug_EbGb(); return;
      case 0x19: bx_printf("sbb_evgv "); bx_debug_EvGv(); return;
      case 0x1A: bx_printf("sbb_gbeb "); bx_debug_GbEb(); return;
      case 0x1B: bx_printf("sbb_gvev "); bx_debug_GvEv(); return;
      case 0x1C: bx_printf("sbb_alib "); bx_debug_ALIb(); return;
      case 0x1D: bx_printf("sbb_eaxiv "); bx_debug_eAXIv(); return;
      case 0x1E: bx_printf("push_ds "); bx_debug_DS(); return;
      case 0x1F: bx_printf("pop_ds "); bx_debug_DS(); return;


      case 0x20: bx_printf("and_ebgb "); bx_debug_EbGb(); return;
      case 0x21: bx_printf("and_evgv "); bx_debug_EvGv(); return;
      case 0x22: bx_printf("and_gveb "); bx_debug_GbEb(); return;
      case 0x23: bx_printf("and_gvev "); bx_debug_GvEv(); return;
      case 0x24: bx_printf("and_alib "); bx_debug_ALIb(); return;
      case 0x25: bx_printf("and_eaxiv "); bx_debug_eAXIv(); return;
      case 0x26: 
        bx_debug_seg_override = &bx_cpu.es;
        break;
      case 0x27: bx_printf("daa "); return;
      case 0x28: bx_printf("sub_ebgb "); bx_debug_EbGb(); return;
      case 0x29: bx_printf("sub_evgv "); bx_debug_EvGv(); return;
      case 0x2A: bx_printf("sub_gbeb "); bx_debug_GbEb(); return;
      case 0x2B: bx_printf("sub_gvev "); bx_debug_GvEv(); return;
      case 0x2C: bx_printf("sub_alib "); bx_debug_ALIb(); return;
      case 0x2D: bx_printf("sub_eaxiv "); bx_debug_eAXIv(); return;
      case 0x2E:
        bx_debug_seg_override = &bx_cpu.cs;
        break;
      case 0x2F: bx_printf("das "); return;

      case 0x30: bx_printf("xor_ebgb "); bx_debug_EbGb(); return;
      case 0x31: bx_printf("xor_evgv "); bx_debug_EvGv(); return;
      case 0x32: bx_printf("xor_gbeb "); bx_debug_GbEb(); return;
      case 0x33: bx_printf("xor_gvev "); bx_debug_GvEv(); return;
      case 0x34: bx_printf("xor_alib "); bx_debug_ALIb(); return;
      case 0x35: bx_printf("xor_eaxiv "); bx_debug_eAXIv(); return;
      case 0x36: 
        bx_debug_seg_override = &bx_cpu.ss;
        break;
      case 0x37: bx_printf("aaa "); return;
      case 0x38: bx_printf("cmp_ebgb "); bx_debug_EbGb(); return;
      case 0x39: bx_printf("cmp_evgv "); bx_debug_EvGv(); return;
      case 0x3A: bx_printf("cmp_gbeb "); bx_debug_GbEb(); return;
      case 0x3B: bx_printf("cmp_gvev "); bx_debug_GvEv(); return;
      case 0x3C: bx_printf("cmp_alib "); bx_debug_ALIb(); return;
      case 0x3D: bx_printf("cmp_eaxiv "); bx_debug_eAXIv(); return;
      case 0x3E: 
        bx_debug_seg_override = &bx_cpu.ds;
        break;
      case 0x3F: bx_printf("aas "); return;

      case 0x40: bx_printf("inc_eax "); bx_debug_eAX(); return;
      case 0x41: bx_printf("inc_ecx "); bx_debug_eCX(); return;
      case 0x42: bx_printf("inc_edx "); bx_debug_eDX(); return;
      case 0x43: bx_printf("inc_ebx "); bx_debug_eBX(); return;
      case 0x44: bx_printf("inc_esp "); bx_debug_eSP(); return;
      case 0x45: bx_printf("inc_ebp "); bx_debug_eBP(); return;
      case 0x46: bx_printf("inc_esi "); bx_debug_eSI(); return;
      case 0x47: bx_printf("inc_edi "); bx_debug_eDI(); return;
      case 0x48: bx_printf("dec_eax "); bx_debug_eAX(); return;
      case 0x49: bx_printf("dec_ecx "); bx_debug_eCX(); return;
      case 0x4A: bx_printf("dec_edx "); bx_debug_eDX(); return;
      case 0x4B: bx_printf("dec_ebx "); bx_debug_eBX(); return;
      case 0x4C: bx_printf("dec_esp "); bx_debug_eSP(); return;
      case 0x4D: bx_printf("dec_ebp "); bx_debug_eBP(); return;
      case 0x4E: bx_printf("dec_esi "); bx_debug_eSI(); return;
      case 0x4F: bx_printf("dec_edi "); bx_debug_eDI(); return;
 

      case 0x50: bx_printf("push_eax "); bx_debug_eAX(); return;
      case 0x51: bx_printf("push_ecx "); bx_debug_eCX(); return;
      case 0x52: bx_printf("push_edx "); bx_debug_eDX(); return;
      case 0x53: bx_printf("push_ebx "); bx_debug_eBX(); return;
      case 0x54: bx_printf("push_esp "); bx_debug_eSP(); return;
      case 0x55: bx_printf("push_ebp "); bx_debug_eBP(); return;
      case 0x56: bx_printf("push_esi "); bx_debug_eSI(); return;
      case 0x57: bx_printf("push_edi "); bx_debug_eDI(); return;
      case 0x58: bx_printf("pop_eax "); bx_debug_eAX(); return;
      case 0x59: bx_printf("pop_ecx "); bx_debug_eCX(); return;
      case 0x5A: bx_printf("pop_edx "); bx_debug_eDX(); return;
      case 0x5B: bx_printf("pop_ebx "); bx_debug_eBX(); return;
      case 0x5C: bx_printf("pop_esp "); bx_debug_eSP(); return;
      case 0x5D: bx_printf("pop_ebp "); bx_debug_eBP(); return;
      case 0x5E: bx_printf("pop_esi "); bx_debug_eSI(); return;
      case 0x5F: bx_printf("pop_edi "); bx_debug_eDI(); return;


      case 0x60: bx_printf("pushad "); return;
      case 0x61: bx_printf("popad "); return;
      case 0x62: bx_printf("bound_gvma "); bx_debug_GvMa(); return;
      case 0x63: bx_printf("arpl_ewrw "); bx_debug_EwRw(); return;
      case 0x64: 
        bx_debug_seg_override = &bx_cpu.fs;
        break;
      case 0x65: 
        bx_debug_seg_override = &bx_cpu.gs;
        break;
      case 0x66:
        db_32bit_opsize = !db_32bit_opsize;
        break;
      case 0x67:
        db_32bit_addrsize = !db_32bit_addrsize;
        break;
      case 0x68: bx_printf("push_iv "); bx_debug_Iv(); return;
      case 0x69: bx_printf("imul_gveviv "); bx_debug_GvEvIv(); return;
      case 0x6A: bx_printf("push_ib "); bx_debug_Ib(); return;
      case 0x6B: bx_printf("imul_gvevib "); bx_debug_GvEvIb(); return;
      case 0x6C: bx_printf("insb_ybdx "); bx_debug_YbDX(); return;
      case 0x6D: bx_printf("insw_yvdx "); bx_debug_YvDX(); return;
      case 0x6E: bx_printf("outsb_dxxb "); bx_debug_DXXb(); return;
      case 0x6F: bx_printf("outsw_dxxv "); bx_debug_DXXv(); return;


      case 0x70: bx_printf("jo_jb "); bx_debug_Jb(); return;
      case 0x71: bx_printf("jno_jb "); bx_debug_Jb(); return;
      case 0x72: bx_printf("jb_jb "); bx_debug_Jb(); return;
      case 0x73: bx_printf("jnb_jb "); bx_debug_Jb(); return;
      case 0x74: bx_printf("jz_jb "); bx_debug_Jb(); return;
      case 0x75: bx_printf("jnz_jb "); bx_debug_Jb(); return;
      case 0x76: bx_printf("jbe_jb "); bx_debug_Jb(); return;
      case 0x77: bx_printf("jnbe_jb "); bx_debug_Jb(); return;
      case 0x78: bx_printf("js_jb "); bx_debug_Jb(); return;
      case 0x79: bx_printf("jns_jb "); bx_debug_Jb(); return;
      case 0x7A: bx_printf("jp_jb "); bx_debug_Jb(); return;
      case 0x7B: bx_printf("jnp_jb "); bx_debug_Jb(); return;
      case 0x7C: bx_printf("jl_jb "); bx_debug_Jb(); return;
      case 0x7D: bx_printf("jnl_jb "); bx_debug_Jb(); return;
      case 0x7E: bx_printf("jle_jb "); bx_debug_Jb(); return;
      case 0x7F: bx_printf("jnle_jb "); bx_debug_Jb(); return;

      case 0x80: /* Immdediate Grp 1 EbIb */
        mod_rm_byte = bx_debug_peek_next_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        switch (opcode) {
          case 0x00: bx_printf("add_ebib "); bx_debug_EbIb(); return;
          case 0x01: bx_printf("or_ebib "); bx_debug_EbIb();  return;
          case 0x02: bx_printf("adc_ebib "); bx_debug_EbIb(); return;
          case 0x03: bx_printf("sbb_ebib "); bx_debug_EbIb(); return;
          case 0x04: bx_printf("and_ebib "); bx_debug_EbIb(); return;
          case 0x05: bx_printf("sub_ebib "); bx_debug_EbIb(); return;
          case 0x06: bx_printf("xor_ebib "); bx_debug_EbIb(); return;
          case 0x07: bx_printf("cmp_ebib "); bx_debug_EbIb(); return;
          default: bx_panic("debugger: invalid opcode\n"); return;
          }
        return;
        break;

      case 0x81: /* Immdediate Grp 1 EvIv */
        mod_rm_byte = bx_debug_peek_next_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        switch (opcode) {
          case 0x00: bx_printf("add_eviv "); bx_debug_EvIv(); return;
          case 0x01: bx_printf("or_eviv "); bx_debug_EvIv(); return;
          case 0x02: bx_printf("adc_eviv "); bx_debug_EvIv(); return;
          case 0x03: bx_printf("sbb_eviv "); bx_debug_EvIv(); return;
          case 0x04: bx_printf("and_eviv "); bx_debug_EvIv(); return;
          case 0x05: bx_printf("sub_eviv "); bx_debug_EvIv(); return;
          case 0x06: bx_printf("xor_eviv "); bx_debug_EvIv(); return;
          case 0x07: bx_printf("cmp_eviv "); bx_debug_EvIv(); return;
          default: bx_panic("debugger: invalid opcode\n"); return;
          }
        return;

      case 0x82: debug_invalid_opcode(); return;

      case 0x83: /* Immdediate Grp 1 EvIb */
        mod_rm_byte = bx_debug_peek_next_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        switch (opcode) {
          case 0x00: bx_printf("add_evib "); bx_debug_EvIb(); return;
          case 0x01: bx_printf("or_evib "); bx_debug_EvIb(); return;
          case 0x02: bx_printf("adc_evib "); bx_debug_EvIb(); return;
          case 0x03: bx_printf("sbb_evib "); bx_debug_EvIb(); return;
          case 0x04: bx_printf("and_evib "); bx_debug_EvIb(); return;
          case 0x05: bx_printf("sub_evib "); bx_debug_EvIb(); return;
          case 0x06: bx_printf("xor_evib "); bx_debug_EvIb(); return;
          case 0x07: bx_printf("cmp_evib "); bx_debug_EvIb(); return;
          default: bx_panic("debugger: invalid opcode\n"); return;
          }
        return;

      case 0x84: bx_printf("test_ebgb "); bx_debug_EbGb(); return;
      case 0x85: bx_printf("test_evgv "); bx_debug_EvGv(); return;
      case 0x86: bx_printf("xchg_ebgb "); bx_debug_EbGb(); return;
      case 0x87: bx_printf("xchg_evgv "); bx_debug_EvGv(); return;
      case 0x88: bx_printf("mov_ebgb "); bx_debug_EbGb(); return;
      case 0x89: bx_printf("mov_evgv "); bx_debug_EvGv(); return;
      case 0x8A: bx_printf("mov_gbeb "); bx_debug_GbEb(); return;
      case 0x8B: bx_printf("mov_gvev "); bx_debug_GvEv(); return;
      case 0x8C: bx_printf("mov_ewsw "); bx_debug_EwSw(); return;
      case 0x8D: bx_printf("lea_gvm "); bx_debug_GvM(); return;
      case 0x8E: bx_printf("mov_swew "); bx_debug_SwEw(); return;
      case 0x8F: bx_printf("pop_ev "); bx_debug_Ev(); return;
 

      case 0x90: /* NOP */ return;
      case 0x91: bx_printf("xchg_ecxeax "); bx_debug_eCXeAX(); return;
      case 0x92: bx_printf("xchg_edxeax "); bx_debug_eDXeAX(); return;
      case 0x93: bx_printf("xchg_ebxeax "); bx_debug_eBXeAX(); return;
      case 0x94: bx_printf("xchg_espeax "); bx_debug_eSPeAX(); return;
      case 0x95: bx_printf("xchg_ebpeax "); bx_debug_eBPeAX(); return;
      case 0x96: bx_printf("xchg_esieax "); bx_debug_eSIeAX(); return;
      case 0x97: bx_printf("xchg_edieax "); bx_debug_eDIeAX(); return;
      case 0x98: bx_printf("cbw "); return;
      case 0x99: bx_printf("cwd "); return;
      case 0x9A: bx_printf("call_ap "); bx_debug_Ap(); return;
      case 0x9B: bx_printf("wait "); return;
      case 0x9C: bx_printf("pushf_fv "); bx_debug_Fv(); return;
      case 0x9D: bx_printf("popf_fv "); bx_debug_Fv(); return;
      case 0x9E: bx_printf("sahf "); return;
      case 0x9F: bx_printf("lahf "); return;
 

      case 0xA0: bx_printf("mov_alob "); bx_debug_ALOb(); return;
      case 0xA1: bx_printf("mov_eaxov "); bx_debug_eAXOv(); return;
      case 0xA2: bx_printf("mov_obal "); bx_debug_ObAL(); return;
      case 0xA3: bx_printf("mov_oveax "); bx_debug_OveAX(); return;
      case 0xA4: bx_printf("movsb_xbyb "); bx_debug_XbYb(); return;
      case 0xA5: bx_printf("movsw_xvyv "); bx_debug_XvYv(); return;
      case 0xA6: bx_printf("cmpsb_xbyb "); bx_debug_XbYb(); return;
      case 0xA7: bx_printf("cmpsw_xvyv "); bx_debug_XvYv(); return;
      case 0xA8: bx_printf("test_alib "); bx_debug_ALIb(); return;
      case 0xA9: bx_printf("test_eaxiv "); bx_debug_eAXIv(); return;
      case 0xAA: bx_printf("stosb_ybal "); bx_debug_YbAL(); return;
      case 0xAB: bx_printf("stosw_yveax "); bx_debug_YveAX(); return;
      case 0xAC: bx_printf("lodsb_alxb "); bx_debug_ALXb(); return;
      case 0xAD: bx_printf("lodsw_eaxxv "); bx_debug_eAXXv(); return;
      case 0xAE: bx_printf("scasb_alxb "); bx_debug_ALXb(); return;
      case 0xAF: bx_printf("scasw_eaxxv "); bx_debug_eAXXv(); return;
 

      case 0xB0: bx_printf("mov_alib "); bx_debug_ALIb(); return;
      case 0xB1: bx_printf("mov_clib "); bx_debug_CLIb(); return;
      case 0xB2: bx_printf("mov_dlib "); bx_debug_DLIb(); return;
      case 0xB3: bx_printf("mov_blib "); bx_debug_BLIb(); return;
      case 0xB4: bx_printf("mov_ahib "); bx_debug_AHIb(); return;
      case 0xB5: bx_printf("mov_chib "); bx_debug_CHIb(); return;
      case 0xB6: bx_printf("mov_dhib "); bx_debug_DHIb(); return;
      case 0xB7: bx_printf("mov_bhib "); bx_debug_BHIb(); return;
      case 0xB8: bx_printf("mov_eaxiv "); bx_debug_eAXIv(); return;
      case 0xB9: bx_printf("mov_ecxiv "); bx_debug_eCXIv(); return;
      case 0xBA: bx_printf("mov_edxiv "); bx_debug_eDXIv(); return;
      case 0xBB: bx_printf("mov_ebxiv "); bx_debug_eBXIv(); return;
      case 0xBC: bx_printf("mov_espiv "); bx_debug_eSPIv(); return;
      case 0xBD: bx_printf("mov_ebpiv "); bx_debug_eBPIv(); return;
      case 0xBE: bx_printf("mov_esiiv "); bx_debug_eSIIv(); return;
      case 0xBF: bx_printf("mov_ediiv "); bx_debug_eDIIv(); return;
 
      case 0xC0: /* Group 2 Eb,Ib */
        mod_rm_byte = bx_debug_peek_next_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        switch (opcode) {
          case 0x00: bx_printf("rol_ebib "); bx_debug_EbIb(); return;
          case 0x01: bx_printf("ror_ebib "); bx_debug_EbIb(); return;
          case 0x02: bx_printf("rcl_ebib "); bx_debug_EbIb(); return;
          case 0x03: bx_printf("rcr_ebib "); bx_debug_EbIb(); return;
          case 0x04: bx_printf("shl_ebib "); bx_debug_EbIb(); return;
          case 0x05: bx_printf("shr_ebib "); bx_debug_EbIb(); return;
          case 0x06: debug_invalid_opcode(); return;
          case 0x07: bx_printf("sar_ebib "); bx_debug_EbIb(); return;
          default: bx_panic("debugger: invalid opcode\n"); return;
          }
        return;

      case 0xC1: /* Group 2 Ev,Ib */
        mod_rm_byte = bx_debug_peek_next_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        switch (opcode) {
          case 0x00: bx_printf("rol_evib "); bx_debug_EvIb(); return;
          case 0x01: bx_printf("ror_evib "); bx_debug_EvIb(); return;
          case 0x02: bx_printf("rcl_evib "); bx_debug_EvIb(); return;
          case 0x03: bx_printf("rcr_evib "); bx_debug_EvIb(); return;
          case 0x04: bx_printf("shl_evib "); bx_debug_EvIb(); return;
          case 0x05: bx_printf("shr_evib "); bx_debug_EvIb(); return;
          case 0x06: debug_invalid_opcode(); return;
          case 0x07: bx_printf("sar_evib "); bx_debug_EvIb(); return;
          default: bx_panic("debugger: invalid opcode\n"); return;
          }
        return;

      case 0xC2: bx_printf("ret_neariw "); bx_debug_Iw(); return;
      case 0xC3: bx_printf("ret_near "); return;
      case 0xC4: bx_printf("les_gvmp "); bx_debug_GvMp(); return;
      case 0xC5: bx_printf("lds_gvmp "); bx_debug_GvMp(); return;
      case 0xC6: bx_printf("mov_ebib "); bx_debug_EbIb(); return;
      case 0xC7: bx_printf("mov_eviv "); bx_debug_EvIv(); return;
      case 0xC8: bx_printf("enter_iwib "); bx_debug_IwIb(); return;
      case 0xC9: bx_printf("leave "); return;
      case 0xCA: bx_printf("ret_fariw "); bx_debug_Iw(); return;
      case 0xCB: bx_printf("ret_far "); return;
      case 0xCC: bx_printf("int_3 "); return;
      case 0xCD: bx_printf("int_ib "); bx_debug_Ib(); return;
      case 0xCE: bx_printf("into "); return;
      case 0xCF: bx_printf("iret "); return;
 

      case 0xD0: /* Group 2 Eb,1 */
        mod_rm_byte = bx_debug_peek_next_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        switch (opcode) {
          case 0x00: bx_printf("rol_eb1 "); bx_debug_Eb1(); return;
          case 0x01: bx_printf("ror_eb1 "); bx_debug_Eb1(); return;
          case 0x02: bx_printf("rcl_eb1 "); bx_debug_Eb1(); return;
          case 0x03: bx_printf("rcr_eb1 "); bx_debug_Eb1(); return;
          case 0x04: bx_printf("shl_eb1 "); bx_debug_Eb1(); return;
          case 0x05: bx_printf("shr_eb1 "); bx_debug_Eb1(); return;
          case 0x06: debug_invalid_opcode(); return;
          case 0x07: bx_printf("sar_eb1 "); bx_debug_Eb1(); return;
          default: bx_panic("debugger: invalid opcode\n"); return;
          }
        return;

      case 0xD1: /* group2 Ev,1 */
        mod_rm_byte = bx_debug_peek_next_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        switch (opcode) {
          case 0x00: bx_printf("rol_ev1 "); bx_debug_Ev1(); return;
          case 0x01: bx_printf("ror_ev1 "); bx_debug_Ev1(); return;
          case 0x02: bx_printf("rcl_ev1 "); bx_debug_Ev1(); return;
          case 0x03: bx_printf("rcr_ev1 "); bx_debug_Ev1(); return;
          case 0x04: bx_printf("shl_ev1 "); bx_debug_Ev1(); return;
          case 0x05: bx_printf("shr_ev1 "); bx_debug_Ev1(); return;
          case 0x06: debug_invalid_opcode(); return;
          case 0x07: bx_printf("sar_ev1 "); bx_debug_Ev1(); return;
          default: bx_panic("debugger: invalid opcode\n"); return;
          }
        return;

      case 0xD2: /* group2 Eb,CL */
        mod_rm_byte = bx_debug_peek_next_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        switch (opcode) {
          case 0x00: bx_printf("rol_ebcl "); bx_debug_EbCL(); return;
          case 0x01: bx_printf("ror_ebcl "); bx_debug_EbCL(); return;
          case 0x02: bx_printf("rcl_ebcl "); bx_debug_EbCL(); return;
          case 0x03: bx_printf("rcr_ebcl "); bx_debug_EbCL(); return;
          case 0x04: bx_printf("shl_ebcl "); bx_debug_EbCL(); return;
          case 0x05: bx_printf("shr_ebcl "); bx_debug_EbCL(); return;
          case 0x06: debug_invalid_opcode(); return;
          case 0x07: bx_printf("sar_ebcl "); bx_debug_EbCL(); return;
          default: bx_panic("debugger: invalid opcode\n"); return;
          }
        return;

      case 0xD3: /* group2 Ev,CL */
        mod_rm_byte = bx_debug_peek_next_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        switch (opcode) {
          case 0x00: bx_printf("rol_evcl "); bx_debug_EvCL(); return;
          case 0x01: bx_printf("ror_evcl "); bx_debug_EvCL(); return;
          case 0x02: bx_printf("rcl_evcl "); bx_debug_EvCL(); return;
          case 0x03: bx_printf("rcr_evcl "); bx_debug_EvCL(); return;
          case 0x04: bx_printf("shl_evcl "); bx_debug_EvCL(); return;
          case 0x05: bx_printf("shr_evcl "); bx_debug_EvCL(); return;
          case 0x06: debug_invalid_opcode(); return;
          case 0x07: bx_printf("sar_evcl "); bx_debug_EvCL(); return;
          default: bx_panic("debugger: invalid opcode\n"); return;
          }
        return;

      case 0xD4: bx_printf("aam "); return;
      case 0xD5: bx_printf("aad "); return;
      case 0xD6: debug_invalid_opcode(); return;
      case 0xD7: bx_printf("xlat "); return;

      case 0xD8: /* ESC0 */
        mod_rm_byte = bx_debug_peek_next_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        if (mod != 3) {
          switch (opcode) {
            case 0x00: bx_printf("fadd_es "); bx_debug_Es(); return;
            case 0x01: bx_printf("fmul_es "); bx_debug_Es(); return;
            case 0x02: bx_printf("fcom_es "); bx_debug_Es(); return;
            case 0x03: bx_printf("fcomp_es "); bx_debug_Es(); return;
            case 0x04: bx_printf("fsub_es "); bx_debug_Es(); return;
            case 0x05: bx_printf("fsubr_es "); bx_debug_Es(); return;
            case 0x06: bx_printf("fdiv_es "); bx_debug_Es(); return;
            case 0x07: bx_printf("fdivr_es "); bx_debug_Es(); return;
            default: bx_panic("debugger: invalid opcode\n"); return;
            }
          }
        else { /* mod == 3 */
          switch (opcode) {
            case 0x00: bx_printf("fadd_st_sti "); bx_debug_ST_STi(); return;
            case 0x01: bx_printf("fmul_st_sti "); bx_debug_ST_STi(); return;
            case 0x02: bx_printf("fcom_st_sti "); bx_debug_ST_STi(); return;
            case 0x03: bx_printf("fcomp_st_sti "); bx_debug_ST_STi(); return;
            case 0x04: bx_printf("fsub_st_sti "); bx_debug_ST_STi(); return;
            case 0x05: bx_printf("fsubr_st_sti "); bx_debug_ST_STi(); return;
            case 0x06: bx_printf("fdiv_st_sti "); bx_debug_ST_STi(); return;
            case 0x07: bx_printf("fdivr_st_sti "); bx_debug_ST_STi(); return;
            default: bx_panic("debugger: invalid opcode\n"); return;
            }
          }
        return;

      case 0xD9: /* ESC1 */
        mod_rm_byte = bx_debug_peek_next_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        if (mod != 3) {
          switch (opcode) {
            case 0x00: bx_printf("fld_es "); bx_debug_Es(); return;
            case 0x01: debug_invalid_opcode(); return;
            case 0x02: bx_printf("fst_es "); bx_debug_Es(); return;
            case 0x03: bx_printf("fstp_es "); bx_debug_Es(); return;
            case 0x04: bx_printf("fldenv_ea "); bx_debug_Ea(); return;
            case 0x05: bx_printf("fldcw_ew "); bx_debug_Ew(); return;
            case 0x06: bx_printf("fstenv_ea "); bx_debug_Ea(); return;
            case 0x07: bx_printf("fstcw_ew "); bx_debug_Ew(); return;
            default: bx_panic("debugger: invalid opcode\n"); return;
            }
          }
        else { /* mod == 3 */
          switch (opcode) {
            case 0x00:
              bx_printf("fld_sti "); bx_debug_STi(); return;
            case 0x01:
              bx_printf("fxch_sti "); bx_debug_STi(); return;
            case 0x02:
              if (rm == 0) {
                bx_printf("fnop "); return;
	        }
              else {
                debug_invalid_opcode(); return;
	        }
            case 0x03:
              bx_printf("fstp_sti "); bx_debug_STi(); return;

            case 0x04:
              switch (rm) {
                case 0x00: bx_printf("fchs "); return;
                case 0x01: bx_printf("fabs "); return;
                case 0x02:
                case 0x03: debug_invalid_opcode(); return;
                case 0x04: bx_printf("ftst "); return;
                case 0x05: bx_printf("fxam "); return;
                case 0x06:
                case 0x07: debug_invalid_opcode(); return;
                }
            case 0x05:
              switch (rm) {
                case 0x00: bx_printf("fld1 "); return;
                case 0x01: bx_printf("fldl2t "); return;
                case 0x02: bx_printf("fldl2e "); return;
                case 0x03: bx_printf("fldpi "); return;
                case 0x04: bx_printf("fldlg2 "); return;
                case 0x05: bx_printf("fldln2 "); return;
                case 0x06: bx_printf("fldz "); return;
                case 0x07: debug_invalid_opcode(); return;
                }
            case 0x06:
              switch (rm) {
                case 0x00: bx_printf("f2xm1 "); return;
                case 0x01: bx_printf("fyl2x "); return;
                case 0x02: bx_printf("fptan "); return;
                case 0x03: bx_printf("fpatan "); return;
                case 0x04: bx_printf("fxtract "); return;
                case 0x05: bx_printf("fprem1 "); return;
                case 0x06: bx_printf("fdecstp "); return;
                case 0x07: bx_printf("fincstp "); return;
                }
            case 0x07:
              switch (rm) {
                case 0x00: bx_printf("fprem "); return;
                case 0x01: bx_printf("fyl2xp1 "); return;
                case 0x02: bx_printf("fsqrt "); return;
                case 0x03: bx_printf("fsincos "); return;
                case 0x04: bx_printf("frndint "); return;
                case 0x05: bx_printf("fscale "); return;
                case 0x06: bx_printf("fsin "); return;
                case 0x07: bx_printf("fcos "); return;
                }
            default: bx_panic("debugger: invalid opcode\n"); return;
            }
          }
        return;

      case 0xDA: /* ESC2 */
        mod_rm_byte = bx_debug_peek_next_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        if (mod != 3) {
          switch (opcode) {
            case 0x00: bx_printf("fiadd_ed "); bx_debug_Ed(); return;
            case 0x01: bx_printf("fimul_ed "); bx_debug_Ed(); return;
            case 0x02: bx_printf("ficom_ed "); bx_debug_Ed(); return;
            case 0x03: bx_printf("ficomp_ed "); bx_debug_Ed(); return;
            case 0x04: bx_printf("fisub_ed "); bx_debug_Ed(); return;
            case 0x05: bx_printf("fisubr_ed "); bx_debug_Ed(); return;
            case 0x06: bx_printf("fidiv_ed "); bx_debug_Ed(); return;
            case 0x07: bx_printf("fidivr_ed "); bx_debug_Ed(); return;
            default: bx_panic("debugger: invalid opcode\n"); return;
            }
          }
        else { /* mod == 3 */
          switch (opcode) {
            case 0x05:
              if (rm == 1) {
                bx_printf("fucompp "); return;
	        }
              else {
                debug_invalid_opcode(); return;
                }
            default: debug_invalid_opcode(); return;
            }
          }

      case 0xDB: /* ESC3 */
        mod_rm_byte = bx_debug_peek_next_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        if (mod != 3) {
          switch (opcode) {
            case 0x00: bx_printf("fild_ed "); bx_debug_Ed(); return;
            case 0x01: debug_invalid_opcode(); return;
            case 0x02: bx_printf("fist_ed "); bx_debug_Ed(); return;
            case 0x03: bx_printf("fistp_ed "); bx_debug_Ed(); return;
            case 0x04: debug_invalid_opcode(); return;
            case 0x05: bx_printf("fld_et "); bx_debug_Et(); return;
            case 0x06: debug_invalid_opcode(); return;
            case 0x07: bx_printf("fstp_et "); bx_debug_Et(); return;
            default: bx_panic("debugger: invalid opcode\n"); return;
            }
          }
        else { /* mod == 3 */
          switch (opcode) {
            case 0x04:
              switch (rm) {
                case 0x00: bx_printf("feni "); return;
                case 0x01: bx_printf("fdisi "); return;
                case 0x02: bx_printf("fclex "); return;
                case 0x03: bx_printf("finit "); return;
                case 0x04: bx_printf("fsetpm "); return;
                default: debug_invalid_opcode(); return;
                }
            default: debug_invalid_opcode(); return;
            }
          }

      case 0xDC: /* ESC4 */
        mod_rm_byte = bx_debug_peek_next_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        if (mod != 3) {
          switch (opcode) {
            case 0x00: bx_printf("fadd_el "); bx_debug_El(); return;
            case 0x01: bx_printf("fmul_el "); bx_debug_El(); return;
            case 0x02: bx_printf("fcom_el "); bx_debug_El(); return;
            case 0x03: bx_printf("fcomp_el "); bx_debug_El(); return;
            case 0x04: bx_printf("fsub_el "); bx_debug_El(); return;
            case 0x05: bx_printf("fsubr_el "); bx_debug_El(); return;
            case 0x06: bx_printf("fdiv_el "); bx_debug_El(); return;
            case 0x07: bx_printf("fdivr_el "); bx_debug_El(); return;
            default: bx_panic("debugger: invalid opcode\n"); return;
            }
          }
        else { /* mod == 3 */
          switch (opcode) {
            case 0x00: bx_printf("fadd_sti_st "); bx_debug_STi_ST(); return;
            case 0x01: bx_printf("fmul_sti_st "); bx_debug_STi_ST(); return;
            case 0x02: bx_printf("fcom_sti_st "); bx_debug_STi_ST(); return;
            case 0x03: bx_printf("fcomp_sti_st "); bx_debug_STi_ST(); return;
            case 0x04: bx_printf("fsubr_sti_st "); bx_debug_STi_ST(); return;
            case 0x05: bx_printf("fsub_sti_st "); bx_debug_STi_ST(); return;
            case 0x06: bx_printf("fdivr_sti_st "); bx_debug_STi_ST(); return;
            case 0x07: bx_printf("fdiv_sti_st "); bx_debug_STi_ST(); return;
            default: debug_invalid_opcode(); return;
            }
          }


      case 0xDD: /* ESC5 */
        mod_rm_byte = bx_debug_peek_next_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        if (mod != 3) {
          switch (opcode) {
            case 0x00: bx_printf("fld_el "); bx_debug_El(); return;
            case 0x01: debug_invalid_opcode(); return;
            case 0x02: bx_printf("fst_el "); bx_debug_El(); return;
            case 0x03: bx_printf("fstp_el "); bx_debug_El(); return;
            case 0x04: bx_printf("frstor_ea "); bx_debug_Ea(); return;
            case 0x05: debug_invalid_opcode(); return;
            case 0x06: bx_printf("fsave_ea "); bx_debug_Ea(); return;
            case 0x07: bx_printf("fstsw_ew "); bx_debug_Ew(); return;
            default: bx_panic("debugger: invalid opcode\n"); return;
            }
          }
        else { /* mod == 3 */
          switch (opcode) {
            case 0x00: bx_printf("ffree_sti "); bx_debug_STi(); return;
            case 0x01: bx_printf("fxch_sti "); bx_debug_STi(); return;
            case 0x02: bx_printf("fst_sti "); bx_debug_STi(); return;
            case 0x03: bx_printf("fstp_sti "); bx_debug_STi(); return;
            case 0x04: bx_printf("fucom_sti_st "); bx_debug_STi_ST(); return;
            case 0x05: bx_printf("fucomp_sti "); bx_debug_STi(); return;
            case 0x06: debug_invalid_opcode(); return;
            case 0x07: debug_invalid_opcode(); return;
            default: debug_invalid_opcode(); return;
            }
          }

      case 0xDE: /* ESC6 */
        mod_rm_byte = bx_debug_peek_next_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        if (mod != 3) {
          switch (opcode) {
            case 0x00: bx_printf("fiadd_ew "); bx_debug_Ew(); return;
            case 0x01: bx_printf("fimul_ew "); bx_debug_Ew(); return;
            case 0x02: bx_printf("ficom_ew "); bx_debug_Ew(); return;
            case 0x03: bx_printf("ficomp_ew "); bx_debug_Ew(); return;
            case 0x04: bx_printf("fisub_ew "); bx_debug_Ew(); return;
            case 0x05: bx_printf("fisubr_ew "); bx_debug_Ew(); return;
            case 0x06: bx_printf("fidiv_ew "); bx_debug_Ew(); return;
            case 0x07: bx_printf("fidivr_ew "); bx_debug_Ew(); return;
            default: bx_panic("debugger: invalid opcode\n"); return;
            }
          }
        else { /* mod == 3 */
          switch (opcode) {
            case 0x00: bx_printf("faddp_sti_st "); bx_debug_STi_ST(); return;
            case 0x01: bx_printf("fmulp_sti_st "); bx_debug_STi_ST(); return;
            case 0x02: bx_printf("fcomp_sti "); bx_debug_STi(); return;
            case 0x03:
              switch (rm) {
                case 0x01: bx_printf("fcompp "); return;
                default: debug_invalid_opcode(); return;
                }
            case 0x04: bx_printf("fsubrp_sti_st "); bx_debug_STi_ST(); return;
            case 0x05: bx_printf("fsubp_sti_si "); bx_debug_STi_ST(); return;
            case 0x06: bx_printf("fdivrp_sti_st "); bx_debug_STi_ST(); return;
            case 0x07: bx_printf("fdivp_sti_st "); bx_debug_STi_ST(); return;
            default: debug_invalid_opcode(); return;
            }
          }

      case 0xDF: /* ESC7 */
        mod_rm_byte = bx_debug_peek_next_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        if (mod != 3) {
          switch (opcode) {
            case 0x00: bx_printf("fild_ew "); bx_debug_Ew(); return;
            case 0x01: debug_invalid_opcode(); return;
            case 0x02: bx_printf("fist_ew "); bx_debug_Ew(); return;
            case 0x03: bx_printf("fistp_ew "); bx_debug_Ew(); return;
            case 0x04: bx_printf("fbld_eb "); bx_debug_Eb(); return;
            case 0x05: bx_printf("fild_eq "); bx_debug_Eq(); return;
            case 0x06: bx_printf("fbstp_eb "); bx_debug_Eb(); return;
            case 0x07: bx_printf("fistp_eq "); bx_debug_Eq(); return;
            default: bx_panic("debugger: invalid opcode\n"); return;
            }
          }
        else { /* mod == 3 */
          switch (opcode) {
            case 0x00: bx_printf("ffree_sti "); bx_debug_STi(); return;
            case 0x01: bx_printf("fxch_sti "); bx_debug_STi(); return;
	    case 0x02: bx_printf("fst_sti "); bx_debug_STi(); return;
	    case 0x03: bx_printf("fstp_sti "); bx_debug_STi(); return;
            case 0x04:
              switch (rm) {
                case 0x01: bx_printf("fstsw_ax "); bx_debug_AX(); return;
                default: debug_invalid_opcode(); return;
                }
            default: debug_invalid_opcode(); return;
            }
          }
 

      case 0xE0: bx_printf("loopne_jb "); bx_debug_Jb(); return;
      case 0xE1: bx_printf("loope_jb "); bx_debug_Jb(); return;
      case 0xE2: bx_printf("loop_jb "); bx_debug_Jb(); return;
      case 0xE3: bx_printf("jcxz_jb "); bx_debug_Jb(); return;
      case 0xE4: bx_printf("in_alib "); bx_debug_ALIb(); return;
      case 0xE5: bx_printf("in_eaxib "); bx_debug_eAXIb(); return;
      case 0xE6: bx_printf("out_ibal "); bx_debug_IbAL(); return;
      case 0xE7: bx_printf("out_ibeax "); bx_debug_IbeAX(); return;
      case 0xE8: bx_printf("call_av "); bx_debug_Av(); return;
      case 0xE9: bx_printf("jmp_jv "); bx_debug_Jv(); return;
      case 0xEA: bx_printf("jmp_ap "); bx_debug_Ap(); return;
      case 0xEB: bx_printf("jmp_jb "); bx_debug_Jb(); return;
      case 0xEC: bx_printf("in_aldx "); bx_debug_ALDX(); return;
      case 0xED: bx_printf("in_eaxdx "); bx_debug_eAXDX(); return;
      case 0xEE: bx_printf("out_dxal "); bx_debug_DXAL(); return;
      case 0xEF: bx_printf("out_dxeax "); bx_debug_DXeAX(); return;

      case 0xF0: /* LOCK */
        break;
      case 0xF1: debug_invalid_opcode(); return;
      case 0xF2: /* REPNE/REPNZ */
        db_repne_prefix = 1;
        break;
      case 0xF3: /* REP/REPE/REPZ */
        db_rep_prefix = 1;
        break;
      case 0xF4: bx_printf("hlt "); return;
      case 0xF5: bx_printf("cmc "); return;
      case 0xF6: /* Group 3 Eb */
        mod_rm_byte = bx_debug_peek_next_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        switch (opcode) {
          case 0x00: bx_printf("test_ebib "); bx_debug_EbIb(); return;
          case 0x01: debug_invalid_opcode(); return;
          case 0x02: bx_printf("not_eb "); bx_debug_Eb(); return;
          case 0x03: bx_printf("neg_eb "); bx_debug_Eb(); return;
          case 0x04: bx_printf("mul_aleb "); bx_debug_ALEb(); return;
          case 0x05: bx_printf("imul_aleb "); bx_debug_ALEb(); return;
          case 0x06: bx_printf("div_aleb "); bx_debug_ALEb(); return;
          case 0x07: bx_printf("idiv_aleb "); bx_debug_ALEb(); return;
          default: bx_panic("debugger: invalid opcode\n"); return;
          }
        return;

      case 0xF7: /* GROUP3 Ev */
        mod_rm_byte = bx_debug_peek_next_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        switch (opcode) {
          case 0x00: bx_printf("test_eviv "); bx_debug_EvIv(); return;
          case 0x01: debug_invalid_opcode(); return;
          case 0x02: bx_printf("not_ev "); bx_debug_Ev(); return;
          case 0x03: bx_printf("neg_ev "); bx_debug_Ev(); return;
          case 0x04: bx_printf("mul_eaxev "); bx_debug_eAXEv(); return;
          case 0x05: bx_printf("imul_eaxev "); bx_debug_eAXEv(); return;
          case 0x06: bx_printf("div_eaxev "); bx_debug_eAXEv(); return;
          case 0x07: bx_printf("idiv_eaxev "); bx_debug_eAXEv(); return;
          default: bx_panic("debugger: invalid opcode\n"); return;
          }
        return;
      case 0xF8: bx_printf("clc "); return;
      case 0xF9: bx_printf("stc "); return;
      case 0xFA: bx_printf("cli "); return;
      case 0xFB: bx_printf("sti "); return;
      case 0xFC: bx_printf("cld "); return;
      case 0xFD: bx_printf("std "); return;
      case 0xFE: /* GROUP4 */
        mod_rm_byte = bx_debug_peek_next_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        switch (opcode) {
          case 0x00: bx_printf("inc_eb "); bx_debug_Eb(); return;
          case 0x01: bx_printf("dec_eb "); bx_debug_Eb(); return;
          default: debug_invalid_opcode(); return;
          }
        return;

      case 0xFF: /* GROUP 5 */
        mod_rm_byte = bx_debug_peek_next_byte();
        BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
        switch (opcode) {
          case 0x00: bx_printf("inc_ev "); bx_debug_Ev(); return;
          case 0x01: bx_printf("dec_ev "); bx_debug_Ev(); return;
          case 0x02: bx_printf("call_ev "); bx_debug_Ev(); return;
          case 0x03: bx_printf("call_ep "); bx_debug_Ep(); return;
          case 0x04: bx_printf("jmp_ev "); bx_debug_Ev(); return;
          case 0x05: bx_printf("jmp_ep "); bx_debug_Ep(); return;
          case 0x06: bx_printf("push_ev "); bx_debug_Ev(); return;
          default: debug_invalid_opcode(); return;
          }
        return;

      default: /* only invalid instructions left */
        bx_panic("debugger: invalid opcode\n");
        return;
        break;
      } /* switch (next_byte) */
    } /* for (byte_count... */
  debug_invalid_opcode();
}


  Bit8u
bx_debug_next_byte(void)
{
  Bit8u next_byte;

  bx_access_linear(debug_segptr->cache.u.segment.base + debug_offset,
    1, 0, BX_READ, &next_byte);
  debug_offset++;
  bx_printf("[%02x] ", (int) next_byte);
  return(next_byte);
}

  Bit8u
bx_debug_peek_next_byte(void)
{
  Bit8u next_byte;

  bx_access_linear(debug_segptr->cache.u.segment.base + debug_offset,
    1, 0, BX_READ, &next_byte);
  bx_printf("[%02x] ", (unsigned) next_byte);
  return(next_byte);
}

  Bit16u
bx_debug_next_word(void)
{
  Bit16u next_word;

  bx_access_linear(debug_segptr->cache.u.segment.base + debug_offset,
    2, 0, BX_READ, &next_word);
  debug_offset += 2;
  bx_printf("[%04x] ", (unsigned) next_word);
  return(next_word);
}

  Bit32u
bx_debug_next_dword()
{
  Bit32u next_dword;

  bx_access_linear(debug_segptr->cache.u.segment.base + debug_offset,
    4, 0, BX_READ, &next_dword);
  debug_offset += 4;
  bx_printf("[%08x] ", (unsigned) next_dword); 
  return(next_dword);
}

  void
debug_invalid_opcode(void)
{
  bx_printf("(INVALID OPCODE)\n");
}
