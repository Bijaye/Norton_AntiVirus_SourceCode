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



#define BX_MEMORY_C 1


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ??? */
#include <ctype.h>

#include "bochs.h"
#include "iodev/iodev.h"

#include "..\bochsw\watcher\watcher.h"
#include "memory.h"

#ifdef USEWIN32VIRTUALMEM
#define PAGESZ 4096
#define NUMPAG ((BX_PHY_MEM_SIZE+MEMMARGIN)/PAGESZ)
#include <windows.h>
Bit8u *bx_phy_memory;
Bit8u *mem_audit;
BYTE page_state[NUMPAG];
BYTE aud_commit[NUMPAG];
static BYTE mem_guard_dismissed=0;
#else
Bit8u bx_phy_memory[BX_PHY_MEM_SIZE + MEMMARGIN];
#endif //USEWIN32VIRTUALMEM



/* Address line 20 control:
   1 = enabled: extended memory is accessible
   0 = disabled: A20 address line is forced low to simulate an 8088 address map
 */
Boolean bx_enable_a20 = 1;

/* start out masking physical memory addresses to:
 *   8086:      20 bits
 *    286:      24 bits
 *    386:      32 bits
 * when A20 line is disabled, mask physical memory addresses to:
 *    286:      20 bits
 *    386:      20 bits
 */

#if BX_CPU < 2
  Bit32u  bx_a20_mask   =    0xfffff;
#elif BX_CPU == 2
  Bit32u  bx_a20_mask   =   0xffffff;
#else /* 386+ */
  Bit32u  bx_a20_mask   = 0xffffffff;
#endif


#define A20ADDR(x) ( (x) & bx_a20_mask )


extern Bit8u bx_io_handler_id[0x10000];  /* 64K */
extern bx_io_handler_t bx_io_handler[BX_MAX_IO_DEVICES];


  char *
bx_strseg(bx_segment_reg_t *seg)
{
  if (seg == &bx_cpu.es) return("ES");
  else if (seg == &bx_cpu.cs) return("CS");
  else if (seg == &bx_cpu.ss) return("SS");
  else if (seg == &bx_cpu.ds) return("DS");
  else if (seg == &bx_cpu.fs) return("FS");
  else if (seg == &bx_cpu.gs) return("GS");
  else {
    bx_printf("undefined segment passed to bx_strseg()!\n");
    return("??");
    }
}

  void
bx_init_memory(int memsize)
{
#ifdef USEWIN32VIRTUALMEM
	// Allocate the address space for the memory and commit all of it
	bx_phy_memory = (Bit8u *) VirtualAlloc(NULL,memsize,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
	if (bx_phy_memory == NULL) {
		bx_panic("Can't allocate memory\n");
	}
	// Set the video memory pointer
	bx_hga_memory = &bx_phy_memory[0xb0000];
	// Allocate the address space for the audit array describing which parts of memory are audited
	// don't commit yet
	mem_audit = (Bit8u *) VirtualAlloc(NULL,memsize,MEM_RESERVE,PAGE_READWRITE);
	if (mem_audit == NULL) {
		bx_panic("Can't allocate memory for audit\n");
	}
	// Zero the arrays describing page states and audit memory chunks allocation states
	memset(page_state,0,NUMPAG*sizeof(BYTE));
	memset(aud_commit,0,NUMPAG*sizeof(BYTE));
#endif
	memset(bx_phy_memory, 0x00, memsize);
}

  void
bx_virtual_block_write(bx_segment_reg_t *seg, Bit16u offset, Bit32u length,
  void *data)
{
  int i;
  Bit8u temp8;

#if BX_CPU >= 2
  if (bx_protected_mode()) {
    bx_panic(
      "virtual_block_write() not yet supported for protected mode\n");
    }
#endif

  for (i=0; i < length; i++) {
    temp8 = *(((Bit8u*) data) + i);
    bx_write_virtual_byte(seg, offset + i, &temp8);
    if (bx_cpu.errno) {
      bx_panic("virtual_block_write(): errno set\n");
      }
    }
}

  void
bx_virtual_block_read(bx_segment_reg_t *seg, Bit16u offset, Bit32u length,
  void *data)
{
  int i;
  Bit8u temp8;

#if BX_CPU >= 2
  if (bx_protected_mode()) {
    bx_panic(
      "virtual_block_read() not yet supported for protected mode\n");
    }
#endif

  for (i=0; i < length; i++) {
    bx_read_virtual_byte(seg, offset + i, &temp8);
    *( ((Bit8u*) data) + i ) = temp8;
    if (bx_cpu.errno) {
      bx_panic("virtual_block_read(): errno set\n");
      }
    }
}



#if BX_CPU >= 2
  void
bx_access_virtual(bx_segment_reg_t *seg, Bit32u offset, unsigned length, 
    unsigned rw, void *data)
{
  Bit32u upper_limit;


  if ( bx_protected_mode() ) {
    if ( seg->cache.valid==0 ) {
      bx_printf("seg = %s\n", bx_strseg(seg));
      bx_printf("seg->selector.value = %04x\n", (unsigned) seg->selector.value);
      bx_printf("access_virtual: valid bit = 0\n");
      bx_printf("CS: %04x\n", (unsigned) bx_cpu.cs.selector.value);
      bx_printf("IP: %04x\n", (unsigned) bx_cpu.prev_eip);
      bx_exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }

    if (seg->cache.p == 0) { /* not present */
      bx_printf("access_virtual(): segment not present\n");
      bx_exception(bx_int_number(seg), 0, 0);
      return;
      }

    switch ( seg->cache.type ) {
      case 0: case 1: /* read only */
      case 10: case 11: /* execute/read */
      case 14: case 15: /* execute/read-only, conforming */
        if (rw==BX_WRITE) {
          bx_printf("access_virtual(): write to read-only\n");
          bx_exception(bx_int_number(seg), 0, 0);
          return;
          }
        if ( (offset+length-1) > seg->cache.u.segment.limit_scaled ) {
          bx_printf("access_virtual(): write beyond limit\n");
          bx_exception(bx_int_number(seg), 0, 0);
          return;
          }
        break;

      case 2: case 3: /* read/write */
        if ( (offset+length-1) > seg->cache.u.segment.limit_scaled ) {
          bx_printf("access_virtual(): write beyond limit\n");
          bx_exception(bx_int_number(seg), 0, 0);
          return;
          }
        break;

      case 4: case 5: /* read only, expand down */
        if (rw==BX_WRITE) {
          bx_printf("access_virtual(): read-only\n");
          bx_exception(bx_int_number(seg), 0, 0);
          return;
          }
        if (seg->cache.u.segment.d_b)
          upper_limit = 0xffffffff;
        else
          upper_limit = 0x0000ffff;
        if ( (offset <= seg->cache.u.segment.limit_scaled) ||
             (offset > upper_limit) ||
             ((upper_limit - offset) < (length - 1)) ) {
          bx_printf("access_virtual(): write beyond limit\n");
          bx_exception(bx_int_number(seg), 0, 0);
          return;
          }
        break;

      case 6: case 7: /* read write, expand down */
        if (seg->cache.u.segment.d_b)
          upper_limit = 0xffffffff;
        else
          upper_limit = 0x0000ffff;
        if ( (offset <= seg->cache.u.segment.limit_scaled) ||
             (offset > upper_limit) ||
             ((upper_limit - offset) < (length - 1)) ) {
          bx_printf("access_virtual(): write beyond limit\n");
          bx_exception(bx_int_number(seg), 0, 0);
          return;
          }
        break;

      case 8: case 9: /* execute only */
      case 12: case 13: /* execute only, conforming */
        /* can't read or write an execute-only segment */
        bx_printf("access_virtual(): execute only\n");
        bx_exception(bx_int_number(seg), 0, 0);
        return;
        break;
      }

    /* all checks OK */
#if BX_CPU >= 3
    /* need to look at access crossing multiple page boundary ??? */
    if (bx_cpu.cr0.pg)
      bx_access_linear(seg->cache.u.segment.base + offset,
        length, CPL, rw, (void *) data);
    else
#endif
      bx_access_physical(seg->cache.u.segment.base + offset, length, rw, data);
    /* will return if error anyway */
    return;
    }

  else { /* real mode */
    if ( (offset + length - 1)  >  seg->cache.u.segment.limit_scaled) {
      bx_printf("access_virtual() SEG EXCEPTION:  %x:%x + %x\n",
        (unsigned) seg->selector.value, (unsigned) offset, (unsigned) length);
      if (seg == &bx_cpu.ss) bx_exception(BX_SS_EXCEPTION, 0, 0);
      else bx_exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }
#if BX_CPU >= 3
    if (bx_cpu.cr0.pg)
      bx_access_linear(seg->cache.u.segment.base + offset,
        length, CPL, rw, (void *) data);
    else
#endif
      bx_access_physical(seg->cache.u.segment.base + offset, length, rw, data);
    /* will return if error anyway */
    return;
    }
}
#endif /* BX_CPU >= 2 */




#ifdef USEWIN32VIRTUALMEM

// This is the function to add an audited memory location
// It takes the address as an offset in the physical memory array (it may be useful to have an equivalent
// function taking a virtual or linear address instead...), the length of the zone to be spied and the type
// of access being spied as specified in the notify parameter.

// Notify is a combination of SPYMEMREAD and SPYMEMWRITE, and causes the page containing the targeted location
// to be protected in read or write access

// There are some limitations to this function, I'll change the implementation only if we bump into them :) Fred

void audit_mem(DWORD addr, DWORD len, BYTE notify)
{
	DWORD pag;
	DWORD i;
	BYTE page_notify;
#if 0
	DWORD access;
	DWORD oldacc;
#endif

	pag = addr >> 12;

	// Check the location is within the physical memory array bounds
	// Check notify is 1, 2 or 3 (hard-coded for SPYMEMREAD, SPYMEMWRITE, SPYMEMREAD|SPYMEMWRITE !)
	// Check the end of the targeted location is in the same page as the beginning

	// *** DO NOT SUPPORT CROSS-PAGE AUDIT ***

	if (addr+len-1>=BX_PHY_MEM_SIZE ||
		notify == 0 || notify > 3 ||
		pag != ((addr+len-1) >> 12))
		bx_panic("Unable to audit memory location\n");

	// Commit memory for the audit array if necessary
	if (aud_commit[pag] == 0) {
		if (VirtualAlloc(mem_audit+pag*PAGESZ,PAGESZ,MEM_COMMIT,PAGE_READWRITE) == NULL)
			bx_panic("Can't commit memory for audit\n");
		aud_commit[pag] = notify;
	}

	// Mark the location in the audit array
	for (i=addr ; i<addr+len ; i++)
		mem_audit[i] |= notify;
	
	// Compute the global protection policy for the page, ORing all protection policies for its bytes
	page_notify = 0;
	for (i=pag*PAGESZ ; i<(pag+1)*PAGESZ ; i++)
		page_notify |= mem_audit[i];


#if 0
	// We don't do this because we want to be able to tag several locations before we start to receive
	// any audit record (useful?)
	// Instead, use guard_mem to start the actual memory audit

	
	if (page_notify == SPYMEMREAD || page_notify == SPYMEMREAD|SPYMEMWRITE) access = PAGE_NOACCESS;
	else if (page_notify == SPYMEMWRITE) access = PAGE_READONLY;
	else bx_panic("Bad notify type\n");
	// Change the page protection according to this policy
	if (VirtualProtect(bx_phy_memory+pag*PAGESZ,PAGESZ,access,&oldacc) == 0)
		bx_panic("Can't protect memory for audit\n");
#endif



	// Keep the new status of the page so that we can switch protection off and on easily
	page_state[pag] = page_notify;
}

// This function removes all protections from the memory
// It should be used before any access initiated by an audit component (any access that's not supposed to happen!)
// After the access, the guard_mem function should be used to restore the protection

void dismiss_mem_guard(void)
{
	DWORD oldacc;

	// Simply set all pages protection to PAGE_READWRITE

	if (VirtualProtect(bx_phy_memory,BX_PHY_MEM_SIZE,PAGE_READWRITE,&oldacc) == 0)
		bx_panic("Can't dismiss memory guard\n");
	mem_guard_dismissed = 1;
}

// This function restores the protection on the pages containing the physical memory array
void guard_mem(void)
{
	DWORD oldacc;
	DWORD access;
	DWORD p;

	// Restore the protection for each page according to the status kept in the page_state array

	for (p=0 ; p<NUMPAG ; p++) {
			if (page_state [p] == 0)
				access = PAGE_READWRITE;
			else if ((page_state [p] & SPYMEMREAD) == SPYMEMREAD)
				access = PAGE_NOACCESS;
			else if (page_state [p] == SPYMEMWRITE)
				access = PAGE_READONLY;
			else
				bx_panic("Bad notify type\n");

			if (VirtualProtect(bx_phy_memory+p*PAGESZ,PAGESZ,access,&oldacc) == 0)
				bx_panic("Can't set memory guard\n");
	}
	mem_guard_dismissed = 0;
}
#endif


#ifdef USEWIN32VIRTUALMEM

// This is the exception filter

LONG OneTimeRecoverFilter(DWORD exccode, DWORD addr, Bit8u *data, unsigned len, unsigned rw)
{
	DWORD pag;
	DWORD access;
	DWORD oldacc;
//	static int debugcounter=0;
//	debugcounter++;

	// Check this is an access violation, else don't handle it
	if (exccode != EXCEPTION_ACCESS_VIOLATION)
			return EXCEPTION_CONTINUE_SEARCH;
	// Check we are supposed to watch the memory, else don't handle
	if (mem_guard_dismissed != 0)
			return EXCEPTION_CONTINUE_SEARCH;
	pag = addr >> 12;
	// Check the page accessed is supposed to be protected
	if (page_state[pag] == 0)
			return EXCEPTION_CONTINUE_SEARCH;
	
	// Remove the protection on the page momentarily
	if (VirtualProtect(bx_phy_memory+pag*PAGESZ,PAGESZ,PAGE_READWRITE,&access) == 0)
		bx_panic("Can't change page protection\n");

	// Now, do the actual access
	// There are only 3 cases for the access length: byte, word or dword
	// This is because all mem access with a length other than that has been decomposed in several one-byte accesses
	// in bx_access_physical

	if (len == 1) {
		// Move a byte
		if (rw == BX_READ) {
				*(Bit8u *) data = bx_phy_memory[addr];
		}
		else /* WRITE */
			bx_phy_memory[addr] = *(Bit8u *) data;
	}
	else if (len == 2) {
		// Move a whole word
		if (rw == BX_READ)
			*(Bit16u *) data = *(Bit16u *) &bx_phy_memory[addr];
		else /* WRITE */
			*(Bit16u *) &bx_phy_memory[addr] = *(Bit16u *) data;
	}
	else if (len == 4) {
		// Move a whole dword
		if (rw == BX_READ)
			*(Bit32u *) data = *(Bit32u *) &bx_phy_memory[addr];
		else /* WRITE */
			*(Bit32u *) &bx_phy_memory[addr] = *(Bit32u *) data;
	}
	else bx_panic("Bad memory access\n");


	// Now that the access is done, restore the memory protection on the page
	if (VirtualProtect(bx_phy_memory+pag*PAGESZ,PAGESZ,access,&oldacc) == 0)
		bx_panic("Can't change page protection\n");

	// If everything went fine, execute the exception handler, that will send an audit record
	return EXCEPTION_EXECUTE_HANDLER;
}

#endif



#ifdef USEWIN32VIRTUALMEM

// In this version of the memory audit, we use SEH (Structured Exception Handling) to trigger the audit records
// when audited parts of the memory are accessed.
// Most of the time, the page accessed by reading from or writing to the bx_phy_memory array is not protected, and
// there is no exception (thus no delay, cool!)
// When a protected page is accessed, the exception filter carries out the actual memory read or write after having
// momentarily dismissed the protection, and the exception handler sends an audit record to the expert system.
//		Fred.

// (The memory audit functions in asax have been #ifdef'ed too)

//WARNING: We assume this is using win32 ****ON INTEL****, thus using little endian

// This function is also different from the old version in the way it deals with transfer length
// It compares len with 1, 2 and 4 before falling on the general n bytes case
// Most of the time, it avoids a for(...) (see statistics in old version)

void
bx_access_physical(Bit32u addr, unsigned len, unsigned rw, void *data)
{
	Bit32u i;
	Bit8u  *data_ptr;

#if BX_SUPPORT_VGA
	Bit8u bx_vga_mem_read(Bit32u addr);
	void  bx_vga_mem_write(Bit32u addr, Bit8u value);
#endif

	addr = A20ADDR(addr);

	if((addr + len) > BX_PHY_MEM_SIZE) {
		if(rw==BX_READ)
			* (Bit8u *) data = 0xff;
		return;
	}

#if !BX_SUPPORT_VGA
	if (len == 1) {
		// Move a byte
		if (rw == BX_READ) {
			__try {
				*(Bit8u *) data = bx_phy_memory[addr];
			}
			__except(OneTimeRecoverFilter(GetExceptionCode(), addr, data, len, rw)) {
				if (mem_audit[addr] != 0)
					note_memaccess(addr, len, rw, data);
			}
		}
		else { /* WRITE */
			__try {
				bx_phy_memory[addr] = *(Bit8u *) data;
			}
			__except(OneTimeRecoverFilter(GetExceptionCode(), addr, data, len, rw)) {
				if (mem_audit[addr] != 0)
					note_memaccess(addr, len, rw, data);
			}
		}
	}
	else if (len == 2) {
		// Move a whole word
		if (rw == BX_READ) {
			__try {
				*(Bit16u *) data = *(Bit16u *) &bx_phy_memory[addr];
			}
			__except(OneTimeRecoverFilter(GetExceptionCode(), addr, data, len, rw)) {
				if (mem_audit[addr] != 0)
					note_memaccess(addr, len, rw, data);
			}
		}
		else { /* WRITE */
			__try {
				*(Bit16u *) &bx_phy_memory[addr] = *(Bit16u *) data;
			}
			__except(OneTimeRecoverFilter(GetExceptionCode(), addr, data, len, rw)) {
				if (mem_audit[addr] != 0)
					note_memaccess(addr, len, rw, data);
			}
		}
	}
	else if (len == 4) {
		// Move a whole dword
		if (rw == BX_READ) {
			__try {
				*(Bit32u *) data = *(Bit32u *) &bx_phy_memory[addr];
			}
			__except(OneTimeRecoverFilter(GetExceptionCode(), addr, data, len, rw)) {
				if (mem_audit[addr] != 0)
					note_memaccess(addr, len, rw, data);
			}
		}
		else { /* WRITE */
			__try {
				*(Bit32u *) &bx_phy_memory[addr] = *(Bit32u *) data;
			}
			__except(OneTimeRecoverFilter(GetExceptionCode(), addr, data, len, rw)) {
				if (mem_audit[addr] != 0)
					note_memaccess(addr, len, rw, data);
			}
		}
	}
	else {
#endif //!BX_SUPPORT_VGA //we have to move the bytes one by one...

	if (rw == BX_READ) {
		data_ptr = (Bit8u *) data;

		for (i=1; i<=len; i++, addr++, data_ptr++) {
#if BX_SUPPORT_VGA
			if((addr & 0xfffe0000) == 0x000a0000)
				*(Bit8u *) data_ptr = bx_vga_mem_read(addr);
			else
#endif
			{
				__try {
					*(Bit8u *) data_ptr = bx_phy_memory[addr];
				}
				__except(OneTimeRecoverFilter(GetExceptionCode(), addr, data_ptr, 1, rw)) {
					if (mem_audit[addr] != 0)
						note_memaccess(addr, 1, rw, data_ptr);
				}
			}
		} // end for(...)
	}
	else { /* WRITE */
		data_ptr = (Bit8u *) data;

		for (i=1; i<=len; i++, addr++, data_ptr++) {
#if BX_SUPPORT_VGA
			if((addr & 0xfffe0000) == 0x000a0000)
				bx_vga_mem_write(addr, *(Bit8u *) data_ptr);
			else
#endif
			{
				__try {
					bx_phy_memory[addr] = *(Bit8u *) data_ptr;
				}
				__except(OneTimeRecoverFilter(GetExceptionCode(), addr, data_ptr, 1, rw)) {
					if (mem_audit[addr] != 0)
						note_memaccess(addr, 1, rw, data_ptr);
				}
			}
		} // end for(...)
	}

#if !BX_SUPPORT_VGA
	}
#endif
}





#else // USEWIN32VIRTUALMEM not defined


// Old version

  void
bx_access_physical(Bit32u addr, unsigned len, unsigned rw, void *data)
{
   Bit32u i;
   Bit8u  *data_ptr;
   DWORD shift, tbl_addr;

#if BX_SUPPORT_VGA
   Bit8u bx_vga_mem_read(Bit32u addr);
   void  bx_vga_mem_write(Bit32u addr, Bit8u value);
#endif

#if 0
   // DEBUG
   static int count1=0; // 18E34FB Vast majority
   static int count2=0; // 0A3AA97
   static int count4=0; // 000024C
   static int counto=0; // 000000A

   if (len == 1) count1++;
   else if (len == 2) count2++;
   else if (len == 4) count4++;
   else counto++;

   if (counto > 10) {
	   counto = 0;
   }
#endif

   addr = A20ADDR(addr);

   if((addr + len) > BX_PHY_MEM_SIZE)
   {
// TO DO: make this correct.
      if(rw==BX_READ)
         * (Bit8u *) data = 0xff;
      return;
#if 0
      bx_panic("access beyond physical memory boundary! (%08x+%02x)\n",
         (unsigned) addr, (unsigned) len);
#endif
   }


   /* Hooman: watching the memory */
   if(watching_mem)
   {
      tbl_addr = addr >> 5;
      if((shift = (addr & 0x1f)) + len <= 0x20)
      {
         /* contained in a single 32-byte page */
         if(memwatch[tbl_addr] & (0xffffffff >> (32 - len)) << shift)
         {
            if(note_memaccess(addr, len, rw, data))
               return; /* cancel */
         }
      }
      else
      {
         /* area accross 2 or more 32-byte pages */
         if(memarea_is_guarded(len, shift, tbl_addr))
         { /* something is to be watched in that area */
            if(note_memaccess(addr, len, rw, data))
               return; /* cancel */
         }
      }
   }


   if (rw == BX_READ)
   {
#ifdef LITTLE_ENDIAN
      data_ptr = (Bit8u *) data;

      for (i=1; i<=len; i++, addr++, data_ptr++)
#else /* BIG_ENDIAN */
      data_ptr = (Bit8u *) data + sizeof(Bit8u) * (len - 1);

      for (i=1; i<=len; i++, addr++, data_ptr--)
#endif
      {

#if BX_SUPPORT_VGA
         if((addr & 0xfffe0000) == 0x000a0000)
            *(Bit8u *) data_ptr = bx_vga_mem_read(addr);
         else
#endif
            *(Bit8u *) data_ptr = bx_phy_memory[addr];

            /*#ifdef WIN32DEBUG
      bx_printf("READ:::addr=%08X [%02X]\n", addr, *(Bit8u *) data_ptr);
#endif*/
      } /* for (i=... */
   }
   else
   { /* WRITE */
#ifdef LITTLE_ENDIAN
      data_ptr = (Bit8u *) data;

      for (i=1; i<=len; i++, addr++, data_ptr++)
#else /* BIG_ENDIAN */
      data_ptr = (Bit8u *) data + sizeof(Bit8u) * (len - 1);

      for (i=1; i<=len; i++, addr++, data_ptr--)
#endif
      {
#if BX_SUPPORT_VGA
         if((addr & 0xfffe0000) == 0x000a0000)
            bx_vga_mem_write(addr, *(Bit8u *) data_ptr);
         else
#endif
         bx_phy_memory[addr] = *(Bit8u *) data_ptr;
#ifdef WIN32DEBUG2
         bx_printf("WRITE:::addr=%08X [%02X]\n", addr, *(Bit8u *) data_ptr);
#endif
      } /* for (i=... */
   }
}


#endif //USEWIN32VIRTUALMEM



  void
bx_set_interrupt_vector(int interrupt, Bit32u address)
{
  Bit16u ip, cs;


  ip = (Bit16u) (address & 0xFFFF);
  cs = (Bit16u) ((address & 0xF0000) >> 4);
  bx_access_physical(interrupt*4, 2, BX_WRITE, &ip);
  bx_access_physical(interrupt*4 + 2, 2, BX_WRITE, &cs);
}


/* bx_inp()
 *
 * Read a byte of data from the IO memory address space
 */

  Bit32u
bx_inp(Bit16u addr, unsigned io_len)
{
  Bit8u handle;
  Bit32u ret;

  handle = bx_io_handler_id[addr];
  ret = (* bx_io_handler[handle].read_funct)((Bit32u) addr, io_len);

  if (bx_dbg.io)
    bx_printf("IO: inp(%04x) = %02x\n", (unsigned) addr, (unsigned) ret);
  return(ret);
}


/* bx_outp()
 *
 * Write a byte of data to the IO memory address space.
 */

  void
bx_outp(Bit16u addr, Bit32u value, unsigned io_len)
{
  Bit8u handle;

  handle = bx_io_handler_id[addr];
  (* bx_io_handler[handle].write_funct)((Bit32u) addr, value, io_len);

  if (bx_dbg.io)
    bx_printf("IO: outp(%04x) = %02x\n", (unsigned) addr, (unsigned) value);
}


  void
bx_set_enable_a20(Bit8u value)
{
#if BX_CPU < 2
    bx_panic("set_enable_a20() called: 8086 emulation\n");
#else

  if (value) {
    bx_enable_a20 = 1;
#if BX_CPU == 2
    bx_a20_mask   = 0xffffff;   /* 286: enable all 24 address lines */
#else /* 386+ */
    bx_a20_mask   = 0xffffffff; /* 386: enable all 32 address lines */
#endif
    }
  else {
    bx_enable_a20 = 0;
    bx_a20_mask   = 0x0fffff;   /* mask off all but lower 20 address lines */
    /* ??? maybe should only mask off A20 line */
    }

  if (bx_dbg.a20)
    bx_printf("A20: set() = %u\n", (unsigned) bx_enable_a20);
#endif
}

  Boolean
bx_get_enable_a20(void)
{
  if (bx_dbg.a20)
    bx_printf("A20: get() = %u\n", (unsigned) bx_enable_a20);

  if (bx_enable_a20) return(1);
  else return(0);
}

// Hooman: this is useless now

//  void 
//mybx_access_virtual(bx_segment_reg_t *seg, Bit32u offset,
//                         unsigned length,
//                         unsigned rw, void *data)
//{
//#ifdef AUDIT_ENABLED
//  Bit32u addr;
//  int ctr;
//#endif
//
//  bx_access_virtual(seg, offset, length, rw, data);
//  BX_HANDLE_EXCEPTION() /* returns if bx.cpu.errno != 0 */
//
//#ifdef AUDIT_ENABLED
//  addr = A20ADDR(seg->cache.u.segment.base + offset);
//  for (ctr=0; ctr<MemAuditLen; ctr++)
//    if ((addr >= MemAuditList[ctr].start)
//        && (addr <= MemAuditList[ctr].end)
//        && (MemAuditList[ctr].flags &
//            (rw==BX_WRITE)?MEM_AUDIT_WRITE:MEM_AUDIT_READ))
//      AUDIT_MEMORY(addr, (rw==BX_WRITE)?1:0, length ,&data);
//#endif
// }

