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
#include "iodev/iodev.h"
#include "..\bochsw\watcher\watcher.h"


/*
Bit8u   bx_cpu_inp8(Bit16u addr);
void    bx_cpu_outp8(Bit16u addr, Bit8u value);
Bit16u  bx_cpu_inp16(Bit16u addr);
void    bx_cpu_outp16(Bit16u addr, Bit16u value);
Bit32u  bx_cpu_inp32(Bit16u addr);
void    bx_cpu_outp32(Bit16u addr, Bit32u value);
*/

static Boolean allow_io(Bit16u addr, unsigned len);


   Bit16u
bx_cpu_inp16(Bit16u addr)
{
  if (bx_cpu.cr0.pe && (bx_cpu.eflags.vm || (CPL>IOPL))) {
    if ( !allow_io(addr, 2) ) {
      bx_printf("cpu_inp16: GP0()!\n");
      bx_exception(BX_GP_EXCEPTION, 0, 0);
      return(0);
      }
    }

  return( bx_inp(addr, 2) );
}

   void
bx_cpu_outp16(Bit16u addr, Bit16u value)
{
  /* If CPL <= IOPL, then all IO addresses are accessible.
   * Otherwise, must check the IO permission map on >286.
   * On the 286, there is no IO permissions map */

  if (bx_cpu.cr0.pe && (bx_cpu.eflags.vm || (CPL>IOPL))) {
    if ( !allow_io(addr, 2) ) {
      bx_printf("cpu_outp16: GP0()!\n");
      bx_exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }
    }

  bx_outp(addr, value, 2);
}

   Bit32u
bx_cpu_inp32(Bit16u addr)
{
  if (bx_cpu.cr0.pe && (bx_cpu.eflags.vm || (CPL>IOPL))) {
    if ( !allow_io(addr, 4) ) {
      bx_printf("cpu_inp32: GP0()!\n");
      bx_exception(BX_GP_EXCEPTION, 0, 0);
      return(0);
      }
    }

  return( bx_inp(addr, 4) );
}

   void
bx_cpu_outp32(Bit16u addr, Bit32u value)
{
  /* If CPL <= IOPL, then all IO addresses are accessible.
   * Otherwise, must check the IO permission map on >286.
   * On the 286, there is no IO permissions map */

  if (bx_cpu.cr0.pe && (bx_cpu.eflags.vm || (CPL>IOPL))) {
    if ( !allow_io(addr, 4) ) {
      bx_printf("cpu_outp32: GP0()!\n");
      bx_exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }
    }

  bx_outp(addr, value, 4);
}

   Bit8u
bx_cpu_inp8(Bit16u addr)
{
  if (bx_cpu.cr0.pe && (bx_cpu.eflags.vm || (CPL>IOPL))) {
    if ( !allow_io(addr, 1) ) {
      bx_printf("cpu_inp8: GP0()!\n");
      bx_exception(BX_GP_EXCEPTION, 0, 0);
      return(0);
      }
    }

  return( bx_inp(addr, 1) );
}


   void
bx_cpu_outp8(Bit16u addr, Bit8u value)
{
  /* If CPL <= IOPL, then all IO addresses are accessible.
   * Otherwise, must check the IO permission map on >286.
   * On the 286, there is no IO permissions map */

  if (bx_cpu.cr0.pe && (bx_cpu.eflags.vm || (CPL>IOPL))) {
    if ( !allow_io(addr, 1) ) {
      bx_printf("cpu_outp8: GP0()!\n");
      bx_exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }
    }

  bx_outp(addr, value, 1);
}



  INLINE void
bx_INSB_YbDX()
{
  Bit8u value8;

  /* output value16 to port DX */
  value8 = bx_cpu_inp8(DX);
  BX_HANDLE_EXCEPTION()

  if (bx_cpu.is_32bit_addrsize) {
    /* no seg override possible */
    bx_write_virtual_byte(&bx_cpu.es, EDI, &value8);
    BX_HANDLE_EXCEPTION()

    if (bx_cpu.eflags.df) {
      EDI = EDI - 1;
      }
    else {
      EDI = EDI + 1;
      }
    }
  else {
    /* no seg override possible */
    bx_write_virtual_byte(&bx_cpu.es, DI, &value8);
    BX_HANDLE_EXCEPTION()

    if (bx_cpu.eflags.df) {
      DI = DI - 1;
      }
    else {
      DI = DI + 1;
      }
    }
}

  INLINE void
bx_INSW_YvDX()
{
  Bit16u value16;

  if (bx_cpu.is_32bit_addrsize || bx_cpu.is_32bit_opsize) {
    bx_panic("insw(): 32bit addrsize/opsize unhandled\n");
    }

  /* input value16 from port DX */
  value16 = bx_cpu_inp16(DX);
  BX_HANDLE_EXCEPTION()

  /* no seg override allowed */
  bx_write_virtual_word(&bx_cpu.es, DI, &value16);
  BX_HANDLE_EXCEPTION()

  if (bx_cpu.eflags.df) {
    DI = DI - 2;
    }
  else {
    DI = DI + 2;
    }
}

  INLINE void
bx_OUTSB_DXXb()
{
  bx_segment_reg_t *seg;
  Bit8u value8;


  if (bx_cpu.is_32bit_addrsize) {
    bx_panic("outsb(): 32bit addrsize unhandled\n");
    }

  if (bx_cpu.segment_override) {
    seg = bx_cpu.segment_override;
    }
  else {
    seg = &bx_cpu.ds;
    }

  bx_read_virtual_byte(seg, SI, &value8);
  BX_HANDLE_EXCEPTION()

  /* output value16 to port DX */
  bx_cpu_outp8(DX, value8);
  BX_HANDLE_EXCEPTION()

  if (bx_cpu.eflags.df) {
    SI = SI - 1;
    }
  else {
    SI = SI + 1;
    }
}

  INLINE void
bx_OUTSW_DXXv()
{
  bx_segment_reg_t *seg;
  Bit16u value16;


  if (bx_cpu.is_32bit_addrsize || bx_cpu.is_32bit_opsize) {
    bx_panic("outsw(): 32bit addrsize/opsize unhandled\n");
    }

  if (bx_cpu.segment_override) {
    seg = bx_cpu.segment_override;
    }
  else {
    seg = &bx_cpu.ds;
    }

  bx_read_virtual_word(seg, SI, &value16);
  BX_HANDLE_EXCEPTION()

  /* output value16 to port DX */
  bx_cpu_outp16(DX, value16);
  BX_HANDLE_EXCEPTION()

  if (bx_cpu.eflags.df) {
    SI = SI - 2;
    }
  else {
    SI = SI + 2;
    }
}


  INLINE void
bx_IN_ALIb()
{
  Bit8u al, imm8;

  imm8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  al = bx_cpu_inp8(imm8);
  BX_HANDLE_EXCEPTION()

  AL = al;
}

  INLINE void
bx_IN_eAXIb()
{
  Bit8u imm8;


  imm8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    Bit32u eax;

    eax = bx_cpu_inp32(imm8);
    BX_HANDLE_EXCEPTION()
    EAX = eax;
    }
  else
#endif /* BX_CPU > 2 */
    {
    Bit16u ax;

    ax = bx_cpu_inp16(imm8);
    BX_HANDLE_EXCEPTION()
    AX = ax;
    }
}

  INLINE void
bx_OUT_IbAL()
{
  Bit8u al, imm8;

  imm8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  al = AL;

/*  if (bx_protected_mode() && bx_dbg.protected)
    bx_printf("out_ibal(%02x, %02x):\n", (unsigned) imm8, (unsigned) al);
*/
  bx_cpu_outp8(imm8, al);
  BX_HANDLE_EXCEPTION()
}

  INLINE void
bx_OUT_IbeAX()
{
  Bit8u imm8;

  imm8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    bx_cpu_outp32(imm8, EAX);
    BX_HANDLE_EXCEPTION()
    }
  else
#endif /* BX_CPU > 2 */
    {
    bx_cpu_outp16(imm8, AX);
    BX_HANDLE_EXCEPTION()
    }
}

  INLINE void
bx_IN_ALDX()
{
  Bit8u al;

  al = bx_cpu_inp8(DX);
  BX_HANDLE_EXCEPTION()

  AL = al;
}

  INLINE void
bx_IN_eAXDX()
{
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    Bit32u eax;

    eax = bx_cpu_inp32(DX);
    BX_HANDLE_EXCEPTION()
    EAX = eax;
    }
  else
#endif /* BX_CPU > 2 */
    {
    Bit16u ax;

    ax = bx_cpu_inp16(DX);
    BX_HANDLE_EXCEPTION()
    AX = ax;
    }
}

  INLINE void
bx_OUT_DXAL()
{
  Bit16u dx;
  Bit8u al;

  dx = DX;
  al = AL;

  bx_cpu_outp8(dx, al);
  BX_HANDLE_EXCEPTION()
}

  INLINE void
bx_OUT_DXeAX()
{
  Bit16u dx;

  dx = DX;

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    bx_cpu_outp32(dx, EAX);
    BX_HANDLE_EXCEPTION()
    }
  else
#endif /* BX_CPU > 2 */
    {
    bx_cpu_outp16(dx, AX);
    BX_HANDLE_EXCEPTION()
    }
}

  static Boolean
allow_io(Bit16u addr, unsigned len)
{
  Bit16u io_base, permission16;
  unsigned bit_index, i;

  if (bx_cpu.tr.cache.valid==0 || bx_cpu.tr.cache.type!=9) {
    bx_printf("allow_io(): TR doesn't point to a valid 32bit TSS\n");
    return(0);
    }

#if 0
  bx_printf("allow_io(): TR.limit = %u\n",
    (unsigned) bx_cpu.tr.cache.u.tss386.limit);
#endif
  if (bx_cpu.tr.cache.u.tss386.limit < 103) {
    bx_panic("allow_io(): TR.limit < 103\n");
    }

  bx_access_linear(bx_cpu.tr.cache.u.tss386.base + 102, 2, 0, BX_READ,
                         &io_base);
  if (io_base <= 103) {
    bx_panic("allow_io(): TR:io_base <= 103\n");
    }

  if (io_base > bx_cpu.tr.cache.u.tss386.limit) {
    bx_printf("allow_io(): CPL > IOPL: no IO bitmap defined #GP(0)\n");
    return(0);
    }

  bx_access_linear(bx_cpu.tr.cache.u.tss386.base + io_base + addr/8,
                   2, 0, BX_READ, &permission16);

  bit_index = addr & 0x07;
  permission16 >>= bit_index;
  for (i=0; i<len; i++) {
    if (permission16 & 0x01)
      return(0);
    permission16 >>= 1;
    }

  return(1);
}
