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


#ifndef _FETCH_H
#define _FETCH_H

#ifdef FOR_BOO
#include "forboo.h"
#endif

extern int g_do_boo; /* riad */

  static inline Bit8u
bx_fetch_next_byte(void)
{
  Bit8u data8;

#ifdef FOR_BOO
  if(g_do_boo && recordFetch) recordFetch(bx_cpu.cs.selector.value, bx_cpu.eip, 1);
#endif /* FOR_BOO */

  if (bx_cpu.eip > bx_cpu.cs.cache.u.segment.limit_scaled) {
  if(!bx_real_mode()) bx_panic("fetch_next_byte: EIP > CS.limit\n");
  //bx_panic("fetch_next_byte: EIP > CS.limit\n");
    }
#if BX_CPU >= 3
  if (bx_cpu.cr0.pg) {
    bx_access_linear(bx_cpu.cs.cache.u.segment.base + bx_cpu.eip++,
                     1, CPL==3, BX_READ, &data8);
    return(data8);
    }
#endif

  bx_access_physical(bx_cpu.cs.cache.u.segment.base + bx_cpu.eip++,
                     1, BX_READ, &data8);
  return(data8);
}


  static inline Bit8u
bx_peek_next_byte(void)
{
  Bit8u data8;

#ifdef FOR_BOO
  if(g_do_boo && recordFetch) recordFetch(bx_cpu.cs.selector.value, bx_cpu.eip, 1);
#endif /* FOR_BOO */

  if (bx_cpu.eip > bx_cpu.cs.cache.u.segment.limit_scaled) {
  if(!bx_real_mode()) bx_panic("peek_next_byte: EIP > CS.limit\n");
  //bx_panic("peek_next_byte: EIP > CS.limit\n");
    }
#if BX_CPU >= 3
  if (bx_cpu.cr0.pg) {
    bx_access_linear(bx_cpu.cs.cache.u.segment.base + bx_cpu.eip,
                     1, CPL==3, BX_READ, &data8);
    return(data8);
    }
#endif

  bx_access_physical(bx_cpu.cs.cache.u.segment.base + bx_cpu.eip,
                     1, BX_READ, &data8);
  return(data8);
}


  static inline Bit16u
bx_fetch_next_word(void)
{
  Bit16u data16;

#ifdef FOR_BOO
  if(g_do_boo && recordFetch) recordFetch(bx_cpu.cs.selector.value, bx_cpu.eip, 1);
#endif /* FOR_BOO */

  if (bx_cpu.eip > bx_cpu.cs.cache.u.segment.limit_scaled) {
#ifdef MYSTUFF /* riad */
	if(!bx_real_mode()) bx_panic("fetch_next_byte: EIP > CS.limit\n");
#else
    bx_panic("fetch_next_byte: EIP > CS.limit\n");
#endif
    }
#if BX_CPU >= 3
  if (bx_cpu.cr0.pg) {
    bx_access_linear(bx_cpu.cs.cache.u.segment.base + bx_cpu.eip,
                     2, CPL==3, BX_READ, &data16);
    bx_cpu.eip += 2;
    return(data16);
    }
#endif

  bx_access_physical(bx_cpu.cs.cache.u.segment.base + bx_cpu.eip,
                     2, BX_READ, &data16);
  bx_cpu.eip += 2;
  return(data16);
}


#if BX_CPU >= 3
  static inline Bit32u
bx_fetch_next_dword(void)
{
  Bit32u data32;

#ifdef FOR_BOO
  if(g_do_boo && recordFetch) recordFetch(bx_cpu.cs.selector.value, bx_cpu.eip, 1);
#endif /* FOR_BOO */

  if (bx_cpu.eip > bx_cpu.cs.cache.u.segment.limit_scaled) {
#ifdef MYSTUFF /* riad */
	if(!bx_real_mode()) bx_panic("fetch_next_byte: EIP > CS.limit\n");
#else
    bx_panic("fetch_next_byte: EIP > CS.limit\n");
#endif
    }
#if BX_CPU >= 3
  if (bx_cpu.cr0.pg) {
    bx_access_linear(bx_cpu.cs.cache.u.segment.base + bx_cpu.eip,
                     4, CPL==3, BX_READ, &data32);
    bx_cpu.eip += 4;
    return(data32);
    }
#endif

  bx_access_physical(bx_cpu.cs.cache.u.segment.base + bx_cpu.eip,
                     4, BX_READ, &data32);
  bx_cpu.eip += 4;
  return(data32);
}
#endif



#endif /* _FETCH_H */
