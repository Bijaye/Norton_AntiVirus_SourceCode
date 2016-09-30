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





  INLINE void
bx_SAHF()
{
  bx_set_SF((AH & 0x80) >> 7);
  bx_set_ZF((AH & 0x40) >> 6);
  bx_set_AF((AH & 0x10) >> 4);
  bx_set_CF(AH & 0x01);
  bx_set_PF((AH & 0x04) >> 2);
}

  INLINE void
bx_LAHF()
{
  AH = (bx_get_SF() ? 0x80 : 0) |
       (bx_get_ZF() ? 0x40 : 0) |
       (bx_get_AF() ? 0x10 : 0) |
       (bx_get_PF() ? 0x04 : 0) |
       (0x02) |
       (bx_get_CF() ? 0x01 : 0);
}

  INLINE void
bx_CLC()
{
  bx_set_CF(0);
}

  INLINE void
bx_STC()
{
  bx_set_CF(1);
}

  INLINE void
bx_CLI()
{
#if BX_CPU >= 2
  if (bx_protected_mode()) {
    if (CPL > IOPL) {
      bx_printf("CLI: CPL > IOPL\n"); /* ??? */
      bx_exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }
    }
#if BX_CPU >= 3
  else if (bx_v8086_mode()) {
    if (IOPL != 3) {
      bx_printf("CLI: IOPL != 3\n"); /* ??? */
      bx_exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }
    }
#endif
#endif

  bx_cpu.eflags.if_ = 0; 
}

  INLINE void
bx_STI()
{
#if BX_CPU >= 2
  if (bx_protected_mode()) {
    if (CPL > IOPL) {
      bx_printf("STI: CPL > IOPL\n"); /* ??? */
      bx_exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }
    }
#if BX_CPU >= 3
  else if (bx_v8086_mode()) {
    if (IOPL != 3) {
      bx_printf("STI: IOPL != 3\n"); /* ??? */
      bx_exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }
    }
#endif
#endif

  if (!bx_cpu.eflags.if_) {
    bx_cpu.eflags.if_ = 1;
    bx_cpu.inhibit_interrupts = 1;
    bx_async_event = 1;
    }
}

  INLINE void
bx_CLD()
{
  bx_cpu.eflags.df = 0;
}

  INLINE void
bx_STD()
{
  bx_cpu.eflags.df = 1;
}

  INLINE void
bx_CMC()
{
  bx_set_CF( !bx_get_CF() );
}


  void
bx_write_flags(Bit16u flags, Boolean change_IOPL, Boolean change_IF)
{
  bx_set_CF(flags & 0x01);
  bx_set_PF((flags >> 2) & 0x01);
  bx_set_AF((flags >> 4) & 0x01);
  bx_set_ZF((flags >> 6) & 0x01);
  bx_set_SF((flags >> 7) & 0x01);
  bx_cpu.eflags.tf = (flags >> 8) & 0x01;

  if (bx_cpu.eflags.tf) {
	bx_async_event = 1;
	//bx_dbg.debugger = 1;
	//bx_debug(&bx_cpu.cs, bx_cpu.prev_eip);
	//bx_panic("write_flags(): TF set: bailing\n");
    }

  if (change_IF)
    bx_cpu.eflags.if_ = (flags >> 9) & 0x01;

  bx_cpu.eflags.df = (flags >> 10) & 0x01;
  bx_set_OF((flags >> 11) & 0x01);

  /* real-mode doesn't change IOPL & NT flags ??? */
  if (!bx_real_mode()) {
    if (change_IOPL)
      bx_cpu.eflags.iopl = (flags >> 12) & 0x03;
    bx_cpu.eflags.nt = (flags >> 14) & 0x01;
    }
  else { /* real mode */
    /* 286: bits 12-15 always clear
     * 386: bit 15 always clear, bits 14-12 maintain last value loaded
     */
#if BX_CPU == 2
    bx_cpu.eflags.iopl = 0;
    bx_cpu.eflags.nt = 0;
#elif BX_CPU >= 3
    /* values shouldn't be used in real mode, but are loaded into eflags */
    bx_cpu.eflags.iopl = (flags >> 12) & 0x03;
    bx_cpu.eflags.nt = (flags >> 14) & 0x01;
#endif
    }
}

#if BX_CPU >= 3
  void
bx_write_eflags(Bit32u eflags, Boolean change_IOPL, Boolean change_IF,
                Boolean change_VM, Boolean change_RF)
{
  if (bx_v8086_mode()) bx_panic("write_eflags: v8086 mode unsupported\n");

  bx_set_CF(eflags & 0x01);
  bx_set_PF((eflags >> 2) & 0x01);
  bx_set_AF((eflags >> 4) & 0x01);
  bx_set_ZF((eflags >> 6) & 0x01);
  bx_set_SF((eflags >> 7) & 0x01);
  bx_cpu.eflags.tf = (eflags >> 8) & 0x01;
  if (bx_cpu.eflags.tf) {
    bx_async_event = 1;
    bx_panic("write_eflags(): TF set: bailing\n");
    }

  if (change_IF)
    bx_cpu.eflags.if_ = (eflags >> 9) & 0x01;

  bx_cpu.eflags.df = (eflags >> 10) & 0x01;
  bx_set_OF((eflags >> 11) & 0x01);

  /* real-mode doesn't change IOPL & NT flags ??? */
  if (bx_protected_mode()) {
    if (change_IOPL)
      bx_cpu.eflags.iopl = (eflags >> 12) & 0x03;
    bx_cpu.eflags.nt = (eflags >> 14) & 0x01;
    }
  else { /* real mode */
    /* 386+: bit 15 always clear, bits 14-12 maintain last value loaded
     */
    /* values shouldn't be used in real mode, but are loaded into eflags */
    bx_cpu.eflags.iopl = (eflags >> 12) & 0x03;
    bx_cpu.eflags.nt = (eflags >> 14) & 0x01;
    }

  if (change_VM) {
    bx_cpu.eflags.vm = (eflags >> 17) & 0x01;
#if BX_SUPPORT_V8086_MODE == 0
    if (bx_cpu.eflags.vm)
      bx_panic("write_eflags: VM bit set: BX_SUPPORT_V8086_MODE==0\n");
#endif
    }
  if (change_RF) {
    bx_cpu.eflags.rf = (eflags >> 16) & 0x01;
    }
}
#endif /* BX_CPU >= 3 */

  Bit16u
bx_read_flags(void)
{
  Bit16u flags;

  flags = (bx_get_CF()) |
          (bx_cpu.eflags.bit1 << 1) |
          ((bx_get_PF()) << 2) |
          (bx_cpu.eflags.bit3 << 3) |
          ((bx_get_AF()>0) << 4) |
          (bx_cpu.eflags.bit5 << 5) |
          ((bx_get_ZF()>0) << 6) |
          ((bx_get_SF()>0) << 7) |
          (bx_cpu.eflags.tf << 8) |
          (bx_cpu.eflags.if_ << 9) |
          (bx_cpu.eflags.df << 10) |
          ((bx_get_OF()>0) << 11) |
          (bx_cpu.eflags.iopl << 12) |
          (bx_cpu.eflags.nt << 14) |
          (bx_cpu.eflags.bit15 << 15);

  /* 8086: bits 12-15 always set to 1.
   * 286: in real mode, bits 12-15 always cleared.
   * 386+: real-mode: bit15 cleared, bits 14..12 are last loaded value
   *       protected-mode: bit 15 clear, bit 14 = last loaded, IOPL?
   */
#if BX_CPU < 2
  flags |= 0xF000;  /* 8086 nature */
#elif BX_CPU == 2
  if (bx_real_mode()) {
    flags &= 0x0FFF;  /* 80286 in real mode nature */
    }
#else /* 386+ */
  if (bx_real_mode()) {
    flags &= 0x7FFF;  /* 80386 in real mode nature */
    }
#endif

  return(flags);
}


#if BX_CPU >= 3
  Bit32u
bx_read_eflags(void)
{
  Bit32u eflags;

#if BX_CPU >= 4
  bx_panic("read_eflags(): not implemented for 80486+\n");
#endif

  eflags = (bx_get_CF()) |
          (bx_cpu.eflags.bit1 << 1) |
          ((bx_get_PF()) << 2) |
          (bx_cpu.eflags.bit3 << 3) |
          ((bx_get_AF()>0) << 4) |
          (bx_cpu.eflags.bit5 << 5) |
          ((bx_get_ZF()>0) << 6) |
          ((bx_get_SF()>0) << 7) |
          (bx_cpu.eflags.tf << 8) |
          (bx_cpu.eflags.if_ << 9) |
          (bx_cpu.eflags.df << 10) |
          ((bx_get_OF()>0) << 11) |
          (bx_cpu.eflags.iopl << 12) |
          (bx_cpu.eflags.nt << 14) |
          (bx_cpu.eflags.bit15 << 15) |
          (bx_cpu.eflags.rf << 16) |
          (bx_cpu.eflags.vm << 17);

  /*
   * 386+: real-mode: bit15 cleared, bits 14..12 are last loaded value
   *       protected-mode: bit 15 clear, bit 14 = last loaded, IOPL?
   */
  if (bx_real_mode()) {
    eflags &= 0x7FFF;  /* 80386 in real mode nature */
    }

  return(eflags);
}
#endif /* BX_CPU >= 3 */


  INLINE void
bx_PUSHF_Fv()
{
  if (bx_v8086_mode() && IOPL<3) {
    bx_exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    bx_push_32(bx_read_eflags() & 0x00fcffff);
    /* will return after error anyway */
    }
  else
#endif
    {
    bx_push_16(bx_read_flags());
    /* will return after error anyway */
    }
}


  INLINE void
bx_POPF_Fv()
{

#if BX_CPU >= 3
  if (bx_v8086_mode()) {
    if (IOPL < 3) {
      bx_printf("popf_fv: IOPL < 3\n");
      bx_exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }
    if (bx_cpu.is_32bit_opsize) {
      bx_panic("POPFD(): not supported in virtual mode\n");
      bx_exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }
    }

  if (bx_cpu.is_32bit_opsize) {
    Bit32u eflags;

    bx_pop_32(&eflags);
    BX_HANDLE_EXCEPTION()

    eflags &= 0x00277fd7;
    if (!bx_real_mode()) {
      bx_write_eflags(eflags, /*IOPL*/ CPL==0, /*IF*/ CPL<=IOPL, 0, 0);
      }
    else { /* real mode */
      bx_write_eflags(eflags, 1, 1, 0, 0);
      }
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit opsize */
    Bit16u flags;

    bx_pop_16(&flags);
    BX_HANDLE_EXCEPTION()

    if (!bx_real_mode()) {
      bx_write_flags(flags, /*IOPL*/ CPL==0, /*IF*/ CPL<=IOPL);
      }
    else { /* real mode */
      bx_write_flags(flags, 1, 1);
      }
    }
}
