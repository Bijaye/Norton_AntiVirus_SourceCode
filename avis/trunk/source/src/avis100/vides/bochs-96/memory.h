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


#ifndef BX_MEMORY_H

#define BX_MEMORY_H 1

#ifndef BYTE
typedef unsigned char BYTE;
#endif
#ifndef DWORD
typedef unsigned long DWORD;
#endif

#ifdef USEWIN32VIRTUALMEM
#define SPYMEMREAD 1
#define SPYMEMWRITE 2
extern Bit8u *bx_phy_memory;
void audit_mem(DWORD addr, DWORD len, BYTE notify);
void dismiss_mem_guard(void);
void guard_mem(void);
#else
extern Bit8u bx_phy_memory[];
#endif //USEWIN32VIRTUALMEM

#define bx_write_virtual_byte(seg, offset, ptr) \
  bx_access_virtual(seg, offset, 1, BX_WRITE, ptr)
#define bx_write_virtual_word(seg, offset, ptr) \
  bx_access_virtual(seg, offset, 2, BX_WRITE, ptr)
#define bx_write_virtual_dword(seg, offset, ptr) \
  bx_access_virtual(seg, offset, 4, BX_WRITE, ptr)

#define bx_read_virtual_byte(seg, offset, ptr) \
  bx_access_virtual(seg, offset, 1, BX_READ, ptr)
#define bx_read_virtual_word(seg, offset, ptr) \
  bx_access_virtual(seg, offset, 2, BX_READ, ptr)
#define bx_read_virtual_dword(seg, offset, ptr) \
  bx_access_virtual(seg, offset, 4, BX_READ, ptr)

#if 0
void bx_write_virtual_byte(bx_segment_reg_t *seg, Bit32u offset, Bit8u *ptr);
void bx_write_virtual_word(bx_segment_reg_t *seg, Bit32u offset, Bit16u *ptr);
void bx_write_virtual_dword(bx_segment_reg_t *seg, Bit32u offset, Bit32u *ptr);
void bx_read_virtual_byte(bx_segment_reg_t *seg, Bit32u offset, Bit8u *ptr);
void bx_read_virtual_word(bx_segment_reg_t *seg, Bit32u offset, Bit16u *ptr);
void bx_read_virtual_dword(bx_segment_reg_t *seg, Bit32u offset, Bit32u *ptr);
#endif

void bx_access_virtual(bx_segment_reg_t *seg, Bit32u offset,
    unsigned length, unsigned rw, void *data);
void bx_access_linear(Bit32u address, unsigned length, unsigned pl,
    unsigned rw, void *data);
void bx_access_physical(Bit32u addr, unsigned len, unsigned rw, void *data);


#endif /* BX_MEMORY_H */
