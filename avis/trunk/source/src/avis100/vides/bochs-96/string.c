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


/* MOVSB ES:[EDI], DS:[ESI]   DS may be overridden
 *   mov string from DS:[ESI] into ES:[EDI]
 */

  INLINE void
bx_MOVSB_XbYb()
{
  bx_segment_reg_t *seg;
  Bit8u temp8;

  if (bx_cpu.segment_override) {
    seg = bx_cpu.segment_override;
    }
  else {
    seg = &bx_cpu.ds;
    }

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_addrsize) {
    Bit32u esi, edi;

    esi = ESI;
    edi = EDI;

    bx_read_virtual_byte(seg, esi, &temp8);
    BX_HANDLE_EXCEPTION()

    bx_write_virtual_byte(&bx_cpu.es, edi, &temp8);
    BX_HANDLE_EXCEPTION()

    if (bx_cpu.eflags.df) {
      /* decrement ESI, EDI */
      esi--;
      edi--;
      }
    else {
      /* increment ESI, EDI */
      esi++;
      edi++;
      }

    ESI = esi;
    EDI = edi;
    }

  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit address mode */
    Bit16u si, di;

    si = SI;
    di = DI;

    bx_read_virtual_byte(seg, si, &temp8);
    BX_HANDLE_EXCEPTION()

    bx_write_virtual_byte(&bx_cpu.es, di, &temp8);
    BX_HANDLE_EXCEPTION()

    if (bx_cpu.eflags.df) {
      /* decrement SI, DI */
      si--;
      di--;
      }
    else {
      /* increment SI, DI */
      si++;
      di++;
      }

    SI = si;
    DI = di;
    }
}

  INLINE void
bx_MOVSW_XvYv()
{
  bx_segment_reg_t *seg;


  if (bx_cpu.segment_override) {
    seg = bx_cpu.segment_override;
    }
  else {
    seg = &bx_cpu.ds;
    }

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_addrsize) {
    Bit32u  temp32;

    Bit32u esi, edi;

    esi = ESI;
    edi = EDI;

    if (bx_cpu.is_32bit_opsize) {
      bx_read_virtual_dword(seg, esi, &temp32);
      BX_HANDLE_EXCEPTION()

      bx_write_virtual_dword(&bx_cpu.es, edi, &temp32);
      BX_HANDLE_EXCEPTION()

      if (bx_cpu.eflags.df) {
        /* decrement ESI */
        esi -= 4;
        edi -= 4;
        }
      else {
        /* increment ESI */
        esi += 4;
        edi += 4;
        }
      } /* if (bx_cpu.is_32bit_opsize) ... */
    else { /* 16 bit opsize mode */
      Bit16u  temp16;

      bx_read_virtual_word(seg, esi, &temp16);
      BX_HANDLE_EXCEPTION()

      bx_write_virtual_word(&bx_cpu.es, edi, &temp16);
      BX_HANDLE_EXCEPTION()

      if (bx_cpu.eflags.df) {
        /* decrement ESI */
        esi -= 2;
        edi -= 2;
        }
      else {
        /* increment ESI */
        esi += 2;
        edi += 2;
        }
      }

    ESI = esi;
    EDI = edi;
    }

  else
#endif /* BX_CPU >= 3 */
    { /* 16bit address mode */
    Bit16u si, di;

    si = SI;
    di = DI;

#if BX_CPU >= 3
    if (bx_cpu.is_32bit_opsize) {
      Bit32u temp32;

      bx_read_virtual_dword(seg, si, &temp32);
      BX_HANDLE_EXCEPTION()

      bx_write_virtual_dword(&bx_cpu.es, di, &temp32);
      BX_HANDLE_EXCEPTION()

      if (bx_cpu.eflags.df) {
        /* decrement ESI */
        si -= 4;
        di -= 4;
        }
      else {
        /* increment ESI */
        si += 4;
        di += 4;
        }
      } /* if (bx_cpu.is_32bit_opsize) ... */
    else
#endif /* BX_CPU >= 3 */
      { /* 16 bit opsize mode */
      Bit16u  temp16;

      bx_read_virtual_word(seg, si, &temp16);
      BX_HANDLE_EXCEPTION()

      bx_write_virtual_word(&bx_cpu.es, di, &temp16);
      BX_HANDLE_EXCEPTION()

      if (bx_cpu.eflags.df) {
        /* decrement SI, DI */
        si -= 2;
        di -= 2;
        }
      else {
        /* increment SI, DI */
        si += 2;
        di += 2;
        }
      }

    SI = si;
    DI = di;
    }
}

  INLINE void
bx_CMPSB_XbYb()
{
  bx_segment_reg_t *seg;
  Bit8u op1_8, op2_8, diff_8;


  if (bx_cpu.segment_override) {
    seg = bx_cpu.segment_override;
    }
  else {
    seg = &bx_cpu.ds;
    }

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_addrsize) {
    Bit32u esi, edi;

    esi = ESI;
    edi = EDI;

    bx_read_virtual_byte(seg, esi, &op1_8);
    BX_HANDLE_EXCEPTION()

    bx_read_virtual_byte(&bx_cpu.es, edi, &op2_8);
    BX_HANDLE_EXCEPTION()

    diff_8 = op1_8 - op2_8;

    BX_SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_CMPS8);

    if (bx_cpu.eflags.df) {
      /* decrement ESI */
      esi--;
      edi--;
      }
    else {
      /* increment ESI */
      esi++;
      edi++;
      }

    EDI = edi;
    ESI = esi;
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16bit address mode */
    Bit16u si, di;

    si = SI;
    di = DI;

    bx_read_virtual_byte(seg, si, &op1_8);
    BX_HANDLE_EXCEPTION()

    bx_read_virtual_byte(&bx_cpu.es, di, &op2_8);
    BX_HANDLE_EXCEPTION()

    diff_8 = op1_8 - op2_8;

    BX_SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_CMPS8);

    if (bx_cpu.eflags.df) {
      /* decrement ESI */
      si--;
      di--;
      }
    else {
      /* increment ESI */
      si++;
      di++;
      }

    DI = di;
    SI = si;
    }
}

  INLINE void
bx_CMPSW_XvYv()
{
  bx_segment_reg_t *seg;


  if (bx_cpu.segment_override) {
    seg = bx_cpu.segment_override;
    }
  else {
    seg = &bx_cpu.ds;
    }

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_addrsize) {
    Bit32u op1_32, op2_32, diff_32;
    Bit32u esi, edi;

    esi = ESI;
    edi = EDI;


    if (bx_cpu.is_32bit_opsize) {
      bx_read_virtual_dword(seg, esi, &op1_32);
      BX_HANDLE_EXCEPTION()

      bx_read_virtual_dword(&bx_cpu.es, edi, &op2_32);
      BX_HANDLE_EXCEPTION()

      diff_32 = op1_32 - op2_32;

      BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_CMPS32);

      if (bx_cpu.eflags.df) {
        /* decrement ESI */
        esi -= 4;
        edi -= 4;
        }
      else {
        /* increment ESI */
        esi += 4;
        edi += 4;
        }
      }
    else { /* 16 bit opsize */
      Bit16u op1_16, op2_16, diff_16;

      bx_read_virtual_word(seg, esi, &op1_16);
      BX_HANDLE_EXCEPTION()

      bx_read_virtual_word(&bx_cpu.es, edi, &op2_16);
      BX_HANDLE_EXCEPTION()

      diff_16 = op1_16 - op2_16;

      BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_CMPS16);

      if (bx_cpu.eflags.df) {
        /* decrement ESI */
        esi -= 2;
        edi -= 2;
        }
      else {
        /* increment ESI */
        esi += 2;
        edi += 2;
        }
      }


    EDI = edi;
    ESI = esi;
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit address mode */
    Bit16u si, di;

    si = SI;
    di = DI;

#if BX_CPU >= 3
    if (bx_cpu.is_32bit_opsize) {
      Bit32u op1_32, op2_32, diff_32;

      bx_read_virtual_dword(seg, si, &op1_32);
      BX_HANDLE_EXCEPTION()

      bx_read_virtual_dword(&bx_cpu.es, di, &op2_32);
      BX_HANDLE_EXCEPTION()

      diff_32 = op1_32 - op2_32;

      BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_CMPS32);

      if (bx_cpu.eflags.df) {
        /* decrement ESI */
        si -= 4;
        di -= 4;
        }
      else {
        /* increment ESI */
        si += 4;
        di += 4;
        }
      }
    else
#endif /* BX_CPU >= 3 */
      { /* 16 bit opsize */
      Bit16u op1_16, op2_16, diff_16;

      bx_read_virtual_word(seg, si, &op1_16);
      BX_HANDLE_EXCEPTION()

      bx_read_virtual_word(&bx_cpu.es, di, &op2_16);
      BX_HANDLE_EXCEPTION()

      diff_16 = op1_16 - op2_16;

      BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_CMPS16);

      if (bx_cpu.eflags.df) {
        /* decrement ESI */
        si -= 2;
        di -= 2;
        }
      else {
        /* increment ESI */
        si += 2;
        di += 2;
        }
      }


    DI = di;
    SI = si;
    }
}

  INLINE void
bx_SCASB_ALXb()
{
  Bit8u op1_8, op2_8, diff_8;


#if BX_CPU >= 3
  if (bx_cpu.is_32bit_addrsize) {
    Bit32u edi;

    edi = EDI;

    op1_8 = AL;

    bx_read_virtual_byte(&bx_cpu.es, edi, &op2_8);
    BX_HANDLE_EXCEPTION()

    diff_8 = op1_8 - op2_8;

    BX_SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_SCAS8);


    if (bx_cpu.eflags.df) {
      /* decrement ESI */
      edi--;
      }
    else {
      /* increment ESI */
      edi++;
      }

    EDI = edi;
    }

  else
#endif /* BX_CPU >= 3 */
    { /* 16bit address mode */
    Bit16u di;

    di = DI;

    op1_8 = AL;

    bx_read_virtual_byte(&bx_cpu.es, di, &op2_8);
    BX_HANDLE_EXCEPTION()

    diff_8 = op1_8 - op2_8;

    BX_SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_SCAS8);

    if (bx_cpu.eflags.df) {
      /* decrement ESI */
      di--;
      }
    else {
      /* increment ESI */
      di++;
      }

    DI = di;
    }
}

  INLINE void
bx_SCASW_eAXXv()
{
#if BX_CPU >= 3
  if (bx_cpu.is_32bit_addrsize) {
    Bit32u edi;

    edi = EDI;

    if (bx_cpu.is_32bit_opsize) {
      Bit32u op1_32, op2_32, diff_32;

      op1_32 = EAX;
      bx_read_virtual_dword(&bx_cpu.es, edi, &op2_32);
      BX_HANDLE_EXCEPTION()

      diff_32 = op1_32 - op2_32;

      BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SCAS32);

      if (bx_cpu.eflags.df) {
        /* decrement ESI */
        edi -= 4;
        }
      else {
        /* increment ESI */
        edi += 4;
        }
      }
    else { /* 16 bit opsize */
      Bit16u op1_16, op2_16, diff_16;

      op1_16 = AX;
      bx_read_virtual_word(&bx_cpu.es, edi, &op2_16);
      BX_HANDLE_EXCEPTION()

      diff_16 = op1_16 - op2_16;

      BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_SCAS16);

      if (bx_cpu.eflags.df) {
        /* decrement ESI */
        edi -= 2;
        }
      else {
        /* increment ESI */
        edi += 2;
        }
      }

    EDI = edi;
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16bit address mode */
    Bit16u di;

    di = DI;

#if BX_CPU >= 3
    if (bx_cpu.is_32bit_opsize) {
      Bit32u op1_32, op2_32, diff_32;

      op1_32 = EAX;
      bx_read_virtual_dword(&bx_cpu.es, di, &op2_32);
      BX_HANDLE_EXCEPTION()

      diff_32 = op1_32 - op2_32;

      BX_SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SCAS32);

      if (bx_cpu.eflags.df) {
        /* decrement ESI */
        di -= 4;
        }
      else {
        /* increment ESI */
        di += 4;
        }
      }
    else
#endif /* BX_CPU >= 3 */
      { /* 16 bit opsize */
      Bit16u op1_16, op2_16, diff_16;

      op1_16 = AX;
      bx_read_virtual_word(&bx_cpu.es, di, &op2_16);
      BX_HANDLE_EXCEPTION()

      diff_16 = op1_16 - op2_16;

      BX_SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_SCAS16);

      if (bx_cpu.eflags.df) {
        /* decrement ESI */
        di -= 2;
        }
      else {
        /* increment ESI */
        di += 2;
        }
      }

    DI = di;
    }
}

  INLINE void
bx_STOSB_YbAL()
{
  Bit8u al;

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_addrsize) {
    Bit32u edi;

    edi = EDI;

    al = AL;
    bx_write_virtual_byte(&bx_cpu.es, edi, &al);
    BX_HANDLE_EXCEPTION()

    if (bx_cpu.eflags.df) {
      /* decrement EDI */
      edi--;
      }
    else {
      /* increment EDI */
      edi++;
      }

    EDI = edi;
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16bit address size */
    Bit16u di;

    di = DI;

    al = AL;
    bx_write_virtual_byte(&bx_cpu.es, di, &al);
    BX_HANDLE_EXCEPTION()

    if (bx_cpu.eflags.df) {
      /* decrement EDI */
      di--;
      }
    else {
      /* increment EDI */
      di++;
      }

    DI = di;
    }
}

  INLINE void
bx_STOSW_YveAX()
{
#if BX_CPU >= 3
  if (bx_cpu.is_32bit_addrsize) {
    Bit32u edi;

    edi = EDI;

    if (bx_cpu.is_32bit_opsize) {
        Bit32u eax;

        eax = EAX;
        bx_write_virtual_dword(&bx_cpu.es, edi, &eax);
        BX_HANDLE_EXCEPTION()

        if (bx_cpu.eflags.df) {
          /* decrement EDI */
          edi -= 4;
          }
        else {
          /* increment EDI */
          edi += 4;
          }
      } /* if (bx_cpu.is_32bit_opsize) ... */
    else { /* 16 bit opsize mode */
        Bit16u ax;

        ax = AX;
        bx_write_virtual_word(&bx_cpu.es, edi, &ax);
        BX_HANDLE_EXCEPTION()

        if (bx_cpu.eflags.df) {
          /* decrement EDI */
          edi -= 2;
          }
        else {
          /* increment EDI */
          edi += 2;
          }
      }

    EDI = edi;
    }

  else
#endif /* BX_CPU >= 3 */
    { /* 16bit address size */
    Bit16u di;

    di = DI;

#if BX_CPU >= 3
    if (bx_cpu.is_32bit_opsize) {
        Bit32u eax;

        eax = EAX;
        bx_write_virtual_dword(&bx_cpu.es, di, &eax);
        BX_HANDLE_EXCEPTION()

        if (bx_cpu.eflags.df) {
          /* decrement EDI */
          di -= 4;
          }
        else {
          /* increment EDI */
          di += 4;
          }
      } /* if (bx_cpu.is_32bit_opsize) ... */
    else
#endif /* BX_CPU >= 3 */
      { /* 16 bit opsize mode */
        Bit16u ax;

        ax = AX;
        bx_write_virtual_word(&bx_cpu.es, di, &ax);
        BX_HANDLE_EXCEPTION()

        if (bx_cpu.eflags.df) {
          /* decrement EDI */
          di -= 2;
          }
        else {
          /* increment EDI */
          di += 2;
          }
      }

    DI = di;
    }
}


  INLINE void
bx_LODSB_ALXb()
{
  bx_segment_reg_t *seg;
  Bit8u al;

  if (bx_cpu.segment_override) {
    seg = bx_cpu.segment_override;
    }
  else {
    seg = &bx_cpu.ds;
    }

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_addrsize) {
    Bit32u esi;

    esi = ESI;

    bx_read_virtual_byte(seg, esi, &al);
    BX_HANDLE_EXCEPTION()

    AL = al;
    if (bx_cpu.eflags.df) {
      /* decrement ESI */
      esi--;
      }
    else {
      /* increment ESI */
      esi++;
      }

    ESI = esi;
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16bit address mode */
    Bit16u si;

    si = SI;

    bx_read_virtual_byte(seg, si, &al);
    BX_HANDLE_EXCEPTION()

    AL = al;
    if (bx_cpu.eflags.df) {
      /* decrement ESI */
      si--;
      }
    else {
      /* increment ESI */
      si++;
      }

    SI = si;
    }
}

  INLINE void
bx_LODSW_eAXXv()
{
  bx_segment_reg_t *seg;

  if (bx_cpu.segment_override) {
    seg = bx_cpu.segment_override;
    }
  else {
    seg = &bx_cpu.ds;
    }

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_addrsize) {
    Bit32u esi;

    esi = ESI;

    if (bx_cpu.is_32bit_opsize) {
      Bit32u eax;

      bx_read_virtual_dword(seg, esi, &eax);
      BX_HANDLE_EXCEPTION()

      EAX = eax;
      if (bx_cpu.eflags.df) {
        /* decrement ESI */
        esi -= 4;
        }
      else {
        /* increment ESI */
        esi += 4;
        }
      } /* if (bx_cpu.is_32bit_opsize) ... */
    else { /* 16 bit opsize mode */
      Bit16u ax;
      bx_read_virtual_word(seg, esi, &ax);
      BX_HANDLE_EXCEPTION()

      AX = ax;
      if (bx_cpu.eflags.df) {
        /* decrement ESI */
        esi -= 2;
        }
      else {
        /* increment ESI */
        esi += 2;
        }
      }

    ESI = esi;
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16bit address mode */
    Bit16u si;

    si = SI;

#if BX_CPU >= 3
    if (bx_cpu.is_32bit_opsize) {
      Bit32u eax;

      bx_read_virtual_dword(seg, si, &eax);
      BX_HANDLE_EXCEPTION()

      EAX = eax;
      if (bx_cpu.eflags.df) {
        /* decrement ESI */
        si -= 4;
        }
      else {
        /* increment ESI */
        si += 4;
        }
      } /* if (bx_cpu.is_32bit_opsize) ... */
    else
#endif /* BX_CPU >= 3 */
      { /* 16 bit opsize mode */
      Bit16u ax;

      bx_read_virtual_word(seg, si, &ax);
      BX_HANDLE_EXCEPTION()

      AX = ax;
      if (bx_cpu.eflags.df) {
        /* decrement ESI */
        si -= 2;
        }
      else {
        /* increment ESI */
        si += 2;
        }
      }

    SI = si;
    }
}
