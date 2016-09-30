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




extern Boolean db_32bit_opsize;
extern Boolean db_32bit_addrsize;

char *bx_segment_name[8] = {
  "ES",
  "CS",
  "SS",
  "DS",
  "FS",
  "GS",
  "(invalid segment reg)",
  "(invalid segment reg)"
  };

char *bx_general_8bit_reg_name[8] = {
  "AL",
  "CL",
  "DL",
  "BL",
  "AH",
  "CH",
  "DH",
  "BH"
  };

char *bx_general_16bit_reg_name[8] = {
  "AX",
  "CX",
  "DX",
  "BX",
  "SP",
  "BP",
  "SI",
  "DI"
  };

char *bx_general_32bit_reg_name[8] = {
  "EAX",
  "ECX",
  "EDX",
  "EBX",
  "ESP",
  "EBP",
  "ESI",
  "EDI"
  };

char *bx_16bit_base_name[8] = {
  "BX", "BX", "BP", "BP", NULL, NULL, "BP", "BX"
  };

char *bx_16bit_index_name[8] = {
  "SI", "DI", "SI", "DI", "SI", "DI", NULL, NULL
  };

char *bx_32bit_index_name[8] = {
  "EAX", "ECX", "EDX", "EBX", NULL, "EBP", "ESI", "EDI"
  };

extern bx_segment_reg_t *bx_sreg_mod00_rm16[8];
extern bx_segment_reg_t *bx_sreg_mod01_rm16[8];
extern bx_segment_reg_t *bx_sreg_mod10_rm16[8];

extern bx_segment_reg_t *bx_sreg_mod00_rm32[8];
extern bx_segment_reg_t *bx_sreg_mod01_rm32[8];
extern bx_segment_reg_t *bx_sreg_mod10_rm32[8];

extern bx_segment_reg_t *bx_sreg_mod00_base32[8];
extern bx_segment_reg_t *bx_sreg_mod01_base32[8];
extern bx_segment_reg_t *bx_sreg_mod10_base32[8];



extern bx_segment_reg_t *bx_debug_seg_override;

void debug_out_reg_name(int reg, int reg_type);
void debug_out_16bit_base(int base);
void debug_out_16bit_index(int index);


  void
bx_debug_decode_exgx(int reg_type, int modrm_reg_type)
{
  Bit32u mod_rm_addr;
  bx_segment_reg_t *mod_rm_seg_reg;

  Bit8u modrm, mod, ttt, rm;
  Bit8u  displ8;
  Bit16u displ16;


#if BX_CPU > 2
  if (db_32bit_addrsize) {
    Bit8u sib, ss, index, base;
    Bit32u displ;

    /* use 32bit addressing modes.  orthogonal base & index registers,
       scaling available, etc. */
    modrm = bx_debug_next_byte();
    mod = modrm >> 6;
    ttt = (modrm >> 3) & 0x07;
    rm = modrm & 0x07;
    bx_printf("|MOD%d|REG%d|RM%d| ", (int) mod, (int) ttt, (int) rm);


    if (mod == 3) { /* mod, reg, reg */
      debug_out_reg_name(ttt, reg_type);
      bx_printf(",");
      debug_out_reg_name(rm, modrm_reg_type);
      }
    else { /* mod != 3 */
      if (rm != 4) { /* rm != 100b, no s-i-b byte */
        /* one byte modrm */
        switch (mod) {
          case 0:
            if (bx_debug_seg_override)
              mod_rm_seg_reg = bx_debug_seg_override;
            else
              mod_rm_seg_reg = &bx_cpu.ds;
            if (rm == 5) { /* no reg, 32-bit displacement */
              mod_rm_addr = bx_debug_next_dword();
              bx_printf("%s:%08x ", bx_strseg(mod_rm_seg_reg), mod_rm_addr);
              }
            else {
              bx_printf("%s:[%s] ", bx_strseg(mod_rm_seg_reg),
                bx_general_32bit_reg_name[rm]);
              }
            break;
          case 1:
            if (bx_debug_seg_override)
              mod_rm_seg_reg = bx_debug_seg_override;
            else
              mod_rm_seg_reg = bx_sreg_mod01_rm32[rm];
            /* reg, 8-bit displacement, sign extend */
            displ = bx_debug_next_byte();
            bx_printf("%s:[%s + %02x] ", bx_strseg(mod_rm_seg_reg),
              bx_general_32bit_reg_name[rm], (int) displ);
            break;
          case 2:
            if (bx_debug_seg_override)
              mod_rm_seg_reg = bx_debug_seg_override;
            else
              mod_rm_seg_reg = bx_sreg_mod10_rm32[rm];
            /* reg, 32-bit displacement */
            displ = bx_debug_next_dword();
            bx_printf("%s:[%s + %08x] ", bx_strseg(mod_rm_seg_reg),
              bx_general_32bit_reg_name[rm], (unsigned) displ);
            /* BX_READ_32BIT_REG(*mod_rm_addr, rm); */
            break;
          } /* switch (mod) */
        } /* if (rm != 4) */
      else { /* rm == 4, s-i-b byte follows */
        sib = bx_debug_next_byte();
        ss = sib >> 6;
        index = (sib >> 3) & 0x07;
        base = sib & 0x07;
        bx_printf("|SS%u|IND%u|BASE%u| ", (unsigned) ss,
                  (unsigned) index, (unsigned) base);

        switch (mod) {
          case 0:
            if (bx_debug_seg_override)
              mod_rm_seg_reg = bx_debug_seg_override;
            else
              mod_rm_seg_reg = bx_sreg_mod00_base32[base];
            bx_printf("%s:[", bx_strseg(mod_rm_seg_reg));
            if (base != 5)
              bx_printf("%s", bx_general_32bit_reg_name[rm]);
            else {
              displ = bx_debug_next_dword();
              bx_printf("%08x", (unsigned) displ);
              }

            if (index != 4)
              bx_printf("+ %s<<%u", bx_32bit_index_name[index], ss);
            bx_printf("]\n");
            break;
          case 1:
            if (bx_debug_seg_override)
              mod_rm_seg_reg = bx_debug_seg_override;
            else
              mod_rm_seg_reg = bx_sreg_mod01_base32[base];
            displ = bx_debug_next_byte();
            bx_printf("%s:[%s", bx_strseg(mod_rm_seg_reg),
              bx_general_32bit_reg_name[rm]);

            if (index != 4)
              bx_printf("+ %s<<%u", bx_32bit_index_name[index], ss);
            bx_printf(" + %02x]\n", (unsigned) displ);
            break;
          case 2:
            if (bx_debug_seg_override)
              mod_rm_seg_reg = bx_debug_seg_override;
            else
              mod_rm_seg_reg = bx_sreg_mod10_base32[base];
            displ = bx_debug_next_dword();
            bx_printf("%s:[%s", bx_strseg(mod_rm_seg_reg),
              bx_general_32bit_reg_name[rm]);

            if (index != 4)
              bx_printf("+ %s<<%u", bx_32bit_index_name[index], ss);
            bx_printf(" + %08x]\n", (unsigned) displ);
            break;
          }
        } /* s-i-b byte follows */
      } /* if (mod != 3) */
    }

  else {
#endif /* BX_CPU > 2 */
    /* 16 bit addressing modes. */
    modrm = bx_debug_next_byte();
    mod = modrm >> 6;
    ttt = (modrm >> 3) & 0x07;
    rm = modrm & 0x07;
    bx_printf("|MOD%d|REG%d|RM%d| ", (int) mod, (int) ttt, (int) rm);
    
    if (reg_type != BX_NO_REG_TYPE) {
      debug_out_reg_name(ttt, reg_type);
      bx_printf(",");
      }

    switch (mod) {
      case 3: /* mod, reg, reg */
        debug_out_reg_name(rm, modrm_reg_type);
        break;
      case 0:
        if (rm != 6) { /* no displacement */
          if (bx_debug_seg_override)
            mod_rm_seg_reg = bx_debug_seg_override;
          else
            mod_rm_seg_reg = bx_sreg_mod00_rm16[rm];
          bx_printf("%s:", bx_strseg(mod_rm_seg_reg));
          debug_out_16bit_base(rm);
          debug_out_16bit_index(rm);
          bx_printf("\n");
          }
        else {
          mod_rm_addr = (Bit32u) bx_debug_next_word();
          if (bx_debug_seg_override)
            mod_rm_seg_reg = bx_debug_seg_override;
          else
            mod_rm_seg_reg = &bx_cpu.ds;
          bx_printf("%s:%x\n", bx_strseg(mod_rm_seg_reg), mod_rm_addr);
          }
        break;
      case 1:
        displ8 = bx_debug_next_byte();
        if (bx_debug_seg_override)
          mod_rm_seg_reg = bx_debug_seg_override;
        else
          mod_rm_seg_reg = bx_sreg_mod01_rm16[rm];
        bx_printf("%s:", bx_strseg(mod_rm_seg_reg));
        debug_out_16bit_base(rm);
        debug_out_16bit_index(rm);
        bx_printf("[%x]\n", (int) displ8);
        break;
      case 2:
        displ16 = bx_debug_next_word();
        if (bx_debug_seg_override)
          mod_rm_seg_reg = bx_debug_seg_override;
        else
          mod_rm_seg_reg = bx_sreg_mod10_rm16[rm];
        bx_printf("%s:", bx_strseg(mod_rm_seg_reg));
        debug_out_16bit_base(rm);
        debug_out_16bit_index(rm);
        bx_printf("[%x]\n", (int) displ16);
        break;
      } /* switch (mod) ... */
#if BX_CPU > 2
    }
#endif /* BX_CPU > 2 */
}

  void
debug_out_reg_name(int reg, int reg_type)
{
  switch (reg_type) {
    case BX_SEGMENT_REG:
      bx_printf("%s", bx_segment_name[reg]);
      break;
    case BX_GENERAL_8BIT_REG:
      bx_printf("%s", bx_general_8bit_reg_name[reg]);
      break;
    case BX_GENERAL_16BIT_REG:
      bx_printf("%s", bx_general_16bit_reg_name[reg]);
      break;
    case BX_GENERAL_32BIT_REG:
      bx_printf("%s", bx_general_32bit_reg_name[reg]);
      break;
    }
}

  void
debug_out_16bit_base(int base)
{
  if (bx_16bit_base_name[base])
    bx_printf("[%s]",  bx_16bit_base_name[base]);
}

  void
debug_out_16bit_index(int index)
{
  if (bx_16bit_index_name[index])
    bx_printf("[%s]", bx_16bit_index_name[index]);
}
