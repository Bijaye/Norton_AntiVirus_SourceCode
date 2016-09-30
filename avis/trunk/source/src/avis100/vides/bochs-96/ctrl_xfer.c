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
bx_JO_Jb()
{
  Bit8s offset;

  offset = (Bit8s) bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  if (bx_get_OF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
#if BX_CPU >= 2
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jo_jb: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
#endif
    EIP = new_EIP;
    }
}

  INLINE void
bx_JNO_Jb()
{
  Bit8s offset;

  offset = (Bit8s) bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  if (!bx_get_OF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
#if BX_CPU >= 2
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jno_jb: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
#endif
    EIP = new_EIP;
    }
}

  INLINE void
bx_JB_Jb()
{
  Bit8s offset;

  offset = (Bit8s) bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  if (bx_get_CF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
#if BX_CPU >= 2
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jb_jb: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
#endif
    EIP = new_EIP;
    }
}

  INLINE void
bx_JNB_Jb()
{
  Bit8s offset;


  offset = (Bit8s) bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  if (!bx_get_CF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
#if BX_CPU >= 2
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jnb_jb: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
#endif
    EIP = new_EIP;
    }
}

  INLINE void
bx_JZ_Jb()
{
  Bit8s offset;

  offset = (Bit8s) bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  if (bx_get_ZF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
#if BX_CPU >= 2
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jz_jb: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
#endif
    EIP = new_EIP;
    }
}

  INLINE void
bx_JNZ_Jb()
{
  Bit8s offset;

  offset = (Bit8s) bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  if (!bx_get_ZF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
#if BX_CPU >= 2
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jnz_jb: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
#endif
    EIP = new_EIP;
    }
}

  INLINE void
bx_JBE_Jb()
{
  Bit8s offset;

  offset = (Bit8s) bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  if (bx_get_CF() || bx_get_ZF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
#if BX_CPU >= 2
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jbe_jb: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
#endif
    EIP = new_EIP;
    }
}

  INLINE void
bx_JNBE_Jb()
{
  Bit8s offset;

  offset = (Bit8s) bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  if (!bx_get_CF() && !bx_get_ZF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
#if BX_CPU >= 2
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jnbe_jb: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
#endif
    EIP = new_EIP;
    }
}

  INLINE void
bx_JS_Jb()
{
  Bit8s offset;

  offset = (Bit8s) bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  if (bx_get_SF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
#if BX_CPU >= 2
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("js_jb: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
#endif
    EIP = new_EIP;
    }
}

  INLINE void
bx_JNS_Jb()
{
  Bit8s offset;


  offset = (Bit8s) bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  if (!bx_get_SF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
#if BX_CPU >= 2
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jns_jb: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
#endif
    EIP = new_EIP;
    }
}

  INLINE void
bx_JP_Jb()
{
  Bit8s offset;

  offset = (Bit8s) bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  if (bx_get_PF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
#if BX_CPU >= 2
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jp_jb: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
#endif
    EIP = new_EIP;
    }
}

  INLINE void
bx_JNP_Jb()
{
  Bit8s offset;

  offset = (Bit8s) bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  if (!bx_get_PF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
#if BX_CPU >= 2
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jnp_jb: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
#endif
    EIP = new_EIP;
    }
}

  INLINE void
bx_JL_Jb()
{
  Bit8s offset;

  offset = (Bit8s) bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  if ( bx_get_SF() != bx_get_OF() ) {
    Bit32u new_EIP;

    new_EIP = EIP + offset;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
#if BX_CPU >= 2
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jl_jb: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
#endif
    EIP = new_EIP;
    }
}

  INLINE void
bx_JNL_Jb()
{
  Bit8s offset;

  offset = (Bit8s) bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  if (bx_get_SF() == bx_get_OF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
#if BX_CPU >= 2
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jnl_jb: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
#endif
    EIP = new_EIP;
    }
}

  INLINE void
bx_JLE_Jb()
{
  Bit8s offset;

  offset = (Bit8s) bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  if (bx_get_ZF() || (bx_get_SF() != bx_get_OF()) ) {
    Bit32u new_EIP;

    new_EIP = EIP + offset;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
#if BX_CPU >= 2
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jle_jb: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
#endif
    EIP = new_EIP;
    }
}

  INLINE void
bx_JNLE_Jb()
{
  Bit8s offset;

  offset = (Bit8s) bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  if ( (bx_get_SF() == bx_get_OF())  &&  !bx_get_ZF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
#if BX_CPU >= 2
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jnle_jb: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
#endif
    EIP = new_EIP;
    }
}

  INLINE void
bx_JCXZ_Jb()
{
  Bit8s offset;
  Bit32u temp_ECX;

  offset = (Bit8s) bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  if (bx_cpu.is_32bit_addrsize)
    temp_ECX = ECX;
  else
    temp_ECX = CX;

  if ( temp_ECX == 0 ) {
    Bit32u new_EIP;

    new_EIP = EIP + offset;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
#if BX_CPU >= 2
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jcxz_jb: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
#endif
    EIP = new_EIP;
    }
}


  INLINE void
bx_RETnear_Iw()
{
  Bit16u imm16;
  Bit32u temp_ESP;

  if (bx_cpu.ss.cache.u.segment.d_b) /* 32bit stack */
    temp_ESP = ESP;
  else
    temp_ESP = SP;

  imm16 = bx_fetch_next_word();
  BX_HANDLE_EXCEPTION()

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32u return_EIP;
    
    if (bx_protected_mode()) {
      if ( !bx_can_pop(4) ) {
        bx_panic("retnear_iw: can't pop EIP\n");
        /* ??? #SS(0) -or #GP(0) */
        return;
        }
      
      bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 0,
        4, CPL==3, BX_READ, &return_EIP);
  
      if (bx_protected_mode() &&
          (return_EIP > bx_cpu.cs.cache.u.segment.limit_scaled) ) {
        bx_panic("retnear_iw: EIP > limit\n");
        }
  
      /* Pentium book says imm16 is number of words ??? */
      if ( !bx_can_pop(4 + imm16) ) {
        bx_panic("retnear_iw: can't release bytes from stack\n");
        /* #GP(0) -or #SS(0) ??? */
        }
  
      bx_cpu.eip = return_EIP;
      if (bx_cpu.ss.cache.u.segment.d_b) /* 32bit stack */
        ESP += 4 + imm16; /* ??? should it be 2*imm16 ? */
      else
        SP  += 4 + imm16;
      }
    else {
      bx_pop_32(&return_EIP);
      bx_cpu.eip = return_EIP;
      if (bx_cpu.ss.cache.u.segment.d_b) /* 32bit stack */
        ESP += imm16; /* ??? should it be 2*imm16 ? */
      else
        SP  += imm16;
      }
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit opsize */
    Bit16u return_IP;

    if (bx_protected_mode()) {
      if ( !bx_can_pop(2) ) {
        bx_panic("retnear_iw: can't pop IP\n");
        /* ??? #SS(0) -or #GP(0) */
        return;
        }
      
      bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 0,
        2, CPL==3, BX_READ, &return_IP);
  
      if ( return_IP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("retnear_iw: IP > limit\n");
        }
  
      if ( !bx_can_pop(2 + imm16) ) {
        bx_panic("retnear_iw: can't release bytes from stack\n");
        /* #GP(0) -or #SS(0) ??? */
        }
  
      bx_cpu.eip = return_IP;
      if (bx_cpu.ss.cache.u.segment.d_b) /* 32bit stack */
        ESP += 2 + imm16; /* ??? should it be 2*imm16 ? */
      else
        SP  += 2 + imm16;
      }
    else {
      bx_pop_16(&return_IP);
      bx_cpu.eip = return_IP;
      if (bx_cpu.ss.cache.u.segment.d_b) /* 32bit stack */
        ESP += imm16; /* ??? should it be 2*imm16 ? */
      else
        SP  += imm16;
      }
    }
}

  INLINE void
bx_RETnear()
{
  Bit32u temp_ESP;


  if (bx_cpu.ss.cache.u.segment.d_b) /* 32bit stack */
    temp_ESP = ESP;
  else
    temp_ESP = SP;

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32u return_EIP;
    
    if (bx_protected_mode()) {
      if ( !bx_can_pop(4) ) {
        bx_panic("retnear_iw: can't pop EIP\n");
        /* ??? #SS(0) -or #GP(0) */
        return;
        }
    
      bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 0,
        4, CPL==3, BX_READ, &return_EIP);

      if ( return_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("retnear_iw: EIP > limit\n");
        }
      bx_cpu.eip = return_EIP;
      if (bx_cpu.ss.cache.u.segment.d_b) /* 32bit stack */
        ESP += 4;
      else
        SP  += 4;
      }
    else {
      bx_pop_32(&return_EIP);
      bx_cpu.eip = return_EIP;
      }
    }
  else
#endif /* BX_CPU >= 3 */
    {
    Bit16u return_IP;

    if (bx_protected_mode()) {
      if ( !bx_can_pop(2) ) {
        bx_panic("retnear_iw: can't pop IP\n");
        /* ??? #SS(0) -or #GP(0) */
        return;
        }
    
      bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 0,
        2, CPL==3, BX_READ, &return_IP);

      if ( return_IP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("retnear_iw: IP > limit\n");
        }

      bx_cpu.eip = return_IP;
      if (bx_cpu.ss.cache.u.segment.d_b) /* 32bit stack */
        ESP += 2;
      else
        SP  += 2;
      }
    else {
      bx_pop_16(&return_IP);
      bx_cpu.eip = return_IP;
      }
    }
}

  INLINE void
bx_RETfar_Iw()
{
  Bit16s imm16;

  /* ??? is imm16, number of bytes/words depending on operandsize ? */

  imm16 = bx_fetch_next_word();
  BX_HANDLE_EXCEPTION()

#if BX_CPU >= 2
  if (bx_protected_mode()) {
    if (bx_dbg.protected) bx_printf("RETfar_Iw: entering\n");
    bx_return_protected(imm16);
    return;
    }
#endif

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32u eip, ecs;

    bx_pop_32(&eip);
    bx_pop_32(&ecs);
    bx_cpu.eip = eip;
    bx_load_seg_reg(&bx_cpu.cs, (Bit16u) ecs);
    if (bx_cpu.ss.cache.u.segment.d_b)
      ESP += imm16;
    else
      SP  += imm16;
    }
  else
#endif /* BX_CPU >= 3 */
    {
    Bit16u ip, cs;

    bx_pop_16(&ip);
    bx_pop_16(&cs);
    bx_cpu.eip = (Bit32u) ip;
    bx_load_seg_reg(&bx_cpu.cs, cs);
    if (bx_cpu.ss.cache.u.segment.d_b)
      ESP += imm16;
    else
      SP  += imm16;
    }
}

  INLINE void
bx_RETfar()
{

#if BX_CPU >= 2
  if ( bx_protected_mode() ) {
    if (bx_dbg.protected) bx_printf("RETfar: entering\n");
    bx_return_protected(0);
    return;
    }
#endif

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32u eip, ecs;

    bx_pop_32(&eip);
    bx_pop_32(&ecs); /* 32bit pop, MSW discarded */
    bx_cpu.eip = eip;
    bx_load_seg_reg(&bx_cpu.cs, (Bit16u) ecs);
    }
  else
#endif /* BX_CPU >= 3 */
    {
    Bit16u ip, cs;

    bx_pop_16(&ip);
    bx_pop_16(&cs);
    bx_cpu.eip = (Bit32u) ip;
    bx_load_seg_reg(&bx_cpu.cs, cs);
    }
}

  INLINE void
bx_ENTER_IwIb()
{
#if BX_CPU < 2
  bx_panic("ENTER_IwIb: not supported by 8086!\n");
#else
  Bit16u dynamic_size;
  Bit8u  level;
  Bit32u frame_ptr32;
  Bit16u frame_ptr16;

  dynamic_size = bx_fetch_next_word();
  BX_HANDLE_EXCEPTION()

  level = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  level %= 32;
/* ??? */
if (level) bx_panic("enter(): level > 0\n");
if (bx_cpu.is_32bit_opsize != bx_cpu.is_32bit_addrsize) {
  bx_printf("enter(): opsize != addrsize: I'm unsure of the code for this\n");
  bx_panic("         The Intel manuals are a mess on this one!\n");
  }
if (bx_cpu.ss.cache.u.segment.d_b && bx_cpu.is_32bit_opsize==0) {
  bx_printf("enter(): stacksize!=opsize: I'm unsure of the code for this\n");
  bx_panic("         The Intel manuals are a mess on this one!\n");
  }

  if ( bx_protected_mode() ) {
    Bit32u bytes_to_push, temp_ESP;

    if (level == 0) {
      if (bx_cpu.is_32bit_opsize)
        bytes_to_push = 4 + dynamic_size;
      else
        bytes_to_push = 2 + dynamic_size;
      }
    else { /* level > 0 */
      if (bx_cpu.is_32bit_opsize)
        bytes_to_push = 4 + (level-1)*4 + 4 + dynamic_size;
      else
        bytes_to_push = 2 + (level-1)*2 + 2 + dynamic_size;
      }
    if (bx_cpu.ss.cache.u.segment.d_b)
      temp_ESP = ESP;
    else
      temp_ESP = SP;
    if ( !bx_can_push(&bx_cpu.ss.cache, temp_ESP, bytes_to_push) ) {
      bx_panic("ENTER: not enough room on stack!\n");
      bx_exception(BX_SS_EXCEPTION, 0, 0);
      return;
      }
    if (bx_dbg.protected) bx_printf("ENTER(protected mode): succeeding\n");
    }

  if (bx_cpu.is_32bit_opsize)
    bx_push_32(EBP);
  else
    bx_push_16(BP);
  if (bx_cpu.errno) bx_panic("ENTER: internal error\n");

  if (bx_cpu.ss.cache.u.segment.d_b)
    frame_ptr32 = ESP;
  else
    frame_ptr32 = SP;

  if (level > 0) {
    /* do level-1 times */
    while (--level) {
      if (bx_cpu.is_32bit_opsize) {
        Bit32u temp32;

        if (bx_cpu.ss.cache.u.segment.d_b) { /* 32bit stacksize */
          EBP -= 4;
          bx_read_virtual_dword(&bx_cpu.ss, EBP, &temp32);
          if (bx_cpu.errno) bx_panic("ENTER: internal error\n");
          ESP -= 4;
          bx_write_virtual_dword(&bx_cpu.ss, ESP, &temp32);
          if (bx_cpu.errno) bx_panic("ENTER: internal error\n");
          }
        else { /* 16bit stacksize */
          BP -= 4;
          bx_read_virtual_dword(&bx_cpu.ss, BP, &temp32);
          if (bx_cpu.errno) bx_panic("ENTER: internal error\n");
          SP -= 4;
          bx_write_virtual_dword(&bx_cpu.ss, SP, &temp32);
          if (bx_cpu.errno) bx_panic("ENTER: internal error\n");
          }
        }
      else { /* 16bit opsize */
        Bit16u temp16;

        if (bx_cpu.ss.cache.u.segment.d_b) { /* 32bit stacksize */
          EBP -= 2;
          bx_read_virtual_word(&bx_cpu.ss, EBP, &temp16);
          if (bx_cpu.errno) bx_panic("ENTER: internal error\n");
          ESP -= 2;
          bx_write_virtual_word(&bx_cpu.ss, ESP, &temp16);
          if (bx_cpu.errno) bx_panic("ENTER: internal error\n");
          }
        else { /* 16bit stacksize */
          BP -= 2;
          bx_read_virtual_word(&bx_cpu.ss, BP, &temp16);
          if (bx_cpu.errno) bx_panic("ENTER: internal error\n");
          SP -= 2;
          bx_write_virtual_word(&bx_cpu.ss, SP, &temp16);
          if (bx_cpu.errno) bx_panic("ENTER: internal error\n");
          }
        }
      } /* while (--level) */

    /* push(frame pointer) */
    if (bx_cpu.is_32bit_opsize) {
      if (bx_cpu.ss.cache.u.segment.d_b) { /* 32bit stacksize */
        ESP -= 4;
        bx_write_virtual_dword(&bx_cpu.ss, ESP, &frame_ptr32);
        if (bx_cpu.errno) bx_panic("ENTER: internal error\n");
        }
      else {
        SP -= 4;
        bx_write_virtual_dword(&bx_cpu.ss, SP, &frame_ptr32);
        if (bx_cpu.errno) bx_panic("ENTER: internal error\n");
        }
      }
    else { /* 16bit opsize */
      if (bx_cpu.ss.cache.u.segment.d_b) { /* 32bit stacksize */
        frame_ptr16 = frame_ptr32;
        ESP -= 2;
        bx_write_virtual_word(&bx_cpu.ss, ESP, &frame_ptr16);
        if (bx_cpu.errno) bx_panic("ENTER: internal error\n");
        }
      else {
        frame_ptr16 = frame_ptr32;
        SP -= 2;
        bx_write_virtual_word(&bx_cpu.ss, SP, &frame_ptr16);
        if (bx_cpu.errno) bx_panic("ENTER: internal error\n");
        }
      }
    } /* if (level > 0) ... */

  if (bx_cpu.ss.cache.u.segment.d_b) { /* 32bit stacksize */
    EBP = frame_ptr32;
    ESP = ESP - dynamic_size;
    }
  else { /* 16bit stack */
    BP = frame_ptr32;
    SP = SP - dynamic_size;
    }
#endif
}

  INLINE void
bx_LEAVE()
{
#if BX_CPU < 2
  bx_panic("LEAVE: not supported by 8086!\n");
#else
  Bit32u temp_EBP;


#if BX_CPU >= 3
  if (bx_cpu.ss.cache.u.segment.d_b)
    temp_EBP = EBP;
  else
#endif
    temp_EBP = BP;

  if ( bx_protected_mode() ) {
    if (bx_cpu.ss.cache.u.segment.c_ed) { /* expand up */
      if (temp_EBP <= bx_cpu.ss.cache.u.segment.limit_scaled) {
        bx_panic("LEAVE: BP > SS.limit\n");
        bx_exception(BX_SS_EXCEPTION, 0, 0);
        return;
        }
      }
    else { /* normal */
      if (temp_EBP > bx_cpu.ss.cache.u.segment.limit_scaled) {
        bx_panic("LEAVE: BP > SS.limit\n");
        bx_exception(BX_SS_EXCEPTION, 0, 0);
        return;
        }
      }
    if (bx_dbg.protected) bx_printf("LEAVE(protected mode): succeeding\n");
    }


#if BX_CPU >= 3
  if (bx_cpu.ss.cache.u.segment.d_b)
    ESP = EBP;
  else
#endif
    SP = BP;

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32u temp32;

    bx_pop_32(&temp32);
    if (bx_cpu.errno) bx_panic("LEAVE: couldn't pop EBP\n");
    EBP = temp32;
    }
  else
#endif
    {
    Bit16u temp16;

    bx_pop_16(&temp16);
    if (bx_cpu.errno) bx_panic("LEAVE: couldn't pop BP\n");
    BP = temp16;
    }
#endif
}

  INLINE void
bx_LOOPNE_Jb()
{
  Bit8s imm8;
  Bit32u count, new_EIP;

  imm8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_addrsize)
    count = bx_cpu.ecx;
  else
#endif /* BX_CPU >= 3 */
    count = CX;

  count--;
  if ( (count!=0) && (bx_get_ZF()==0) ) {
    new_EIP = EIP + imm8;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
    if (bx_protected_mode()) {
      if (new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled) {
        bx_panic("loopne_jb: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
    bx_cpu.eip = new_EIP;
    }

  if (bx_cpu.is_32bit_addrsize)
    bx_cpu.ecx--;
  else
    CX--;
}

  INLINE void
bx_LOOPE_Jb()
{
  Bit8s imm8;
  Bit32u count, new_EIP;

  imm8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_addrsize)
    count = bx_cpu.ecx;
  else
#endif /* BX_CPU >= 3 */
    count = CX;

  count--;
  if ( (count!=0) && bx_get_ZF()) {
    new_EIP = EIP + imm8;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
    if (bx_protected_mode()) {
      if (new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled) {
        bx_panic("loope_jb: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
    bx_cpu.eip = new_EIP;
    }
  if (bx_cpu.is_32bit_addrsize)
    bx_cpu.ecx--;
  else
    CX--;
}

  INLINE void
bx_LOOP_Jb()
{
  Bit8s imm8;
  Bit32u count, new_EIP;

  imm8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_addrsize)
    count = bx_cpu.ecx;
  else
#endif /* BX_CPU >= 3 */
    count = CX;

  count--;
  if (count != 0) {
    new_EIP = EIP + imm8;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
    if (bx_protected_mode()) {
      if (new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled) {
        bx_panic("loop_jb: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
    bx_cpu.eip = new_EIP;
    }
  if (bx_cpu.is_32bit_addrsize)
    bx_cpu.ecx--;
  else
    CX--;
}

  INLINE void
bx_CALL_Av()
{
  Bit32u new_EIP;


#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32s disp32;

    disp32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()

    new_EIP = EIP + disp32;

    if ( bx_protected_mode() ) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("call_av: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }

    /* push 32 bit EA of next instruction */
    bx_push_32(bx_cpu.eip);
    BX_HANDLE_EXCEPTION()
    bx_cpu.eip = new_EIP;
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16bit opsize mode */
    Bit16s disp16;

    disp16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    new_EIP = EIP + disp16;
    new_EIP &= 0x0000ffff;
#if BX_CPU >= 2
    if ( bx_protected_mode() &&
         (new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled) ) {
      bx_panic("call_av: new_IP > CS.limit\n");
      bx_exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }
#endif

    /* push 16 bit EA of next instruction */
    bx_push_16(IP);
    BX_HANDLE_EXCEPTION()

    bx_cpu.eip = new_EIP;
    }
}

  INLINE void
bx_CALL_Ap()
{
  Bit16u cs;

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32u disp32;

    disp32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()
    cs     = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    if (bx_protected_mode()) {
      bx_call_protected(cs, disp32);
      return;
      }
    bx_push_32(bx_cpu.cs.selector.value);
    bx_push_32(bx_cpu.eip);
    bx_cpu.eip = disp32;
    bx_load_seg_reg(&bx_cpu.cs, cs);
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16bit opsize mode */
    Bit16u disp16;

    disp16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()
    cs     = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

#if BX_CPU >= 2
    if (bx_protected_mode()) {
      bx_call_protected(cs, disp16);
      return;
      }
#endif

    bx_push_16(bx_cpu.cs.selector.value);
    bx_push_16((Bit16u) bx_cpu.eip);
    bx_cpu.eip = (Bit32u) disp16;
    bx_load_seg_reg(&bx_cpu.cs, cs);
    }
}

  INLINE void
bx_CALL_Ev()
{
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;
  Bit32u temp_ESP;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (bx_cpu.ss.cache.u.segment.d_b)
    temp_ESP = ESP;
  else
    temp_ESP = SP;

#if BX_CPU >= 3
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

    if (bx_protected_mode()) {
      if (op1_32 > bx_cpu.cs.cache.u.segment.limit_scaled) {
        bx_panic("call_ev: EIP out of CS limits!\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      if ( !bx_can_push(&bx_cpu.ss.cache, temp_ESP, 4) ) {
        bx_panic("call_ev: can't push EIP\n");
        }
      }

    bx_push_32(bx_cpu.eip);
    if (bx_cpu.errno)
      bx_panic("call_ev: stack problem\n");

    bx_cpu.eip = op1_32;
    }
  else
#endif /* BX_CPU >= 3 */
    {
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

#if BX_CPU >= 2
    if (bx_protected_mode()) {
      if (op1_16 > bx_cpu.cs.cache.u.segment.limit_scaled) {
        bx_panic("call_ev: IP out of CS limits!\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      if ( !bx_can_push(&bx_cpu.ss.cache, temp_ESP, 2) ) {
        bx_panic("call_ev: can't push IP\n");
        }
      }
#endif

    bx_push_16(IP);
    if (bx_cpu.errno)
      bx_panic("call_ev: stack problem\n");

    bx_cpu.eip = op1_16;
    }
}

  INLINE void
bx_CALL_Ep()
{
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;

  Bit16u cs;

  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32;

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      bx_panic("CALL_Ep: op1 is a register\n");
      return;
      }

    /* pointer, segment address pair */
    bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
    BX_HANDLE_EXCEPTION()
    bx_read_virtual_word(op1_seg_reg, op1_addr+4, &cs);
    BX_HANDLE_EXCEPTION()

    if ( bx_protected_mode() ) {
      bx_call_protected(cs, op1_32);
      return;
      }

    bx_push_32(bx_cpu.cs.selector.value);
    bx_push_32(bx_cpu.eip);

    bx_cpu.eip = op1_32;
    bx_load_seg_reg(&bx_cpu.cs, cs);
    }
  else
#endif /* BX_CPU >= 3 */
    {
    Bit16u op1_16;

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      bx_panic("CALL_Ep: op1 is a register");
      return;
      }

    /* pointer, segment address pair */
    bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
    BX_HANDLE_EXCEPTION()
    bx_read_virtual_word(op1_seg_reg, op1_addr+2, &cs);
    BX_HANDLE_EXCEPTION()

    if ( bx_protected_mode() ) {
      bx_call_protected(cs, op1_16);
      return;
      }

    bx_push_16(bx_cpu.cs.selector.value);
    bx_push_16(IP);

    bx_cpu.eip = op1_16;
    bx_load_seg_reg(&bx_cpu.cs, cs);
    }
}


  INLINE void
bx_JMP_Jv()
{
  Bit32u new_EIP;

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32s imm32;

    imm32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()

    new_EIP = EIP + imm32;
    }
  else
#endif /* BX_CPU >= 3 */
    { /* 16 bit operand size mode */
    Bit16s imm16;

    imm16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()

    new_EIP = EIP + imm16;
    new_EIP &= 0x0000ffff;
    }

#if BX_CPU >= 2
  if (bx_protected_mode()) {
    if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
      bx_panic("jmp_jv: offset outside of CS limits\n");
      bx_exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }
    }
#endif

  bx_cpu.eip = new_EIP;
}

  INLINE void
bx_JMP_Ap()
{
  Bit16u cs;
  Bit32u disp32;

  if (bx_cpu.is_32bit_opsize) {
    disp32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()
    }
  else { /* 16bit operands */
    disp32 = bx_fetch_next_word(); /* clear upper 16 bits */
    BX_HANDLE_EXCEPTION()
    }

  cs = bx_fetch_next_word();
  BX_HANDLE_EXCEPTION()

#if BX_CPU >= 2
  if (bx_protected_mode()) {
    if (bx_dbg.protected) bx_printf("jmp_ap():\n");
    bx_jump_protected(cs, disp32);
    return;
    }
#endif

  bx_load_seg_reg(&bx_cpu.cs, cs);
  bx_cpu.eip = disp32;
}



  INLINE void
bx_JMP_Jb()
{
  Bit8s imm8;
  Bit32u new_EIP;

  imm8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

#if BX_WEIRDISMS
  if (imm8 == -2) {
    if (bx_cpu.eflags.if_ == 0)
      bx_panic("endless loop detected!\n");
    }
#endif

  new_EIP = EIP + imm8;
  if (bx_cpu.is_32bit_opsize==0)
    new_EIP &= 0x0000ffff;
#if BX_CPU >= 2
  if (bx_protected_mode()) {
    if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
      bx_panic("jmp_jb: offset outside of CS limits\n");
      bx_exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }
    }
#endif

  EIP = new_EIP;
}

  INLINE void
bx_JO_Jv()
{
#if BX_CPU < 3
  bx_panic("JO_Jv: not supported on < 386\n");
#else
  Bit32s offset32;

  if (bx_cpu.is_32bit_opsize) {
    offset32 = (Bit32s) bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()
    }
  else {
    offset32 = (Bit16s) bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()
    }

  if (bx_get_OF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset32;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jo_jv: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
    EIP = new_EIP;
    }
#endif
}

  INLINE void
bx_JNO_Jv()
{
#if BX_CPU < 3
  bx_panic("JNO_Jv: not supported on < 386\n");
#else
  Bit32s offset32;

  if (bx_cpu.is_32bit_opsize) {
    offset32 = (Bit32s) bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()
    }
  else {
    offset32 = (Bit16s) bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()
    }

  if (!bx_get_OF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset32;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jno_jv: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
    EIP = new_EIP;
    }
#endif
}

  INLINE void
bx_JB_Jv()
{
#if BX_CPU < 3
  bx_panic("JB_Jv: not supported on < 386\n");
#else
  Bit32s offset32;

  if (bx_cpu.is_32bit_opsize) {
    offset32 = (Bit32s) bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()
    }
  else {
    offset32 = (Bit16s) bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()
    }

  if (bx_get_CF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset32;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jb_jv: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
    EIP = new_EIP;
    }
#endif
}

  INLINE void
bx_JNB_Jv()
{
#if BX_CPU < 3
  bx_panic("JNB_Jv: not supported on < 386\n");
#else
  Bit32s offset32;

  if (bx_cpu.is_32bit_opsize) {
    offset32 = (Bit32s) bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()
    }
  else {
    offset32 = (Bit16s) bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()
    }

  if (!bx_get_CF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset32;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jnb_jv: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
    EIP = new_EIP;
    }
#endif
}

  INLINE void
bx_JZ_Jv()
{
#if BX_CPU < 3
  bx_panic("JZ_Jv: not supported on < 386\n");
#else
  Bit32s offset32;

  if (bx_cpu.is_32bit_opsize) {
    offset32 = (Bit32s) bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()
    }
  else {
    offset32 = (Bit16s) bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()
    }

  if (bx_get_ZF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset32;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jz_jv: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
    EIP = new_EIP;
    }
#endif
}

  INLINE void
bx_JNZ_Jv()
{
#if BX_CPU < 3
  bx_panic("JNZ_Jv: not supported on < 386\n");
#else
  Bit32s offset32;

  if (bx_cpu.is_32bit_opsize) {
    offset32 = (Bit32s) bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()
    }
  else {
    offset32 = (Bit16s) bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()
    }

  if (!bx_get_ZF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset32;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jnz_jv: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
    EIP = new_EIP;
    }
#endif
}

  INLINE void
bx_JBE_Jv()
{
#if BX_CPU < 3
  bx_panic("JBE_Jv: not supported on < 386\n");
#else
  Bit32s offset32;

  if (bx_cpu.is_32bit_opsize) {
    offset32 = (Bit32s) bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()
    }
  else {
    offset32 = (Bit16s) bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()
    }

  if (bx_get_CF() || bx_get_ZF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset32;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jbe_jv: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
    EIP = new_EIP;
    }
#endif
}

  INLINE void
bx_JNBE_Jv()
{
#if BX_CPU < 3
  bx_panic("JNBE_Jv: not supported on < 386\n");
#else
  Bit32s offset32;

  if (bx_cpu.is_32bit_opsize) {
    offset32 = (Bit32s) bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()
    }
  else {
    offset32 = (Bit16s) bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()
    }

  if (!bx_get_CF() && !bx_get_ZF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset32;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jnbe_jv: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
    EIP = new_EIP;
    }
#endif
}

  INLINE void
bx_JS_Jv()
{
#if BX_CPU < 3
  bx_panic("JS_Jv: not supported on < 386\n");
#else
  Bit32s offset32;

  if (bx_cpu.is_32bit_opsize) {
    offset32 = (Bit32s) bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()
    }
  else {
    offset32 = (Bit16s) bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()
    }

  if (bx_get_SF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset32;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("js_jv: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
    EIP = new_EIP;
    }
#endif
}

  INLINE void
bx_JNS_Jv()
{
#if BX_CPU < 3
  bx_panic("JNS_Jv: not supported on < 386\n");
#else
  Bit32s offset32;

  if (bx_cpu.is_32bit_opsize) {
    offset32 = (Bit32s) bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()
    }
  else {
    offset32 = (Bit16s) bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()
    }

  if (!bx_get_SF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset32;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jns_jv: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
    EIP = new_EIP;
    }
#endif
}

  INLINE void
bx_JP_Jv()
{
#if BX_CPU < 3
  bx_panic("JP_Jv: not supported on < 386\n");
#else
  Bit32s offset32;

  if (bx_cpu.is_32bit_opsize) {
    offset32 = (Bit32s) bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()
    }
  else {
    offset32 = (Bit16s) bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()
    }

  if (bx_get_PF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset32;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jp_jv: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
    EIP = new_EIP;
    }
#endif
}

  INLINE void
bx_JNP_Jv()
{
#if BX_CPU < 3
  bx_panic("JNP_Jv: not supported on < 386\n");
#else
  Bit32s offset32;

  if (bx_cpu.is_32bit_opsize) {
    offset32 = (Bit32s) bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()
    }
  else {
    offset32 = (Bit16s) bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()
    }

  if (!bx_get_PF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset32;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jnp_jv: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
    EIP = new_EIP;
    }
#endif
}

  INLINE void
bx_JL_Jv()
{
#if BX_CPU < 3
  bx_panic("JL_Jv: not supported on < 386\n");
#else
  Bit32s offset32;

  if (bx_cpu.is_32bit_opsize) {
    offset32 = (Bit32s) bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()
    }
  else {
    offset32 = (Bit16s) bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()
    }

  if ( bx_get_SF() != bx_get_OF() ) {
    Bit32u new_EIP;

    new_EIP = EIP + offset32;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jl_jv: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
    EIP = new_EIP;
    }
#endif
}

  INLINE void
bx_JNL_Jv()
{
#if BX_CPU < 3
  bx_panic("JNL_Jv: not supported on < 386\n");
#else
  Bit32s offset32;

  if (bx_cpu.is_32bit_opsize) {
    offset32 = (Bit32s) bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()
    }
  else {
    offset32 = (Bit16s) bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()
    }

  if (bx_get_SF() == bx_get_OF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset32;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jnl_jv: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
    EIP = new_EIP;
    }
#endif
}

  INLINE void
bx_JLE_Jv()
{
#if BX_CPU < 3
  bx_panic("JLE_Jv: not supported on < 386\n");
#else
  Bit32s offset32;

  if (bx_cpu.is_32bit_opsize) {
    offset32 = (Bit32s) bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()
    }
  else {
    offset32 = (Bit16s) bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()
    }

  if (bx_get_ZF() || (bx_get_SF() != bx_get_OF()) ) {
    Bit32u new_EIP;

    new_EIP = EIP + offset32;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jle_jv: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
    EIP = new_EIP;
    }
#endif
}

  INLINE void
bx_JNLE_Jv()
{
#if BX_CPU < 3
  bx_panic("JNLE_Jv: not supported on < 386\n");
#else
  Bit32s offset32;

  if (bx_cpu.is_32bit_opsize) {
    offset32 = (Bit32s) bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()
    }
  else {
    offset32 = (Bit16s) bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()
    }

  if ( (bx_get_SF() == bx_get_OF())  &&  !bx_get_ZF()) {
    Bit32u new_EIP;

    new_EIP = EIP + offset32;
    if (bx_cpu.is_32bit_opsize==0)
      new_EIP &= 0x0000ffff;
    if (bx_protected_mode()) {
      if ( new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled ) {
        bx_panic("jnle_jv: offset outside of CS limits\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      }
    EIP = new_EIP;
    }
#endif
}



  INLINE void
bx_JMP_Ev()
{
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;

  Bit32u new_EIP;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU >= 3
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

    new_EIP = op1_32;
    }
  else
#endif /* BX_CPU >= 3 */
    {
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

    new_EIP = op1_16;
    }

#if BX_CPU >= 2
  if (bx_protected_mode()) {
    if (new_EIP > bx_cpu.cs.cache.u.segment.limit_scaled) {
      bx_panic("jmp_ev: IP out of CS limits!\n");
      bx_exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }
    }
#endif

  bx_cpu.eip = new_EIP;
}

  /* Far indirect jump */

  INLINE void
bx_JMP_Ep()
{
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;

  Bit16u cs;

  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32;

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      /* far indirect must specify a memory address */
      bx_panic("JMP_Ep(): op1 is a register\n");
      return;
      }

    /* pointer, segment address pair */
    bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
    BX_HANDLE_EXCEPTION()
    bx_read_virtual_word(op1_seg_reg, op1_addr+4, &cs);
    BX_HANDLE_EXCEPTION()

    if ( bx_protected_mode() ) {
      if (bx_dbg.protected) bx_printf("jmp_ep():\n");
      bx_jump_protected(cs, op1_32);
      return;
      }

    bx_cpu.eip = op1_32;
    bx_load_seg_reg(&bx_cpu.cs, cs);
    }
  else
#endif /* BX_CPU >= 3 */
    {
    Bit16u op1_16;

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      /* far indirect must specify a memory address */
      bx_panic("JMP_Ep(): op1 is a register\n");
      return;
      }

    /* pointer, segment address pair */
    bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
    BX_HANDLE_EXCEPTION()
    bx_read_virtual_word(op1_seg_reg, op1_addr+2, &cs);
    BX_HANDLE_EXCEPTION()

#if BX_CPU >= 2
    if ( bx_protected_mode() ) {
      if (bx_dbg.protected) bx_printf("jmp_ep():\n");
      bx_jump_protected(cs, op1_16);
      return;
      }
#endif


    bx_cpu.eip = op1_16;
    bx_load_seg_reg(&bx_cpu.cs, cs);
    }
}

  INLINE void
bx_IRET()
{

  if (bx_v8086_mode()) {
    bx_stack_return_from_v86();
    return;
    }

#if BX_CPU >= 2
  if (bx_protected_mode()) {
    bx_iret_protected();
    return;
    }
#endif

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32u eip, ecs, eflags;

    bx_pop_32(&eip);
    bx_pop_32(&ecs);
    bx_pop_32(&eflags);

    bx_load_seg_reg(&bx_cpu.cs, (Bit16u) ecs);
    bx_cpu.eip = eip;
    bx_write_eflags(eflags, 1, 1, 0, 1);
    }
  else
#endif /* BX_CPU >= 3 */
    {
    Bit16u ip, cs, flags;

    bx_pop_16(&ip);
    bx_pop_16(&cs);
    bx_pop_16(&flags);

    bx_load_seg_reg(&bx_cpu.cs, cs);
    bx_cpu.eip = (Bit32u) ip;
    bx_write_flags(flags, 1, 1);

    }
}
