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
bx_PUSH_ES()
{
#if BX_CPU >= 3
  /* bx_push_16() & bx_push_32() will use StackAddrSize to determine
   * use of either SP or ESP
   */
  if (bx_cpu.is_32bit_opsize) { /* OperandSize = 32 */
    bx_push_32(bx_cpu.es.selector.value);
    }
  else
#endif
    { /* OperandSize = 16 */
    bx_push_16(bx_cpu.es.selector.value);
    }
}

  INLINE void
bx_PUSH_CS()
{
#if BX_CPU >= 3
  /* bx_push_16() & bx_push_32() will use StackAddrSize to determine
   * use of either SP or ESP
   */
  if (bx_cpu.is_32bit_opsize) { /* OperandSize = 32 */
    bx_push_32(bx_cpu.cs.selector.value);
    }
  else
#endif
    { /* OperandSize = 16 */
    bx_push_16(bx_cpu.cs.selector.value);
    }
}

  INLINE void
bx_PUSH_SS()
{
#if BX_CPU >= 3
  /* bx_push_16() & bx_push_32() will use StackAddrSize to determine
   * use of either SP or ESP
   */
  if (bx_cpu.is_32bit_opsize) { /* OperandSize = 32 */
    bx_push_32(bx_cpu.ss.selector.value);
    }
  else
#endif
    { /* OperandSize = 16 */
    bx_push_16(bx_cpu.ss.selector.value);
    }
}

  INLINE void
bx_PUSH_DS()
{
#if BX_CPU >= 3
  /* bx_push_16() & bx_push_32() will use StackAddrSize to determine
   * use of either SP or ESP
   */
  if (bx_cpu.is_32bit_opsize) { /* OperandSize = 32 */
    bx_push_32(bx_cpu.ds.selector.value);
    }
  else
#endif
    { /* OperandSize = 16 */
    bx_push_16(bx_cpu.ds.selector.value);
    }
}

  INLINE void
bx_PUSH_FS()
{
#if BX_CPU >= 3
  /* bx_push_16() & bx_push_32() will use StackAddrSize to determine
   * use of either SP or ESP
   */
  if (bx_cpu.is_32bit_opsize) { /* OperandSize = 32 */
    bx_push_32(bx_cpu.fs.selector.value);
    }
  else
#endif
    { /* OperandSize = 16 */
    bx_push_16(bx_cpu.fs.selector.value);
    }
}

  INLINE void
bx_PUSH_GS()
{
#if BX_CPU >= 3
  /* bx_push_16() & bx_push_32() will use StackAddrSize to determine
   * use of either SP or ESP
   */
  if (bx_cpu.is_32bit_opsize) { /* OperandSize = 32 */
    bx_push_32(bx_cpu.gs.selector.value);
    }
  else
#endif
    { /* OperandSize = 16 */
    bx_push_16(bx_cpu.gs.selector.value);
    }
}

  INLINE void
bx_POP_ES()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    Bit32u es;

    bx_pop_32(&es);
    BX_HANDLE_EXCEPTION()

    bx_load_seg_reg(&bx_cpu.es, (Bit16u) es);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit opsize */
    Bit16u es;

    bx_pop_16(&es);
    BX_HANDLE_EXCEPTION()

    bx_load_seg_reg(&bx_cpu.es, es);
    }
}


  INLINE void
bx_POP_SS()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    Bit32u ss;

    bx_pop_32(&ss);
    BX_HANDLE_EXCEPTION()

    bx_load_seg_reg(&bx_cpu.ss, (Bit16u) ss);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit opsize */
    Bit16u ss;

    bx_pop_16(&ss);
    BX_HANDLE_EXCEPTION()

    bx_load_seg_reg(&bx_cpu.ss, ss);
    }
}

  INLINE void
bx_POP_DS()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    Bit32u ds;

    bx_pop_32(&ds);
    BX_HANDLE_EXCEPTION()

    bx_load_seg_reg(&bx_cpu.ds, (Bit16u) ds);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit opsize */
    Bit16u ds;

    bx_pop_16(&ds);
    BX_HANDLE_EXCEPTION()

    bx_load_seg_reg(&bx_cpu.ds, ds);
    }
}

  INLINE void
bx_POP_FS()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    Bit32u fs;

    bx_pop_32(&fs);
    BX_HANDLE_EXCEPTION()

    bx_load_seg_reg(&bx_cpu.fs, (Bit16u) fs);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit opsize */
    Bit16u fs;

    bx_pop_16(&fs);
    BX_HANDLE_EXCEPTION()

    bx_load_seg_reg(&bx_cpu.fs, fs);
    }
}

  INLINE void
bx_POP_GS()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    Bit32u gs;

    bx_pop_32(&gs);
    BX_HANDLE_EXCEPTION()

    bx_load_seg_reg(&bx_cpu.gs, (Bit16u) gs);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit opsize */
    Bit16u gs;

    bx_pop_16(&gs);
    BX_HANDLE_EXCEPTION()

    bx_load_seg_reg(&bx_cpu.gs, gs);
    }
}

  INLINE void
bx_PUSH_eAX()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    bx_push_32(EAX);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit opsize */
    bx_push_16(AX);
    }
}

  INLINE void
bx_PUSH_eCX()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    bx_push_32(ECX);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit opsize */
    bx_push_16(CX);
    }
}

  INLINE void
bx_PUSH_eDX()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    bx_push_32(EDX);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit opsize */
    bx_push_16(DX);
    }
}

  INLINE void
bx_PUSH_eBX()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    bx_push_32(EBX);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit opsize */
    bx_push_16(BX);
    }
}

  INLINE void
bx_PUSH_eSP()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    bx_push_32(ESP);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit opsize */
    bx_push_16(SP);
    }
}

  INLINE void
bx_PUSH_eBP()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    bx_push_32(EBP);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit opsize */
    bx_push_16(BP);
    }
}

  INLINE void
bx_PUSH_eSI()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    bx_push_32(ESI);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit opsize */
    bx_push_16(SI);
    }
}

  INLINE void
bx_PUSH_eDI()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    bx_push_32(EDI);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit opsize */
    bx_push_16(DI);
    }
}


  INLINE void
bx_POP_eAX()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    Bit32u eax;

    bx_pop_32(&eax);
    BX_HANDLE_EXCEPTION()

    bx_cpu.eax = eax;
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit opsize */
    Bit16u ax;

    bx_pop_16(&ax);
    BX_HANDLE_EXCEPTION()

    AX = ax;
    }
}

  INLINE void
bx_POP_eCX()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    Bit32u ecx;

    bx_pop_32(&ecx);
    BX_HANDLE_EXCEPTION()

    bx_cpu.ecx = ecx;
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit opsize */
    Bit16u cx;

    bx_pop_16(&cx);
    BX_HANDLE_EXCEPTION()

    CX = cx;
    }
}

  INLINE void
bx_POP_eDX()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    Bit32u edx;

    bx_pop_32(&edx);
    BX_HANDLE_EXCEPTION()

    bx_cpu.edx = edx;
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit opsize */
    Bit16u dx;

    bx_pop_16(&dx);
    BX_HANDLE_EXCEPTION()

    DX = dx;
    }
}

  INLINE void
bx_POP_eBX()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    Bit32u ebx;

    bx_pop_32(&ebx);
    BX_HANDLE_EXCEPTION()

    bx_cpu.ebx = ebx;
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit opsize */
    Bit16u bx;

    bx_pop_16(&bx);
    BX_HANDLE_EXCEPTION()

    BX = bx;
    }
}

  INLINE void
bx_POP_eSP()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    Bit32u esp;

    bx_pop_32(&esp);
    BX_HANDLE_EXCEPTION()

    bx_cpu.esp = esp;
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit opsize */
    Bit16u sp;

    bx_pop_16(&sp);
    BX_HANDLE_EXCEPTION()

    bx_cpu.esp = sp;
    }
}

  INLINE void
bx_POP_eBP()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    Bit32u ebp;

    bx_pop_32(&ebp);
    BX_HANDLE_EXCEPTION()

    bx_cpu.ebp = ebp;
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit opsize */
    Bit16u bp;

    bx_pop_16(&bp);
    BX_HANDLE_EXCEPTION()

    BP = bp;
    }
}

  INLINE void
bx_POP_eSI()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    Bit32u esi;

    bx_pop_32(&esi);
    BX_HANDLE_EXCEPTION()

    bx_cpu.esi = esi;
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit opsize */
    Bit16u si;

    bx_pop_16(&si);
    BX_HANDLE_EXCEPTION()

    SI = si;
    }
}

  INLINE void
bx_POP_eDI()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    Bit32u edi;

    bx_pop_32(&edi);
    BX_HANDLE_EXCEPTION()

    bx_cpu.edi = edi;
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit opsize */
    Bit16u di;

    bx_pop_16(&di);
    BX_HANDLE_EXCEPTION()

    DI = di;
    }
}

  INLINE void
bx_PUSHAD()
{
#if BX_CPU < 2
  bx_panic("PUSHAD: not supported on an 8086\n");
#else
  Bit32u temp_ESP;

  if (bx_cpu.ss.cache.u.segment.d_b)
    temp_ESP = ESP;
  else
    temp_ESP = SP;

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32u esp;

    if (bx_protected_mode()) {
      if ( !bx_can_push(&bx_cpu.ss.cache, temp_ESP, 32) ) {
        bx_panic("PUSHAD(): stack doesn't have enough room!\n");
        bx_exception(BX_SS_EXCEPTION, 0, 0);
        return;
        }
      }
    else {
      if (temp_ESP < 32)
        bx_panic("pushad: eSP < 32\n");
      }

    esp = ESP;

    /* ??? optimize this by using virtual write, all checks passed */
    bx_push_32(EAX);
    bx_push_32(ECX);
    bx_push_32(EDX);
    bx_push_32(EBX);
    bx_push_32(esp);
    bx_push_32(EBP);
    bx_push_32(ESI);
    bx_push_32(EDI);
    if (bx_cpu.errno)
      bx_panic("PUSHAD: error found after calling push_32()!\n");
    }
  else
#endif
    {
    Bit16u sp;

#if BX_CPU >= 2
    if (bx_protected_mode()) {
      if ( !bx_can_push(&bx_cpu.ss.cache, temp_ESP, 16) ) {
        bx_panic("PUSHA(): stack doesn't have enough room!\n");
        bx_exception(BX_SS_EXCEPTION, 0, 0);
        return;
        }
      }
    else
#endif
      {
      if (temp_ESP < 16)
        bx_panic("pushad: eSP < 16\n");
      }

    sp = SP;

    /* ??? optimize this by using virtual write, all checks passed */
    bx_push_16(AX);
    bx_push_16(CX);
    bx_push_16(DX);
    bx_push_16(BX);
    bx_push_16(sp);
    bx_push_16(BP);
    bx_push_16(SI);
    bx_push_16(DI);
    if (bx_cpu.errno) bx_panic("PUSHA: error found after calling push_16()!\n");
    }
  if (bx_dbg.protected)
    bx_printf("PUSHAD\n");
#endif
}

  INLINE void
bx_POPAD()
{
#if BX_CPU < 2
  bx_panic("POPAD not supported on an 8086\n");
#else /* 286+ */

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32u edi, esi, ebp, etmp, ebx, edx, ecx, eax;

    if (bx_protected_mode()) {
      if ( !bx_can_pop(32) ) {
        bx_panic("pop_ad: not enough bytes on stack\n");
        bx_exception(BX_SS_EXCEPTION, 0, 0);
        return;
        }
      }

    if (bx_dbg.protected)
      bx_printf("POPAD:\n");
    /* ??? optimize this */
    bx_pop_32(&edi);
    bx_pop_32(&esi);
    bx_pop_32(&ebp);
    bx_pop_32(&etmp); /* value for ESP discarded */
    bx_pop_32(&ebx);
    bx_pop_32(&edx);
    bx_pop_32(&ecx);
    bx_pop_32(&eax);
    if (bx_cpu.errno) bx_panic("popad: internal error\n");
    EDI = edi;
    ESI = esi;
    EBP = ebp;
    EBX = ebx;
    EDX = edx;
    ECX = ecx;
    EAX = eax;
    }
  else
#endif
    {
    Bit16u di, si, bp, tmp, bx, dx, cx, ax;

    if (bx_protected_mode()) {
      if ( !bx_can_pop(16) ) {
        bx_panic("pop_a: not enough bytes on stack\n");
        bx_exception(BX_SS_EXCEPTION, 0, 0);
        return;
        }
      }

    if (bx_dbg.protected)
      bx_printf("POPA:\n");
    /* ??? optimize this */
    bx_pop_16(&di);
    bx_pop_16(&si);
    bx_pop_16(&bp);
    bx_pop_16(&tmp); /* value for SP discarded */
    bx_pop_16(&bx);
    bx_pop_16(&dx);
    bx_pop_16(&cx);
    bx_pop_16(&ax);
    if (bx_cpu.errno) bx_panic("popad: internal error\n");
    DI = di;
    SI = si;
    BP = bp;
    BX = bx;
    DX = dx;
    CX = cx;
    AX = ax;
    }
#endif
}

  INLINE void
bx_PUSH_Iv()
{
#if BX_CPU < 2
  bx_panic("PUSH_Iv: not supported on 8086!\n");
#else

  if (bx_cpu.is_32bit_opsize) {
    Bit32u imm32;

    imm32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()

    bx_push_32(imm32);
    /* will return after error */
    }
  else
    {
    Bit16u imm16;

    imm16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    bx_push_16(imm16);
    /* will return after error */
    }
#endif
}

  INLINE void
bx_PUSH_Ib()
{
#if BX_CPU < 2
  bx_panic("PUSH_Ib: not supported on 8086!\n");
#else
  Bit8s  imm8;


  imm8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32s imm32;

    imm32 = imm8;
    bx_push_32(imm32);
    /* will return after error */
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16bit operand size */
    Bit16s imm16;

    imm16 = imm8;
    bx_push_16(imm16);
    /* will return after error */
    }
#endif /* BX_CPU < 2*/
}

  INLINE void
bx_POP_Ev()
{
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    Bit32u val32;

    if ( bx_protected_mode() ) {
      if ( !bx_can_pop(4) ) {
        bx_panic("pop_ev: can't pop 4 bytes\n");
        bx_exception(BX_SS_EXCEPTION, 0, 0);
        return;
        }
      }

    if (bx_cpu.ss.cache.u.segment.d_b)
       bx_read_virtual_dword(&bx_cpu.ss, ESP, &val32);
    else
       bx_read_virtual_dword(&bx_cpu.ss, SP, &val32);
    if (bx_cpu.errno)
      bx_panic("pop_ev: stack problem!\n");

    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, val32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &val32);
      BX_HANDLE_EXCEPTION()
      }

    if (bx_cpu.ss.cache.u.segment.d_b)
      ESP += 4;
    else
      SP += 4;
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
    Bit16u val16;

    if ( bx_protected_mode() ) {
      if ( !bx_can_pop(2) ) {
        bx_panic("pop_ev: can't pop 2 bytes\n");
        bx_exception(BX_SS_EXCEPTION, 0, 0);
        return;
        }
      }

    if (bx_cpu.ss.cache.u.segment.d_b)
       bx_read_virtual_word(&bx_cpu.ss, ESP, &val16);
    else
       bx_read_virtual_word(&bx_cpu.ss, SP, &val16);
    if (bx_cpu.errno)
      bx_panic("pop_ev: stack problem!\n");

    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, val16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &val16);
      BX_HANDLE_EXCEPTION()
      }

    if (bx_cpu.ss.cache.u.segment.d_b)
      ESP += 2;
    else
      SP += 2;
    }
}

  INLINE void
bx_PUSH_Ev()
{
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
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

    bx_push_32(op1_32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit operand size mode */
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

    bx_push_16(op1_16);
    }
}

  void
bx_push_16(Bit16u value16)
{
  Bit8u  prev_errno;
  Bit32u temp_ESP;


#if BX_CPU >= 2
  if (bx_protected_mode()) {
#if BX_CPU >= 3
    if (bx_cpu.ss.cache.u.segment.d_b)
      temp_ESP = ESP;
    else
#endif
      temp_ESP = SP;
    if (!bx_can_push(&bx_cpu.ss.cache, temp_ESP, 2)) {
      bx_panic("push_16(): can't push on stack\n");
      bx_exception(BX_SS_EXCEPTION, 0, 0);
      return;
      }

    /* access within limits */
    prev_errno = bx_cpu.errno;
    bx_write_virtual_word(&bx_cpu.ss, temp_ESP - 2, &value16);
    if (bx_cpu.errno > prev_errno) {
/* ??? */
bx_printf("SS.dpl = %u\n", (unsigned) bx_cpu.ss.cache.dpl);
bx_printf("CPL = %u\n", (unsigned) CPL);
      bx_panic("error in push_16()\n");
      }
    if (bx_cpu.ss.cache.u.segment.d_b)
      ESP -= 2;
    else
      SP -= 2;
    return;
    }
  else
#endif
    { /* real mode */
    if (bx_cpu.ss.cache.u.segment.d_b) {
      if (ESP < 2)
        bx_panic("CPU shutting down due to lack of stack space, ESP<2\n");
      ESP -= 2;
      temp_ESP = ESP;
      }
    else {
      if (SP == 1)
        bx_panic("CPU shutting down due to lack of stack space, SP==1\n");
      SP -= 2;
      temp_ESP = SP;
      }

    bx_write_virtual_word(&bx_cpu.ss, temp_ESP, &value16);
    return;
    }
}

#if BX_CPU >= 3
  /* push 32 bit operand size */
  void
bx_push_32(Bit32u value32)
{
  Bit8u prev_errno;

  prev_errno = bx_cpu.errno;

  /* must use StackAddrSize, and either ESP or SP accordingly */
  if (bx_cpu.ss.cache.u.segment.d_b) { /* StackAddrSize = 32 */
    /* 32bit stack size: pushes use SS:ESP  */
    if (bx_protected_mode()) {
      if (!bx_can_push(&bx_cpu.ss.cache, ESP, 4)) {
        bx_panic("push_32(): push outside stack limits\n");
        /* #SS(0) */
        }
      }
    else { /* real mode */
      if (ESP < 4) {
        bx_panic("CPU shutting down due to lack of stack space, ESP<4\n");
        }
      }

    bx_write_virtual_dword(&bx_cpu.ss, ESP-4, &value32);
    if (bx_cpu.errno > prev_errno)
      bx_panic("push_32(): internal error\n");
    ESP -= 4;
    /* will return after error anyway */
    return;
    }
  else { /* 16bit stack size: pushes use SS:SP  */
    if (bx_protected_mode()) {
      if (!bx_can_push(&bx_cpu.ss.cache, SP, 4)) {
        bx_panic("push_32(): push outside stack limits\n");
        /* #SS(0) */
        }
      }
    else { /* real mode */
      if (SP < 4) {
        bx_panic("CPU shutting down due to lack of stack space, SP<4\n");
        }
      }

    bx_write_virtual_dword(&bx_cpu.ss, SP-4, &value32);
    if (bx_cpu.errno > prev_errno)
      bx_panic("push_32(): internal error\n");
    SP -= 4;
    /* will return after error anyway */
    return;
    }
}
#endif /* BX_CPU >= 3 */

  void
bx_pop_16(Bit16u *value16_ptr)
{
  Bit32u temp_ESP;
  Bit8u  prev_errno;

#if BX_CPU >= 3
  if (bx_cpu.ss.cache.u.segment.d_b)
    temp_ESP = ESP;
  else
#endif
    temp_ESP = SP;

#if BX_CPU >= 2
  if (bx_protected_mode()) {
    if ( !bx_can_pop(2) ) {
      bx_panic("pop_16(): can't pop from stack\n");
      bx_exception(BX_SS_EXCEPTION, 0, 0);
      return;
      }
    }
#endif

  prev_errno = bx_cpu.errno;

  /* access within limits */
  bx_read_virtual_word(&bx_cpu.ss, temp_ESP, value16_ptr);
  if (bx_cpu.errno > prev_errno)
    bx_panic("pop_32: error encountered\n");

  if (bx_cpu.ss.cache.u.segment.d_b)
    ESP += 2;
  else
    SP += 2;
}

#if BX_CPU >= 3
  void
bx_pop_32(Bit32u *value32_ptr)
{
  Bit32u temp_ESP;
  Bit8u  prev_errno;

  /* 32 bit stack mode: use SS:ESP */
  if (bx_cpu.ss.cache.u.segment.d_b)
    temp_ESP = ESP;
  else
    temp_ESP = SP;

  /* 16 bit stack mode: use SS:SP */
  if (bx_protected_mode()) {
    if ( !bx_can_pop(4) ) {
      bx_panic("pop_32(): can't pop from stack\n");
      bx_exception(BX_SS_EXCEPTION, 0, 0);
      return;
      }
    }

  prev_errno = bx_cpu.errno;

  /* access within limits */
  bx_read_virtual_dword(&bx_cpu.ss, temp_ESP, value32_ptr);
  if (bx_cpu.errno > prev_errno)
    bx_panic("pop_32: error encountered\n");

  if (bx_cpu.ss.cache.u.segment.d_b==1)
    ESP += 4;
  else
    SP += 4;
}
#endif



#if BX_CPU >= 2
  Boolean
bx_can_push(bx_descriptor_t *descriptor, Bit32u esp, Bit32u bytes)
{
  Bit32u expand_down_limit;

  if ( bx_real_mode() ) { /* code not needed ??? */
    bx_panic("can_push(): called in real mode\n");
    return(0); /* never gets here */
    }

  if (descriptor->u.segment.d_b)
    expand_down_limit = 0xffffffff;
  else
    expand_down_limit = 0x0000ffff;

  if (descriptor->valid==0) {
    bx_panic("can_push(): SS invalidated.\n");
    return(0);
    }

  if (descriptor->p==0) {
    bx_panic("can_push(): not present\n");
    return(0);
    }

  if (descriptor->u.segment.limit_scaled==0) {
    bx_panic("can_push(): found limit of 0\n");
    return(0);
    }

  if (esp==0) {
    bx_panic("can_push(): esp=0, wraparound?\n");
    return(0);
    }

  if (descriptor->u.segment.c_ed) { /* expand down segment */
    if (esp < bytes) {
      bx_panic("can_push(): expand-down: esp < N\n");
      return(0);
      }
    if ( (esp - bytes) <= descriptor->u.segment.limit_scaled ) {
      bx_panic("can_push(): expand-down: esp-N < limit\n");
      return(0);
      }
    if ( esp > expand_down_limit ) {
      bx_panic("can_push(): esp > expand-down-limit\n");
      return(0);
      }
    return(1);
    }
  else { /* normal (expand-up) segment */
    if (esp < bytes) {
      bx_panic("can_push(): expand-up: esp < N\n");
      return(0);
      }
#if 0
/* ??? */
if (esp==0x00001000 && bx_cpu.ss.cache.u.segment.limit==0x0fff)
  bx_dbg.protected = 1;
#endif
    if ((esp-1) > descriptor->u.segment.limit_scaled) {
      bx_panic("can_push(): expand-up: SP > limit\n");
      return(0);
      }
    /* all checks pass */
    return(1);
    }
}
#endif

#if BX_CPU >= 2
  Boolean
bx_can_pop(Bit32u bytes)
{
  Bit32u temp_ESP, expand_down_limit;

  /* ??? */
  if (bx_real_mode()) bx_panic("can_pop(): called in real mode?\n");

  if (bx_cpu.ss.cache.u.segment.d_b) { /* Big bit set: use ESP */
    temp_ESP = ESP;
    expand_down_limit = 0xFFFFFFFF;
    }
  else { /* Big bit clear: use SP */
    temp_ESP = SP;
    expand_down_limit = 0xFFFF;
    }

  if (bx_cpu.ss.cache.valid==0) {
    bx_panic("can_pop(): SS invalidated.\n");
    return(0); /* never gets here */
    }

  if (bx_cpu.ss.cache.p==0) { /* ??? */
    bx_panic("can_pop(): SS.p = 0\n");
    return(0);
    }

  if (bx_cpu.ss.cache.u.segment.limit_scaled==0) {
    bx_panic("can_pop(): SS.limit = 0\n");
    }

  if (bx_cpu.ss.cache.u.segment.c_ed) { /* expand down segment */
    if ( temp_ESP == expand_down_limit ) {
      bx_panic("can_pop(): found SP=ffff\n");
      return(0);
      }
    if ( ((expand_down_limit - temp_ESP) + 1) >= bytes )
      return(1);
    return(0);
    }
  else { /* normal (expand-up) segment */
    if ( temp_ESP == expand_down_limit ) {
      bx_panic("can_pop(): found SP=ffff\n");
      return(0);
      }
    if ( temp_ESP > bx_cpu.ss.cache.u.segment.limit_scaled ) {
      bx_panic("can_pop(): eSP > SS.limit\n");
      return(0);
      }
    if ( ((bx_cpu.ss.cache.u.segment.limit_scaled - temp_ESP) + 1) >= bytes )
      return(1);
    return(0);
    }
}
#endif
