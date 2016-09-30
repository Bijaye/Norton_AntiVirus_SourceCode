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


#pragma warning(disable:4761) // 'integral size mismatch; conversion supplied'
#pragma warning(disable:4244) // 'conversion from unsigned short to unsigned char'



#include "bochs.h"


  INLINE void
bx_SHLD_EvGvIb()
{
  Bit8u op3_8;
  Bit32u op1_addr;
  unsigned op2_addr, op1_type;
  bx_segment_reg_t *op1_seg_reg;

  /* op1:op2 << imm8.  result stored in op1 */

  bx_decode_exgx(&op2_addr, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op3_8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  op3_8 &= 0x1F; /* use only 5 LSB's */

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op1_32, op2_32, result_32;

    if (!op3_8) return; /* NOP */

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }
    op2_32 = BX_READ_32BIT_REG(op2_addr);

    result_32 = (op1_32 << op3_8) | (op2_32 >> (32 - op3_8));

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, result_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * SHLD count affects the following flags: S,Z,P,C,O
     */
    bx_set_CF((op1_32 >> (32 - op3_8)) & 0x01);
    bx_set_OF(((op1_32 ^ result_32) & 0x80000000) > 0);
    bx_set_ZF(result_32 == 0);
    bx_set_PF_base(result_32);
    bx_set_SF(result_32 >> 31);
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit opsize */
    Bit16u op1_16, op2_16, result_16;

    if (!op3_8) return; /* NOP */

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }
    op2_16 = BX_READ_16BIT_REG(op2_addr);

    result_16 = (op1_16 << op3_8) | (op2_16 >> (16 - op3_8));

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, result_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * SHLD count affects the following flags: S,Z,P,C,O
     */
    if (op3_8 <= 16) {
      bx_set_CF((op1_16 >> (16 - op3_8)) & 0x01);
      }
    else {
      bx_set_CF(0);
      }
    bx_set_OF(((op1_16 ^ result_16) & 0x8000) > 0);
    bx_set_ZF(result_16 == 0);
    bx_set_SF(result_16 >> 15);
    bx_set_PF_base(result_16);
    }
}

  INLINE void
bx_SHLD_EvGvCL()
{
bx_panic("SHLD_EvGvCL: not implemented yet\n");
}

  INLINE void
bx_SHRD_EvGvIb()
{
#if BX_CPU < 3
  bx_panic("shrd_evgvib: not supported on < 386\n");
#else
  Bit8u op3_8;
  Bit32u op1_addr;
  unsigned op2_addr, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&op2_addr, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op3_8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  op3_8 &= 0x1F; /* use only 5 LSB's */

  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op1_32, op2_32, result_32;

    if (!op3_8) return; /* NOP */

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }
    op2_32 = BX_READ_32BIT_REG(op2_addr);

    result_32 = (op2_32 << (32 - op3_8)) | (op1_32 >> op3_8);

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, result_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * SHRD count affects the following flags: S,Z,P,C,O
     */

    bx_set_CF((op1_32 >> (op3_8 - 1)) & 0x01);
    bx_set_ZF(result_32 == 0);
    bx_set_SF(result_32 >> 31);
    /* for shift of 1, OF set if sign change occurred. */
    bx_set_OF(((op1_32 ^ result_32) & 0x80000000) > 0);
    bx_set_PF_base(result_32);
    }
  else { /* 16 bit opsize */
    Bit16u op1_16, op2_16, result_16;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }
    op2_16 = BX_READ_16BIT_REG(op2_addr);

    if (!op3_8) return; /* NOP */

    /* result & flags undefined for shift count > operand size */
    result_16 = (op2_16 << (16 - op3_8)) | (op1_16 >> op3_8);

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, result_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * SHRD count affects the following flags: S,Z,P,C,O
     */

    bx_set_CF((op1_16 >> (op3_8 - 1)) & 0x01);
    bx_set_ZF(result_16 == 0);
    bx_set_SF(result_16 >> 15);
    /* for shift of 1, OF set if sign change occurred. */
    bx_set_OF(((op1_16 ^ result_16) & 0x8000) > 0);
    bx_set_PF_base(result_16);
    }
#endif /* BX_CPU >= 3 */
}

  INLINE void
bx_SHRD_EvGvCL()
{
bx_panic("SHRD_EvGvCL: not implemented yet\n");
}

  INLINE void
bx_ROL_EbIb()
{
  Bit8u op2_8, op1_8, result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  op2_8 &= 0x1f; /* use only bottom 5 bits */

/* riad: may be a bug her */

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  if (op2_8) {
    op2_8 %= 8; /* modulus operand size, since its a circular operation */
    result_8 = (op1_8 << op2_8) | (op1_8 >> (8 - op2_8));

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_8BIT_REG(op1_addr, result_8);
      }
    else {
      bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * ROL count affects the following flags: C
     */

    bx_set_CF(result_8 & 0x01);
    bx_set_OF(((op1_8 ^ result_8) & 0x80) > 0);
    }
}

  INLINE void
bx_ROL_EvIb()
{
  Bit8u op2_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = bx_fetch_next_byte();
  op2_8 &= 0x1f; /* only use bottom 5 bits */
  BX_HANDLE_EXCEPTION()

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op1_32, result_32;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    if (op2_8) {
      result_32 = (op1_32 << op2_8) | (op1_32 >> (32 - op2_8));

      /* now write result back to destination */
      if (op1_type == BX_REGISTER_REF) {
        BX_WRITE_32BIT_REG(op1_addr, result_32);
        }
      else {
        bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
        BX_HANDLE_EXCEPTION()
        }

      /* set eflags:
       * ROL count affects the following flags: C
       */

      bx_set_CF(result_32 & 0x01);
      bx_set_OF(((op1_32 ^ result_32) & 0x80000000) > 0);
      }
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit opsize */
    Bit16u op1_16, result_16;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    if (op2_8) {
      op2_8 %= 16; /* modulus operand size, since its a circular operation */
      result_16 = (op1_16 << op2_8) | (op1_16 >> (16 - op2_8));

      /* now write result back to destination */
      if (op1_type == BX_REGISTER_REF) {
        BX_WRITE_16BIT_REG(op1_addr, result_16);
        }
      else {
        bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
        BX_HANDLE_EXCEPTION()
        }

      /* set eflags:
       * ROL count affects the following flags: C
       */

      bx_set_CF(result_16 & 0x01);
      bx_set_OF(((op1_16 ^ result_16) & 0x8000) > 0);
      }
    }
}

  INLINE void
bx_ROL_Eb1()
{
  Bit8u op1_8, result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;
  Bit8u result_b7;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  result_8 = (op1_8 << 1) | (op1_8 >> 7);

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }

  /* set eflags:
   * ROL 1 affects the following flags: O,C
   */
  result_b7 = result_8 & 0x80;

  bx_set_CF(result_8 & 0x01);
  bx_set_OF(bx_get_CF() ^ (result_b7?1:0));
}

  INLINE void
bx_ROL_Ev1()
{
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op1_32, result_32, result_b31;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    result_32 = (op1_32 << 1) | (op1_32 >> 31);

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, result_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * ROL 1 affects the following flags: O,C
     */
    result_b31 = result_32 & 0x80000000;

    bx_set_CF(result_32 & 0x01);
    bx_set_OF(bx_get_CF() ^ (result_b31?1:0));
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit opsize */
    Bit16u op1_16, result_16, result_b15;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    result_16 = (op1_16 << 1) | (op1_16 >> 15);

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, result_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * ROL 1 affects the following flags: O,C
     */
    result_b15 = result_16 & 0x8000;

    bx_set_CF(result_16 & 0x01);
    bx_set_OF(bx_get_CF() ^ (result_b15?1:0));
    }
}

  INLINE void
bx_ROL_EbCL()
{
  Bit8u op2_8, op1_8, result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = CL;
  op2_8 &= 0x1f; /* only use bottom 5 bits */

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  if (op2_8) {
    op2_8 %= 8; /* modulus operand size, since its a circular operation */
    result_8 = (op1_8 << op2_8) | (op1_8 >> (8 - op2_8));

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_8BIT_REG(op1_addr, result_8);
      }
    else {
      bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * ROL count affects the following flags: C
     */

    bx_set_CF(result_8 & 0x01);
    bx_set_OF(((op1_8 ^ result_8) & 0x80) > 0);
    }
}

  INLINE void
bx_ROL_EvCL()
{
  Bit8u op2_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = CL;
  op2_8 &= 0x1f; /* only use 5 bottom bits */

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op1_32, result_32;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    if (op2_8) {
      result_32 = (op1_32 << op2_8) | (op1_32 >> (32 - op2_8));

      /* now write result back to destination */
      if (op1_type == BX_REGISTER_REF) {
        BX_WRITE_32BIT_REG(op1_addr, result_32);
        }
      else {
        bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
        BX_HANDLE_EXCEPTION()
        }

      /* set eflags:
       * ROL count affects the following flags: C
       */

      bx_set_CF(result_32 & 0x01);
      bx_set_OF(((op1_32 ^ result_32) & 0x80000000) > 0);
      }
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit opsize */
    Bit16u op1_16, result_16;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    if (op2_8) {
      op2_8 %= 16; /* modulus operand size, since its a circular operation */
      result_16 = (op1_16 << op2_8) | (op1_16 >> (16 - op2_8));

      /* now write result back to destination */
      if (op1_type == BX_REGISTER_REF) {
        BX_WRITE_16BIT_REG(op1_addr, result_16);
        }
      else {
        bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
        BX_HANDLE_EXCEPTION()
        }

      /* set eflags:
       * ROL count affects the following flags: C
       */

      bx_set_CF(result_16 & 0x01);
      bx_set_OF(((op1_16 ^ result_16) & 0x8000) > 0);
      }
    }
}

  INLINE void
bx_ROR_EbIb()
{
  Bit8u op2_8, op1_8, result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;
  Bit8u result_b7;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  op2_8 &= 0x1f; /* use only bottom 5 bits */

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  if (op2_8) {
    op2_8 %= 8;
    result_8 = (op1_8 >> op2_8) | (op1_8 << (8 - op2_8));

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_8BIT_REG(op1_addr, result_8);
      }
    else {
      bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * ROR count affects the following flags: C
     */
    result_b7 = result_8 & 0x80;

    bx_set_CF(result_b7 != 0);
    bx_set_OF(((op1_8 ^ result_8) & 0x80) > 0);
    }
}

  INLINE void
bx_ROR_EvIb()
{
  Bit8u op2_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  op2_8 &= 0x1f;  /* use only 5 LSB's */

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op1_32, result_32, result_b31;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    if (op2_8) {
      result_32 = (op1_32 >> op2_8) | (op1_32 << (32 - op2_8));

      /* now write result back to destination */
      if (op1_type == BX_REGISTER_REF) {
        BX_WRITE_32BIT_REG(op1_addr, result_32);
        }
      else {
        bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
        BX_HANDLE_EXCEPTION()
        }

      /* set eflags:
       * ROR count affects the following flags: C
       */
      result_b31 = result_32 & 0x80000000;

      bx_set_CF(result_b31 != 0);
      bx_set_OF(((op1_32 ^ result_32) & 0x80000000) > 0);
      }
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit opsize */
    Bit16u op1_16, result_16, result_b15;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    if (op2_8) {
      op2_8 %= 16;
      result_16 = (op1_16 >> op2_8) | (op1_16 << (16 - op2_8));

      /* now write result back to destination */
      if (op1_type == BX_REGISTER_REF) {
        BX_WRITE_16BIT_REG(op1_addr, result_16);
        }
      else {
        bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
        BX_HANDLE_EXCEPTION()
        }

      /* set eflags:
       * ROR count affects the following flags: C
       */
      result_b15 = result_16 & 0x8000;

      bx_set_CF(result_b15 != 0);
      bx_set_OF(((op1_16 ^ result_16) & 0x8000) > 0);
      }
    }
}

  INLINE void
bx_ROR_Eb1()
{
  Bit8u op1_8, result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  result_8 = (op1_8 >> 1) | (op1_8 << 7);

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }

  /* set eflags:
   * ROR 1 affects the following flags: O,C
   */

  bx_set_OF(((op1_8 ^ result_8) & 0x80) > 0);
  bx_set_CF(op1_8 & 0x01);
}

  INLINE void
bx_ROR_Ev1()
{
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op1_32, result_32;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    result_32 = (op1_32 >> 1) | (op1_32 << 31);

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, result_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * ROR 1 affects the following flags: O,C
     */

    bx_set_OF(((op1_32 ^ result_32) & 0x80000000) > 0);
    bx_set_CF(op1_32 & 0x01);
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit opsize */
    Bit16u op1_16, result_16;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    result_16 = (op1_16 >> 1) | (op1_16 << 15);

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, result_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * ROR 1 affects the following flags: O,C
     */

    bx_set_OF(((op1_16 ^ result_16) & 0x8000) > 0);
    bx_set_CF(op1_16 & 0x01);
    }
}

  INLINE void
bx_ROR_EbCL()
{
  Bit8u op2_8, op1_8, result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;
  Bit8u result_b7;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = CL;
  op2_8 &= 0x1f; /* use only bottom 5 bits */

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  if (op2_8) {
    op2_8 %= 8;
    result_8 = (op1_8 >> op2_8) | (op1_8 << (8 - op2_8));

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_8BIT_REG(op1_addr, result_8);
      }
    else {
      bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * ROR count affects the following flags: C
     */
    result_b7 = result_8 & 0x80;

    bx_set_OF(((op1_8 ^ result_8) & 0x80) > 0);
    bx_set_CF(result_b7 != 0);
    }
}

  INLINE void
bx_ROR_EvCL()
{
  Bit8u op2_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = CL;
  op2_8 &= 0x1f; /* only use 5 bottom bits */

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op1_32, result_32, result_b31;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    if (op2_8) {
      result_32 = (op1_32 >> op2_8) | (op1_32 << (32 - op2_8));

      /* now write result back to destination */
      if (op1_type == BX_REGISTER_REF) {
        BX_WRITE_32BIT_REG(op1_addr, result_32);
        }
      else {
        bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
        BX_HANDLE_EXCEPTION()
        }

      /* set eflags:
       * ROR count affects the following flags: C
       */
      result_b31 = result_32 & 0x80000000;

      bx_set_OF(((op1_32 ^ result_32) & 0x80000000) > 0);
      bx_set_CF(result_b31 != 0);
      }
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit opsize */
    Bit16u op1_16, result_16, result_b15;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    if (op2_8) {
      op2_8 %= 16;
      result_16 = (op1_16 >> op2_8) | (op1_16 << (16 - op2_8));

      /* now write result back to destination */
      if (op1_type == BX_REGISTER_REF) {
        BX_WRITE_16BIT_REG(op1_addr, result_16);
        }
      else {
        bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
        BX_HANDLE_EXCEPTION()
        }

      /* set eflags:
       * ROR count affects the following flags: C
       */
      result_b15 = result_16 & 0x8000;

      bx_set_OF(((op1_16 ^ result_16) & 0x8000) > 0);
      bx_set_CF(result_b15 != 0);
      }
    }
}

  INLINE void
bx_RCL_EbIb()
{
  Bit8u op2_8, op1_8, result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = bx_fetch_next_byte();
  op2_8 = (op2_8 & 0x1F) % 9;
  BX_HANDLE_EXCEPTION()


  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  if (op2_8) {
    result_8 = (op1_8 << op2_8) |
             (bx_get_CF() << (op2_8 - 1)) |
             (op1_8 >> (9 - op2_8));

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_8BIT_REG(op1_addr, result_8);
      }
    else {
      bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * RCL count affects the following flags: C
     */
    bx_set_OF(((op1_8 ^ result_8) & 0x80) > 0);
    bx_set_CF((op1_8 >> (8 - op2_8)) & 0x01);
    }
}

  INLINE void
bx_RCL_EvIb()
{
  Bit8u op2_8;
  Bit16u op1_16, result_16;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = bx_fetch_next_byte();
  op2_8 &= 0x1F;
  BX_HANDLE_EXCEPTION()


#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op1_32, result_32;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    if (op2_8) {
      result_32 = (op1_32 << op2_8) |
                (bx_get_CF() << (op2_8 - 1)) |
                (op1_32 >> (33 - op2_8));

      /* now write result back to destination */
      if (op1_type == BX_REGISTER_REF) {
        BX_WRITE_32BIT_REG(op1_addr, result_32);
        }
      else {
        bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
        BX_HANDLE_EXCEPTION()
        }

      /* set eflags:
       * RCL count affects the following flags: C
       */
      bx_set_OF(((op1_32 ^ result_32) & 0x80000000) > 0);
      bx_set_CF((op1_32 >> (32 - op2_8)) & 0x01);
      }
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit opsize */

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    op2_8 %= 17;
    if (op2_8) {
      result_16 = (op1_16 << op2_8) |
		(bx_get_CF() << (op2_8 - 1)) |
		(op1_16 >> (17 - op2_8));

      /* now write result back to destination */
      if (op1_type == BX_REGISTER_REF) {
	BX_WRITE_16BIT_REG(op1_addr, result_16);
	}
      else {
	bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
        BX_HANDLE_EXCEPTION()
        }

      /* set eflags:
       * RCL count affects the following flags: C
       */

      bx_set_OF(((op1_16 ^ result_16) & 0x8000) > 0);
      bx_set_CF((op1_16 >> (16 - op2_8)) & 0x01);
      }
    }
}

  INLINE void
bx_RCL_Eb1() 
{
  Bit8u op1_8, result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  result_8 = (op1_8 << 1) | bx_get_CF();

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }

  /* set eflags:
   * RCL 1 affects the following flags: O,C
   */

  bx_set_CF(op1_8 >> 7);
  bx_set_OF((result_8 >> 7) ^ bx_get_CF());
}


  INLINE void
bx_RCL_Ev1() 
{
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op1_32, result_32;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    result_32 = (op1_32 << 1) | bx_get_CF();

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, result_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * RCL 1 affects the following flags: O,C
     */

    bx_set_CF(op1_32 >> 31);
    bx_set_OF((result_32 >> 31) ^ bx_get_CF());
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit opsize */
    Bit16u op1_16, result_16;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    result_16 = (op1_16 << 1) | bx_get_CF();

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, result_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * RCL 1 affects the following flags: O,C
     */

    bx_set_CF(op1_16 >> 15);
    bx_set_OF((result_16 >> 15) ^ bx_get_CF());
    }
}


  INLINE void
bx_RCL_EbCL()
{
  Bit8u op2_8, op1_8, result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = (CL & 0x1F) % 9;

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  if (op2_8) {
    result_8 = (op1_8 << op2_8) |
             (bx_get_CF() << (op2_8 - 1)) |
             (op1_8 >> (9 - op2_8));

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_8BIT_REG(op1_addr, result_8);
      }
    else {
      bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * RCL count affects the following flags: C
     */
    bx_set_OF(((op1_8 ^ result_8) & 0x80) > 0);
    bx_set_CF((op1_8 >> (8 - op2_8)) & 0x01);
    }

}

  INLINE void
bx_RCL_EvCL()
{
  Bit8u op2_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = (CL & 0x1F);

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op1_32, result_32;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    if (op2_8) {
      result_32 = (op1_32 << op2_8) |
                (bx_get_CF() << (op2_8 - 1)) |
                (op1_32 >> (33 - op2_8));

      /* now write result back to destination */
      if (op1_type == BX_REGISTER_REF) {
	BX_WRITE_32BIT_REG(op1_addr, result_32);
	}
      else {
	bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
        BX_HANDLE_EXCEPTION()
        }

      /* set eflags:
       * RCL count affects the following flags: C
       */

      bx_set_CF((op1_32 >> (32 - op2_8)) & 0x01);
      bx_set_OF(((op1_32 ^ result_32) & 0x80000000) > 0);
      }
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit opsize */
    Bit16u op1_16, result_16;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    op2_8 %= 17;
    if (op2_8) {
      result_16 = (op1_16 << op2_8) |
		(bx_get_CF() << (op2_8 - 1)) |
		(op1_16 >> (17 - op2_8));

      /* now write result back to destination */
      if (op1_type == BX_REGISTER_REF) {
	BX_WRITE_16BIT_REG(op1_addr, result_16);
	}
      else {
	bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
        BX_HANDLE_EXCEPTION()
        }

      /* set eflags:
       * RCL count affects the following flags: C
       */

      bx_set_CF((op1_16 >> (16 - op2_8)) & 0x01);
      bx_set_OF(((op1_16 ^ result_16) & 0x8000) > 0);
      }
    }
}


  INLINE void
bx_RCR_EbIb()
{
  Bit8u op2_8, op1_8, result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  op2_8 = ( op2_8 & 0x1F ) % 9;

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  if (op2_8) {
    result_8 = (op1_8 >> op2_8) |
             (bx_get_CF() << (8 - op2_8)) |
             (op1_8 << (9 - op2_8));

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_8BIT_REG(op1_addr, result_8);
      }
    else {
      bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * RCR count affects the following flags: C
     */

    bx_set_CF((op1_8 >> (op2_8 - 1)) & 0x01);
    bx_set_OF(((op1_8 ^ result_8) & 0x80) > 0);
    }
}

  INLINE void
bx_RCR_EvIb()
{
  Bit8u op2_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  op2_8 = op2_8 & 0x1F;

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op1_32, result_32;


    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    if (op2_8) {
      result_32 = (op1_32 >> op2_8) |
                (bx_get_CF() << (32 - op2_8)) |
                (op1_32 << (33 - op2_8));

      /* now write result back to destination */
      if (op1_type == BX_REGISTER_REF) {
	BX_WRITE_32BIT_REG(op1_addr, result_32);
	}
      else {
	bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
        BX_HANDLE_EXCEPTION()
        }

      /* set eflags:
       * RCR count affects the following flags: C
       */

      bx_set_CF((op1_32 >> (op2_8 - 1)) & 0x01);
      bx_set_OF(((op1_32 ^ result_32) & 0x80000000) > 0);
      }
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit opsize */
    Bit16u op1_16, result_16;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    op2_8 %= 17;
    if (op2_8) {
      result_16 = (op1_16 >> op2_8) |
		(bx_get_CF() << (16 - op2_8)) |
		(op1_16 << (17 - op2_8));

      /* now write result back to destination */
      if (op1_type == BX_REGISTER_REF) {
	BX_WRITE_16BIT_REG(op1_addr, result_16);
	}
      else {
	bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
        BX_HANDLE_EXCEPTION()
        }

      /* set eflags:
       * RCR count affects the following flags: C
       */

      bx_set_CF((op1_16 >> (op2_8 - 1)) & 0x01);
      bx_set_OF(((op1_16 ^ result_16) & 0x8000) > 0);
      }
    }
}

  INLINE void
bx_RCR_Eb1()
{
  Bit8u op1_8, result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  result_8 = (op1_8 >> 1) | (bx_get_CF() << 7);

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }

  /* set eflags:
   * RCR 1 affects the following flags: O,C
   */

  bx_set_OF((op1_8 >> 7) ^ bx_get_CF());
  bx_set_CF(op1_8 & 0x01);
}

  INLINE void
bx_RCR_Ev1()
{
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op1_32, result_32;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    result_32 = (op1_32 >> 1) | (bx_get_CF() << 31);

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, result_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * RCR 1 affects the following flags: O,C
     */

    bx_set_OF((op1_32 >> 31) ^ bx_get_CF());
    bx_set_CF(op1_32 & 0x01);
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit opsize */
    Bit16u op1_16, result_16;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

      result_16 = (op1_16 >> 1) | (bx_get_CF() << 15);

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, result_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * RCR 1 affects the following flags: O,C
     */

    bx_set_OF((result_16 >> 15) ^ bx_get_CF());
    bx_set_CF(op1_16 & 0x01);
    }
}

  INLINE void
bx_RCR_EbCL()
{
  Bit8u op2_8, op1_8, result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = ( CL & 0x1F ) % 9;

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  if (op2_8) {
    result_8 = (op1_8 >> op2_8) |
             (bx_get_CF() << (8 - op2_8)) |
             (op1_8 << (9 - op2_8));

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_8BIT_REG(op1_addr, result_8);
      }
    else {
      bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * RCR count affects the following flags: C
     */

    bx_set_OF(((op1_8 ^ result_8) & 0x80) > 0);
    bx_set_CF((op1_8 >> (op2_8 - 1)) & 0x01);
    }
}

  INLINE void
bx_RCR_EvCL()
{
  Bit8u op2_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = CL;
  op2_8 &= 0x1F;

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op1_32, result_32;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    if (op2_8) {
      result_32 = (op1_32 >> op2_8) |
                (bx_get_CF() << (32 - op2_8)) |
                (op1_32 << (33 - op2_8));

      /* now write result back to destination */
      if (op1_type == BX_REGISTER_REF) {
	BX_WRITE_32BIT_REG(op1_addr, result_32);
	}
      else {
	bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
        BX_HANDLE_EXCEPTION()
        }

      /* set eflags:
       * RCR count affects the following flags: C
       */

      bx_set_OF(((op1_32 ^ result_32) & 0x80000000) > 0);
      bx_set_CF((op1_32 >> (op2_8 - 1)) & 0x01);
      }
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit opsize */
    Bit16u op1_16, result_16;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    op2_8 %= 17;
    if (op2_8) {
      result_16 = (op1_16 >> op2_8) |
		(bx_get_CF() << (16 - op2_8)) |
		(op1_16 << (17 - op2_8));

      /* now write result back to destination */
      if (op1_type == BX_REGISTER_REF) {
	BX_WRITE_16BIT_REG(op1_addr, result_16);
	}
      else {
	bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
        BX_HANDLE_EXCEPTION()
        }

      /* set eflags:
       * RCR count affects the following flags: C
       */

      bx_set_OF(((op1_16 ^ result_16) & 0x8000) > 0);
      bx_set_CF((op1_16 >> (op2_8 - 1)) & 0x01);
      }
    }
}

  INLINE void
bx_SHL_EbIb()
{
  Bit8u op2_8, op1_8, result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  op2_8 &= 0x1F;

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  if (!op2_8) return;

  result_8 = (op1_8 << op2_8);

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }

  BX_SET_FLAGS_OSZAPC_8(op1_8, op2_8, result_8, BX_INSTR_SHL8);
#if 0
  /* set eflags:
   * SHL count affects the following flags: S,Z,P,C
   */
  if (op2_8 <= 8) {
    bx_set_CF((op1_8 >> (8 - op2_8)) & 0x01);
    }
  else {
    bx_set_CF(0);
    }

  bx_set_ZF(result_8 == 0);
  bx_set_SF(result_8 >> 7);
  bx_set_OF(((op1_8 ^ result_8) & 0x80) > 0);
  bx_set_PF_base(result_8);
#endif
}

  INLINE void
bx_SHL_EvIb()
{
  Bit8u op2_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  op2_8 &= 0x1F; /* use only 5 LSB's */

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op1_32, result_32;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    if (!op2_8) return;

    result_32 = (op1_32 << op2_8);

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, result_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_8, result_32, BX_INSTR_SHL32);
#if 0
    /* set eflags:
     * SHL count affects the following flags: S,Z,P,C
     */
    bx_set_CF((op1_32 >> (32 - op2_8)) & 0x01);
    bx_set_ZF(result_32 == 0);
    bx_set_SF(result_32 >> 31);
    bx_set_OF(((op1_32 ^ result_32) & 0x80000000) > 0);
    bx_set_PF_base(result_32);
#endif
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit opsize */
    Bit16u op1_16, result_16;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    if (!op2_8) return;

    result_16 = (op1_16 << op2_8);

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, result_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_8, result_16, BX_INSTR_SHL16);
#if 0
    /* set eflags:
     * SHL count affects the following flags: S,Z,P,C
     */
    if (op2_8 <= 16) {
      bx_set_CF((op1_16 >> (16 - op2_8)) & 0x01);
      }
    else {
      bx_set_CF(0);
      }

    bx_set_ZF(result_16 == 0);
    bx_set_SF(result_16 >> 15);
    bx_set_OF(((op1_16 ^ result_16) & 0x8000) > 0);
    bx_set_PF_base(result_16);
#endif
    }
}

  INLINE void
bx_SHL_Eb1() 
{
  Bit8u op1_8, result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  result_8 = (op1_8 << 1);

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }

  /* set eflags:
   * SHL 1 affects the following flags: O,S,Z,P,C
   */
  bx_set_CF(op1_8 >> 7);
  bx_set_ZF(result_8 == 0);
  bx_set_SF(result_8 >> 7);
  bx_set_OF(bx_get_SF() != bx_get_CF());
  bx_set_PF_base(result_8);
}

  INLINE void
bx_SHL_Ev1() 
{
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op1_32, result_32;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    result_32 = (op1_32 << 1);

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, result_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * SHL 1 affects the following flags: O,S,Z,P,C
     */

    bx_set_CF(op1_32 >> 31);
    bx_set_ZF(result_32 == 0);
    bx_set_SF(result_32 >> 31);
    bx_set_OF(bx_get_SF() != bx_get_CF());
    bx_set_PF_base(result_32);
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit opsize */
    Bit16u op1_16, result_16;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    result_16 = (op1_16 << 1);

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, result_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * SHL 1 affects the following flags: O,S,Z,P,C
     */

    bx_set_CF(op1_16 >> 15);
    bx_set_ZF(result_16 == 0);
    bx_set_SF(result_16 >> 15);
    bx_set_OF(bx_get_SF() != bx_get_CF());
    bx_set_PF_base(result_16);
    }
}

  INLINE void
bx_SHL_EbCL()
{
  Bit8u op2_8, op1_8, result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = CL;
  op2_8 &= 0x1F;

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  if (!op2_8) return;

  result_8 = (op1_8 << op2_8);

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }

  BX_SET_FLAGS_OSZAPC_8(op1_8, op2_8, result_8, BX_INSTR_SHL8);
#if 0
  /* set eflags:
   * SHL count affects the following flags: S,Z,P,Z
   */
  if (op2_8 <= 8) {
    bx_set_CF((op1_8 >> (8 - op2_8)) & 0x01);
    }
  else {
    bx_set_CF(0);
    }
  bx_set_ZF(result_8 == 0);
  bx_set_SF(result_8 >> 7);
  bx_set_OF(((op1_8 ^ result_8) & 0x80) > 0);
  bx_set_PF_base(result_8);
#endif
}

  INLINE void
bx_SHL_EvCL()
{
  Bit8u op2_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = CL;
  op2_8 &= 0x1F; /* use only 5 LSB's */

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op1_32, result_32;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    if (!op2_8) return;

    result_32 = (op1_32 << op2_8);

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, result_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_8, result_32, BX_INSTR_SHL32);
#if 0
    /* set eflags:
     * SHL count affects the following flags: S,Z,P,C
     */
    if (op2_8 <= 32) {
      bx_set_CF((op1_32 >> (32 - op2_8)) & 0x01);
      }
    else {
      bx_set_CF(0);
      }

    bx_set_ZF(result_32 == 0);
    bx_set_SF(result_32 >> 31);
    bx_set_OF(((op1_32 ^ result_32) & 0x80000000) > 0);
    bx_set_PF_base(result_32);
#endif
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit opsize */
    Bit16u op1_16, result_16;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }


    if (!op2_8) return;


    result_16 = (op1_16 << op2_8);

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, result_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_8, result_16, BX_INSTR_SHL16);
#if 0
    /* set eflags:
     * SHL count affects the following flags: S,Z,P,C
     */
    if (op2_8 <= 16) {
      bx_set_CF((op1_16 >> (16 - op2_8)) & 0x01);
      }
    else {
      bx_set_CF(0);
      }

    bx_set_ZF(result_16 == 0);
    bx_set_SF(result_16 >> 15);
    bx_set_OF(((op1_16 ^ result_16) & 0x8000) > 0);
    bx_set_PF_base(result_16);
#endif
    }
}


  INLINE void
bx_SHR_EbIb()
{
  Bit8u op2_8, op1_8, result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  op2_8 &= 0x1F;

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  if (!op2_8) return;

  result_8 = (op1_8 >> op2_8);

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }

  BX_SET_FLAGS_OSZAPC_8(op1_8, op2_8, result_8, BX_INSTR_SHR8);
}


  INLINE void
bx_SHR_EvIb()
{
  Bit8u op2_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  op2_8 &= 0x1F; /* use only 5 LSB's */

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op1_32, result_32;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    if (!op2_8) return;

    result_32 = (op1_32 >> op2_8);

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, result_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_8, result_32, BX_INSTR_SHR32);
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit opsize */
    Bit16u op1_16, result_16;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    if (!op2_8) return;

    result_16 = (op1_16 >> op2_8);


    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, result_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_8, result_16, BX_INSTR_SHR16);
    }
}

  INLINE void
bx_SHR_Eb1() 
{
  Bit8u op1_8, result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  result_8 = (op1_8 >> 1);

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }

  /* set eflags:
   * SHR 1 affects the following flags: O,S,Z,P,C
   */

  bx_set_CF(op1_8 & 0x01);
  bx_set_ZF(result_8 == 0);
  bx_set_SF(result_8 >> 7);
  bx_set_OF(op1_8 >> 7);
  bx_set_PF_base(result_8);
}

  INLINE void
bx_SHR_Ev1() 
{
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op1_32, result_32;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    result_32 = (op1_32 >> 1);


    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, result_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * SHR 1 affects the following flags: O,S,Z,P,C
     */

    bx_set_CF(op1_32 & 0x01);
    bx_set_ZF(result_32 == 0);
    bx_set_SF(result_32 >> 31);
    bx_set_OF(op1_32 >> 31);
    bx_set_PF_base(result_32);
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit opsize */
    Bit16u op1_16, result_16;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    result_16 = (op1_16 >> 1);


    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, result_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * SHR 1 affects the following flags: O,S,Z,P,C
     */

    bx_set_CF(op1_16 & 0x01);
    bx_set_ZF(result_16 == 0);
    bx_set_SF(result_16 >> 15);
    bx_set_OF(op1_16 >> 15);
    bx_set_PF_base(result_16);
    }
}

  INLINE void
bx_SHR_EbCL()
{
  Bit8u op2_8, op1_8, result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = CL;
  op2_8 &= 0x1F;

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  if (!op2_8) return;

  result_8 = (op1_8 >> op2_8);

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }

  BX_SET_FLAGS_OSZAPC_8(op1_8, op2_8, result_8, BX_INSTR_SHR8);
}

  INLINE void
bx_SHR_EvCL()
{
  Bit8u op2_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = CL;
  op2_8 &= 0x1F; /* use only 5 LSB's */

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op1_32, result_32;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    if (!op2_8) return;

    result_32 = (op1_32 >> op2_8);

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, result_32);
      }
    else { 
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_8, result_32, BX_INSTR_SHR32);
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit opsize */
    Bit16u op1_16, result_16;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    if (!op2_8) return;

    result_16 = (op1_16 >> op2_8);


    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, result_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_8, result_16, BX_INSTR_SHR16);
    }
}


  INLINE void
bx_SAR_EbIb()
{
  Bit8u op2_8, op1_8, result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  op2_8 &= 0x1F;

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  if (!op2_8) return;

  if (op2_8 < 8) {
    if (op1_8 & 0x80) {
      result_8 = (op1_8 >> op2_8) | (0xff << (8 - op2_8));
      }
    else {
      result_8 = (op1_8 >> op2_8);
      }
    }
  else {
    if (op1_8 & 0x80) {
      result_8 = 0xff;
      }
    else {
      result_8 = 0;
      }
    }

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }

  /* set eflags:
   * SAR count affects the following flags: S,Z,P,C
   */

  if (op2_8 < 8) {
    bx_set_CF((op1_8 >> (op2_8 - 1)) & 0x01);
    }
  else {
    if (op1_8 & 0x80) {
      bx_set_CF(1);
      }
    else {
      bx_set_CF(0);
      }
    }

  bx_set_ZF(result_8 == 0);
  bx_set_SF(result_8 >> 7);
  bx_set_OF(0);
  bx_set_PF_base(result_8);
}

  INLINE void
bx_SAR_EvIb()
{
  Bit8u op2_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;

  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  op2_8 &= 0x1F;  /* use only 5 LSB's */

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op1_32, result_32;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    if (!op2_8) return;

    /* op2_8 < 32, since only lower 5 bits used */
    if (op1_32 & 0x80000000) {
      result_32 = (op1_32 >> op2_8) | (0xffffffff << (32 - op2_8));
      }
    else {
      result_32 = (op1_32 >> op2_8);
      }

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, result_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * SAR count affects the following flags: S,Z,P,C
     */

    bx_set_CF((op1_32 >> (op2_8 - 1)) & 0x01);
    bx_set_ZF(result_32 == 0);
    bx_set_SF(result_32 >> 31);
    bx_set_OF(0);
    bx_set_PF_base(result_32);
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit opsize */
    Bit16u op1_16, result_16;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    if (!op2_8) return;

    if (op2_8 < 16) {
      if (op1_16 & 0x8000) {
	result_16 = (op1_16 >> op2_8) | (0xffff << (16 - op2_8));
	}
      else {
	result_16 = (op1_16 >> op2_8);
	}
      }
    else {
      if (op1_16 & 0x8000) {
	result_16 = 0xffff;
	}
      else {
	result_16 = 0;
	}
      }



    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, result_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * SAR count affects the following flags: S,Z,P,C
     */
    if (op2_8 < 16) {
      bx_set_CF((op1_16 >> (op2_8 - 1)) & 0x01);
      }
    else {
      if (op1_16 & 0x8000) {
	bx_set_CF(1);
	}
      else {
	bx_set_CF(0);
	}
      }

    bx_set_ZF(result_16 == 0);
    bx_set_SF(result_16 >> 15);
    bx_set_OF(0);
    bx_set_PF_base(result_16);
    }
}

  INLINE void
bx_SAR_Eb1() 
{
  Bit8u op1_8, result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }


  if (op1_8 & 0x80) {
    result_8 = (op1_8 >> 1) | 0x80;
    }
  else {
    result_8 = (op1_8 >> 1);
    }

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }

  /* set eflags:
   * SAR 1 affects the following flags: O,S,Z,P,C
   */

  bx_set_CF(op1_8 & 0x01);
  bx_set_ZF(result_8 == 0);
  bx_set_OF(0);
  bx_set_SF(result_8 >> 7);
  bx_set_PF_base(result_8);
}

  INLINE void
bx_SAR_Ev1() 
{
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op1_32, result_32;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    if (op1_32 & 0x80000000) {
      result_32 = (op1_32 >> 1) | 0x80000000;
      }
    else {
      result_32 = (op1_32 >> 1);
      }

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, result_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * SAR 1 affects the following flags: O,S,Z,P,C
     */

    bx_set_CF(op1_32 & 0x01);
    bx_set_ZF(result_32 == 0);
    bx_set_OF(0);
    bx_set_SF(result_32 >> 31);
    bx_set_PF_base(result_32);
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit opsize */
    Bit16u op1_16, result_16;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    if (op1_16 & 0x8000)
      result_16 = (op1_16 >> 1) | 0x8000;
    else
      result_16 = (op1_16 >> 1);

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, result_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * SAR 1 affects the following flags: O,S,Z,P,C
     */

    bx_set_CF(op1_16 & 0x01);
    bx_set_ZF(result_16 == 0);
    bx_set_OF(0);
    bx_set_SF(result_16 >> 15);
    bx_set_PF_base(result_16);
    }
}

  INLINE void
bx_SAR_EbCL()
{
  Bit8u op2_8, op1_8, result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = CL;
  op2_8 &= 0x1F;

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  if (!op2_8) return;

  if (op2_8 < 8) {
    if (op1_8 & 0x80) {
      result_8 = (op1_8 >> op2_8) | (0xff << (8 - op2_8));
      }
    else {
      result_8 = (op1_8 >> op2_8);
      }
    }
  else {
    if (op1_8 & 0x80) {
      result_8 = 0xff;
      }
    else {
      result_8 = 0;
      }
    }


  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }

  /* set eflags:
   * SAR count affects the following flags: S,Z,P,C
   */
  if (op2_8 < 8) {
    bx_set_CF((op1_8 >> (op2_8 - 1)) & 0x01);
    }
  else {
    if (op1_8 & 0x80) {
      bx_set_CF(1);
      }
    else {
      bx_set_CF(0);
      }
    }

  bx_set_SF(result_8 >> 7);
  bx_set_ZF(result_8 == 0);
  bx_set_OF(0);
  bx_set_PF_base(result_8);
}

  INLINE void
bx_SAR_EvCL()
{
  Bit8u op2_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = CL;
  op2_8 &= 0x1F;  /* use only 5 LSB's */

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op1_32, result_32;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    if (!op2_8) return;

    /* op2_8 < 32, since only lower 5 bits used */
    if (op1_32 & 0x80000000) {
      result_32 = (op1_32 >> op2_8) | (0xffffffff << (32 - op2_8));
      }
    else {
      result_32 = (op1_32 >> op2_8);
      }

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, result_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * SAR count affects the following flags: S,Z,P,C
     */

    bx_set_CF((op1_32 >> (op2_8 - 1)) & 0x01);
    bx_set_ZF(result_32 == 0);
    bx_set_SF(result_32 >> 31);
    bx_set_OF(0);
    bx_set_PF_base(result_32);
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit opsize */
    Bit16u op1_16, result_16;

    /* op1 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    if (!op2_8) return;

    if (op2_8 < 16) {
      if (op1_16 & 0x8000) {
	result_16 = (op1_16 >> op2_8) | (0xffff << (16 - op2_8));
	}
      else {
	result_16 = (op1_16 >> op2_8);
	}
      }
    else {
      if (op1_16 & 0x8000) {
	result_16 = 0xffff;
	}
      else {
	result_16 = 0;
	}
      }

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, result_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
      BX_HANDLE_EXCEPTION()
      }

    /* set eflags:
     * SAR count affects the following flags: S,Z,P,C
     */
    if (op2_8 < 16) {
      bx_set_CF((op1_16 >> (op2_8 - 1)) & 0x01);
      }
    else {
      if (op1_16 & 0x8000) {
	bx_set_CF(1);
	}
      else {
	bx_set_CF(0);
	}
      }

    bx_set_ZF(result_16 == 0);
    bx_set_SF(result_16 >> 15);
    bx_set_OF(0);
    bx_set_PF_base(result_16);
    }
}

#pragma warning(default:4761) // 'integral size mismatch; conversion supplied'
#pragma warning(default:4244) // 'conversion from unsigned short to unsigned char'
