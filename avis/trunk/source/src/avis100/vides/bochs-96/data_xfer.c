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
bx_MOV_EbGb()
{
  Bit8u op2;
  Bit32u op1_addr;
  unsigned op2_addr, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&op2_addr, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op2 is a register, op2_addr is an index of a register */
  op2 = BX_READ_8BIT_REG(op2_addr);

  /* now write op2 to op1 */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, op2);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &op2);
    BX_HANDLE_EXCEPTION()
    }
}

  INLINE void
bx_MOV_EvGv()
{
  Bit32u op1_addr;
  unsigned op2_addr, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&op2_addr, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op2_32;

    /* op2_32 is a register, op2_addr is an index of a register */
    op2_32 = BX_READ_32BIT_REG(op2_addr);

    /* op1_32 is a register or memory reference */
    /* now write op2 to op1 */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, op2_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &op2_32);
      BX_HANDLE_EXCEPTION()
      }
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit operand size mode */
    Bit16u op2_16;

    /* op2_16 is a register, op2_addr is an index of a register */
    op2_16 = BX_READ_16BIT_REG(op2_addr);

    /* op1_16 is a register or memory reference */
    /* now write op2 to op1 */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, op2_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &op2_16);
      BX_HANDLE_EXCEPTION()
      }
    }
}

  INLINE void
bx_MOV_GbEb()
{
  Bit8u op2;
  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;


  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op1 is a register, op1_addr is an index of a register */
  /* op2 is a register or memory reference */
  if (op2_type == BX_REGISTER_REF) {
    op2 = BX_READ_8BIT_REG(op2_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op2_seg_reg, op2_addr, &op2);
    BX_HANDLE_EXCEPTION()
    }

  BX_WRITE_8BIT_REG(op1_addr, op2);
}

  INLINE void
bx_MOV_GvEv()
{
  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;

  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op2_32;

    /* op1_32 is a register, op1_addr is an index of a register */
    /* op2_32 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_32 = BX_READ_32BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op2_seg_reg, op2_addr, &op2_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_WRITE_32BIT_REG(op1_addr, op2_32);
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit operand size mode */
    Bit16u op2_16;

    /* op1_16 is a register, op1_addr is an index of a register */
    /* op2_16 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_16 = BX_READ_16BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op2_seg_reg, op2_addr, &op2_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_WRITE_16BIT_REG(op1_addr, op2_16);
    }
}

  INLINE void
bx_MOV_EwSw()
{
  unsigned seg_reg_index, mem_type;
  Bit32u mem_addr;
  Bit16u seg_reg;
  bx_segment_reg_t *mem_seg_reg;


  bx_decode_exgx(&seg_reg_index, &mem_addr, &mem_type, &mem_seg_reg);
  BX_HANDLE_EXCEPTION()

  seg_reg = bx_read_segment_register(seg_reg_index);

  if (mem_type == BX_REGISTER_REF) {
    BX_WRITE_16BIT_REG(mem_addr, seg_reg);
    }
  else {
    bx_write_virtual_word(mem_seg_reg, mem_addr, &seg_reg);
    BX_HANDLE_EXCEPTION()
    }
}

  INLINE void
bx_MOV_SwEw()
{
  unsigned seg_reg_index, op2_type;
  Bit32u op2_addr;
  bx_segment_reg_t *op2_seg_reg;

  Bit16u op2_16;


  bx_decode_exgx(&seg_reg_index, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (op2_type == BX_REGISTER_REF) {
    op2_16 = BX_READ_16BIT_REG(op2_addr);
    }
  else {
    bx_read_virtual_word(op2_seg_reg, op2_addr, &op2_16);
    BX_HANDLE_EXCEPTION()
    }

  bx_write_segment_register(seg_reg_index, op2_16);
  /* will return if there's an error anyway */
}

  INLINE void
bx_LEA_GvM()
{
  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;

  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (op2_type == BX_REGISTER_REF) {
    bx_panic("LEA_GvM: op2 is a register");
    bx_exception(6, 0, 0);
    return;
    }

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* write effective address of op2 in op1 */
    BX_WRITE_32BIT_REG(op1_addr, op2_addr);
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit operand size mode */
    BX_WRITE_16BIT_REG(op1_addr, (Bit16u) op2_addr);
    }
}

  INLINE void
bx_XCHG_eCXeAX()
{

#if BX_CPU >= 3
   if (bx_cpu.is_32bit_opsize) {
      Bit32u temp32;

      WORD tempmarker;
      temp32     = bx_cpu.eax;
      bx_cpu.eax = bx_cpu.ecx;
      bx_cpu.ecx = temp32;
      tempmarker = reg_markers[0];
      reg_markers[0] = reg_markers[1];
      reg_markers[1] = tempmarker;
   }
   else
#endif /* BX_CPU >= 3 */
   {
      Bit16u temp16;

      temp16 = AX;
      AX     = CX;
      CX     = temp16;
   }
}

  INLINE void
bx_XCHG_eDXeAX()
{

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32u temp32;

   WORD tempmarker;
    temp32     = bx_cpu.eax;
    bx_cpu.eax = bx_cpu.edx;
    bx_cpu.edx = temp32;
   tempmarker = reg_markers[0];
   reg_markers[0] = reg_markers[2];
   reg_markers[2] = tempmarker;
    }
  else
#endif /* BX_CPU >= 3 */
    {
    Bit16u temp16;

    temp16 = AX;
    AX     = DX;
    DX     = temp16;
    }
}

  INLINE void
bx_XCHG_eBXeAX()
{

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32u temp32;

    WORD tempmarker;
    temp32     = bx_cpu.eax;
    bx_cpu.eax = bx_cpu.ebx;
    bx_cpu.ebx = temp32;
   tempmarker = reg_markers[0];
   reg_markers[0] = reg_markers[3];
   reg_markers[3] = tempmarker;
    }
  else
#endif /* BX_CPU >= 3 */
    {
    Bit16u temp16;

    temp16 = AX;
    AX     = BX;
    BX     = temp16;
    }
}

  INLINE void
bx_XCHG_eSPeAX()
{

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32u temp32;

   WORD tempmarker;
    temp32     = bx_cpu.eax;
    bx_cpu.eax = bx_cpu.esp;
    bx_cpu.esp = temp32;
   tempmarker = reg_markers[0];
   reg_markers[0] = reg_markers[4];
   reg_markers[4] = tempmarker;
    }
  else
#endif /* BX_CPU >= 3 */
    {
    Bit16u temp16;

    temp16 = AX;
    AX     = SP;
    SP     = temp16;
    }
}

  INLINE void
bx_XCHG_eBPeAX()
{

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32u temp32;

   WORD tempmarker;
   tempmarker = reg_markers[0];
   reg_markers[0] = reg_markers[5];
   reg_markers[5] = tempmarker;
    temp32     = bx_cpu.eax;
    bx_cpu.eax = bx_cpu.ebp;
    bx_cpu.ebp = temp32;
    }
  else
#endif /* BX_CPU >= 3 */
    {
    Bit16u temp16;

    temp16 = AX;
    AX     = BP;
    BP     = temp16;
    }
}

  INLINE void
bx_XCHG_eSIeAX()
{

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32u temp32;

   WORD tempmarker;
   tempmarker = reg_markers[0];
   reg_markers[0] = reg_markers[6];
   reg_markers[6] = tempmarker;
    temp32     = bx_cpu.eax;
    bx_cpu.eax = bx_cpu.esi;
    bx_cpu.esi = temp32;
    }
  else
#endif /* BX_CPU >= 3 */
    {
    Bit16u temp16;

    temp16 = AX;
    AX     = SI;
    SI     = temp16;
    }
}

  INLINE void
bx_XCHG_eDIeAX()
{

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32u temp32;

   WORD tempmarker;
   tempmarker = reg_markers[0];
   reg_markers[0] = reg_markers[7];
   reg_markers[7] = tempmarker;
    temp32     = bx_cpu.eax;
    bx_cpu.eax = bx_cpu.edi;
    bx_cpu.edi = temp32;
    }
  else
#endif /* BX_CPU >= 3 */
    {
    Bit16u temp16;

    temp16 = AX;
    AX     = DI;
    DI     = temp16;
    }
}


  INLINE void
bx_MOV_ALOb()
{
  Bit32u addr_32;
  Bit8u  temp_8;



#if BX_CPU >= 3
  if (bx_cpu.is_32bit_addrsize) {
    addr_32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()
    }
  else
#endif /* BX_CPU >= 3 */
    {
    addr_32 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()
    }

  /* read from memory address */
  if (bx_cpu.segment_override) {
    bx_read_virtual_byte(bx_cpu.segment_override, addr_32, &temp_8);
    BX_HANDLE_EXCEPTION()
    }
  else {
    bx_read_virtual_byte(&bx_cpu.ds, addr_32, &temp_8);
    BX_HANDLE_EXCEPTION()
    }
  

  /* write to register */
  AL = temp_8;
}

  INLINE void
bx_MOV_eAXOv()
{
  Bit32u addr_32;


#if BX_CPU >= 3
  if (bx_cpu.is_32bit_addrsize) {
    addr_32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()
    }
  else
#endif /* BX_CPU >= 3 */
    {
    addr_32 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()
    }

  /* read from memory address */
#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32u temp_32;

    if (bx_cpu.segment_override) {
      bx_read_virtual_dword(bx_cpu.segment_override, addr_32, &temp_32);
      BX_HANDLE_EXCEPTION()
      }
    else {
      bx_read_virtual_dword(&bx_cpu.ds, addr_32, &temp_32);
      BX_HANDLE_EXCEPTION()
      }

    /* write to register */
    EAX = temp_32;
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit opsize */
    Bit16u temp_16;

    if (bx_cpu.segment_override) {
      bx_read_virtual_word(bx_cpu.segment_override, addr_32, &temp_16);
      BX_HANDLE_EXCEPTION()
      }
    else {
      bx_read_virtual_word(&bx_cpu.ds, addr_32, &temp_16);
      BX_HANDLE_EXCEPTION()
      }

    /* write to register */
    AX = temp_16;
    }
}

  INLINE void
bx_MOV_ObAL()
{
  Bit32u addr_32;
  Bit8u  temp_8;


#if BX_CPU >= 3
  if (bx_cpu.is_32bit_addrsize) {
    addr_32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()
    }
  else
#endif /* BX_CPU >= 3 */
    {
    addr_32 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()
    }

  /* read from register */
  temp_8 = AL;

  /* write to memory address */
  if (bx_cpu.segment_override) {
    bx_write_virtual_byte(bx_cpu.segment_override, addr_32, &temp_8);
    BX_HANDLE_EXCEPTION()
    }
  else {
    bx_write_virtual_byte(&bx_cpu.ds, addr_32, &temp_8);
    BX_HANDLE_EXCEPTION()
    }
}

  INLINE void
bx_MOV_OveAX()
{
  Bit32u addr_32;


#if BX_CPU >= 3
  if (bx_cpu.is_32bit_addrsize) {
    addr_32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()
    }
  else
#endif /* BX_CPU >= 3 */
    {
    addr_32 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()
    }

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32u temp_32;

    /* read from register */
    temp_32 = EAX;

    /* write to memory address */
    if (bx_cpu.segment_override) {
      bx_write_virtual_dword(bx_cpu.segment_override, addr_32, &temp_32);
      BX_HANDLE_EXCEPTION()
      }
    else {
      bx_write_virtual_dword(&bx_cpu.ds, addr_32, &temp_32);
      BX_HANDLE_EXCEPTION()
      }
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit opsize */
    Bit16u temp_16;

    /* read from register */
    temp_16 = AX;

    /* write to memory address */
    if (bx_cpu.segment_override) {
      bx_write_virtual_word(bx_cpu.segment_override, addr_32, &temp_16);
      BX_HANDLE_EXCEPTION()
      }
    else {
      bx_write_virtual_word(&bx_cpu.ds, addr_32, &temp_16);
      BX_HANDLE_EXCEPTION()
      }
    }
}

  INLINE void
bx_MOV_ALIb()
{
  Bit8u imm8;


  imm8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  AL = imm8;
}

  INLINE void
bx_MOV_CLIb()
{
  Bit8u imm8;


  imm8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  CL = imm8;
}

  INLINE void
bx_MOV_DLIb()
{
  Bit8u imm8;


  imm8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  DL = imm8;
}

  INLINE void
bx_MOV_BLIb()
{
  Bit8u imm8;


  imm8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  BL = imm8;
}

  INLINE void
bx_MOV_AHIb()
{
  Bit8u imm8;


  imm8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  AH = imm8;
}

  INLINE void
bx_MOV_CHIb()
{
  Bit8u imm8;


  imm8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  CH = imm8;
}

  INLINE void
bx_MOV_DHIb()
{
  Bit8u imm8;


  imm8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  DH = imm8;
}

  INLINE void
bx_MOV_BHIb()
{
  Bit8u imm8;


  imm8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  BH = imm8;
}


  INLINE void
bx_MOV_eAXIv()
{

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32u imm32;

    imm32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()

    bx_cpu.eax = imm32;
    }
  else
#endif /* BX_CPU >= 3 */
    {
    Bit16u imm16;

    imm16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    AX = imm16;
    }
}

  INLINE void
bx_MOV_eCXIv()
{

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32u imm32;

    imm32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()

    ECX = imm32;
    }
  else
#endif /* BX_CPU >= 3 */
    {
    Bit16u imm16;

    imm16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    CX = imm16;
    }
}

  INLINE void
bx_MOV_eDXIv()
{

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32u imm32;

    imm32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()

    bx_cpu.edx = imm32;
    }
  else
#endif /* BX_CPU >= 3 */
    {
    Bit16u imm16;

    imm16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    DX = imm16;
    }
}

  INLINE void
bx_MOV_eBXIv()
{

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32u imm32;

    imm32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()

    EBX = imm32;
    }
  else
#endif /* BX_CPU >= 3 */
    {
    Bit16u imm16;

    imm16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    BX = imm16;
    }
}

  INLINE void
bx_MOV_eSPIv()
{

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32u imm32;

    imm32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()

    ESP = imm32;
    }
  else 
#endif /* BX_CPU >= 3 */
    {
    Bit16u imm16;

    imm16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    SP = imm16;
    }
}

  INLINE void
bx_MOV_eBPIv()
{

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32u imm32;

    imm32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()

    bx_cpu.ebp = imm32;
    }
  else
#endif /* BX_CPU >= 3 */
    {
    Bit16u imm16;

    imm16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    BP = imm16;
    }
}

  INLINE void
bx_MOV_eSIIv()
{

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32u imm32;

    imm32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()

    ESI = imm32;
    }
  else
#endif /* BX_CPU >= 3 */
    {
    Bit16u imm16;

    imm16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    SI = imm16;
    }
}

  INLINE void
bx_MOV_eDIIv()
{

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32u imm32;

    imm32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()

    EDI = imm32;
    }
  else
#endif /* BX_CPU >= 3 */
    {
    Bit16u imm16;

    imm16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    DI = imm16;
    }
}



  INLINE void
bx_MOV_EbIb()
{
  Bit8u op2;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  /* op1 is mod+r/m, op2 is an 8bit immediate */
  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  op2 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  /* now write op2 back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, op2);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &op2);
    BX_HANDLE_EXCEPTION()
    }
}

  INLINE void
bx_MOV_EvIv()
{
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op2_32;

    op2_32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()

    /* now write sum back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, op2_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &op2_32);
      BX_HANDLE_EXCEPTION()
      }
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit operand size mode */
    Bit16u op2_16;

    op2_16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    /* now write sum back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, op2_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &op2_16);
      BX_HANDLE_EXCEPTION()
      }
    }
}


  INLINE void
bx_MOVZX_GvEb()
{
#if BX_CPU < 3
  bx_panic("MOVZX_GvEb: not supported on < 386\n");
#else
  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;
  Bit8u op2_8;


  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()


  /* op1_addr is an index of a register */
  /* op2_8 is a register or memory reference */
  if (op2_type == BX_REGISTER_REF) {
    op2_8 = BX_READ_8BIT_REG(op2_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op2_seg_reg, op2_addr, &op2_8);
    BX_HANDLE_EXCEPTION()
    }

  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* zero extend byte op2 into dword op1 */
    BX_WRITE_32BIT_REG(op1_addr, (Bit32u) op2_8);
    }
  else {
    /* zero extend byte op2 into word op1 */
    BX_WRITE_16BIT_REG(op1_addr, (Bit16u) op2_8);
    }
#endif /* BX_CPU < 3 */
}

  INLINE void
bx_MOVZX_GvEw()
{
#if BX_CPU < 3
  bx_panic("MOVZX_GvEw: not supported on < 386\n");
#else
  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;
  Bit16u op2_16;


  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()


  /* op1_16 is a register, op1_addr is an index of a register */
  /* op2_16 is a register or memory reference */
  if (op2_type == BX_REGISTER_REF) {
    op2_16 = BX_READ_16BIT_REG(op2_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_word(op2_seg_reg, op2_addr, &op2_16);
    BX_HANDLE_EXCEPTION()
    }

  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* zero extend word op2 into dword op1 */
    BX_WRITE_32BIT_REG(op1_addr, (Bit32u) op2_16);
    }
  else {
    /* normal move */
    BX_WRITE_16BIT_REG(op1_addr, op2_16);
    }
#endif /* BX_CPU < 3 */
}

  INLINE void
bx_MOVSX_GvEb()
{
#if BX_CPU < 3
  bx_panic("MOVSX_GvEb: not supported on < 386\n");
#else
  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;
  Bit8u op2_8;

  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (op2_type == BX_REGISTER_REF) {
    op2_8 = BX_READ_8BIT_REG(op2_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op2_seg_reg, op2_addr, &op2_8);
    BX_HANDLE_EXCEPTION()
    }

  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* sign extend byte op2 into dword op1 */
    BX_WRITE_32BIT_REG(op1_addr, (Bit8s) op2_8);
    }
  else {
    /* sign extend byte op2 into word op1 */
    BX_WRITE_16BIT_REG(op1_addr, (Bit8s) op2_8);
    }
#endif /* BX_CPU < 3 */
}

  INLINE void
bx_MOVSX_GvEw()
{
#if BX_CPU < 3
  bx_panic("MOVSX_GvEw: not supported on < 386\n");
#else
  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;
  Bit16u op2_16;

  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (op2_type == BX_REGISTER_REF) {
    op2_16 = BX_READ_16BIT_REG(op2_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_word(op2_seg_reg, op2_addr, &op2_16);
    BX_HANDLE_EXCEPTION()
    }

  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* sign extend word op2 into dword op1 */
    BX_WRITE_32BIT_REG(op1_addr, (Bit16s) op2_16);
    }
  else {
    /* normal move */
    BX_WRITE_16BIT_REG(op1_addr, op2_16);
    }
#endif /* BX_CPU < 3 */
}

  INLINE void
bx_XLAT()
{
  Bit32u offset_32;
  Bit8u  al;


#if BX_CPU >= 3
  if (bx_cpu.is_32bit_addrsize) {
    offset_32 = EBX + AL;
    }
  else
#endif /* BX_CPU >= 3 */
    {
    offset_32 = BX + AL;
    }

  if (bx_cpu.segment_override) {
    bx_read_virtual_byte(bx_cpu.segment_override, offset_32, &al);
    BX_HANDLE_EXCEPTION()
    }
  else {
    bx_read_virtual_byte(&bx_cpu.ds, offset_32, &al);
    BX_HANDLE_EXCEPTION()
    }
  AL = al;
}

  INLINE void
bx_XCHG_EbGb()
{
  Bit8u op2, op1;
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
    BX_WRITE_8BIT_REG(op1_addr, op2);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op1_seg_reg, op1_addr, &op1);
    BX_HANDLE_EXCEPTION()
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &op2);
    BX_HANDLE_EXCEPTION()
    }

  BX_WRITE_8BIT_REG(op2_addr, op1);
}

  INLINE void
bx_XCHG_EvGv()
{
  Bit32u op1_addr;
  unsigned op2_addr, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&op2_addr, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u op2_32, op1_32;

    /* op2_32 is a register, op2_addr is an index of a register */
    op2_32 = BX_READ_32BIT_REG(op2_addr);

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      BX_WRITE_32BIT_REG(op1_addr, op2_32);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &op2_32);
      BX_HANDLE_EXCEPTION()
      }

    BX_WRITE_32BIT_REG(op2_addr, op1_32);
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit operand size mode */
    Bit16u op2_16, op1_16;

    /* op2_16 is a register, op2_addr is an index of a register */
    op2_16 = BX_READ_16BIT_REG(op2_addr);

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      BX_WRITE_16BIT_REG(op1_addr, op2_16);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      bx_write_virtual_word(op1_seg_reg, op1_addr, &op2_16);
      BX_HANDLE_EXCEPTION()
      }

    BX_WRITE_16BIT_REG(op2_addr, op1_16);
    }
}
