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



#define BX_ARITH_C 1

#include "bochs.h"


  INLINE void
bx_ADD_EbGb()
{
  Bit32u op1_addr;
  unsigned op2_addr, op1_type;
  bx_segment_reg_t *op1_seg_reg;

  Bit8u op2, op1, sum;

  bx_decode_exgx(&op2_addr, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op2 is a register, op2_addr is an index of a register */
  op2 = BX_READ_8BIT_REG(op2_addr);

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1);
    BX_HANDLE_EXCEPTION()
    }

  sum = op1 + op2;

  /* now write sum back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, sum);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &sum);
    BX_HANDLE_EXCEPTION()
    }

  BX_SET_FLAGS_OSZAPC_8(op1, op2, sum, BX_INSTR_ADD8);
}


  INLINE void
bx_ADD_EvGv()
{
  Bit32u op1_addr;
  unsigned op1_type, op2_addr;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&op2_addr, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, sum_32;

    /* op2_32 is a register, op2_addr is an index of a register */
    op2_32 = BX_READ_32BIT_REG(op2_addr);

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    sum_32 = op1_32 + op2_32;

    /* now write sum back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, sum_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &sum_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_ADD32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, sum_16;

    /* op2_16 is a register, op2_addr is an index of a register */
    op2_16 = BX_READ_16BIT_REG(op2_addr);

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    sum_16 = op1_16 + op2_16;

    /* now write sum back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, sum_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &sum_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, sum_16, BX_INSTR_ADD16);
    }
}


  INLINE void
bx_ADD_GbEb()
{
  Bit8u op1, op2, sum;
  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;



  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op1 is a register, op1_addr is an index of a register */
  op1 = BX_READ_8BIT_REG(op1_addr);

  /* op2 is a register or memory reference */
  if (op2_type == BX_REGISTER_REF) {
    op2 = BX_READ_8BIT_REG(op2_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op2_seg_reg, op2_addr, &op2);
    BX_HANDLE_EXCEPTION()
    }

  sum = op1 + op2;

  /* now write sum back to destination, which is a register */
  BX_WRITE_8BIT_REG(op1_addr, sum);

  BX_SET_FLAGS_OSZAPC_8(op1, op2, sum, BX_INSTR_ADD8);
}

  INLINE void
bx_ADD_GvEv()
{
  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;


  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, sum_32;

    /* op1_32 is a register, op1_addr is an index of a register */
    op1_32 = BX_READ_32BIT_REG(op1_addr);

    /* op2_32 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_32 = BX_READ_32BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op2_seg_reg, op2_addr, &op2_32);
      BX_HANDLE_EXCEPTION()
      }

    sum_32 = op1_32 + op2_32;

    /* now write sum back to destination */
    BX_WRITE_32BIT_REG(op1_addr, sum_32);

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_ADD32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16, sum_16;


    /* op1_16 is a register, op1_addr is an index of a register */
    op1_16 = BX_READ_16BIT_REG(op1_addr);

    /* op2_16 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_16 = BX_READ_16BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op2_seg_reg, op2_addr, &op2_16);
      BX_HANDLE_EXCEPTION()
      }

    sum_16 = op1_16 + op2_16;
    /* now write sum back to destination */

    BX_WRITE_16BIT_REG(op1_addr, sum_16);

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, sum_16, BX_INSTR_ADD16);
    }
}

  INLINE void
bx_ADD_ALIb()
{
  Bit8u op1, op2, sum;


  op1 = AL;

  op2 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  sum = op1 + op2;

  /* now write sum back to destination, which is a register */
  AL = sum;

  BX_SET_FLAGS_OSZAPC_8(op1, op2, sum, BX_INSTR_ADD8);
}

  INLINE void
bx_ADD_eAXIv()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, sum_32;

    op1_32 = EAX;

    op2_32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()

    sum_32 = op1_32 + op2_32;

    /* now write sum back to destination */
    EAX = sum_32;

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_ADD32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16, sum_16;

    op1_16 = AX;

    op2_16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    sum_16 = op1_16 + op2_16;

    /* now write sum back to destination */
    AX = sum_16;

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, sum_16, BX_INSTR_ADD16);
    }
}

  INLINE void
bx_ADC_EbGb()
{
  Bit8u op2, op1, sum;
  Bit32u op1_addr;
  unsigned op1_type, op2_addr;
  bx_segment_reg_t *op1_seg_reg;
  Boolean temp_CF;

  temp_CF = bx_get_CF();


  bx_decode_exgx(&op2_addr, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op2 is a register, op2_addr is an index of a register */
  op2 = BX_READ_8BIT_REG(op2_addr);

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1);
    BX_HANDLE_EXCEPTION()
    }

  sum = op1 + op2 + temp_CF;


  /* now write sum back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, sum);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &sum);
    BX_HANDLE_EXCEPTION()
    }

  BX_SET_FLAGS_OSZAPC_8_CF(op1, op2, sum, BX_INSTR_ADC8, temp_CF);
}

  INLINE void
bx_ADC_EvGv()
{
  Bit32u op1_addr;
  unsigned op1_type, op2_addr;
  bx_segment_reg_t *op1_seg_reg;
  Boolean temp_CF;

  temp_CF = bx_get_CF();


  bx_decode_exgx(&op2_addr, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, sum_32;

    /* op2_32 is a register, op2_addr is an index of a register */
    op2_32 = BX_READ_32BIT_REG(op2_addr);

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    sum_32 = op1_32 + op2_32 + temp_CF;

    /* now write sum back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, sum_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &sum_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_32_CF(op1_32, op2_32, sum_32, BX_INSTR_ADC32,
                              temp_CF);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, sum_16;


    /* op2_16 is a register, op2_addr is an index of a register */
    op2_16 = BX_READ_16BIT_REG(op2_addr);

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    sum_16 = op1_16 + op2_16 + temp_CF;

    /* now write sum back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, sum_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &sum_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_16_CF(op1_16, op2_16, sum_16, BX_INSTR_ADC16,
                              temp_CF);
    }
}

  INLINE void
bx_ADC_GbEb()
{
  Bit8u op1, op2, sum;
  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;
  Boolean temp_CF;

  temp_CF = bx_get_CF();


  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op1 is a register, op1_addr is an index of a register */
  op1 = BX_READ_8BIT_REG(op1_addr);

  /* op2 is a register or memory reference */
  if (op2_type == BX_REGISTER_REF) {
    op2 = BX_READ_8BIT_REG(op2_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op2_seg_reg, op2_addr, &op2);
    BX_HANDLE_EXCEPTION()
    }

  sum = op1 + op2 + temp_CF;

  BX_SET_FLAGS_OSZAPC_8_CF(op1, op2, sum, BX_INSTR_ADC8,
                           temp_CF);

  /* now write sum back to destination, which is a register */
  BX_WRITE_8BIT_REG(op1_addr, sum);
}

  INLINE void
bx_ADC_GvEv()
{
  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;
  Boolean temp_CF;

  temp_CF = bx_get_CF();


  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, sum_32;

    /* op1_32 is a register, op1_addr is an index of a register */
    op1_32 = BX_READ_32BIT_REG(op1_addr);

    /* op2_32 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_32 = BX_READ_32BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op2_seg_reg, op2_addr, &op2_32);
      BX_HANDLE_EXCEPTION()
      }

    sum_32 = op1_32 + op2_32 + temp_CF;

    /* now write sum back to destination */
    BX_WRITE_32BIT_REG(op1_addr, sum_32);

    BX_SET_FLAGS_OSZAPC_32_CF(op1_32, op2_32, sum_32, BX_INSTR_ADC32,
                             temp_CF);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16, sum_16;


    /* op1_16 is a register, op1_addr is an index of a register */
    op1_16 = BX_READ_16BIT_REG(op1_addr);

    /* op2_16 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_16 = BX_READ_16BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op2_seg_reg, op2_addr, &op2_16);
      BX_HANDLE_EXCEPTION()
      }

    sum_16 = op1_16 + op2_16 + temp_CF;

    /* now write sum back to destination */
    BX_WRITE_16BIT_REG(op1_addr, sum_16);

    BX_SET_FLAGS_OSZAPC_16_CF(op1_16, op2_16, sum_16, BX_INSTR_ADC16,
                             temp_CF);
    }
}

  INLINE void
bx_ADC_ALIb()
{
  Bit8u op1, op2, sum;
  Boolean temp_CF;

  temp_CF = bx_get_CF();


  op1 = AL;

  op2 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  sum = op1 + op2 + temp_CF;

  /* now write sum back to destination, which is a register */
  AL = sum;

  BX_SET_FLAGS_OSZAPC_8_CF(op1, op2, sum, BX_INSTR_ADC8,
                           temp_CF);
}

  INLINE void
bx_ADC_eAXIv()
{
  Boolean temp_CF;

  temp_CF = bx_get_CF();

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, sum_32;

    op1_32 = EAX;

    op2_32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()

    sum_32 = op1_32 + op2_32 + temp_CF;

    /* now write sum back to destination */
    EAX = sum_32;

    BX_SET_FLAGS_OSZAPC_32_CF(op1_32, op2_32, sum_32, BX_INSTR_ADC32,
                           temp_CF);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16, sum_16;

    op1_16 = AX;

    op2_16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    sum_16 = op1_16 + op2_16 + temp_CF;

    /* now write sum back to destination */
    AX = sum_16;

    BX_SET_FLAGS_OSZAPC_16_CF(op1_16, op2_16, sum_16, BX_INSTR_ADC16,
                           temp_CF);
    }
}

  INLINE void
bx_SBB_EbGb()
{
  Bit8u op2_8, op1_8, diff_8;
  Bit32u op1_addr;
  unsigned op1_type, op2_addr;
  bx_segment_reg_t *op1_seg_reg;
  Boolean temp_CF;

  temp_CF = bx_get_CF();


  bx_decode_exgx(&op2_addr, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op2 is a register, op2_addr is an index of a register */
  op2_8 = BX_READ_8BIT_REG(op2_addr);

  /* op1_8 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  diff_8 = op1_8 - (op2_8 + temp_CF);

  /* now write diff back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, diff_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &diff_8);
    BX_HANDLE_EXCEPTION()
    }

  BX_SET_FLAGS_OSZAPC_8_CF(op1_8, op2_8, diff_8, BX_INSTR_SBB8,
                           temp_CF);
}

  INLINE void
bx_SBB_EvGv()
{
  Bit32u op1_addr;
  unsigned op1_type, op2_addr;
  bx_segment_reg_t *op1_seg_reg;
  Boolean temp_CF;

  temp_CF = bx_get_CF();


  bx_decode_exgx(&op2_addr, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, diff_32;

    /* op2_32 is a register, op2_addr is an index of a register */
    op2_32 = BX_READ_32BIT_REG(op2_addr);

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    diff_32 = op1_32 - (op2_32 + temp_CF);

    /* now write diff back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, diff_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &diff_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_32_CF(op1_32, op2_32, diff_32, BX_INSTR_SBB32,
                              temp_CF);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, diff_16;


    /* op2_16 is a register, op2_addr is an index of a register */
    op2_16 = BX_READ_16BIT_REG(op2_addr);

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    diff_16 = op1_16 - (op2_16 + temp_CF);

    /* now write diff back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, diff_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &diff_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_16_CF(op1_16, op2_16, diff_16, BX_INSTR_SBB16,
                              temp_CF);
    }
}

  INLINE void
bx_SBB_GbEb()
{
  Bit8u op1_8, op2_8, diff_8;
  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;
  Boolean temp_CF;

  temp_CF = bx_get_CF();


  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op1 is a register, op1_addr is an index of a register */
  op1_8 = BX_READ_8BIT_REG(op1_addr);

  /* op2 is a register or memory reference */
  if (op2_type == BX_REGISTER_REF) {
    op2_8 = BX_READ_8BIT_REG(op2_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op2_seg_reg, op2_addr, &op2_8);
    BX_HANDLE_EXCEPTION()
    }

  diff_8 = op1_8 - (op2_8 + temp_CF);

  /* now write diff back to destination, which is a register */
  BX_WRITE_8BIT_REG(op1_addr, diff_8);

  BX_SET_FLAGS_OSZAPC_8_CF(op1_8, op2_8, diff_8, BX_INSTR_SBB8,
                           temp_CF);
}

  INLINE void
bx_SBB_GvEv()
{
  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;
  Boolean temp_CF;

  temp_CF = bx_get_CF();


  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, diff_32;

    /* op1_32 is a register, op1_addr is an index of a register */
    op1_32 = BX_READ_32BIT_REG(op1_addr);

    /* op2_32 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_32 = BX_READ_32BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op2_seg_reg, op2_addr, &op2_32);
      BX_HANDLE_EXCEPTION()
      }

    diff_32 = op1_32 - (op2_32 + temp_CF);

    /* now write diff back to destination */
    BX_WRITE_32BIT_REG(op1_addr, diff_32);

    BX_SET_FLAGS_OSZAPC_32_CF(op1_32, op2_32, diff_32, BX_INSTR_SBB32,
                              temp_CF);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16, diff_16;


    /* op1_16 is a register, op1_addr is an index of a register */
    op1_16 = BX_READ_16BIT_REG(op1_addr);

    /* op2_16 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_16 = BX_READ_16BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op2_seg_reg, op2_addr, &op2_16);
      BX_HANDLE_EXCEPTION()
      }

    diff_16 = op1_16 - (op2_16 + temp_CF);

    /* now write diff back to destination */
    BX_WRITE_16BIT_REG(op1_addr, diff_16);

    BX_SET_FLAGS_OSZAPC_16_CF(op1_16, op2_16, diff_16, BX_INSTR_SBB16,
                              temp_CF);
    }
}

  INLINE void
bx_SBB_ALIb()
{
  Bit8u op1_8, op2_8, diff_8;
  Boolean temp_CF;

  temp_CF = bx_get_CF();


  op1_8 = AL;

  op2_8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  diff_8 = op1_8 - (op2_8 + temp_CF);

  /* now write diff back to destination, which is a register */
  AL = diff_8;

  BX_SET_FLAGS_OSZAPC_8_CF(op1_8, op2_8, diff_8, BX_INSTR_SBB8,
                           temp_CF);
}

  INLINE void
bx_SBB_eAXIv()
{
  Boolean temp_CF;

  temp_CF = bx_get_CF();

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, diff_32;

    op1_32 = EAX;

    op2_32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()

    diff_32 = op1_32 - (op2_32 + temp_CF);

    /* now write diff back to destination */
    EAX = diff_32;

    BX_SET_FLAGS_OSZAPC_32_CF(op1_32, op2_32, diff_32, BX_INSTR_SBB32,
                              temp_CF);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16, diff_16;

    op1_16 = AX;

    op2_16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    diff_16 = op1_16 - (op2_16 + temp_CF);

    /* now write diff back to destination */
    AX = diff_16;

    BX_SET_FLAGS_OSZAPC_16_CF(op1_16, op2_16, diff_16, BX_INSTR_SBB16,
                              temp_CF);
    }
}


  INLINE void
bx_SBB_EbIb()
{
  Bit8u op2_8, op1_8, diff_8;
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;
  Boolean temp_CF;

  temp_CF = bx_get_CF();


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  /* op1_8 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  diff_8 = op1_8 - (op2_8 + temp_CF);

  /* now write diff back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, diff_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &diff_8);
    BX_HANDLE_EXCEPTION()
    }

  BX_SET_FLAGS_OSZAPC_8_CF(op1_8, op2_8, diff_8, BX_INSTR_SBB8,
                           temp_CF);
}


  INLINE void
bx_SBB_EvIv()
{
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;
  Boolean temp_CF;

  temp_CF = bx_get_CF();


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, diff_32;

    op2_32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    diff_32 = op1_32 - (op2_32 + temp_CF);

    /* now write diff back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, diff_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &diff_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_32_CF(op1_32, op2_32, diff_32, BX_INSTR_SBB32,
                              temp_CF);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, diff_16;


    op2_16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    diff_16 = op1_16 - (op2_16 + temp_CF);

    /* now write diff back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, diff_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &diff_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_16_CF(op1_16, op2_16, diff_16, BX_INSTR_SBB16,
                              temp_CF);
    }
}

  INLINE void
bx_SBB_EvIb()
{
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;
  Boolean temp_CF;

  temp_CF = bx_get_CF();


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, diff_32;

    op2_32 = (Bit8s) bx_fetch_next_byte();
    BX_HANDLE_EXCEPTION()

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    diff_32 = op1_32 - (op2_32 + temp_CF);

    /* now write diff back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, diff_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &diff_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_32_CF(op1_32, op2_32, diff_32, BX_INSTR_SBB32,
                              temp_CF);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, diff_16;


    op2_16 = (Bit8s) bx_fetch_next_byte();
    BX_HANDLE_EXCEPTION()

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    diff_16 = op1_16 - (op2_16 + temp_CF);

    /* now write diff back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, diff_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &diff_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_16_CF(op1_16, op2_16, diff_16, BX_INSTR_SBB16,
                              temp_CF);
    }
}



  INLINE void
bx_SUB_EbGb()
{
  Bit8u op2_8, op1_8, diff_8;
  Bit32u op1_addr;
  unsigned op2_addr, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&op2_addr, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op2 is a register, op2_addr is an index of a register */
  op2_8 = BX_READ_8BIT_REG(op2_addr);

  /* op1_8 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  diff_8 = op1_8 - op2_8;

  /* now write diff back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, diff_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &diff_8);
    BX_HANDLE_EXCEPTION()
    }

  BX_SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_SUB8);
}

  INLINE void
bx_SUB_EvGv()
{
  Bit32u op1_addr;
  unsigned op1_type, op2_addr;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&op2_addr, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, diff_32;

    /* op2_32 is a register, op2_addr is an index of a register */
    op2_32 = BX_READ_32BIT_REG(op2_addr);

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    diff_32 = op1_32 - op2_32;

    /* now write diff back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, diff_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &diff_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SUB32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, diff_16;


    /* op2_16 is a register, op2_addr is an index of a register */
    op2_16 = BX_READ_16BIT_REG(op2_addr);

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    diff_16 = op1_16 - op2_16;

    /* now write diff back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, diff_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &diff_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_SUB16);
    }
}

  INLINE void
bx_SUB_GbEb()
{
  Bit8u op1_8, op2_8, diff_8;
  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;


  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op1 is a register, op1_addr is an index of a register */
  op1_8 = BX_READ_8BIT_REG(op1_addr);

  /* op2 is a register or memory reference */
  if (op2_type == BX_REGISTER_REF) {
    op2_8 = BX_READ_8BIT_REG(op2_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op2_seg_reg, op2_addr, &op2_8);
    BX_HANDLE_EXCEPTION()
    }

  diff_8 = op1_8 - op2_8;

  /* now write diff back to destination, which is a register */
  BX_WRITE_8BIT_REG(op1_addr, diff_8);

  BX_SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_SUB8);
}

  INLINE void
bx_SUB_GvEv()
{
  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;


  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, diff_32;

    /* op1_32 is a register, op1_addr is an index of a register */
    op1_32 = BX_READ_32BIT_REG(op1_addr);

    /* op2_32 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_32 = BX_READ_32BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op2_seg_reg, op2_addr, &op2_32);
      BX_HANDLE_EXCEPTION()
      }

    diff_32 = op1_32 - op2_32;

    /* now write diff back to destination */
    BX_WRITE_32BIT_REG(op1_addr, diff_32);

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SUB32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16, diff_16;


    /* op1_16 is a register, op1_addr is an index of a register */
    op1_16 = BX_READ_16BIT_REG(op1_addr);

    /* op2_16 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_16 = BX_READ_16BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op2_seg_reg, op2_addr, &op2_16);
      BX_HANDLE_EXCEPTION()
      }

    diff_16 = op1_16 - op2_16;

    /* now write diff back to destination */
    BX_WRITE_16BIT_REG(op1_addr, diff_16);

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_SUB16);
    }
}

  INLINE void
bx_SUB_ALIb()
{
  Bit8u op1_8, op2_8, diff_8;

  op1_8 = AL;

  op2_8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  diff_8 = op1_8 - op2_8;

  /* now write diff back to destination, which is a register */
  AL = diff_8;

  BX_SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_SUB8);
}

  INLINE void
bx_SUB_eAXIv()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, diff_32;

    op1_32 = EAX;

    op2_32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()

    diff_32 = op1_32 - op2_32;

    /* now write diff back to destination */
    EAX = diff_32;

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SUB32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16, diff_16;

    op1_16 = AX;

    op2_16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    diff_16 = op1_16 - op2_16;


    /* now write diff back to destination */
    AX = diff_16;

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_SUB16);
    }
}


  INLINE void
bx_CMP_EbGb()
{
  Bit8u op2_8, op1_8, diff_8;
  Bit32u op1_addr;
  unsigned op1_type, op2_addr;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&op2_addr, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op2 is a register, op2_addr is an index of a register */
  op2_8 = BX_READ_8BIT_REG(op2_addr);

  /* op1_8 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  diff_8 = op1_8 - op2_8;

  BX_SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_CMP8);
}

  INLINE void
bx_CMP_EvGv()
{
  Bit32u op1_addr;
  unsigned op1_type, op2_addr;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&op2_addr, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, diff_32;

    /* op2_32 is a register, op2_addr is an index of a register */
    op2_32 = BX_READ_32BIT_REG(op2_addr);

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    diff_32 = op1_32 - op2_32;

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_CMP32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, diff_16;


    /* op2_16 is a register, op2_addr is an index of a register */
    op2_16 = BX_READ_16BIT_REG(op2_addr);

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    diff_16 = op1_16 - op2_16;

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_CMP16);
    }
}

  INLINE void
bx_CMP_GbEb()
{
  Bit8u op1_8, op2_8, diff_8;
  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;


  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op1 is a register, op1_addr is an index of a register */
  op1_8 = BX_READ_8BIT_REG(op1_addr);

  /* op2 is a register or memory reference */
  if (op2_type == BX_REGISTER_REF) {
    op2_8 = BX_READ_8BIT_REG(op2_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op2_seg_reg, op2_addr, &op2_8);
    BX_HANDLE_EXCEPTION()
    }

  diff_8 = op1_8 - op2_8;

  BX_SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_CMP8);
}

  INLINE void
bx_CMP_GvEv()
{
  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;


  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, diff_32;

    /* op1_32 is a register, op1_addr is an index of a register */
    op1_32 = BX_READ_32BIT_REG(op1_addr);

    /* op2_32 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_32 = BX_READ_32BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op2_seg_reg, op2_addr, &op2_32);
      BX_HANDLE_EXCEPTION()
      }

    diff_32 = op1_32 - op2_32;

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_CMP32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16, diff_16;


    /* op1_16 is a register, op1_addr is an index of a register */
    op1_16 = BX_READ_16BIT_REG(op1_addr);

    /* op2_16 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_16 = BX_READ_16BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op2_seg_reg, op2_addr, &op2_16);
      BX_HANDLE_EXCEPTION()
      }

    diff_16 = op1_16 - op2_16;

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_CMP16);
    }
}


  INLINE void
bx_CMP_ALIb()
{
  Bit8u op1_8, op2_8, diff_8;


  op1_8 = AL;

  op2_8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  diff_8 = op1_8 - op2_8;

  BX_SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_CMP8);
}

  INLINE void
bx_CMP_eAXIv()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, diff_32;

    op1_32 = EAX;

    op2_32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()

    diff_32 = op1_32 - op2_32;

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_CMP32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16, diff_16;

    op1_16 = AX;

    op2_16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    diff_16 = op1_16 - op2_16;

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_CMP16);
    }
}

  INLINE void
bx_INC_eAX()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u eax;

    eax = EAX;

    eax++;

    EAX = eax;

    BX_SET_FLAGS_OSZAP_32(0, 0, eax, BX_INSTR_INC32);
    }
  else
#endif /* BX_CPU > 2 */
    {
    Bit16u ax;

    ax = AX;

    ax++;

    AX = ax;

    BX_SET_FLAGS_OSZAP_16(0, 0, ax, BX_INSTR_INC16);
    }
}

  INLINE void
bx_INC_eCX()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u ecx;

    ecx = ECX;

    ecx++;

    ECX = ecx;

    BX_SET_FLAGS_OSZAP_32(0, 0, ecx, BX_INSTR_INC32);
    }
  else
#endif /* BX_CPU > 2 */
    {
    Bit16u cx;

    cx = CX;

    cx++;

    CX = cx;

    BX_SET_FLAGS_OSZAP_16(0, 0, cx, BX_INSTR_INC16);
    }
}

  INLINE void
bx_INC_eDX()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u edx;

    edx = EDX;

    edx++;

    EDX = edx;

    BX_SET_FLAGS_OSZAP_32(0, 0, edx, BX_INSTR_INC32);
    }
  else
#endif /* BX_CPU > 2 */
    {
    Bit16u dx;

    dx = DX;

    dx++;

    DX = dx;

    BX_SET_FLAGS_OSZAP_16(0, 0, dx, BX_INSTR_INC16);
    }
}

  INLINE void
bx_INC_eBX()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u ebx;

    ebx = EBX;

    ebx++;

    EBX = ebx;

    BX_SET_FLAGS_OSZAP_32(0, 0, ebx, BX_INSTR_INC32);
    }
  else
#endif /* BX_CPU > 2 */
    {
    Bit16u bx;

    bx = BX;

    bx++;

    BX = bx;

    BX_SET_FLAGS_OSZAP_16(0, 0, bx, BX_INSTR_INC16);
    }
}

  INLINE void
bx_INC_eSP()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u esp;

    esp = ESP;

    esp++;

    ESP = esp;

    BX_SET_FLAGS_OSZAP_32(0, 0, esp, BX_INSTR_INC32);
    }
  else
#endif /* BX_CPU > 2 */
    {
    Bit16u sp;

    sp = SP;

    sp++;

    SP = sp;

    BX_SET_FLAGS_OSZAP_16(0, 0, sp, BX_INSTR_INC16);
    }
}

  INLINE void
bx_INC_eBP()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u ebp;

    ebp = EBP;

    ebp++;

    EBP = ebp;

    BX_SET_FLAGS_OSZAP_32(0, 0, ebp, BX_INSTR_INC32);
    }
  else
#endif /* BX_CPU > 2 */
    {
    Bit16u bp;

    bp = BP;

    bp++;

    BP = bp;

    BX_SET_FLAGS_OSZAP_16(0, 0, bp, BX_INSTR_INC16);
    }
}

  INLINE void
bx_INC_eSI()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u esi;

    esi = ESI;

    esi++;

    ESI = esi;

    BX_SET_FLAGS_OSZAP_32(0, 0, esi, BX_INSTR_INC32);
    }
  else
#endif /* BX_CPU > 2 */
    {
    Bit16u si;

    si = SI;

    si++;

    SI = si;

    BX_SET_FLAGS_OSZAP_16(0, 0, si, BX_INSTR_INC16);
    }
}

  INLINE void
bx_INC_eDI()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u edi;

    edi = EDI;

    edi++;

    EDI = edi;

    BX_SET_FLAGS_OSZAP_32(0, 0, edi, BX_INSTR_INC32);
    }
  else
#endif /* BX_CPU > 2 */
    {
    Bit16u di;

    di = DI;

    di++;

    DI = di;

    BX_SET_FLAGS_OSZAP_16(0, 0, di, BX_INSTR_INC16);
    }
}

  INLINE void
bx_DEC_eAX()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u eax;

    eax = EAX;

    eax--;

    EAX = eax;

    BX_SET_FLAGS_OSZAP_32(0, 0, eax, BX_INSTR_DEC32);
    }
  else
#endif /* BX_CPU > 2 */
    {
    Bit16u ax;

    ax = AX;

    ax--;

    AX = ax;

    BX_SET_FLAGS_OSZAP_16(0, 0, ax, BX_INSTR_DEC16);
    }
}

  INLINE void
bx_DEC_eCX()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u ecx;

    ecx = ECX;

    ecx--;

    ECX = ecx;

    BX_SET_FLAGS_OSZAP_32(0, 0, ecx, BX_INSTR_DEC32);
    }
  else
#endif /* BX_CPU > 2 */
    {
    Bit16u cx;

    cx = CX;

    cx--;

    CX = cx;

    BX_SET_FLAGS_OSZAP_16(0, 0, cx, BX_INSTR_DEC16);
    }
}

  INLINE void
bx_DEC_eDX()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u edx;

    edx = EDX;

    edx--;

    EDX = edx;

    BX_SET_FLAGS_OSZAP_32(0, 0, edx, BX_INSTR_DEC32);
    }
  else
#endif /* BX_CPU > 2 */
    {
    Bit16u dx;

    dx = DX;

    dx--;

    DX = dx;

    BX_SET_FLAGS_OSZAP_16(0, 0, dx, BX_INSTR_DEC16);
    }
}

  INLINE void
bx_DEC_eBX()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u ebx;

    ebx = EBX;

    ebx--;

    EBX = ebx;

    BX_SET_FLAGS_OSZAP_32(0, 0, ebx, BX_INSTR_DEC32);
    }
  else
#endif /* BX_CPU > 2 */
    {
    Bit16u bx;

    bx = BX;

    bx--;

    BX = bx;

    BX_SET_FLAGS_OSZAP_16(0, 0, bx, BX_INSTR_DEC16);
    }
}

  INLINE void
bx_DEC_eSP()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u esp;

    esp = ESP;

    esp--;

    ESP = esp;

    BX_SET_FLAGS_OSZAP_32(0, 0, esp, BX_INSTR_DEC32);
    }
  else
#endif /* BX_CPU > 2 */
    {
    Bit16u sp;

    sp = SP;

    sp--;

    SP = sp;

    BX_SET_FLAGS_OSZAP_16(0, 0, sp, BX_INSTR_DEC16);
    }
}

  INLINE void
bx_DEC_eBP()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u ebp;

    ebp = EBP;

    ebp--;

    EBP = ebp;

    BX_SET_FLAGS_OSZAP_32(0, 0, ebp, BX_INSTR_DEC32);
    }
  else
#endif /* BX_CPU > 2 */
    {
    Bit16u bp;

    bp = BP;

    bp--;

    BP = bp;

    BX_SET_FLAGS_OSZAP_16(0, 0, bp, BX_INSTR_DEC16);
    }
}

  INLINE void
bx_DEC_eSI()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u esi;

    esi = ESI;

    esi--;

    ESI = esi;

    BX_SET_FLAGS_OSZAP_32(0, 0, esi, BX_INSTR_DEC32);
    }
  else
#endif /* BX_CPU > 2 */
    {
    Bit16u si;

    si = SI;

    si--;

    SI = si;

    BX_SET_FLAGS_OSZAP_16(0, 0, si, BX_INSTR_DEC16);
    }
}

  INLINE void
bx_DEC_eDI()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u edi;

    edi = EDI;

    edi--;

    EDI = edi;

    BX_SET_FLAGS_OSZAP_32(0, 0, edi, BX_INSTR_DEC32);
    }
  else
#endif /* BX_CPU > 2 */
    {
    Bit16u di;

    di = DI;

    di--;

    DI = di;

    BX_SET_FLAGS_OSZAP_16(0, 0, di, BX_INSTR_DEC16);
    }
}


  INLINE void
bx_CBW()
{
  /* CBW: no flags are effected */

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* CWDE opcode */
    EAX = (Bit16s) AX;
    }
  else
#endif /* BX_CPU > 2 */
    { /* CBW opcode */
    AX = (Bit8s) AL;
    }
}

  INLINE void
bx_CWD()
{
  /* CWD: no flags are affected */

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* CDQ opcode */
    if (EAX & 0x80000000) {
      EDX = 0xFFFFFFFF;
      }
    else {
      EDX = 0x00000000;
      }
    }
  else
#endif /* BX_CPU > 2 */
    { /* CWD opcode */
    if (AX & 0x8000) {
      DX = 0xFFFF;
      }
    else {
      DX = 0x0000;
      }
    }
}

  INLINE void
bx_CMPXCHG_XBTS()
{
#if BX_CPU < 4
  bx_printf("CMPXCHG_XBTS:\n");
  bx_exception(6, 0, 0);
  return;
#else
  bx_panic("CMPXCHG_XBTS:\n");
#endif
}

  INLINE void
bx_CMPXCHG_IBTS()
{
  bx_panic("CMPXCHG_IBTS:\n");
}

  INLINE void
bx_XADD_EbGb()
{
#if BX_CPU < 4
  bx_panic("XADD_EbGb: not supported on < 80486\n");
#else

  Bit8u op2, op1, sum;
  Bit32u op1_addr;
  unsigned op2_addr, op1_type;
  bx_segment_reg_t *op1_seg_reg;
  //Bit8u op2_b7, op1_b7, sum_b7;

  /* XADD dst(r/m8), src(r8)
   * temp <-- src + dst         | sum = op2 + op1
   * src  <-- dst               | op2 = op1
   * dst  <-- tmp               | op1 = sum
   */

  bx_decode_exgx(&op2_addr, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op2 is a register, op2_addr is an index of a register */
  op2 = BX_READ_8BIT_REG(op2_addr);

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1);
    BX_HANDLE_EXCEPTION()
    }

  sum = op1 + op2;

  /* now write sum back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, sum);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &sum);
    BX_HANDLE_EXCEPTION()
    }

  /* and write destination into source */
  BX_WRITE_8BIT_REG(op2_addr, op1);

  BX_SET_FLAGS_OSZAPC_8(op1, op2, sum, BX_INSTR_XADD8);
#endif
}

  INLINE void
bx_XADD_EvGv()
{
  bx_panic("XADD_EvGv:\n");
}

  INLINE void
bx_ADD_EbIb()
{
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;

  Bit8u op2, op1, sum;


  /* op1 is mod+r/m, op2 is an 8bit immediate */
  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1);
    BX_HANDLE_EXCEPTION()
    }

  sum = op1 + op2;

  /* now write sum back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, sum);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &sum);
    BX_HANDLE_EXCEPTION()
    }

  BX_SET_FLAGS_OSZAPC_8(op1, op2, sum, BX_INSTR_ADD8);
}

  INLINE void
bx_ADC_EbIb()
{
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;

  Bit8u op2, op1, sum;
  Boolean temp_CF;

  temp_CF = bx_get_CF();


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1);
    BX_HANDLE_EXCEPTION()
    }

  sum = op1 + op2 + temp_CF;

  /* now write sum back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, sum);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &sum);
    BX_HANDLE_EXCEPTION()
    }

  BX_SET_FLAGS_OSZAPC_8_CF(op1, op2, sum, BX_INSTR_ADC8,
                           temp_CF);
}


  INLINE void
bx_SUB_EbIb()
{
  Bit8u op2_8, op1_8, diff_8;
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  /* op1_8 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  diff_8 = op1_8 - op2_8;

  /* now write diff back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, diff_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &diff_8);
    BX_HANDLE_EXCEPTION()
    }

  BX_SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_SUB8);
}

  INLINE void
bx_CMP_EbIb()
{
  Bit8u op2_8, op1_8, diff_8;
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2_8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  /* op1_8 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  diff_8 = op1_8 - op2_8;

  BX_SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_CMP8);
}


  INLINE void
bx_ADD_EvIv()
{
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, sum_32;

    op2_32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    sum_32 = op1_32 + op2_32;

    /* now write sum back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, sum_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &sum_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_ADD32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, sum_16;


    op2_16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    sum_16 = op1_16 + op2_16;

    /* now write sum back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, sum_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &sum_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, sum_16, BX_INSTR_ADD16);
    }
}

  INLINE void
bx_ADC_EvIv()
{
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;
  Boolean temp_CF;

  temp_CF = bx_get_CF();


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, sum_32;

    op2_32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    sum_32 = op1_32 + op2_32 + temp_CF;

    /* now write sum back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, sum_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &sum_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_32_CF(op1_32, op2_32, sum_32, BX_INSTR_ADC32,
                              temp_CF);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, sum_16;


    op2_16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    sum_16 = op1_16 + op2_16 + temp_CF;

    /* now write sum back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, sum_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &sum_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_16_CF(op1_16, op2_16, sum_16, BX_INSTR_ADC16,
                              temp_CF);
    }
}


  INLINE void
bx_SUB_EvIv()
{
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, diff_32;

    op2_32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    diff_32 = op1_32 - op2_32;

    /* now write diff back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, diff_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &diff_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SUB32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, diff_16;


    op2_16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    diff_16 = op1_16 - op2_16;

    /* now write diff back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, diff_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &diff_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_SUB16);
    }
}

  INLINE void
bx_CMP_EvIv()
{
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, diff_32;

    op2_32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    diff_32 = op1_32 - op2_32;

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_CMP32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, diff_16;


    op2_16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    diff_16 = op1_16 - op2_16;

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_CMP16);
    }
}

  INLINE void
bx_ADD_EvIb()
{
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, sum_32;

    op2_32 = (Bit8s) bx_fetch_next_byte();
    BX_HANDLE_EXCEPTION()

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    sum_32 = op1_32 + op2_32;

    /* now write sum back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, sum_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &sum_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_ADD32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, sum_16;


    op2_16 = (Bit8s) bx_fetch_next_byte();
    BX_HANDLE_EXCEPTION()

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    sum_16 = op1_16 + op2_16;

    /* now write sum back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, sum_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &sum_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, sum_16, BX_INSTR_ADD16);
    }
}

  INLINE void
bx_ADC_EvIb()
{
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;
  Boolean temp_CF;

  temp_CF = bx_get_CF();


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, sum_32;

    op2_32 = (Bit8s) bx_fetch_next_byte();
    BX_HANDLE_EXCEPTION()

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    sum_32 = op1_32 + op2_32 + temp_CF;

    /* now write sum back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, sum_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &sum_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_32_CF(op1_32, op2_32, sum_32, BX_INSTR_ADC32,
                              temp_CF);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, sum_16;


    op2_16 = (Bit8s) bx_fetch_next_byte();
    BX_HANDLE_EXCEPTION()

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    sum_16 = op1_16 + op2_16 + temp_CF;

    /* now write sum back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, sum_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &sum_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_16_CF(op1_16, op2_16, sum_16, BX_INSTR_ADC16,
                              temp_CF);
    }
}

  INLINE void
bx_SUB_EvIb()
{
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, diff_32;

    op2_32 = (Bit8s) bx_fetch_next_byte();
    BX_HANDLE_EXCEPTION()

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    diff_32 = op1_32 - op2_32;

    /* now write diff back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, diff_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &diff_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SUB32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, diff_16;


    op2_16 = (Bit8s) bx_fetch_next_byte();
    BX_HANDLE_EXCEPTION()

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    diff_16 = op1_16 - op2_16;

    /* now write diff back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, diff_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &diff_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_SUB16);
    }
}

  INLINE void
bx_CMP_EvIb()
{
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, diff_32;

    op2_32 = (Bit8s) bx_fetch_next_byte();
    BX_HANDLE_EXCEPTION()

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    diff_32 = op1_32 - op2_32;

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_CMP32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, diff_16;


    op2_16 = (Bit8s) bx_fetch_next_byte();
    BX_HANDLE_EXCEPTION()

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    diff_16 = op1_16 - op2_16;

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_CMP16);
    }
}


  INLINE void
bx_NEG_Eb()
{
  Bit8u op1_8, diff_8;
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op1_8 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  diff_8 = 0 - op1_8;

  /* now write diff back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, diff_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &diff_8);
    BX_HANDLE_EXCEPTION()
    }

  BX_SET_FLAGS_OSZAPC_8(op1_8, 0, diff_8, BX_INSTR_NEG8);
}

  INLINE void
bx_NEG_Ev()
{
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, diff_32;

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    diff_32 = 0 - op1_32;

    /* now write diff back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, diff_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &diff_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_32(op1_32, 0, diff_32, BX_INSTR_NEG32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op1_16, diff_16;


    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    diff_16 = 0 - op1_16;

    /* now write diff back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, diff_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &diff_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_16(op1_16, 0, diff_16, BX_INSTR_NEG16);
    }
}


  INLINE void
bx_INC_Eb()
{
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;

  Bit8u  op1;

  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1);
    BX_HANDLE_EXCEPTION()
    }


  op1++;

  /* now write sum back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, op1);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &op1);
    BX_HANDLE_EXCEPTION()
    }

  BX_SET_FLAGS_OSZAP_8(0, 0, op1, BX_INSTR_INC8);
}

  INLINE void
bx_INC_Ev()
{
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op1_32;

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    op1_32++;

    /* now write sum back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, op1_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAP_32(0, 0, op1_32, BX_INSTR_INC32);
    }
  else
#endif /* BX_CPU > 2 */
    {
    Bit16u op1_16;

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    op1_16++;

    /* now write sum back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, op1_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAP_16(0, 0, op1_16, BX_INSTR_INC16);
    }
}

  INLINE void
bx_DEC_Eb()
{
  Bit8u op1_8;

  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op1_8 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    op1_8 = BX_READ_8BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  op1_8--;

  /* now write sum back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, op1_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &op1_8);
    BX_HANDLE_EXCEPTION()
    }

  BX_SET_FLAGS_OSZAP_8(0, 0, op1_8, BX_INSTR_DEC8);
}

  INLINE void
bx_DEC_Ev()
{
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;

  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op1_32;

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    op1_32--;

    /* now write sum back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, op1_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAP_32(0, 0, op1_32, BX_INSTR_DEC32);
    }
  else
#endif /* BX_CPU > 2 */
    {
    Bit16u op1_16;

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    op1_16--;

    /* now write sum back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, op1_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAP_16(0, 0, op1_16, BX_INSTR_DEC16);
    }
}

INLINE void bx_CMPXCHG_EbGb() {bx_panic("CMPXCHG_EbGb:\n");}
INLINE void bx_CMPXCHG_EvGv() {bx_panic("CMPXCHG_EvGv:\n");}
