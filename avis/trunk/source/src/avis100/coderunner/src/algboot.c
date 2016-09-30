/*---------------------------------------------------------------------------*/
/* This module implements the algorithmic boot detector.                     */
/* The original hand-crafted detector is included but #ifdefed out.          */
/* The new detector is in algbnet.h                                          */
/* This module also includes support for excluding certain know-uninfected   */
/* but still suspicious boot records, and a top level routine that calls     */
/* whichever algorithmic detector has been compiled in.                      */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef S_H
#include "mymalloc.h"
#endif

#ifndef TRUE
#define TRUE                (1)
#endif
#ifndef FALSE
#define FALSE               (!TRUE)
#endif
#define XOR                 ^            /* The XOR operator */

#define SIGCMP_LOCAL 0
#if SIGCMP_LOCAL
typedef unsigned char byte;
typedef unsigned int bOOlean;
#else
#include "vsdefs.h"
#include "vstypes.h"
#include "sigcmp.pro"
#endif

#include "algboot.pro"

#include "cr_all.h"     /* Where CodeRunner defs are defined. Used by BOOTWALK.H */
#include "bootwalk.h"   /* Where 'SECTOR' is defined. Used by BOOTRUN.H */
#include "bootrun.h"    /* Where 'BUFFER' is defined. */

extern bOOlean always_warn;
#if !ESDC
extern bOOlean use_old_alg_boot;
#endif


#if SIGCMP_LOCAL
/*
 * Match a very complex signature, i.e. one with n-byte don't cares.
 * This routine is recursive. Don't forget to fix printer in this module.
 * This routine is normally kept in sigcmp.c; a copy has been included here
 * for Jeff Kephart's convenience, but it's not normally compiled in.
 */
bOOlean
match_vcomplex_sig(byte *masked_signature,
                   byte *test_addr,
                   int len)
{
   int i;
   register int j;
   register int N;

   if (*masked_signature == (byte)(0xFE XOR 0xFF))
   {
      N = masked_signature[1] XOR 0xFF;
      for (j=0; j<=N; ++j)
      {
         if (match_vcomplex_sig(&masked_signature[2],
                                &test_addr[j],
                                len-(2)))
         {
            return TRUE;
         }
      }
      return FALSE;
   }
   else
   {
      if (((*masked_signature XOR (byte) 0xFF) != *test_addr) &&
          (*masked_signature != (byte)(0xFF XOR 0xFF))
         )
      {
         return FALSE;
      }
   }
   for (i=1; i<len; ++i)
   {
      if (masked_signature[i] == (byte)(0xFE XOR 0xFF))
      {
         N = masked_signature[i+1] XOR 0xFF;
         for (j=0; j<=N; ++j)
         {
            if (match_vcomplex_sig(&masked_signature[i+2],
                                   &test_addr[i+j],
                                   len-(i+2)))
            {
               return TRUE;
            }
         }
         return FALSE;
      }
      else
      {
         if (((masked_signature[i] XOR (byte) 0xFF) != test_addr[i]) &&
             (masked_signature[i] != (byte)(0xFF XOR 0xFF))
            )
         {
            return FALSE;
         }
      }
   }
   return TRUE;
}
#endif

#if !ESDC
#if 0
A14C00 %A A3???? %2 A14E00 %2 A3????
%s HookINT13(1) %s
BOOT. COM. EXE.
*
C4064C00 %4 A3???? %C 8C06????
%s HookINT13(2) %s
BOOT. COM. EXE.
*
C4064C00 %8 2EA3???? %C C7064C00
%s HookINT13(3) %s
BOOT. COM. EXE.
*
A14C00 %A C7064C00???? %8 8C0E4E00
%s HookINT13(4) %s
BOOT. COM. EXE.
*
FA C7064C00???? %6 A34E00 %4 FB
%s HookINT13(5) %s
BOOT. COM. EXE.
*
BB4C00 %4 8907 %4 8CC0 %4 894702
%s HookINT13(6) %s
BOOT. COM. EXE.
*
BE4C00 /cmvc/home/avsource/vc/0/1/6/8/s.281 A5 /cmvc/home/avsource/vc/0/1/6/8/s.281 C7064C00???? %4 8C0E4E00
%s HookINT13(7) %s
BOOT. COM. EXE.
*
BB4C00 %4 8B0F %6 B8???? %4 8907
%s HookINT13(8) %s
BOOT. COM. EXE.
*
8B1E1304 %C 83EB?? %C 891E1304
%s HookINT13(9) %s
BOOT. COM. EXE.
*
A34E00 %E C7064C00
%s HookINT13(A) %s
BOOT. COM. EXE.
*
BE4C00 %2 A5 A5 %8 C744FC
%s HookINT13(B) %s
BOOT. COM. EXE.
*
BE4C00 %2 A5 A5 %8 C7064C00
%s HookINT13(C) %s
BOOT. COM. EXE.
*
A34C00 %8 8C0E4E00
%s HookINT13(D) %s
BOOT. COM. EXE.
*-------
B80103 %8 BB???? %8 B1?? %8 B601
%s StashOrigBS(1) %s
BOOT. COM. EXE.
*
B80103 %4 31DB %8 CD13
%s StashOrigBS(2) %s
BOOT. COM. EXE.
*
41 33DB %8 B80103 %8 32F6 %4 E8
%s StashOrigBS(3) %s
BOOT. COM. EXE.
*
F3 A4 %8 B80103 %8 33DB %8 FEC1 %8 CD13
%s StashOrigBS(4) %s
BOOT. COM. EXE.
*
33DB %8 B80103 %8 8A36 %8 B90100 %8 CD
%s StashOrigBS(5) %s
BOOT. COM. EXE.
*
B80103 %4 33DB %4 B90100 %4 33D2 %4 9C %4 FF1E
%s StashOrigBS(6) %s
BOOT. COM. EXE.
*
B80103 %2 B1?? %2 CD13
%s StashOrigBS(7) %s
BOOT. COM. EXE.
*
B80103 %2 8B %A 8B %A CD13
%s StashOrigBS(8) %s
BOOT. COM. EXE.
*
B80103 %2 B9??00 %2 B600 %2 CD13
%s StashOrigBS(9) %s
BOOT. COM. EXE.
*-----
B90001 %4 FC F3 A5
%s MoveBoot(1) %s
BOOT. COM. EXE.
*
B90002 %6 FC F3 A4
%s MoveBoot(2) %s
BOOT. COM. EXE.
*
BF0000 %8 B90002 F3 A4
%s MoveBoot(3) %s
BOOT. COM. EXE.
*
BE007C %4 B90001 %4 F3 A5
%s MoveBoot(4) %s
BOOT. COM. EXE.
*
B9??01 FC F3 A4
%s MoveBoot(5) %s
BOOT. COM. EXE.
*
FC %2 B9??01 %2 F3 A4
%s MoveBoot(6) %s
BOOT. COM. EXE.
*
BE007C %8 B900?? %8 FC F3 A5
%s MoveBoot(7) %s
BOOT. COM. EXE.
*
BE007C %20 B9???? %8 FC F3 A4
%s MoveBoot(8) %s
BOOT. COM. EXE.
*
FC %4 B90001 %2 F3 A5
%s MoveBoot(9) %s
BOOT. COM. EXE.
*
B900?? %C BE007C %C FC F3 A4
%s MoveBoot(A) %s
BOOT. COM. EXE.
*-----
A11304 %4 48 %4 48 %4 A31304
%s LowerMemAvail(1) %s
BOOT. COM. EXE.
*
A11304 %4 48 %4 48 %4 832E1304
%s LowerMemAvail(2) %s
BOOT. COM. EXE.
*
FF0E1304
%s LowerMemAvail(3) %s
BOOT. COM. EXE.
*
832E1304
%s LowerMemAvail(4) %s
BOOT. COM. EXE.
*
A11304 %4 48 %4 A31304
%s LowerMemAvail(5) %s
BOOT. COM. EXE.
*
A11304 %A 2D %6 A31304
%s LowerMemAvail(6) %s
BOOT. COM. EXE.
#endif

/* These are short search patterns for the original hand-crafted algorithmic
 * boot detector. Search patterns are masked with hex FF so that they aren't
 * found by AV products.
 * They are grouped into four categories:
 * Category 1: various ways of hooking the INT 13 vector.
 * Category 2: various ways of using INT 13 to write a sector.
 * Category 3: various ways of moving a BSV-sized block around in memory.
 *             (This characteristic is also in some normal boot records.)
 * Category 4: various ways of lowering the amount of system memory available.
 */
unsigned char HookINT13_1[] =
{ 0xA1^0xFF, 0x4C^0xFF, 0x00^0xFF, 0xFE^0xFF,
  0x0A^0xFF, 0xA3^0xFF, 0xFF^0xFF, 0xFF^0xFF,
  0xFE^0xFF, 0x02^0xFF, 0xA1^0xFF, 0x4E^0xFF,
  0x00^0xFF, 0xFE^0xFF, 0x02^0xFF, 0xA3^0xFF };
unsigned char HookINT13_2[] =
{ 0xC4^0xFF, 0x06^0xFF, 0x4C^0xFF, 0x00^0xFF,
  0xFE^0xFF, 0x04^0xFF, 0xA3^0xFF, 0xFF^0xFF,
  0xFF^0xFF, 0xFE^0xFF, 0x0C^0xFF, 0x8C^0xFF,
  0x06^0xFF };
unsigned char HookINT13_3[] =
{ 0xC4^0xFF, 0x06^0xFF, 0x4C^0xFF, 0x00^0xFF,
  0xFE^0xFF, 0x08^0xFF, 0x2E^0xFF, 0xA3^0xFF,
  0xFF^0xFF, 0xFF^0xFF, 0xFE^0xFF, 0x0C^0xFF,
  0xC7^0xFF, 0x06^0xFF, 0x4C^0xFF, 0x00^0xFF };
unsigned char HookINT13_4[] =
{ 0xA1^0xFF, 0x4C^0xFF, 0x00^0xFF, 0xFE^0xFF,
  0x0A^0xFF, 0xC7^0xFF, 0x06^0xFF, 0x4C^0xFF,
  0x00^0xFF, 0xFF^0xFF, 0xFF^0xFF, 0xFE^0xFF,
  0x08^0xFF, 0x8C^0xFF, 0x0E^0xFF, 0x4E^0xFF,
  0x00 };
unsigned char HookINT13_5[] =
{ 0xFA^0xFF, 0xC7^0xFF, 0x06^0xFF, 0x4C^0xFF,
  0x00^0xFF, 0xFF^0xFF, 0xFF^0xFF, 0xFE^0xFF,
  0x06^0xFF, 0xA3^0xFF, 0x4E^0xFF, 0x00^0xFF,
  0xFE^0xFF, 0x04^0xFF, 0xFB^0xFF };
unsigned char HookINT13_6[] =
{ 0xBB^0xFF, 0x4C^0xFF, 0x00^0xFF, 0xFE^0xFF,
  0x04^0xFF, 0x89^0xFF, 0x07^0xFF, 0xFE^0xFF,
  0x04^0xFF, 0x8C^0xFF, 0xC0^0xFF, 0xFE^0xFF,
  0x04^0xFF, 0x89^0xFF, 0x47^0xFF, 0x02^0xFF };
unsigned char HookINT13_7[] =
{ 0xBE^0xFF, 0x4C^0xFF, 0x00^0xFF, 0xFE^0xFF,
  0x20^0xFF, 0xA5^0xFF, 0xFE^0xFF, 0x20^0xFF,
  0xC7^0xFF, 0x06^0xFF, 0x4C^0xFF, 0x00^0xFF,
  0xFF^0xFF, 0xFF^0xFF, 0xFE^0xFF, 0x04^0xFF,
  0x8C^0xFF, 0x0E^0xFF, 0x4E^0xFF, 0x00^0xFF
};
unsigned char HookINT13_8[] =
{ 0xBB^0xFF, 0x4C^0xFF, 0x00^0xFF, 0xFE^0xFF,
  0x04^0xFF, 0x8B^0xFF, 0x0F^0xFF, 0xFE^0xFF,
  0x06^0xFF, 0xB8^0xFF, 0xFF^0xFF, 0xFF^0xFF,
  0xFE^0xFF, 0x04^0xFF, 0x89^0xFF, 0x07^0xFF };
unsigned char HookINT13_9[] =
{ 0x8B^0xFF, 0x1E^0xFF, 0x13^0xFF, 0x04^0xFF,
  0xFE^0xFF, 0x0C^0xFF, 0x83^0xFF, 0xEB^0xFF,
  0xFF^0xFF, 0xFE^0xFF, 0x0C^0xFF, 0x89^0xFF,
  0x1E^0xFF, 0x13^0xFF, 0x04^0xFF };
unsigned char HookINT13_A[] =
{ 0xA3^0xFF, 0x4E^0xFF, 0x00^0xFF, 0xFE^0xFF,
  0x0E^0xFF, 0xC7^0xFF, 0x06^0xFF, 0x4C^0xFF,
  0x00^0xFF };
unsigned char HookINT13_B[] =
{ 0xBE^0xFF, 0x4C^0xFF, 0x00^0xFF, 0xFE^0xFF,
  0x02^0xFF, 0xA5^0xFF, 0xA5^0xFF, 0xFE^0xFF,
  0x08^0xFF, 0xC7^0xFF, 0x44^0xFF, 0xFC^0xFF };
unsigned char HookINT13_C[] =
{ 0xBE^0xFF, 0x4C^0xFF, 0x00^0xFF, 0xFE^0xFF,
  0x02^0xFF, 0xA5^0xFF, 0xA5^0xFF, 0xFE^0xFF,
  0x08^0xFF, 0xC7^0xFF, 0x06^0xFF, 0x4C^0xFF,
  0x00^0xFF };
unsigned char HookINT13_D[] =
{ 0xA3^0xFF, 0x4C^0xFF, 0x00^0xFF, 0xFE^0xFF,
  0x08^0xFF, 0x8C^0xFF, 0x0E^0xFF, 0x4E^0xFF,
  0x00^0xFF };

unsigned char StashOrigBS_1[] =
{ 0xB8^0xFF, 0x01^0xFF, 0x03^0xFF, 0xFE^0xFF,
  0x08^0xFF, 0xBB^0xFF, 0xFF^0xFF, 0xFF^0xFF,
  0xFE^0xFF, 0x08^0xFF, 0xB1^0xFF, 0xFF^0xFF,
  0xFE^0xFF, 0x08^0xFF, 0xB6^0xFF, 0x01^0xFF };
unsigned char StashOrigBS_2[] =
{ 0xB8^0xFF, 0x01^0xFF, 0x03^0xFF, 0xFE^0xFF,
  0x04^0xFF, 0x31^0xFF, 0xDB^0xFF, 0xFE^0xFF,
  0x08^0xFF, 0xCD^0xFF, 0x13^0xFF };
unsigned char StashOrigBS_3[] =
{ 0x41^0xFF, 0x33^0xFF, 0xDB^0xFF, 0xFE^0xFF,
  0x08^0xFF, 0xB8^0xFF, 0x01^0xFF, 0x03^0xFF,
  0xFE^0xFF, 0x08^0xFF, 0x32^0xFF, 0xF6^0xFF,
  0xFE^0xFF, 0x04^0xFF, 0xE8^0xFF };
unsigned char StashOrigBS_4[] =
{ 0xF3^0xFF, 0xA4^0xFF, 0xFE^0xFF, 0x08^0xFF,
  0xB8^0xFF, 0x01^0xFF, 0x03^0xFF, 0xFE^0xFF,
  0x08^0xFF, 0x33^0xFF, 0xDB^0xFF, 0xFE^0xFF,
  0x08^0xFF, 0xFF^0xFF, 0xC1^0xFF, 0xFE^0xFF,
  0x08^0xFF, 0xCD^0xFF, 0x13^0xFF };
unsigned char StashOrigBS_5[] =
{ 0x33^0xFF, 0xDB^0xFF, 0xFE^0xFF, 0x08^0xFF,
  0xB8^0xFF, 0x01^0xFF, 0x03^0xFF, 0xFE^0xFF,
  0x08^0xFF, 0x8A^0xFF, 0x36^0xFF, 0xFE^0xFF,
  0x08^0xFF, 0xB9^0xFF, 0x01^0xFF, 0x00^0xFF,
  0xFE^0xFF, 0x08^0xFF, 0xCD^0xFF };
unsigned char StashOrigBS_6[] =
{ 0xB8^0xFF, 0x01^0xFF, 0x03^0xFF, 0xFE^0xFF,
  0x04^0xFF, 0x33^0xFF, 0xDB^0xFF, 0xFE^0xFF,
  0x04^0xFF, 0xB9^0xFF, 0x01^0xFF, 0x00^0xFF,
  0xFE^0xFF, 0x04^0xFF, 0x33^0xFF, 0xD2^0xFF,
  0xFE^0xFF, 0x04^0xFF, 0x9C^0xFF, 0xFE^0xFF,
  0x04^0xFF, 0xFF^0xFF, 0x1E^0xFF };
unsigned char StashOrigBS_7[] =
{ 0xB8^0xFF, 0x01^0xFF, 0x03^0xFF, 0xFE^0xFF,
  0x02^0xFF, 0xB1^0xFF, 0xFF^0xFF, 0xFE^0xFF,
  0x02^0xFF, 0xCD^0xFF, 0x13^0xFF };
unsigned char StashOrigBS_8[] =
{ 0xB8^0xFF, 0x01^0xFF, 0x03^0xFF, 0xFE^0xFF,
  0x02^0xFF, 0x8B^0xFF, 0xFE^0xFF, 0x0A^0xFF,
  0x8B^0xFF, 0xFE^0xFF, 0x0A^0xFF, 0xCD^0xFF,
  0x13^0xFF };
unsigned char StashOrigBS_9[] =
{ 0xB8^0xFF, 0x01^0xFF, 0x03^0xFF, 0xFE^0xFF,
  0x02^0xFF, 0xB9^0xFF, 0xFF^0xFF, 0x00^0xFF,
  0xFE^0xFF, 0x02^0xFF, 0xB6^0xFF, 0x00^0xFF,
  0xFE^0xFF, 0x02^0xFF, 0xCD^0xFF, 0x13^0xFF };

unsigned char MoveBoot_1[] =
{ 0xB9^0xFF, 0x00^0xFF, 0x01^0xFF, 0xFE^0xFF,
  0x04^0xFF, 0xFC^0xFF, 0xF3^0xFF, 0xA5^0xFF };
unsigned char MoveBoot_2[] =
{ 0xB9^0xFF, 0x00^0xFF, 0x02^0xFF, 0xFE^0xFF,
  0x06^0xFF, 0xFC^0xFF, 0xF3^0xFF, 0xA4^0xFF };
unsigned char MoveBoot_3[] =
{ 0xBF^0xFF, 0x00^0xFF, 0x00^0xFF, 0xFE^0xFF,
  0x08^0xFF, 0xB9^0xFF, 0x00^0xFF, 0x02^0xFF,
  0xF3^0xFF, 0xA4^0xFF };
unsigned char MoveBoot_4[] =
{ 0xBE^0xFF, 0x00^0xFF, 0x7C^0xFF, 0xFE^0xFF,
  0x04^0xFF, 0xB9^0xFF, 0x00^0xFF, 0x01^0xFF,
  0xFE^0xFF, 0x04^0xFF, 0xF3^0xFF, 0xA5^0xFF };
unsigned char MoveBoot_5[] =
{ 0xB9^0xFF, 0xFF^0xFF, 0x01^0xFF, 0xFC^0xFF,
  0xF3^0xFF, 0xA4^0xFF };
unsigned char MoveBoot_6[] =
{ 0xFC^0xFF, 0xFE^0xFF, 0x02^0xFF, 0xB9^0xFF,
  0xFF^0xFF, 0x01^0xFF, 0xFE^0xFF, 0x02^0xFF,
  0xF3^0xFF, 0xA4^0xFF };
unsigned char MoveBoot_7[] =
{ 0xBE^0xFF, 0x00^0xFF, 0x7C^0xFF, 0xFE^0xFF,
  0x08^0xFF, 0xB9^0xFF, 0x00^0xFF, 0xFF^0xFF,
  0xFE^0xFF, 0x08^0xFF, 0xFC^0xFF, 0xF3^0xFF,
  0xA5^0xFF };
unsigned char MoveBoot_8[] =
{ 0xBE^0xFF, 0x00^0xFF, 0x7C^0xFF, 0xFE^0xFF,
  0x20^0xFF, 0xB9^0xFF, 0xFF^0xFF, 0xFF^0xFF,
  0xFE^0xFF, 0x08^0xFF, 0xFC^0xFF, 0xF3^0xFF,
  0xA4^0xFF };
unsigned char MoveBoot_9[] =
{ 0xFC^0xFF, 0xFE^0xFF, 0x04^0xFF, 0xB9^0xFF,
  0x00^0xFF, 0x01^0xFF, 0xFE^0xFF, 0x02^0xFF,
  0xF3^0xFF, 0xA5^0xFF };
unsigned char MoveBoot_A[] =
{ 0xB9^0xFF, 0x00^0xFF, 0xFF^0xFF, 0xFE^0xFF,
  0x0C^0xFF, 0xBE^0xFF, 0x00^0xFF, 0x7C^0xFF,
  0xFE^0xFF, 0x0C^0xFF, 0xFC^0xFF, 0xF3^0xFF,
  0xA4^0xFF };

unsigned char LowerMemAvail_1[] =
{ 0xA1^0xFF, 0x13^0xFF, 0x04^0xFF, 0xFE^0xFF,
  0x04^0xFF, 0x48^0xFF, 0xFE^0xFF, 0x04^0xFF,
  0x48^0xFF, 0xFE^0xFF, 0x04^0xFF, 0xA3^0xFF,
  0x13^0xFF, 0x04^0xFF };
unsigned char LowerMemAvail_2[] =
{ 0xA1^0xFF, 0x13^0xFF, 0x04^0xFF, 0xFE^0xFF,
  0x04^0xFF, 0x48^0xFF, 0xFE^0xFF, 0x04^0xFF,
  0x48^0xFF, 0xFE^0xFF, 0x04^0xFF, 0x83^0xFF,
  0x2E^0xFF, 0x13^0xFF, 0x04^0xFF };
unsigned char LowerMemAvail_3[] =
{ 0xFF^0xFF, 0x0E^0xFF, 0x13^0xFF, 0x04^0xFF };
unsigned char LowerMemAvail_4[] =
{ 0x83^0xFF, 0x2E^0xFF, 0x13^0xFF, 0x04^0xFF };
unsigned char LowerMemAvail_5[] =
{ 0xA1^0xFF, 0x13^0xFF, 0x04^0xFF, 0xFE^0xFF,
  0x04^0xFF, 0x48^0xFF, 0xFE^0xFF, 0x04^0xFF,
  0xA3^0xFF, 0x13^0xFF, 0x04^0xFF };
unsigned char LowerMemAvail_6[] =
{ 0xA1^0xFF, 0x13^0xFF, 0x04^0xFF, 0xFE^0xFF,
  0x0A^0xFF, 0x2D^0xFF, 0xFE^0xFF, 0x06^0xFF,
  0xA3^0xFF, 0x13^0xFF, 0x04^0xFF };

/* These are little twograms which are rather indicative of viruses, though
 * not as strongly as the longer search patterns above.
 */
unsigned char maybe_HookINT13[] =
{ 0x4C^0xFF, 0x00^0xFF };
unsigned char maybe_LowerMemAvail[] =
{ 0x13^0xFF, 0x04^0xFF };
unsigned char maybe_StashOrigBS[] =
{ 0x01^0xFF, 0x03^0xFF };
#if 0
unsigned char maybe_MoveBoot_1[] =
{ 0xF3^0xFF, 0xA4^0xFF };
unsigned char maybe_MoveBoot_2[] =
{ 0xF3^0xFF, 0xA5^0xFF };
#endif

/* The data structures used to hold these search patterns use this type.
 */
typedef struct
{
   unsigned char *key;
   int key_len;
} key_descriptor;

#define MDES(key) { key, sizeof(key) }

#define NUM_HookINT13     0xD
#define NUM_StashOrigBS   9
#define NUM_MoveBoot      0xA
#define NUM_LowerMemAvail 6


key_descriptor v_HookINT13[NUM_HookINT13] =
{ MDES(HookINT13_1), MDES(HookINT13_2), MDES(HookINT13_3),
  MDES(HookINT13_4), MDES(HookINT13_5), MDES(HookINT13_6),
  MDES(HookINT13_7), MDES(HookINT13_8), MDES(HookINT13_9),
  MDES(HookINT13_A), MDES(HookINT13_B), MDES(HookINT13_C),
  MDES(HookINT13_D) };
key_descriptor v_StashOrigBS[NUM_StashOrigBS] =
{ MDES(StashOrigBS_1), MDES(StashOrigBS_2), MDES(StashOrigBS_3),
  MDES(StashOrigBS_4), MDES(StashOrigBS_5), MDES(StashOrigBS_6),
  MDES(StashOrigBS_7), MDES(StashOrigBS_8), MDES(StashOrigBS_9) };
key_descriptor v_MoveBoot[NUM_MoveBoot] =
{ MDES(MoveBoot_1), MDES(MoveBoot_2), MDES(MoveBoot_3),
  MDES(MoveBoot_4), MDES(MoveBoot_5), MDES(MoveBoot_6),
  MDES(MoveBoot_7), MDES(MoveBoot_8), MDES(MoveBoot_9),
  MDES(MoveBoot_9) };
key_descriptor v_LowerMemAvail[NUM_LowerMemAvail] =
{ MDES(LowerMemAvail_1), MDES(LowerMemAvail_2), MDES(LowerMemAvail_3),
  MDES(LowerMemAvail_4), MDES(LowerMemAvail_5), MDES(LowerMemAvail_6) };


/* Returns number of times pattern (key) is found in buffer (buf).
 */
int
pcnt(unsigned char *buf,
     unsigned int buf_len,
     unsigned char *key,
     register unsigned int len_key)
{
   register int i;
   int cnt = 0;

   for (i=0; i<(int)(buf_len-len_key); ++i)
   {
      if (match_vcomplex_sig(key, &buf[i], len_key))
         ++cnt;
   }
   return cnt;
}

/* Returns TRUE if any patterns in pattern vector (kd) are found in buf.
 */
bOOlean
v_pcnt(unsigned char *buf,
       unsigned int buf_len,
       key_descriptor kd[],
       register int num_keys_in_vect)
{
   register int i;

   for (i=0; i<num_keys_in_vect; ++i)
   {
      if (pcnt(buf, buf_len, kd[i].key, kd[i].key_len))
         return 1;
   }
   return 0;
}

/* Results are combined into a single bOOlean here.
 */
int old_alg_boot(unsigned char *buf, unsigned int buf_len)
{
   int sum = 0;
   if (v_pcnt(buf, buf_len, v_HookINT13, NUM_HookINT13))
   {
      sum+=2;
#if !ESDC
      if (always_warn) printf("Found HookINT13\n");
#endif
   }
   else
   if (pcnt(buf, buf_len, maybe_HookINT13, sizeof(maybe_HookINT13)))
   {
      sum+=1;
#if !ESDC
      if (always_warn) printf("Found maybe_HookINT13\n");
#endif
   }

   if (v_pcnt(buf, buf_len, v_StashOrigBS, NUM_StashOrigBS))
   {
      sum+=2;
#if !ESDC
      if (always_warn) printf("Found StashOrigBS\n");
#endif
   }
   else
   if (pcnt(buf, buf_len, maybe_StashOrigBS, sizeof(maybe_StashOrigBS)))
   {
      sum+=1;
#if !ESDC
      if (always_warn) printf("Found maybe_StashOrigBS\n");
#endif
   }

   if (v_pcnt(buf, buf_len, v_MoveBoot, NUM_MoveBoot))
   {
      sum+=1;
#if !ESDC
      if (always_warn) printf("Found MoveBoot\n");
#endif
   }
#if 0
   else
   if (pcnt(buf, buf_len, maybe_MoveBoot_1, sizeof(maybe_MoveBoot_1)) ||
       pcnt(buf, buf_len, maybe_MoveBoot_2, sizeof(maybe_MoveBoot_2)))
   {
      sum+=1;
#if !ESDC
      if (always_warn) printf("Found maybe_MoveBoot\n");
#endif
   }
#endif

   if (v_pcnt(buf, buf_len, v_LowerMemAvail, NUM_LowerMemAvail))
   {
      sum+=2;
#if !ESDC
      if (always_warn) printf("Found LowerMemAvail\n");
#endif
   }
   else
   if (pcnt(buf, buf_len, maybe_LowerMemAvail, sizeof(maybe_LowerMemAvail)))
   {
      sum+=1;
#if !ESDC
      if (always_warn) printf("Found maybe_LowerMemAvail\n");
#endif
   }
   return sum;
}

int
old_alg_boot_output(unsigned char *buf, unsigned int buf_len)
{
   return old_alg_boot(buf, buf_len) >= 3;
}
#endif

#include "algbnet.h"

/* These patterns, at certain offsets, are considered strong-enough indications
 * that an known false positive was found, that we can ignore this false
 * positive.
 */
unsigned char Exclude_1[] =          /* swapdriv.boo */
{
   0x7C^0xFF, 0xFB^0xFF, 0x8B^0xFF, 0xC4^0xFF,
   0xA3^0xFF, 0xFF^0xFF, 0x7D^0xFF, 0x33^0xFF,
   0xC0^0xFF, 0xA3^0xFF, 0xFF^0xFF, 0x7D^0xFF,
   0xA1^0xFF, 0x4C^0xFF, 0x00^0xFF, 0xA3^0xFF,
   0xFF^0xFF, 0x7D^0xFF, 0xA1^0xFF, 0x4E^0xFF,
   0x00^0xFF, 0xA3^0xFF, 0xFF^0xFF, 0x7D^0xFF
};
unsigned char Exclude_2[] =          /* ds1flop.sbr */
{
   0x7D^0xFF, 0xEB^0xFF, 0xC3^0xFF, 0x3C^0xFF,
   0x08^0xFF, 0x74^0xFF, 0xDB^0xFF, 0xD1^0xFF,
   0xCB^0xFF, 0x30^0xFF, 0xC3^0xFF, 0xBE^0xFF,
   0xBE^0xFF, 0x7D^0xFF, 0xE8^0xFF, 0xA0^0xFF,
   0xFF^0xFF, 0xEB^0xFF, 0xDE^0xFF, 0xC3^0xFF,
   0xFA^0xFF, 0x31^0xFF, 0xC0^0xFF, 0x8E^0xFF
};
unsigned char Exclude_3[] =          /* pc-guard */
{
   0xB8^0xFF, 0x00^0xFF, 0x00^0xFF, 0x8E^0xFF,
   0xD8^0xFF, 0xBE^0xFF, 0x00^0xFF, 0x7C^0xFF,
   0x8C^0xFF, 0xC8^0xFF, 0x2D^0xFF, 0x00^0xFF,
   0x10^0xFF, 0x8E^0xFF, 0xC0^0xFF, 0xB8^0xFF,
   0x00^0xFF, 0x04^0xFF, 0x2E^0xFF, 0x8B^0xFF,
   0x1E^0xFF, 0xFF^0xFF, 0xFF^0xFF, 0xD1^0xFF,
   0xEB^0xFF, 0xD1^0xFF, 0xEB^0xFF
};
unsigned char Exclude_4[] =          /* EZ-drive */
{
   0xFF^0xFF, 0xEE^0xFF, 0x0D^0xFF, 0x74^0xFF,
   0x1B^0xFF, 0x83^0xFF, 0xC7^0xFF, 0x10^0xFF,
   0xEB^0xFF, 0xEB^0xFF, 0xBD^0xFF, 0xFF^0xFF,
   0xFF^0xFF, 0xEB^0xFF, 0xC9^0xFF, 0xBD^0xFF,
   0xFF^0xFF, 0xFF^0xFF, 0xE8^0xFF, 0x47^0xFF,
   0x00^0xFF, 0x33^0xFF, 0xC0^0xFF, 0xCD^0xFF,
   0x16^0xFF, 0xCD^0xFF, 0x19^0xFF, 0xEA^0xFF,
   0x00^0xFF, 0x00^0xFF, 0xFF^0xFF, 0xFF^0xFF,
   0xBD^0xFF, 0xFF^0xFF, 0xFF^0xFF, 0xE8^0xFF,
   0x36^0xFF, 0x00^0xFF, 0xEB^0xFF, 0xC2^0xFF
};
unsigned char Exclude_5[] =          /* EZ-drive 95 */
{
   0x81^0xFF, 0xFF^0xFF, 0xEE^0xFF, 0x07^0xFF,
   0x74^0xFF, 0xFF^0xFF, 0x83^0xFF, 0xC7^0xFF,
   0x10^0xFF, 0xEB^0xFF, 0xFF^0xFF, 0xBD^0xFF,
   0x3E^0xFF, 0x07^0xFF, 0xEB^0xFF, 0xFF^0xFF,
   0xBD^0xFF, 0x51^0xFF, 0x07^0xFF, 0xE8^0xFF,
   0xFF^0xFF, 0xFF^0xFF, 0x33^0xFF, 0xC0^0xFF
};
#define Exclude_1_Offset 0x002C
#define Exclude_2_Offset 0x0051
#define Exclude_3_Offset 0x0084
#define Exclude_4_Offset 0x00A7
#define Exclude_5_Offset 0x00DB

typedef struct
{
   unsigned char *exclude;
   int exclude_len;
   int exclude_offset;
} exclude_descriptor;

#define NUM_Exclude       5

#define MEDES(key, N) { key, sizeof(key), N }
exclude_descriptor v_Exclude[NUM_Exclude] =
{ MEDES(Exclude_1, Exclude_1_Offset),
  MEDES(Exclude_2, Exclude_2_Offset),
  MEDES(Exclude_3, Exclude_3_Offset),
  MEDES(Exclude_4, Exclude_4_Offset),
  MEDES(Exclude_5, Exclude_5_Offset)
};

/* Returns TRUE if any patterns in the standard exclusion list were found
 * in the buffer.
 */
bOOlean
in_exclusion_list(unsigned char *buf)
{
   int i;
   for (i=0; i<NUM_Exclude; ++i)
   {
      if (match_vcomplex_sig(v_Exclude[i].exclude,
                             &buf[v_Exclude[i].exclude_offset],
                             v_Exclude[i].exclude_len))
      {
#if !ESDC
         if (always_warn) printf("Exclude: %d\n", i);
#endif
         return 1;
      }
   }
   return 0;

}

/*
 * Returns:
 * 0 if not suspicious.
 * 1 (not zero) if suspicious.
 * Buffer is assumed to be a 512 byte buffer filled with contents of
 * a boot record.
 */
int alg_boot(unsigned char *buf)
{
   unsigned int jtarg = 0;
   unsigned int start_suspcheck_ind;
   unsigned int suspcheck_len;

   if (in_exclusion_list(buf))
     return 0;

   if (buf[0] == 0xE9)   /* Jump far */
   {
      jtarg = *((unsigned int *)&buf[1]);
      jtarg += 3;
      if (jtarg <= 0x40)
         start_suspcheck_ind = jtarg;
      else
         start_suspcheck_ind = 0x00; /* Used to be 0x36 */
   }
   else
   if (buf[0] == 0xEB)   /* Jump near */
   {
      jtarg = ((unsigned int) *((unsigned char *)&buf[1]));
      jtarg += 2;
      if (jtarg <= 0x40)
         start_suspcheck_ind = jtarg;
      else
         start_suspcheck_ind = 0x00; /* Used to be 0x36 */
   }
   else
   {
      start_suspcheck_ind = 0;
   }

   suspcheck_len = 512-start_suspcheck_ind;
#if 0
   printf("suspcheck ind/len = %u(%04X) : %u(%04X)\n",
          start_suspcheck_ind, start_suspcheck_ind,
          suspcheck_len, suspcheck_len);
#endif
#if ESDC
   return alg_boot_net_output(&buf[start_suspcheck_ind], suspcheck_len);
#else
   if (use_old_alg_boot)
      return old_alg_boot_output(&buf[start_suspcheck_ind], suspcheck_len);
   else
      return alg_boot_net_output(&buf[start_suspcheck_ind], suspcheck_len);
#endif
}

//===========================================================================
//
// Function scans an array of buffers with 'iNumBuffers' elements.
// These elements will be viewed as a single buffer for n-gram
// counting.
// Returns:
// 0 if not suspicious.
// 1 (not zero) if suspicious.

int   alg_boot_ex (
         BUFFER   *pBuffers,  // <-  Buffers with data
         int      iNumBuffers // <-  Number of elements in pBuffers.
         )
{
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

   // Note: exclusion list get checked into 'ngram_term_count_ex()' function.
   //
   if (alg_boot_net_output_ex (pBuffers, iNumBuffers))
      return (1);

   return (0);

}
//===========================================================================


//===========================================================================
//
// This callback will be synchroneously called from inside of BootRun()
// to notify us about interesting events.
//
// See   '// *** Warning:'  for looks-like-virus behaviour.

int   BehaviourNotification (
         unsigned long  ulBCode, // Behaviour code.
         MACHINE_STATE  *pMachState,
         RETURN_DATA    *pRetData,
         void           *CodeSpecific
         )
{
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


   switch (ulBCode)
      {
      case  BR_BOOTRUN_STARTED:        // CodeSpecific: none.
         // Here we can also get starting values of vectors, [413], etc... if needed.
         break;

      case  BR_BOOTRUN_ENDED:          // CodeSpecific: none.
         // Bootrun ended. Here we can analyse all memory areas etc..
         break;

      case  BR_CODERUNNER_STOPPED:     // CodeSpecific: none.
         // Here we can figure out a reson for termination by looking into 'pRetData->ulRetCode'.
         break;

      case  BR_BREAK_CONDITION_CHECK:  // CodeSpecific: none.
         // This case will be called by BootRun() before each instruction.
         // We can look into current CS:IP or anywhere else as wanted.
         break;

      case  BR_TRAP_ON_EXISTING_AREA:  // CodeSpecific: none.
         // Code trapped on already allocated memory area.
         // Don't bother.
         break;

      case  BR_TRAP_RECOVERED:         // CodeSpecific: none.
         // Just notification about successfully recovered traps. Could be used
         // for counting of traps number.
         break;

      case  BR_INT_EMULATOR_ENTERED:   // CodeSpecific = int number.
         // Get called every time INT xx instruction is executed.
         // Here we can get behaviour about any INT xx virus called
         // during execution.
         {
         int IntNumber = (int)CodeSpecific;

         if (IntNumber == 0x13 && rAH == 0x03)
            {
            // Code attempts to write!
            if (rDL<0x80)
               // *** Warning: Code Attempts to write on diskette!
               // ... do something about it.
               ;
            else
               // *** Warning: Code Attempts to write on Hard Disk!
               // ... do something about it.
               ;
            }
         }
         break;

      case  BR_INT13_ORIGINAL_CODE_REACHED:  // CodeSpecific: none.
         // Original (our) INT 13h code is about to be executed.
         // Virus could call INT 13h without executing INT 13h instruction,
         // so this is the place, where that could be detected.
         break;

      case  BR_INT13_REDUNDANT_READ:         // CodeSpecific: none.
         // This will be called if INT 13h read operation was called
         // more than twice on the same sector. BootRun() will terminate
         // in this case.
         break;

      case  BR_INT13_EXECUTING_DISK_IO:      // CodeSpecific: DISK_IO_REQUEST *: all data for DiskIO() call.
         // BootRun() is about to execute direct disk I/O request with parameters
         // shown in 'DISK_IO_REQUEST' structure.
         {
         DISK_IO_REQUEST   *pDisk_io_req = (DISK_IO_REQUEST *)CodeSpecific;

         // Check whether about-to-be-overwritten buffer looks suspicious...
         //
         if (pDisk_io_req->ulOperation == 0x02)  // Read?
            {
            // typedef  struct _BUFFER
            // {
            //    unsigned char  *pBuffer;
            //    unsigned long  ulBufferSize;
            // } BUFFER;
            //
            BUFFER   buffer;

            buffer.pBuffer       = pDisk_io_req->sector.pBuffer;
            buffer.ulBufferSize  = pDisk_io_req->ulSectorsNo * 512;

            if (alg_boot_ex (&buffer, 1))
               {
               // Target buffer for read operation is suspicious! We will set a global
               // "suspicious" flag in here and:
               // either abort BootRun() by setting flag: 'pRetData->ulRetCode |= USER_ABORT'
               // or continue BootRun() until the end.
               // After BootRun() will terminate we'll analyse 'g_SuspiciousCode' and
               // run all buffers through alg_boot_ex().

               // g_SuspiciousCode = TRUE;
               }
            else
               // Target buffer for read operation is clean.
               ;
            }
         }
         break;

      case  BR_INT13_DISK_IO_DONE:  // CodeSpecific: int RetCode: after DiskIO().
         // if ((int)CodeSpecific == 0) //DISK_IO_SUCCESS)
         //    // ... Disk I/O succeeded.
         // else
         //    // ... Disk I/O failed.
         break;

      case  BR_JMP_TO_0_7C00:       // CodeSpecific: unsigned char *: ptr to data at 0:7C00
         // Code is about to jump into 0:7C00 area. Take a look into
         // buffer, pointer by CodeSpecific to see inside of buffer.
         {
         unsigned char  *p0_7C00_buff = (unsigned char *)CodeSpecific;
         // ...
         }
         break;

      case  BR_JMP_TO_0_700:        // CodeSpecific: unsigned char *: ptr to data at 0:700
         // Code is about to jump into 0:700 area. Take a look into
         // buffer, pointer by CodeSpecific to see inside of buffer.
         // BootRun() will terminate, assuming, that original OS started booting...
         break;

      case  BR_0_413_MODIFIED:      // CodeSpecific: WORD  new0_413: new value at 0:[413h].
         // *** Warning:
         // Area 0:[413h] was modified. See (WORD)CodeSpecific for a new contents.
         break;

      case  BR_IVT_INT13_MODIFIED:  // CodeSpecific: BYTE *Ptr: ptr to INT13h vector.
         // *** Warning:
         // INT 13h vector was modified. 'CodeSpecific' points to INT 13h vector.
         {
         unsigned char *pIVT_INT13 = (unsigned char *)CodeSpecific;
         }
         break;

      case  BR_JMP_TO_ITSELF:
         // Code about to infinitely jump to itself. BootRun() will terminate.
         break;

      default:
         // Unrecognized (or un'case'-ed) notification.
         break;
      }

return (0);
}
//===========================================================================


//===========================================================================
//
// This callback function will be synchroneously called from inside of
// CodeRunner() and BootRun() to inform, that certain memory is about to be
// overwritten.
// Attempt to overwrite 'wBytes' number of bytes of destination buffer does not
// guarantee, that destination buffer is present in existing MEMORY_AREAs.
//
// Function returns:
//
// USER_EXEC_CONTINUE   0  Continue normal execution of this
//                         instruction.
// USER_EXEC_SKIP       2  Do not execute this instruction.
//                         Skip it and continue execution
//                         from the next instruction.
// USER_EXEC_ABORT     -1  Abort everything and exit.

int   WipingMemoryCallback (
         MACHINE_STATE  *pMachState,
         RETURN_DATA    *pRetData,
         WORD           wSeg,    // Seg of wiping memory.
         WORD           wOff,    // Off of wiping memory.
         WORD           wBytes   // Bytes to wipe.
         )
{
//---------------------------------------------------------------------------
   // typedef  struct _BUFFER
   // {
   //    unsigned char  *pBuffer;
   //    unsigned long  ulBufferSize;
   // } BUFFER;
   //
   MEMORY_AREA    *pMA;
   WORD           BytesToCheck = 0;
   BUFFER         buffer;
   int            RetCode  =  USER_EXEC_CONTINUE;
//---------------------------------------------------------------------------


   // Make sure, that allocated memory area is big enough to hold requested
   // (rAL*512) number of bytes starting from ES:BX...
   // Case 3 will check it.
   //
   pMA = FindAreaInList (
            wSeg,                // Find match for this segment.
            wOff,                // Find match for this offset.
            WRITEABLE,           // Buffer with these flags.
            &BytesToCheck,       // Bytes in found buffer left after match point.
            pMachState->pMemA    // Starting area to search.
            );

   // Is Buffer exists and big enough?
   //
   if (!pMA || BytesToCheck < 4)
      return (USER_EXEC_CONTINUE);  // Nothing to do: buffer exists or too small.


   // Initialize BUFFER.
   // CR_DEFS.H: BufferInMA (...) Macro returns ptr to allocated buffer in MA,
   // corresponding to requested segment and offset.
   //
   buffer.pBuffer       = BufferInMA (pMA, wSeg, wOff);
   buffer.ulBufferSize  = BytesToCheck;

   if (alg_boot_ex (&buffer, 1))
      {
      // Target buffer for read operation is suspicious! We will set a global
      // "suspicious" flag in here and:
      // either abort BootRun() by returning 'USER_EXEC_ABORT'
      // or continue BootRun() until the end.
      // After BootRun() will terminate we'll analyse 'g_SuspiciousCode' and
      // run all buffers through alg_boot_ex().

      // g_SuspiciousCode = TRUE;
      // RetCode  =  USER_EXEC_ABORT;
      }
   else
      // Target buffer is clean.
      ;

   return (RetCode);
}
//===========================================================================
