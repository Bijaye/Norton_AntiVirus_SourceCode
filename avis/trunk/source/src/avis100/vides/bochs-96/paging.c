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


/* Maximum number of megabytes of virtual memory to support.
 * Make this a multiple of 4 for most efficient use of cache memory
 */
#define BX_VIRTUAL_MEM_MEGS_MAX 32
#define NUM_TLB_CACHE_PAGES ((BX_VIRTUAL_MEM_MEGS_MAX+3) / 4)

void TLB_flush(void);
static inline Bit32u translate_linear(Bit32u laddress, unsigned pl, unsigned rw);


static Bit32u cr2;

#if 0
static struct {
  /* cache def here */
  Bit32u   page_directory[4*1024];
  unsigned cache_index[4*1024];
  /* size? */ Boolean  valid[4*1024];

  Bit32u   cache[NUM_TLB_CACHE_PAGES][4*1024];
  unsigned cache_pages_used;
  void (* flush)(void);
  } TLB = { 0, TLB_flush };
#endif

static struct {
  /* cache def here */
  Boolean valid;
  void (* flush)(void);
  } TLB = { 0, TLB_flush };


  void
bx_enable_paging(void)
{
  TLB.flush();
  if (bx_dbg.paging) bx_printf("enable_paging():\n");
}

  void
bx_disable_paging(void)
{
  TLB.flush();
  if (bx_dbg.paging) bx_printf("disable_paging():\n");
}

  void
bx_CR3_change(Bit32u value32)
{
  if (bx_dbg.paging) {
    bx_printf("CR3_change(): flush TLB cache\n");
    bx_printf("Page Directory Base %08x\n", (unsigned) value32);
    }
  TLB.flush();
  bx_cpu.cr3 = value32;
}

  void
TLB_flush(void)
{
  TLB.valid = 0;
}


  static inline Bit32u
translate_linear(Bit32u laddress, unsigned pl, unsigned rw)
{
  Bit32u dir_entry_addr, page_table_entry_addr;
  Bit32u dir_entry, page_table_entry;
  Bit32u page_frame_address, paddress;
  Bit16u errorcode;

  /* Protection:
   *   When the processor is running in supervisor level, all pages are both
   *   readable and writable (write-protect ignored).  When running at user
   *   level, only pages which belong to the user level are accessible;
   *   read/write & read-only are readable, read/write are writable.
   *
   * Accessed bits:
   *   Processor sets the Accessed bits in both levels of page tables before
   *   a read/write operation to a page.
   *
   * Dirty bits:
   *   Processor sets the Dirty bit in the 2nd-level page table before a
   *   write operation to an address mapped by that page table entry.
   *   Dirty bit in directory entries is undefined.
   *   
   */
  dir_entry_addr = (bx_cpu.cr3 & 0xfffff000) |
                   ((laddress & 0xffc00000) >> 20);
  bx_access_physical(dir_entry_addr, 4, BX_READ, &dir_entry);
  /*bx_printf("paging: DIR entry: %08x\n", (unsigned) dir_entry);*/
  if ( (dir_entry & 0x01) == 0) {
    bx_panic("access_linear: dir entry not present\n");
    cr2 = laddress;
    errorcode = 0x04 | (CPL==3);
    bx_exception(BX_PF_EXCEPTION, errorcode, 0);
    return(0);
    }
  if ( pl ) {
    if ( (dir_entry & 0x04) == 0 ) {
      bx_panic("paging: no permission: directory entry\n");
      }
    if ( (rw == BX_WRITE) && ((dir_entry & 0x02)==0) ) {
      bx_panic("paging: no write permission: directory entry\n");
      }
    }

  page_table_entry_addr = (dir_entry & 0xfffff000) |
                          ((laddress & 0x003ff000) >> 10);
  bx_access_physical(page_table_entry_addr, 4, BX_READ, &page_table_entry);
  /*bx_printf("paging: page table entry: %08x\n", (unsigned) page_table_entry);*/

  if ( (page_table_entry & 0x01) == 0 )
    bx_panic("access_linear: page table entry not present\n");

  if ( pl ) {
    if ( (page_table_entry & 0x04) == 0 ) {
      bx_panic("paging: no permission: page table entry\n");
      }
    if ( (rw == BX_WRITE) && ((page_table_entry & 0x02)==0) ) {
      bx_panic("paging: no write permission: page table entry\n");
      }
    }

  page_frame_address = (page_table_entry & 0xfffff000);
  paddress = page_frame_address | (laddress & 0x00000fff);

  dir_entry        |= 0x20;
  page_table_entry |= 0x20;
  if (rw==BX_WRITE)
    page_table_entry |= 0x40;
  bx_access_physical(dir_entry_addr, 4, BX_READ, &dir_entry);
  bx_access_physical(page_table_entry_addr, 4, BX_READ, &page_table_entry);
  
  return(paddress);
}


/* modified by Hooman */
  void
bx_access_linear(Bit32u laddress, unsigned length, unsigned pl,
    unsigned rw, void *data)
{
   Bit32u mod4096;
   unsigned prev_errno;

  /* perhaps put this check before all code which calls this function,
   * so we don't have to here
   */

  /* Hooman: what about doing the opposite? Seems more modular to me! */
   if (bx_cpu.cr0.pg == 0)
   {
      bx_access_physical(laddress, length, rw, data);
      return;
   }

   prev_errno = bx_cpu.errno;

   /* check for reference across multiple pages */
   mod4096 = laddress & 0x00000fff;
   if ( (mod4096 + length) > 4096 ) {
      Bit32u paddress1, len1;

      /* access across 2 pages / Hooman: or more... */
/*****
   Hooman: let's make the procedure iterative.
      That way, I can access as many pages as I want (more than 8192 bytes) */

      paddress1 = translate_linear(laddress, pl, rw);
      if (bx_cpu.errno > prev_errno)
         bx_panic("paging: page exception after translate_linear()\n");
      len1      = 4096 - mod4096;

/* I replaced:
      paddress2 = translate_linear(laddress + len1, pl, rw);
      if (bx_cpu.errno > prev_errno)
         bx_panic("paging: page exception after translate_linear()\n");

      bx_access_physical(paddress1, len1, rw, data);
      bx_access_physical(paddress2, length - len1, rw, ((Bit8u*)data) + len1);

   with: */
      bx_access_physical(paddress1, len1, rw, data);
      bx_access_linear(laddress + len1, length - len1, pl, rw, ((Bit8u*)data) + len1);
   }
   else
   {
      Bit32u paddress1;

      /* access within single page */
      paddress1 = translate_linear(laddress, pl, rw);
      if (bx_cpu.errno > prev_errno)
         bx_panic("paging: page exception after translate_linear()\n");

      bx_access_physical(paddress1, length, rw, data);
   }
}
