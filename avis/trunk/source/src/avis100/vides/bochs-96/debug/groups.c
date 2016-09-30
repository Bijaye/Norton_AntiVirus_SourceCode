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
#include "debug.h"


extern Boolean db_32bit_opsize;
extern Boolean db_32bit_addrsize;

  void
bx_debug_EbGb(void)
{
  bx_debug_decode_exgx(BX_GENERAL_8BIT_REG, BX_GENERAL_8BIT_REG);
  bx_printf("\n");
}

  void
bx_debug_EvGv(void)
{
#if BX_CPU > 2
  if (db_32bit_opsize)
    bx_debug_decode_exgx(BX_GENERAL_32BIT_REG, BX_GENERAL_32BIT_REG);
  else
#endif /* BX_CPU > 2 */
    bx_debug_decode_exgx(BX_GENERAL_16BIT_REG, BX_GENERAL_16BIT_REG);
  bx_printf("\n");
}

void bx_debug_GbEb(void) {
  bx_debug_decode_exgx(BX_GENERAL_8BIT_REG, BX_GENERAL_8BIT_REG);
  bx_printf("\n");
}

  void
bx_debug_GvEv(void)
{
#if BX_CPU > 2
  if (db_32bit_opsize)
    bx_debug_decode_exgx(BX_GENERAL_32BIT_REG, BX_GENERAL_32BIT_REG);
  else
#endif /* BX_CPU > 2 */
    bx_debug_decode_exgx(BX_GENERAL_16BIT_REG, BX_GENERAL_16BIT_REG);
  bx_printf("\n");
}

  void
bx_debug_ALIb(void)
{
  Bit8u imm8;

  imm8 = bx_debug_next_byte();
  bx_printf("AL, #%02x\n", imm8);
}

  void
bx_debug_eAXIv(void)
{

#if BX_CPU > 2
  if (db_32bit_opsize) {
    bx_printf("EAX, #%08x\n", (int) bx_debug_next_dword());
    }
  else {
#endif /* BX_CPU > 2 */
    bx_printf("AX, #%04x\n", (int) bx_debug_next_word());
#if BX_CPU > 2
    }
#endif /* BX_CPU > 2 */
}

void bx_debug_ES(void) {bx_printf("\n");}

void bx_debug_Ew(void) {bx_printf("\n");}
void bx_debug_Ms(void) {bx_printf("\n");}
void bx_debug_GvEw(void) {bx_printf("\n");}
void bx_debug_RdCd(void) {bx_printf("\n");}
void bx_debug_RdDd(void) {bx_printf("\n");}
void bx_debug_CdRd(void) {bx_printf("\n");}
void bx_debug_DdRd(void) {bx_printf("\n");}
void bx_debug_RdTd(void) {bx_printf("\n");}
void bx_debug_TdRd(void) {bx_printf("\n");}

  void
bx_debug_Jv(void)
{
  Bit16u imm16;

#if BX_CPU > 2
  if (db_32bit_opsize) {
    Bit32u imm32;

    imm32 = bx_debug_next_dword();
    bx_printf("%08x\n", (int) imm32);
    }
  else {
#endif /* BX_CPU > 2 */
    imm16 = bx_debug_next_word();
    bx_printf("%04x\n", (int) imm16);
#if BX_CPU > 2
    }
#endif /* BX_CPU > 2 */
}

void bx_debug_Eb(void) {bx_printf("\n");}
void bx_debug_FS(void) {bx_printf("\n");}
void bx_debug_EvGvIb(void) {bx_printf("\n");}
void bx_debug_EvGvCL(void) {bx_printf("\n");}
void bx_debug_XBTS(void) {bx_printf("\n");}
void bx_debug_IBTS(void) {bx_printf("\n");}
void bx_debug_GS(void) {bx_printf("\n");}
void bx_debug_Mp(void) {bx_printf("\n");}

  void
bx_debug_EvIb(void)
{
  Bit8u imm8;

#if BX_CPU > 2
  if (db_32bit_opsize) {
    bx_debug_decode_exgx(BX_NO_REG_TYPE, BX_GENERAL_32BIT_REG);
    imm8 = bx_debug_next_byte();
    bx_printf(", #%02x\n", (int) imm8);
    }
  else {
#endif /* BX_CPU > 2 */
    bx_debug_decode_exgx(BX_NO_REG_TYPE, BX_GENERAL_16BIT_REG);
    imm8 = bx_debug_next_byte();
    bx_printf(", #%02x\n", (int) imm8);
#if BX_CPU > 2
    }
#endif /* BX_CPU > 2 */
}

void bx_debug_GvEb(void) {bx_printf("\n");}
void bx_debug_EAX(void) {bx_printf("\n");}
void bx_debug_ECX(void) {bx_printf("\n");}
void bx_debug_EDX(void) {bx_printf("\n");}
void bx_debug_EBX(void) {bx_printf("\n");}
void bx_debug_ESP(void) {bx_printf("\n");}
void bx_debug_EBP(void) {bx_printf("\n");}
void bx_debug_ESI(void) {bx_printf("\n");}
void bx_debug_EDI(void) {bx_printf("\n");}
void bx_debug_SS(void) {bx_printf("\n");}
void bx_debug_DS(void) {bx_printf("\n");}

void bx_debug_eAX(void) {bx_printf("\n");}
void bx_debug_eCX(void) {bx_printf("\n");}
void bx_debug_eDX(void) {bx_printf("\n");}
void bx_debug_eBX(void) {bx_printf("\n");}
void bx_debug_eSP(void) {bx_printf("\n");}
void bx_debug_eBP(void) {bx_printf("\n");}
void bx_debug_eSI(void) {bx_printf("\n");}
void bx_debug_eDI(void) {bx_printf("\n");}
void bx_debug_GvMa(void) {bx_printf("\n");}
void bx_debug_EwRw(void) {bx_printf("\n");}
void bx_debug_Iv(void) {bx_printf("\n");}
void bx_debug_GvEvIv(void) {bx_printf("\n");}

  void
bx_debug_Ib(void)
{
  Bit8u imm8;

  imm8 = bx_debug_next_byte();
  bx_printf("#%02x\n", imm8);
}

void bx_debug_GvEvIb(void) {bx_printf("\n");}
void bx_debug_YbDX(void) {bx_printf("\n");}
void bx_debug_YvDX(void) {bx_printf("\n");}
void bx_debug_DXXb(void) {bx_printf("\n");}
void bx_debug_DXXv(void) {bx_printf("\n");}

  void
bx_debug_Jb(void)
{
  Bit8s offset;

  offset = (Bit8s) bx_debug_next_byte();
  bx_printf("(%+d)\n", (int) offset);
}

  void
bx_debug_EbIb(void)
{
  Bit8u imm8;

  bx_debug_decode_exgx(BX_NO_REG_TYPE, BX_GENERAL_8BIT_REG);
  imm8 = bx_debug_next_byte();
  bx_printf(" #%x\n", (int) imm8);
}

  void
bx_debug_EvIv(void)
{
  Bit16u imm16;

#if BX_CPU > 2
  if (db_32bit_opsize) {
    Bit32u imm32;

    bx_debug_decode_exgx(BX_NO_REG_TYPE, BX_GENERAL_32BIT_REG);
    imm32 = bx_debug_next_dword();
    }
  else {
#endif /* BX_CPU > 2 */
    bx_debug_decode_exgx(BX_NO_REG_TYPE, BX_GENERAL_16BIT_REG);
    imm16 = bx_debug_next_word();
#if BX_CPU > 2
    }
#endif /* BX_CPU > 2 */
  bx_printf("\n");
}

  void
bx_debug_EwSw(void)
{
  bx_debug_decode_exgx(BX_SEGMENT_REG, BX_GENERAL_16BIT_REG);
  bx_printf("\n");
}

void bx_debug_GvM(void)
{
#if BX_CPU > 2
  if (db_32bit_opsize)
    bx_debug_decode_exgx(BX_GENERAL_32BIT_REG, BX_GENERAL_32BIT_REG);
  else
#endif /* BX_CPU > 2 */
    bx_debug_decode_exgx(BX_GENERAL_16BIT_REG, BX_GENERAL_16BIT_REG);
  bx_printf("\n");
}

  void
bx_debug_SwEw(void)
{
  bx_debug_decode_exgx(BX_SEGMENT_REG, BX_GENERAL_16BIT_REG);
  bx_printf("\n");
}

  void
bx_debug_Ev(void)
{
#if BX_CPU > 2
  if (db_32bit_opsize) {
    bx_debug_decode_exgx(BX_NO_REG_TYPE, BX_GENERAL_32BIT_REG);
    }
  else {
#endif /* BX_CPU > 2 */
    bx_debug_decode_exgx(BX_NO_REG_TYPE, BX_GENERAL_16BIT_REG);
#if BX_CPU > 2
    }
#endif /* BX_CPU > 2 */
  bx_printf("\n");
}

void bx_debug_ECXeAX(void) {bx_printf("\n");}
void bx_debug_eDXeAX(void) {bx_printf("\n");}
void bx_debug_eBXeAX(void) {bx_printf("\n");}
void bx_debug_eSPeAX(void) {bx_printf("\n");}
void bx_debug_eBPeAX(void) {bx_printf("\n");}
void bx_debug_eDIeAX(void) {bx_printf("\n");}
void bx_debug_Ap(void) {bx_printf("\n");}
void bx_debug_Fv(void) {bx_printf("\n");}
void bx_debug_ALOb(void) {bx_printf("\n");}
void bx_debug_eAXOv(void) {bx_printf("\n");}
void bx_debug_ObAL(void) {bx_printf("\n");}
void bx_debug_OveAX(void) {bx_printf("\n");}

  void
bx_debug_XbYb(void)
{
  bx_printf("DS:[SI] ES:[DI]\n");
}

void bx_debug_XvYv(void) {bx_printf("\n");}
void bx_debug_YbAL(void) {bx_printf("\n");}
void bx_debug_YveAX(void) {bx_printf("\n");}
void bx_debug_ALXb(void) {bx_printf("\n");}

  void
bx_debug_eAXXv(void)
{
  bx_printf("\n");
}

void bx_debug_eCXeAX(void) {bx_printf("\n");}

void bx_debug_CLIb(void) {bx_printf("\n");}
void bx_debug_BLIb(void) {bx_printf("\n");}
void bx_debug_AHIb(void) {bx_printf("\n");}
void bx_debug_CHIb(void) {bx_printf("\n");}
void bx_debug_DHIb(void) {bx_printf("\n");}
void bx_debug_BSIb(void) {bx_printf("\n");}

  void
bx_debug_eCXIv(void)
{
  Bit16u imm16;

#if BX_CPU > 2
  if (db_32bit_opsize) {
    Bit32u imm32;
    imm32 = bx_debug_next_dword();
    bx_printf("ECX, %x\n", imm32);
    }
  else {
#endif /* BX_CPU > 2 */
    imm16 = bx_debug_next_word();
    bx_printf("CX, %x\n", imm16);
#if BX_CPU > 2
    }
#endif /* BX_CPU > 2 */
}

  void
bx_debug_eDXIv(void)
{
  Bit16u imm16;

#if BX_CPU > 2
  if (db_32bit_opsize) {
    Bit32u imm32;

    imm32 = bx_debug_next_dword();
    bx_printf("EDX, %x\n", imm32);
    }
  else {
#endif /* BX_CPU > 2 */
    imm16 = bx_debug_next_word();
    bx_printf("DX, %x\n", imm16);
#if BX_CPU > 2
    }
#endif /* BX_CPU > 2 */
}

  void
bx_debug_eBXIv(void)
{
  Bit16u imm16;

#if BX_CPU > 2
  if (db_32bit_opsize) {
    Bit32u imm32;

    imm32 = bx_debug_next_dword();
    bx_printf("EBX, %x\n", imm32);
    }
  else {
#endif /* BX_CPU > 2 */
    imm16 = bx_debug_next_word();
    bx_printf("BX, %x\n", imm16);
#if BX_CPU > 2
    }
#endif /* BX_CPU > 2 */
}

void bx_debug_eSIIv(void) {bx_printf("\n");}

  void
bx_debug_eDIIv(void)
{
  Bit16u imm16;

#if BX_CPU > 2
  if (db_32bit_opsize) {
    Bit32u imm32;

    imm32 = bx_debug_next_dword();
    bx_printf("EDI, %x\n", imm32);
    }
  else {
#endif /* BX_CPU > 2 */
    imm16 = bx_debug_next_word();
    bx_printf("DI, %x\n", imm16);
#if BX_CPU > 2
    }
#endif /* BX_CPU > 2 */
}

void bx_debug_eSIeAX(void) {bx_printf("\n");}
void bx_debug_DLIb(void) {bx_printf("\n");}
void bx_debug_BHIb(void) {bx_printf("\n");}

  void
bx_debug_eSPIv(void)
{
  Bit16u imm16;

#if BX_CPU > 2
  if (db_32bit_opsize) {
    Bit32u imm32;

    imm32 = bx_debug_next_dword();
    bx_printf("ESP, %x\n", imm32);
    }
  else {
#endif /* BX_CPU > 2 */
    imm16 = bx_debug_next_word();
    bx_printf("SP, %x\n", imm16);
#if BX_CPU > 2
    }
#endif /* BX_CPU > 2 */
}

void bx_debug_eBPIv(void) {bx_printf("\n");}
void bx_debug_Iw(void) {bx_printf("\n");}

  void
bx_debug_GvMp(void)
{
#if BX_CPU > 2
  if (db_32bit_opsize)
    bx_debug_decode_exgx(BX_GENERAL_32BIT_REG, BX_GENERAL_32BIT_REG);
  else
#endif /* BX_CPU > 2 */
    bx_debug_decode_exgx(BX_GENERAL_16BIT_REG, BX_GENERAL_16BIT_REG);
  bx_printf("\n");
}

void bx_debug_IwIb(void) {bx_printf("\n");}
void bx_debug_Eb1(void) {bx_printf("\n");}
void bx_debug_Ev1(void) {bx_printf("\n");}
void bx_debug_EbCL(void) {bx_printf("\n");}

void bx_debug_EvCL(void)
{
#if BX_CPU > 2
  if (db_32bit_opsize) {
    bx_debug_decode_exgx(BX_NO_REG_TYPE, BX_GENERAL_32BIT_REG);
    }
  else {
#endif /* BX_CPU > 2 */
    bx_debug_decode_exgx(BX_NO_REG_TYPE, BX_GENERAL_16BIT_REG);
#if BX_CPU > 2
    }
#endif /* BX_CPU > 2 */
  bx_printf(",CL\n");
}

void bx_debug_Es(void) {bx_printf("\n");}
void bx_debug_ST_STi(void) {bx_printf("\n");}

void bx_debug_CS(void) {bx_printf("\n");}
void bx_debug_Ea(void) {bx_printf("\n");}
void bx_debug_STi(void) {bx_printf("\n");}
void bx_debug_Et(void) {bx_printf("\n");}
void bx_debug_Ed(void) {bx_printf("\n");}
void bx_debug_El(void) {bx_printf("\n");}
void bx_debug_AX(void) {bx_printf("\n");}
void bx_debug_eAXIb(void) {bx_printf("\n");}

  void
bx_debug_IbAL(void)
{
  Bit8u imm8;

  imm8 = bx_debug_next_byte();
  bx_printf("#%02x, AL\n", imm8);
}

void bx_debug_IbeAX(void) {bx_printf("\n");}

void bx_debug_STi_ST(void) {bx_printf("\n");}
void bx_debug_Eq(void) {bx_printf("\n");}
void bx_debug_Av(void) {bx_printf("\n");}
void bx_debug_ALDX(void) {bx_printf("\n");}
void bx_debug_eAXDX(void) {bx_printf("\n");}
void bx_debug_DXAL(void) {bx_printf("\n");}
void bx_debug_DXeAX(void) {bx_printf("\n");}
void bx_debug_ALEb(void) {bx_printf("\n");}

  void
bx_debug_eAXEv(void)
{
#if BX_CPU > 2
  if (db_32bit_opsize) {
    bx_printf("EAX, ");
    bx_debug_decode_exgx(BX_NO_REG_TYPE, BX_GENERAL_32BIT_REG);
    }
  else {
#endif /* BX_CPU > 2 */
    bx_printf("AX, ");
    bx_debug_decode_exgx(BX_NO_REG_TYPE, BX_GENERAL_16BIT_REG);
#if BX_CPU > 2
    }
#endif /* BX_CPU > 2 */
  bx_printf("\n");
}

  void
bx_debug_Ep(void)
{
#if BX_CPU > 2
  if (db_32bit_opsize) {
    bx_debug_decode_exgx(BX_NO_REG_TYPE, BX_GENERAL_32BIT_REG);
    }
  else {
#endif /* BX_CPU > 2 */
    bx_debug_decode_exgx(BX_NO_REG_TYPE, BX_GENERAL_16BIT_REG);
#if BX_CPU > 2
    }
#endif /* BX_CPU > 2 */

  bx_printf("\n");
}

