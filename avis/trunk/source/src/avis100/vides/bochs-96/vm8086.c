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

/* NOTE: Some comments may closely follow those from Intel 386 through
 *       Pentium Processor Family Developer's Manual, Volume 3.
 *       I've done this in areas where Bochs source code closely models
 *       Intel's published pseudo-code, as a way to reference between
 *       the two.
 */


#include "bochs.h"


#if BX_CPU >= 3

static void init_v8086_mode();

  void
bx_stack_return_to_v86(Bit32u new_eip, Bit32u raw_cs_selector, Bit32u flags32)
{
  Bit32u temp_ESP, new_esp;
  Bit16u raw_es_selector, raw_ds_selector, raw_fs_selector,
         raw_gs_selector, raw_ss_selector;


  /* Must be 32bit effective opsize, VM is in upper 16bits of eFLAGS */
  /* CPL = 0 to get here */

  /*    unused (ESP from TSS) ???
   * ----------------
   * |     | OLD GS | eSP+32
   * |     | OLD FS | eSP+28
   * |     | OLD DS | eSP+24
   * |     | OLD ES | eSP+20
   * |     | OLD SS | eSP+16
   * |  OLD ESP     | eSP+12
   * | OLD EFLAGS   | eSP+8
   * |     | OLD CS | eSP+4
   * |  OLD EIP     | eSP+0
   * ----------------
   */

  if (bx_cpu.ss.cache.u.segment.d_b)
    temp_ESP = ESP;
  else
    temp_ESP = SP;

  /* top 36 bytes of stack must be within stack limits, else #GP(0) */
  if ( !bx_can_pop(36) ) {
    bx_panic("iret: VM: top 36 bytes not within limits\n");
    bx_exception(BX_SS_EXCEPTION, 0, 0);
    return;
    }

  /* if new IP not within code segment limits, #GP(0) */
  if ( new_eip > 0xffff ) {
    bx_panic("stack_return_to_v86: IP > 65535 limit\n");
    bx_exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  /* EFLAGS = SS:[ESP + 8] , sets VM */
  bx_write_eflags(flags32, /*change IOPL*/ 1, /*change IF*/ 1,
                  /*change VM*/ 1, /*change RM*/ 0);

  /* load CS:EIP from stack; already read and passed as args */
  bx_cpu.cs.selector.value = raw_cs_selector;
  EIP = new_eip;

  /* load ES,DS,FS,GS from stack */
  bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 20,
    2, 0, BX_READ, &raw_es_selector);
  BX_HANDLE_EXCEPTION();

  bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 24,
    2, 0, BX_READ, &raw_ds_selector);
  BX_HANDLE_EXCEPTION();

  bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 28,
    2, 0, BX_READ, &raw_fs_selector);
  BX_HANDLE_EXCEPTION();

  bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 32,
    2, 0, BX_READ, &raw_gs_selector);
  BX_HANDLE_EXCEPTION();

  /* load SS:ESP from stack */
  bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 16,
    2, 0, BX_READ, &raw_ss_selector);
  BX_HANDLE_EXCEPTION();

  bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 12,
    4, 0, BX_READ, &new_esp);
  BX_HANDLE_EXCEPTION();

  bx_cpu.es.selector.value = raw_es_selector;
  bx_cpu.ds.selector.value = raw_ds_selector;
  bx_cpu.fs.selector.value = raw_fs_selector;
  bx_cpu.gs.selector.value = raw_gs_selector;

  bx_cpu.ss.selector.value = raw_ss_selector;
  ESP = new_esp;

  init_v8086_mode();
  if (bx_dbg.v8086) bx_printf("stack_return_to_v86: VM bit set on stack!\n");
}


  void
bx_stack_return_from_v86(void)
{
  if (IOPL != 3) {
    if (bx_dbg.v8086) {
      bx_printf("stack_return_from_v86: IOPL != 3\n");
      bx_printf("CPL = %u\n", (unsigned) CPL);
      }
    bx_exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  if (bx_cpu.is_32bit_opsize) {
    Bit32u eip, ecs, eflags;

    bx_pop_32(&eip);
    bx_pop_32(&ecs);
    bx_pop_32(&eflags);

    bx_load_seg_reg(&bx_cpu.cs, (Bit16u) ecs);
    bx_cpu.eip = eip;
    bx_write_eflags(eflags, /*IOPL*/ CPL==0, /*IF*/ 1, /*VM*/ 0, /*RF*/ 1);
    }
  else {
    Bit16u ip, cs, flags;

    bx_pop_16(&ip);
    bx_pop_16(&cs);
    bx_pop_16(&flags);

    bx_load_seg_reg(&bx_cpu.cs, cs);
    bx_cpu.eip = (Bit32u) ip;
    bx_write_flags(flags, /*IOPL*/ CPL==0, /*IF*/ 1);
    }
}


  static void
init_v8086_mode()
{
  bx_cpu.cs.cache.valid                  = 1;
  bx_cpu.cs.cache.p                      = 1;
  bx_cpu.cs.cache.dpl                    = 3;
  bx_cpu.cs.cache.segment                = 1;
  bx_cpu.cs.cache.u.segment.executable   = 1;
  bx_cpu.cs.cache.u.segment.c_ed         = 0;
  bx_cpu.cs.cache.u.segment.r_w          = 1;
  bx_cpu.cs.cache.u.segment.a            = 1;
  bx_cpu.cs.cache.u.segment.base         = bx_cpu.cs.selector.value << 4;
  bx_cpu.cs.cache.u.segment.limit        = 0xffff;
  bx_cpu.cs.cache.u.segment.limit_scaled = 0xffff;
  bx_cpu.cs.cache.u.segment.g            = 0;
  bx_cpu.cs.cache.u.segment.d_b          = 0;
  bx_cpu.cs.cache.u.segment.avl          = 0;
  bx_cpu.cs.selector.rpl                 = 3;

  bx_cpu.ss.cache.valid                  = 1;
  bx_cpu.ss.cache.p                      = 1;
  bx_cpu.ss.cache.dpl                    = 3;
  bx_cpu.ss.cache.segment                = 1;
  bx_cpu.ss.cache.u.segment.executable   = 0;
  bx_cpu.ss.cache.u.segment.c_ed         = 0;
  bx_cpu.ss.cache.u.segment.r_w          = 1;
  bx_cpu.ss.cache.u.segment.a            = 1;
  bx_cpu.ss.cache.u.segment.base         = bx_cpu.ss.selector.value << 4;
  bx_cpu.ss.cache.u.segment.limit        = 0xffff;
  bx_cpu.ss.cache.u.segment.limit_scaled = 0xffff;
  bx_cpu.ss.cache.u.segment.g            = 0;
  bx_cpu.ss.cache.u.segment.d_b          = 0;
  bx_cpu.ss.cache.u.segment.avl          = 0;
  bx_cpu.ss.selector.rpl                 = 3;

  bx_cpu.es.cache.valid                  = 1;
  bx_cpu.es.cache.p                      = 1;
  bx_cpu.es.cache.dpl                    = 3;
  bx_cpu.es.cache.segment                = 1;
  bx_cpu.es.cache.u.segment.executable   = 0;
  bx_cpu.es.cache.u.segment.c_ed         = 0;
  bx_cpu.es.cache.u.segment.r_w          = 1;
  bx_cpu.es.cache.u.segment.a            = 1;
  bx_cpu.es.cache.u.segment.base         = bx_cpu.es.selector.value << 4;
  bx_cpu.es.cache.u.segment.limit        = 0xffff;
  bx_cpu.es.cache.u.segment.limit_scaled = 0xffff;
  bx_cpu.es.cache.u.segment.g            = 0;
  bx_cpu.es.cache.u.segment.d_b          = 0;
  bx_cpu.es.cache.u.segment.avl          = 0;
  bx_cpu.es.selector.rpl                 = 3;

  bx_cpu.ds.cache.valid                  = 1;
  bx_cpu.ds.cache.p                      = 1;
  bx_cpu.ds.cache.dpl                    = 3;
  bx_cpu.ds.cache.segment                = 1;
  bx_cpu.ds.cache.u.segment.executable   = 0;
  bx_cpu.ds.cache.u.segment.c_ed         = 0;
  bx_cpu.ds.cache.u.segment.r_w          = 1;
  bx_cpu.ds.cache.u.segment.a            = 1;
  bx_cpu.ds.cache.u.segment.base         = bx_cpu.ds.selector.value << 4;
  bx_cpu.ds.cache.u.segment.limit        = 0xffff;
  bx_cpu.ds.cache.u.segment.limit_scaled = 0xffff;
  bx_cpu.ds.cache.u.segment.g            = 0;
  bx_cpu.ds.cache.u.segment.d_b          = 0;
  bx_cpu.ds.cache.u.segment.avl          = 0;
  bx_cpu.ds.selector.rpl                 = 3;

  bx_cpu.fs.cache.valid                  = 1;
  bx_cpu.fs.cache.p                      = 1;
  bx_cpu.fs.cache.dpl                    = 3;
  bx_cpu.fs.cache.segment                = 1;
  bx_cpu.fs.cache.u.segment.executable   = 0;
  bx_cpu.fs.cache.u.segment.c_ed         = 0;
  bx_cpu.fs.cache.u.segment.r_w          = 1;
  bx_cpu.fs.cache.u.segment.a            = 1;
  bx_cpu.fs.cache.u.segment.base         = bx_cpu.fs.selector.value << 4;
  bx_cpu.fs.cache.u.segment.limit        = 0xffff;
  bx_cpu.fs.cache.u.segment.limit_scaled = 0xffff;
  bx_cpu.fs.cache.u.segment.g            = 0;
  bx_cpu.fs.cache.u.segment.d_b          = 0;
  bx_cpu.fs.cache.u.segment.avl          = 0;
  bx_cpu.fs.selector.rpl                 = 3;

  bx_cpu.gs.cache.valid                  = 1;
  bx_cpu.gs.cache.p                      = 1;
  bx_cpu.gs.cache.dpl                    = 3;
  bx_cpu.gs.cache.segment                = 1;
  bx_cpu.gs.cache.u.segment.executable   = 0;
  bx_cpu.gs.cache.u.segment.c_ed         = 0;
  bx_cpu.gs.cache.u.segment.r_w          = 1;
  bx_cpu.gs.cache.u.segment.a            = 1;
  bx_cpu.gs.cache.u.segment.base         = bx_cpu.gs.selector.value << 4;
  bx_cpu.gs.cache.u.segment.limit        = 0xffff;
  bx_cpu.gs.cache.u.segment.limit_scaled = 0xffff;
  bx_cpu.gs.cache.u.segment.g            = 0;
  bx_cpu.gs.cache.u.segment.d_b          = 0;
  bx_cpu.gs.cache.u.segment.avl          = 0;
  bx_cpu.gs.selector.rpl                 = 3;
}


#endif /* BX_CPU >= 3 */
