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


#define BX_DECODE_SIB(sib, s, i, b) \
  { \
  s = (Bit8u) ((sib) >> 6); \
  i = (Bit8u) (((sib) >> 3) & 0x07); \
  b = (Bit8u) ((sib) & 0x07); \
  }

bx_segment_reg_t *bx_sreg_mod00_rm16[8] = {
  &bx_cpu.ds,
  &bx_cpu.ds,
  &bx_cpu.ss,
  &bx_cpu.ss,
  &bx_cpu.ds,
  &bx_cpu.ds,
  &bx_cpu.ds,
  &bx_cpu.ds
  };

bx_segment_reg_t *bx_sreg_mod01_rm16[8] = {
  &bx_cpu.ds,
  &bx_cpu.ds,
  &bx_cpu.ss,
  &bx_cpu.ss,
  &bx_cpu.ds,
  &bx_cpu.ds,
  &bx_cpu.ss,
  &bx_cpu.ds
  };

bx_segment_reg_t *bx_sreg_mod10_rm16[8] = {
  &bx_cpu.ds,
  &bx_cpu.ds,
  &bx_cpu.ss,
  &bx_cpu.ss,
  &bx_cpu.ds,
  &bx_cpu.ds,
  &bx_cpu.ss,
  &bx_cpu.ds
  };

bx_segment_reg_t *bx_sreg_mod00_rm32[8] = {
  &bx_cpu.ds,
  &bx_cpu.ds,
  &bx_cpu.ds,
  &bx_cpu.ds,
  NULL,       /* this entry should never be accessed, escape to 2-byte */
  &bx_cpu.ds,
  &bx_cpu.ds,
  &bx_cpu.ds
  };

/* the default segment to use for a one-byte modrm with mod==01b
   and rm==i
 */
bx_segment_reg_t *bx_sreg_mod01_rm32[8] = {
  &bx_cpu.ds,
  &bx_cpu.ds,
  &bx_cpu.ds,
  &bx_cpu.ds,
  NULL,       /* this entry should never be accessed, escape to 2-byte */
  &bx_cpu.ss,
  &bx_cpu.ds,
  &bx_cpu.ds
  };

/* the default segment to use for a one-byte modrm with mod==10b
   and rm==i
 */
bx_segment_reg_t *bx_sreg_mod10_rm32[8] = {
  &bx_cpu.ds,
  &bx_cpu.ds,
  &bx_cpu.ds,
  &bx_cpu.ds,
  NULL,       /* this entry should never be accessed, escape to 2-byte */
  &bx_cpu.ss,
  &bx_cpu.ds,
  &bx_cpu.ds
  };



/* the default segment to use for a two-byte modrm with mod==00b
   and base==i
 */
bx_segment_reg_t *bx_sreg_mod00_base32[8] = {
  &bx_cpu.ds,
  &bx_cpu.ds,
  &bx_cpu.ds,
  &bx_cpu.ds,
  &bx_cpu.ss,
  &bx_cpu.ds,
  &bx_cpu.ds,
  &bx_cpu.ds
  };

/* the default segment to use for a two-byte modrm with mod==01b
   and base==i
 */
bx_segment_reg_t *bx_sreg_mod01_base32[8] = {
  &bx_cpu.ds,
  &bx_cpu.ds,
  &bx_cpu.ds,
  &bx_cpu.ds,
  &bx_cpu.ss,
  &bx_cpu.ss,
  &bx_cpu.ds,
  &bx_cpu.ds
  };

/* the default segment to use for a two-byte modrm with mod==10b
   and base==i
 */
bx_segment_reg_t *bx_sreg_mod10_base32[8] = {
  &bx_cpu.ds,
  &bx_cpu.ds,
  &bx_cpu.ds,
  &bx_cpu.ds,
  &bx_cpu.ss,
  &bx_cpu.ss,
  &bx_cpu.ds,
  &bx_cpu.ds
  };


/* The 16bit code part of decode_exgx() was rewritten to use a
 * giant switch statement for efficiency.  The 32bit code, after
 * testing, should also be rewritten as such.
 * ???
 */

/* reg_index should be of type unsigned, along with rm_type */

  void
bx_decode_exgx(unsigned *reg_index, Bit32u *rm_addr,
    unsigned *rm_type, bx_segment_reg_t **rm_seg_reg)
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_addrsize) {
    Bit8u  mod, ttt, rm, sib, ss, index, base;
    Bit32u displ32, index_reg_val, base_reg_val;
    Bit8u  modrm;
    Bit8u  displ8;

    /* NOTES:
     * seg_reg_mod01_base & mod10_base aren't correct???
     */
    /* use 32bit addressing modes.  orthogonal base & index registers,
       scaling available, etc. */
    modrm = bx_fetch_next_byte();
    BX_HANDLE_EXCEPTION()
    BX_DECODE_MODRM(modrm, mod, ttt, rm)

    *reg_index = ttt;
    if (mod == 3) { /* mod, reg, reg */
      *rm_addr = rm;
      *rm_type = BX_REGISTER_REF;
#if BX_WEIRDISMS
      *rm_seg_reg = NULL;
#endif
      }
    else { /* mod != 3 */
      *rm_type = BX_MEMORY_REF;
      if (rm != 4) { /* rm != 100b, no s-i-b byte */
        /* one byte modrm */
        switch (mod) {
          case 0:
            if (bx_cpu.segment_override)
              *rm_seg_reg = bx_cpu.segment_override;
            else
              *rm_seg_reg = &bx_cpu.ds;
            if (rm == 5) { /* no reg, 32-bit displacement */
              *rm_addr = bx_fetch_next_dword();
              BX_HANDLE_EXCEPTION()
              }
            else {
              /* else reg indirect, no displacement */
              *rm_addr = BX_READ_32BIT_REG(rm);
              }
            break;
          case 1:
            if (bx_cpu.segment_override)
              *rm_seg_reg = bx_cpu.segment_override;
            else
              *rm_seg_reg = bx_sreg_mod01_rm32[rm];
            /* reg, 8-bit displacement, sign extend */
            displ8 = bx_fetch_next_byte();
            BX_HANDLE_EXCEPTION()
            *rm_addr = BX_READ_32BIT_REG(rm);
            *rm_addr += ((Bit8s) displ8);
            break;
          case 2:
            if (bx_cpu.segment_override)
              *rm_seg_reg = bx_cpu.segment_override;
            else
              *rm_seg_reg = bx_sreg_mod10_rm32[rm];
            /* reg, 32-bit displacement */
            displ32 = bx_fetch_next_dword();
            BX_HANDLE_EXCEPTION()
            *rm_addr = BX_READ_32BIT_REG(rm);
            *rm_addr += displ32;
            break;
          } /* switch (mod) */
        } /* if (rm != 4) */
      else { /* rm == 4, s-i-b byte follows */
        sib = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        BX_DECODE_SIB(sib, ss, index, base)
        switch (mod) {
          case 0:
            if (bx_cpu.segment_override)
              *rm_seg_reg = bx_cpu.segment_override;
            else
              *rm_seg_reg = bx_sreg_mod00_base32[base];
            if (base != 5) /* base != 101b, no displacement */
              base_reg_val = BX_READ_32BIT_REG(base);
            else {
              base_reg_val = bx_fetch_next_dword();
              BX_HANDLE_EXCEPTION()
              }
            index_reg_val = 0;
            if (index != 4) {
              index_reg_val = BX_READ_32BIT_REG(index);
              index_reg_val <<= ss;
              }
            *rm_addr = base_reg_val + index_reg_val;
            break;
          case 1:
            if (bx_cpu.segment_override)
              *rm_seg_reg = bx_cpu.segment_override;
            else
              *rm_seg_reg = bx_sreg_mod01_base32[base];
            displ8 = bx_fetch_next_byte();
            BX_HANDLE_EXCEPTION()
            base_reg_val = BX_READ_32BIT_REG(base);
            index_reg_val = 0;
            if (index != 4) {
              index_reg_val = BX_READ_32BIT_REG(index);
              index_reg_val <<= ss;
              }
            *rm_addr = base_reg_val + index_reg_val + (Bit8s) displ8;
            break;
          case 2:
            if (bx_cpu.segment_override)
              *rm_seg_reg = bx_cpu.segment_override;
            else
              *rm_seg_reg = bx_sreg_mod10_base32[base];
            displ32 = bx_fetch_next_dword();
            BX_HANDLE_EXCEPTION()
            base_reg_val = BX_READ_32BIT_REG(base);
            index_reg_val = 0;
            if (index != 4) {
              index_reg_val = BX_READ_32BIT_REG(index);
              index_reg_val <<= ss;
              }
            *rm_addr = base_reg_val + index_reg_val + displ32;
            break;
          }
        } /* s-i-b byte follows */
      } /* if (mod != 3) */
    }

  else
#endif   /* #if BX_CPU > 2 */
    {
    Bit8u  displ8;
    Bit16u displ16;
    Bit8u  modrm;

#if BX_WEIRDISMS
    *rm_seg_reg = NULL;
#endif

    /* 16 bit addressing modes. */
    modrm = bx_fetch_next_byte();
    BX_HANDLE_EXCEPTION()
    switch (modrm) {
      case 0x0:
        *reg_index = 0x0;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BX + SI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x1:
        *reg_index = 0x0;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BX + DI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x2:
        *reg_index = 0x0;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BP + SI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x3:
        *reg_index = 0x0;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BP + DI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x4:
        *reg_index = 0x0;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (SI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x5:
        *reg_index = 0x0;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (DI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x6:
        *reg_index = 0x0;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x7:
        *reg_index = 0x0;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BX);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x8:
        *reg_index = 0x1;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BX + SI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x9:
        *reg_index = 0x1;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BX + DI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0xa:
        *reg_index = 0x1;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BP + SI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0xb:
        *reg_index = 0x1;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BP + DI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0xc:
        *reg_index = 0x1;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (SI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0xd:
        *reg_index = 0x1;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (DI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0xe:
        *reg_index = 0x1;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0xf:
        *reg_index = 0x1;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BX);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x10:
        *reg_index = 0x2;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BX + SI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x11:
        *reg_index = 0x2;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BX + DI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x12:
        *reg_index = 0x2;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BP + SI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x13:
        *reg_index = 0x2;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BP + DI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x14:
        *reg_index = 0x2;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (SI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x15:
        *reg_index = 0x2;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (DI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x16:
        *reg_index = 0x2;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x17:
        *reg_index = 0x2;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BX);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x18:
        *reg_index = 0x3;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BX + SI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x19:
        *reg_index = 0x3;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BX + DI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x1a:
        *reg_index = 0x3;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BP + SI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x1b:
        *reg_index = 0x3;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BP + DI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x1c:
        *reg_index = 0x3;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (SI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x1d:
        *reg_index = 0x3;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (DI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x1e:
        *reg_index = 0x3;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x1f:
        *reg_index = 0x3;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BX);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x20:
        *reg_index = 0x4;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BX + SI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x21:
        *reg_index = 0x4;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BX + DI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x22:
        *reg_index = 0x4;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BP + SI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x23:
        *reg_index = 0x4;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BP + DI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x24:
        *reg_index = 0x4;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (SI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x25:
        *reg_index = 0x4;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (DI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x26:
        *reg_index = 0x4;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x27:
        *reg_index = 0x4;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BX);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x28:
        *reg_index = 0x5;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BX + SI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x29:
        *reg_index = 0x5;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BX + DI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x2a:
        *reg_index = 0x5;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BP + SI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x2b:
        *reg_index = 0x5;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BP + DI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x2c:
        *reg_index = 0x5;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (SI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x2d:
        *reg_index = 0x5;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (DI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x2e:
        *reg_index = 0x5;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x2f:
        *reg_index = 0x5;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BX);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x30:
        *reg_index = 0x6;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BX + SI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x31:
        *reg_index = 0x6;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BX + DI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x32:
        *reg_index = 0x6;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BP + SI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x33:
        *reg_index = 0x6;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BP + DI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x34:
        *reg_index = 0x6;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (SI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x35:
        *reg_index = 0x6;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (DI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x36:
        *reg_index = 0x6;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x37:
        *reg_index = 0x6;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BX);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x38:
        *reg_index = 0x7;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BX + SI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x39:
        *reg_index = 0x7;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BX + DI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x3a:
        *reg_index = 0x7;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BP + SI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x3b:
        *reg_index = 0x7;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BP + DI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x3c:
        *reg_index = 0x7;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (SI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x3d:
        *reg_index = 0x7;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (DI);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x3e:
        *reg_index = 0x7;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x3f:
        *reg_index = 0x7;
        *rm_type = BX_MEMORY_REF;
        *rm_addr = (Bit16u) (BX);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x40:
        *reg_index = 0x0;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + SI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x41:
        *reg_index = 0x0;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + DI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x42:
        *reg_index = 0x0;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + SI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x43:
        *reg_index = 0x0;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + DI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x44:
        *reg_index = 0x0;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (SI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x45:
        *reg_index = 0x0;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (DI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x46:
        *reg_index = 0x0;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x47:
        *reg_index = 0x0;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x48:
        *reg_index = 0x1;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + SI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x49:
        *reg_index = 0x1;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + DI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x4a:
        *reg_index = 0x1;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + SI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x4b:
        *reg_index = 0x1;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + DI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x4c:
        *reg_index = 0x1;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (SI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x4d:
        *reg_index = 0x1;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (DI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x4e:
        *reg_index = 0x1;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x4f:
        *reg_index = 0x1;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x50:
        *reg_index = 0x2;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + SI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x51:
        *reg_index = 0x2;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + DI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x52:
        *reg_index = 0x2;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + SI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x53:
        *reg_index = 0x2;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + DI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x54:
        *reg_index = 0x2;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (SI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x55:
        *reg_index = 0x2;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (DI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x56:
        *reg_index = 0x2;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x57:
        *reg_index = 0x2;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x58:
        *reg_index = 0x3;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + SI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x59:
        *reg_index = 0x3;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + DI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x5a:
        *reg_index = 0x3;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + SI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x5b:
        *reg_index = 0x3;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + DI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x5c:
        *reg_index = 0x3;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (SI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x5d:
        *reg_index = 0x3;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (DI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x5e:
        *reg_index = 0x3;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x5f:
        *reg_index = 0x3;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x60:
        *reg_index = 0x4;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + SI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x61:
        *reg_index = 0x4;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + DI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x62:
        *reg_index = 0x4;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + SI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x63:
        *reg_index = 0x4;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + DI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x64:
        *reg_index = 0x4;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (SI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x65:
        *reg_index = 0x4;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (DI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x66:
        *reg_index = 0x4;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x67:
        *reg_index = 0x4;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x68:
        *reg_index = 0x5;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + SI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x69:
        *reg_index = 0x5;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + DI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x6a:
        *reg_index = 0x5;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + SI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x6b:
        *reg_index = 0x5;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + DI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x6c:
        *reg_index = 0x5;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (SI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x6d:
        *reg_index = 0x5;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (DI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x6e:
        *reg_index = 0x5;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x6f:
        *reg_index = 0x5;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x70:
        *reg_index = 0x6;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + SI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x71:
        *reg_index = 0x6;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + DI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x72:
        *reg_index = 0x6;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + SI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x73:
        *reg_index = 0x6;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + DI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x74:
        *reg_index = 0x6;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (SI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x75:
        *reg_index = 0x6;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (DI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x76:
        *reg_index = 0x6;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x77:
        *reg_index = 0x6;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x78:
        *reg_index = 0x7;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + SI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x79:
        *reg_index = 0x7;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + DI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x7a:
        *reg_index = 0x7;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + SI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x7b:
        *reg_index = 0x7;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + DI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x7c:
        *reg_index = 0x7;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (SI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x7d:
        *reg_index = 0x7;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (DI + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x7e:
        *reg_index = 0x7;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x7f:
        *reg_index = 0x7;
        *rm_type = BX_MEMORY_REF;
        displ8 = bx_fetch_next_byte();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + (Bit8s) displ8);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x80:
        *reg_index = 0x0;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + SI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x81:
        *reg_index = 0x0;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + DI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x82:
        *reg_index = 0x0;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + SI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x83:
        *reg_index = 0x0;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + DI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x84:
        *reg_index = 0x0;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (SI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x85:
        *reg_index = 0x0;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (DI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x86:
        *reg_index = 0x0;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x87:
        *reg_index = 0x0;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x88:
        *reg_index = 0x1;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + SI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x89:
        *reg_index = 0x1;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + DI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x8a:
        *reg_index = 0x1;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + SI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x8b:
        *reg_index = 0x1;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + DI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x8c:
        *reg_index = 0x1;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (SI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x8d:
        *reg_index = 0x1;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (DI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x8e:
        *reg_index = 0x1;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x8f:
        *reg_index = 0x1;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x90:
        *reg_index = 0x2;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + SI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x91:
        *reg_index = 0x2;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + DI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x92:
        *reg_index = 0x2;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + SI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x93:
        *reg_index = 0x2;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + DI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x94:
        *reg_index = 0x2;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (SI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x95:
        *reg_index = 0x2;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (DI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x96:
        *reg_index = 0x2;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x97:
        *reg_index = 0x2;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x98:
        *reg_index = 0x3;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + SI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x99:
        *reg_index = 0x3;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + DI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x9a:
        *reg_index = 0x3;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + SI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x9b:
        *reg_index = 0x3;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + DI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x9c:
        *reg_index = 0x3;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (SI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x9d:
        *reg_index = 0x3;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (DI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0x9e:
        *reg_index = 0x3;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0x9f:
        *reg_index = 0x3;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0xa0:
        *reg_index = 0x4;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + SI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0xa1:
        *reg_index = 0x4;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + DI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0xa2:
        *reg_index = 0x4;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + SI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0xa3:
        *reg_index = 0x4;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + DI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0xa4:
        *reg_index = 0x4;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (SI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0xa5:
        *reg_index = 0x4;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (DI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0xa6:
        *reg_index = 0x4;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0xa7:
        *reg_index = 0x4;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0xa8:
        *reg_index = 0x5;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + SI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0xa9:
        *reg_index = 0x5;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + DI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0xaa:
        *reg_index = 0x5;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + SI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0xab:
        *reg_index = 0x5;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + DI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0xac:
        *reg_index = 0x5;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (SI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0xad:
        *reg_index = 0x5;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (DI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0xae:
        *reg_index = 0x5;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0xaf:
        *reg_index = 0x5;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0xb0:
        *reg_index = 0x6;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + SI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0xb1:
        *reg_index = 0x6;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + DI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0xb2:
        *reg_index = 0x6;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + SI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0xb3:
        *reg_index = 0x6;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + DI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0xb4:
        *reg_index = 0x6;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (SI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0xb5:
        *reg_index = 0x6;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (DI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0xb6:
        *reg_index = 0x6;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0xb7:
        *reg_index = 0x6;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0xb8:
        *reg_index = 0x7;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + SI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0xb9:
        *reg_index = 0x7;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + DI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0xba:
        *reg_index = 0x7;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + SI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0xbb:
        *reg_index = 0x7;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + DI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0xbc:
        *reg_index = 0x7;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (SI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0xbd:
        *reg_index = 0x7;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (DI + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0xbe:
        *reg_index = 0x7;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BP + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ss;
        break;
      case 0xbf:
        *reg_index = 0x7;
        *rm_type = BX_MEMORY_REF;
        displ16 = bx_fetch_next_word();
        BX_HANDLE_EXCEPTION()
        *rm_addr = (Bit16u) (BX + displ16);
        if (bx_cpu.segment_override)
          *rm_seg_reg = bx_cpu.segment_override;
        else
          *rm_seg_reg = &bx_cpu.ds;
        break;
      case 0xc0:
        *reg_index = 0x0;
        *rm_addr = 0x0;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xc1:
        *reg_index = 0x0;
        *rm_addr = 0x1;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xc2:
        *reg_index = 0x0;
        *rm_addr = 0x2;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xc3:
        *reg_index = 0x0;
        *rm_addr = 0x3;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xc4:
        *reg_index = 0x0;
        *rm_addr = 0x4;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xc5:
        *reg_index = 0x0;
        *rm_addr = 0x5;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xc6:
        *reg_index = 0x0;
        *rm_addr = 0x6;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xc7:
        *reg_index = 0x0;
        *rm_addr = 0x7;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xc8:
        *reg_index = 0x1;
        *rm_addr = 0x0;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xc9:
        *reg_index = 0x1;
        *rm_addr = 0x1;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xca:
        *reg_index = 0x1;
        *rm_addr = 0x2;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xcb:
        *reg_index = 0x1;
        *rm_addr = 0x3;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xcc:
        *reg_index = 0x1;
        *rm_addr = 0x4;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xcd:
        *reg_index = 0x1;
        *rm_addr = 0x5;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xce:
        *reg_index = 0x1;
        *rm_addr = 0x6;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xcf:
        *reg_index = 0x1;
        *rm_addr = 0x7;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xd0:
        *reg_index = 0x2;
        *rm_addr = 0x0;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xd1:
        *reg_index = 0x2;
        *rm_addr = 0x1;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xd2:
        *reg_index = 0x2;
        *rm_addr = 0x2;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xd3:
        *reg_index = 0x2;
        *rm_addr = 0x3;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xd4:
        *reg_index = 0x2;
        *rm_addr = 0x4;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xd5:
        *reg_index = 0x2;
        *rm_addr = 0x5;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xd6:
        *reg_index = 0x2;
        *rm_addr = 0x6;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xd7:
        *reg_index = 0x2;
        *rm_addr = 0x7;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xd8:
        *reg_index = 0x3;
        *rm_addr = 0x0;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xd9:
        *reg_index = 0x3;
        *rm_addr = 0x1;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xda:
        *reg_index = 0x3;
        *rm_addr = 0x2;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xdb:
        *reg_index = 0x3;
        *rm_addr = 0x3;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xdc:
        *reg_index = 0x3;
        *rm_addr = 0x4;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xdd:
        *reg_index = 0x3;
        *rm_addr = 0x5;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xde:
        *reg_index = 0x3;
        *rm_addr = 0x6;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xdf:
        *reg_index = 0x3;
        *rm_addr = 0x7;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xe0:
        *reg_index = 0x4;
        *rm_addr = 0x0;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xe1:
        *reg_index = 0x4;
        *rm_addr = 0x1;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xe2:
        *reg_index = 0x4;
        *rm_addr = 0x2;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xe3:
        *reg_index = 0x4;
        *rm_addr = 0x3;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xe4:
        *reg_index = 0x4;
        *rm_addr = 0x4;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xe5:
        *reg_index = 0x4;
        *rm_addr = 0x5;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xe6:
        *reg_index = 0x4;
        *rm_addr = 0x6;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xe7:
        *reg_index = 0x4;
        *rm_addr = 0x7;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xe8:
        *reg_index = 0x5;
        *rm_addr = 0x0;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xe9:
        *reg_index = 0x5;
        *rm_addr = 0x1;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xea:
        *reg_index = 0x5;
        *rm_addr = 0x2;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xeb:
        *reg_index = 0x5;
        *rm_addr = 0x3;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xec:
        *reg_index = 0x5;
        *rm_addr = 0x4;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xed:
        *reg_index = 0x5;
        *rm_addr = 0x5;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xee:
        *reg_index = 0x5;
        *rm_addr = 0x6;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xef:
        *reg_index = 0x5;
        *rm_addr = 0x7;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xf0:
        *reg_index = 0x6;
        *rm_addr = 0x0;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xf1:
        *reg_index = 0x6;
        *rm_addr = 0x1;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xf2:
        *reg_index = 0x6;
        *rm_addr = 0x2;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xf3:
        *reg_index = 0x6;
        *rm_addr = 0x3;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xf4:
        *reg_index = 0x6;
        *rm_addr = 0x4;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xf5:
        *reg_index = 0x6;
        *rm_addr = 0x5;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xf6:
        *reg_index = 0x6;
        *rm_addr = 0x6;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xf7:
        *reg_index = 0x6;
        *rm_addr = 0x7;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xf8:
        *reg_index = 0x7;
        *rm_addr = 0x0;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xf9:
        *reg_index = 0x7;
        *rm_addr = 0x1;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xfa:
        *reg_index = 0x7;
        *rm_addr = 0x2;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xfb:
        *reg_index = 0x7;
        *rm_addr = 0x3;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xfc:
        *reg_index = 0x7;
        *rm_addr = 0x4;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xfd:
        *reg_index = 0x7;
        *rm_addr = 0x5;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xfe:
        *reg_index = 0x7;
        *rm_addr = 0x6;
        *rm_type = BX_REGISTER_REF;
        break;
      case 0xff:
        *reg_index = 0x7;
        *rm_addr = 0x7;
        *rm_type = BX_REGISTER_REF;
        break;
      }
    } /* else ... */
}

