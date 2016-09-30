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



  INLINE void
bx_XOR_EbGb()
{
  Bit32u op1_addr;
  unsigned op2_addr, op1_type;
  bx_segment_reg_t *op1_seg_reg;

  Bit8u op2, op1, result;

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

  result = op1 ^ op2;

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result);
    BX_HANDLE_EXCEPTION()
    }

  BX_SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_XOR8);
}

  INLINE void
bx_XOR_EvGv()
{
  Bit32u op1_addr;
  unsigned op2_addr, op1_type;
  bx_segment_reg_t *op1_seg_reg;



  bx_decode_exgx(&op2_addr, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, result_32;

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

    result_32 = op1_32 ^ op2_32;

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, result_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_XOR32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, result_16;


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

    result_16 = op1_16 ^ op2_16;

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, result_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_XOR16);
    }
}

  INLINE void
bx_XOR_GbEb()
{
  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;

  Bit8u op1, op2, result;


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

  result = op1 ^ op2;

  /* now write result back to destination, which is a register */
  BX_WRITE_8BIT_REG(op1_addr, result);

  BX_SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_XOR8);
}

  INLINE void
bx_XOR_GvEv()
{
  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;



  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, result_32;

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

    result_32 = op1_32 ^ op2_32;

    /* now write result back to destination */
    BX_WRITE_32BIT_REG(op1_addr, result_32);

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_XOR32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16, result_16;

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

    result_16 = op1_16 ^ op2_16;

    /* now write result back to destination */
    BX_WRITE_16BIT_REG(op1_addr, result_16);

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_XOR16);
    }
}

  INLINE void
bx_XOR_ALIb()
{
  Bit8u op1, op2, sum;

  op1 = AL;

  op2 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  sum = op1 ^ op2;

  /* now write sum back to destination, which is a register */
  AL = sum;

  BX_SET_FLAGS_OSZAPC_8(op1, op2, sum, BX_INSTR_XOR8);
}

  INLINE void
bx_XOR_eAXIv()
{


#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, sum_32;

    op1_32 = EAX;

    op2_32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()

    sum_32 = op1_32 ^ op2_32;

    /* now write sum back to destination */
    EAX = sum_32;

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_XOR32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16, sum_16;

    op1_16 = AX;

    op2_16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    sum_16 = op1_16 ^ op2_16;

    /* now write sum back to destination */
    AX = sum_16;

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, sum_16, BX_INSTR_XOR16);
    }
}

  INLINE void
bx_XOR_EbIb()
{
  Bit8u op2, op1, result;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


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

  result = op1 ^ op2;

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result);
    BX_HANDLE_EXCEPTION()
    }

  BX_SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_XOR8);
}

  INLINE void
bx_XOR_EvIv()
{
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;



  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, result_32;

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

    result_32 = op1_32 ^ op2_32;

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, result_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_XOR32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, result_16;


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

    result_16 = op1_16 ^ op2_16;

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, result_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_XOR16);
    }
}

  INLINE void
bx_XOR_EvIb()
{
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;



  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, result_32;

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

    result_32 = op1_32 ^ op2_32;

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, result_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_XOR32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, result_16;


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

    result_16 = op1_16 ^ op2_16;

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, result_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_XOR16);
    }
}


  INLINE void
bx_OR_EvIv()
{
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, result_32;

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

    result_32 = op1_32 | op2_32;

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, result_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_OR32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, result_16;


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

    result_16 = op1_16 | op2_16;

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, result_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_OR16);
    }
}

  INLINE void
bx_OR_EvIb()
{
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;



  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, result_32;

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

    result_32 = op1_32 | op2_32;

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, result_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_OR32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, result_16;


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

    result_16 = op1_16 | op2_16;

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, result_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_OR16);
    }
}

  INLINE void
bx_OR_EbIb()
{
  Bit8u op2, op1, result;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


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

  result = op1 | op2;

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result);
    BX_HANDLE_EXCEPTION()
    }

  BX_SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_OR8);
}


  INLINE void
bx_NOT_Eb()
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

  result_8 = ~op1_8;

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }
}

  INLINE void
bx_NOT_Ev()
{
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
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

    result_32 = ~op1_32;

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, result_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
      BX_HANDLE_EXCEPTION()
      }
    }
  else
#endif /* BX_CPU > 2 */
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

    result_16 = ~op1_16;

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, result_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
      BX_HANDLE_EXCEPTION()
      }
    }
}

  INLINE void
bx_OR_EbGb()
{
  Bit32u op1_addr;
  unsigned op2_addr, op1_type;
  bx_segment_reg_t *op1_seg_reg;

  Bit8u op2, op1, result;


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

  result = op1 | op2;

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result);
    BX_HANDLE_EXCEPTION()
    }

  BX_SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_OR8);
}

  INLINE void
bx_OR_EvGv()
{
  Bit32u op1_addr;
  unsigned op2_addr, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&op2_addr, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, result_32;

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

    result_32 = op1_32 | op2_32;

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, result_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_OR32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, result_16;


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

    result_16 = op1_16 | op2_16;

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, result_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_OR16);
    }
}

  INLINE void
bx_OR_GbEb()
{
  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;

  Bit8u op1, op2, result;


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

  result = op1 | op2;

  /* now write result back to destination, which is a register */
  BX_WRITE_8BIT_REG(op1_addr, result);


  BX_SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_OR8);
}

  INLINE void
bx_OR_GvEv()
{
  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;


  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, result_32;

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

    result_32 = op1_32 | op2_32;

    /* now write result back to destination */
    BX_WRITE_32BIT_REG(op1_addr, result_32);

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_OR32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16, result_16;


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

    result_16 = op1_16 | op2_16;

    /* now write result back to destination */
    BX_WRITE_16BIT_REG(op1_addr, result_16);

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_OR16);
    }
}

  INLINE void
bx_OR_ALIb()
{
  Bit8u op1, op2, sum;


  op1 = AL;

  op2 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  sum = op1 | op2;

  /* now write sum back to destination, which is a register */
  AL = sum;

  BX_SET_FLAGS_OSZAPC_8(op1, op2, sum, BX_INSTR_OR8);
}

  INLINE void
bx_OR_eAXIv()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, sum_32;

    op1_32 = EAX;

    op2_32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()

    sum_32 = op1_32 | op2_32;

    /* now write sum back to destination */
    EAX = sum_32;

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_OR32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16, sum_16;

    op1_16 = AX;

    op2_16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    sum_16 = op1_16 | op2_16;

    /* now write sum back to destination */
    AX = sum_16;

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, sum_16, BX_INSTR_OR16);
    }
}


  INLINE void
bx_AND_EbGb()
{
  Bit8u op2, op1, result;
  Bit32u op1_addr;
  unsigned op2_addr, op1_type;
  bx_segment_reg_t *op1_seg_reg;


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

  result = op1 & op2;

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result);
    BX_HANDLE_EXCEPTION()
    }

  BX_SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_AND8);
}

  INLINE void
bx_AND_EvGv()
{
  Bit32u op1_addr;
  unsigned op2_addr, op1_type;
  bx_segment_reg_t *op1_seg_reg;



  bx_decode_exgx(&op2_addr, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, result_32;

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

    result_32 = op1_32 & op2_32;

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, result_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_AND32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, result_16;



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

    result_16 = op1_16 & op2_16;

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, result_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_AND16);
    }
}

  INLINE void
bx_AND_GbEb()
{
  Bit8u op1, op2, result;
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

  result = op1 & op2;

  /* now write result back to destination, which is a register */
  BX_WRITE_8BIT_REG(op1_addr, result);

  BX_SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_AND8);
}

  INLINE void
bx_AND_GvEv()
{
  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;



  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, result_32;

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

    result_32 = op1_32 & op2_32;

    /* now write result back to destination */
    BX_WRITE_32BIT_REG(op1_addr, result_32);

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_AND32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16, result_16;


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

    result_16 = op1_16 & op2_16;

    /* now write result back to destination */
    BX_WRITE_16BIT_REG(op1_addr, result_16);

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_AND16);
    }
}

  INLINE void
bx_AND_ALIb()
{
  Bit8u op1, op2, sum;


  op1 = AL;

  op2 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  sum = op1 & op2;

  /* now write sum back to destination, which is a register */
  AL = sum;

  BX_SET_FLAGS_OSZAPC_8(op1, op2, sum, BX_INSTR_AND8);
}

  INLINE void
bx_AND_eAXIv()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, sum_32;

    op1_32 = EAX;

    op2_32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()

    sum_32 = op1_32 & op2_32;

    /* now write sum back to destination */
    EAX = sum_32;

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_AND32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16, sum_16;

    op1_16 = AX;

    op2_16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    sum_16 = op1_16 & op2_16;

    /* now write sum back to destination */
    AX = sum_16;

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, sum_16, BX_INSTR_AND16);
    }
}

  INLINE void
bx_AND_EvIv()
{
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, result_32;

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

    result_32 = op1_32 & op2_32;

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, result_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_AND32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, result_16;

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

    result_16 = op1_16 & op2_16;

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, result_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_AND16);
    }
}

  INLINE void
bx_AND_EvIb()
{
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;



  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, result_32;

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

    result_32 = op1_32 & op2_32;

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, result_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &result_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_AND32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, result_16;


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

    result_16 = op1_16 & op2_16;

    /* now write result back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, result_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &result_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_AND16);
    }
}

  INLINE void
bx_AND_EbIb()
{
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;

  Bit8u op2, op1, result;


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

  result = op1 & op2;

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result);
    BX_HANDLE_EXCEPTION()
    }

  BX_SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_AND8);
}


  INLINE void
bx_TEST_EbGb()
{
  Bit8u op2, op1, result;
  Bit32u op1_addr;
  unsigned op1_type, op2_addr;
  bx_segment_reg_t *op1_seg_reg;


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

  result = op1 & op2;

  BX_SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_TEST8);
}

  INLINE void
bx_TEST_EvGv()
{
  Bit32u op1_addr;
  unsigned op1_type, op2_addr;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&op2_addr, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, result_32;

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

    result_32 = op1_32 & op2_32;

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_TEST32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, result_16;


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

    result_16 = op1_16 & op2_16;

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_TEST16);
    }
}


  INLINE void
bx_TEST_ALIb()
{
  Bit8u op2, op1, result;

  /* op1 is the AL register */
  op1 = AL;

  /* op2 is imm8 */
  op2 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  result = op1 & op2;

  BX_SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_TEST8);
}

  INLINE void
bx_TEST_eAXIv()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, result_32;

    /* op1 is EAX register */
    op1_32 = EAX;

    /* op2 is imm32 */
    op2_32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()

    result_32 = op1_32 & op2_32;

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_TEST32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, result_16;

    op1_16 = AX;

    /* op2_16 is imm16 */
    op2_16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    result_16 = op1_16 & op2_16;

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_TEST16);
    }
}


  INLINE void
bx_TEST_EbIb()
{
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;

  Bit8u op2, op1, result;


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

  result = op1 & op2;

  BX_SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_TEST8);
}

  INLINE void
bx_TEST_EvIv()
{
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, result_32;

    /* op2 is imm32 */
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

    result_32 = op1_32 & op2_32;

    BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_TEST32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16, result_16;


    /* op2_16 is imm16 */
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

    result_16 = op1_16 & op2_16;

    BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_TEST16);
    }
}
