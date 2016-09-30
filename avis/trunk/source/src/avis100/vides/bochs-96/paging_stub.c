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





#include <string.h>
#include "bochs.h"





  void
bx_enable_paging(void)
{
  bx_panic("enable_paging(): not implemented\n");
}

  void
bx_disable_paging(void)
{
  bx_panic("disable_paging() called\n");
}

  void
bx_CR3_change(Bit32u value32)
{
  bx_printf("CR3_change(): flush TLB cache\n");
  bx_printf("Page Directory Base %08x\n", (unsigned) value32);
}


  void
bx_access_linear(Bit32u laddress, unsigned length, unsigned pl,
    unsigned rw, void *data)
{
  /* perhaps put this check before all code which calls this function,
   * so we don't have to here
   */
  if (bx_cpu.cr0.pg == 0) {
    bx_access_physical(laddress, length, rw, data);
    return;
    }

  bx_panic("access_linear: paging not supported\n");
}
