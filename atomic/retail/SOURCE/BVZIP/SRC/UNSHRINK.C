/* ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ» */
/* º Program     :  Unshrink.C                     º */
/* º Release     :  2.51                           º */
/* º Description :  Shrinking expansion routine    º */
/* º                for PAK                        º */
/* ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶ */
/* º Author      :  Gus Smedstad                   º */
/* º Last Updated:  10/25/91                       º */
/* ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶ */
/* º Notice     : This source code is NOT intended º */
/* º              for general distribution.  Care  º */
/* º              should be taken to preserve its  º */
/* º              confidentiality.                 º */
/* ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶ */
/* º Copyright (C) 1988-1991 by NoGate Consulting  º */
/* º             All rights reserved.              º */
/* ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼ */


// Common includes for Pre-compiled Header
// This should go before any other includes or definitions
#ifdef SYM_BUILD_SM
#include "fsv_pch.h"
#else
#include "..\inc\fsv_pch.h"
#endif
#pragma hdrstop
// Put all other includes after this


typedef unsigned char byte;

#define MAX_BITS  13
#define MaxCode   8192
#define FIRST        257
#define Control      256

typedef struct {
  __int16      control_mode;
  __int16      next_code;
  __int16      current_max;
  __int16      firstbyte;
  __int16      code;
  __int16      bitoffset;
  __int16      inbits;
  __int16      stackp;
  __int16      lastcode;
  byte     bits;
 } Shrink_Common;

typedef struct {
  Shrink_Common save;
  __int16          entry_point;
  byte         stack[MaxCode];
  unsigned     prefix[MaxCode];
  byte         suffix[MaxCode];
 } Shrink_Vars;

unsigned WINAPI unshrinkSize(void);
void copy_struct4(void far *dest, void far *src, 
                                    unsigned size);
void WINAPI initUnshrink(Shrink_Vars far *vars);
int WINAPI unshrink(byte far *inbuffer, unsigned short far *input_size,
                        byte far *outbuffer, unsigned short far *output_size,
                        Shrink_Vars far *vars, char final);


#ifdef SYM_BUILD_SM
#include "wnfsv.seg"
#else
#include "..\inc\wnfsv.seg"
#endif

void copy_struct4(void far *dest, void far *src, 
                                    unsigned size)
{
#ifdef WIN32
  memcpy( dest, src, size );
#else
  if (size & 1) {
    *((char *)dest)++ = *((char *)src)++;
   }
  size >>= 1;
  while (size--) *((__int16 *)dest)++ = *((__int16 *)src)++;
#endif
}


/*  ----- Lempel - Zev coding - improvements by Gus Smedstad ------ */
/*  ----- 'crushed' encoding ------ */

unsigned WINAPI unshrinkSize(void)
 {
  return (sizeof(Shrink_Vars));
 }

void WINAPI initUnshrink(Shrink_Vars far *vars)
 {
  __int16 i;

  i = MaxCode;
  while (i-- > 256) {
    vars->prefix[i] = (i + 1) | 0x8000;
   }
  i = 256;
  while (i--) {
    vars->prefix[i] = 0;
    vars->suffix[i] = (byte)i;
   }
  vars->save.bits = 9;
  vars->save.next_code = 256;
  vars->save.current_max = 512;
  vars->save.bitoffset = 0;
  vars->entry_point = 0;
  vars->save.control_mode = 0;
 }

#ifdef __for_basic
int WINAPI unshrink(byte far *inbuffer, unsigned short far *in_offset,
                        unsigned short far *input_size,
                        byte far *outbuffer, unsigned far *out_offset,
                        unsigned far *output_size,
                        Shrink_Vars far *vars, char *final_addr)
 {
  char final = *final_addr;
#else
int WINAPI unshrink(byte far *inbuffer, unsigned short far *input_size,
                        byte far *outbuffer, unsigned short far *output_size,
                        Shrink_Vars far *vars, char final)
 {
#endif
  unsigned char bitmask[9];
  Shrink_Common local;
  unsigned      in_size;
  unsigned      out_size;
  unsigned      i;

#ifdef __for_basic
  inbuffer += *in_offset;
  outbuffer += *out_offset;
#endif
  i = 8;
  bitmask[8] = 0xff;
  while (i--) bitmask[i] = bitmask[i+1] >> 1;
  in_size = *input_size;
  out_size = *output_size;
  copy_struct4(&local, &vars->save, sizeof(local));
  switch (vars->entry_point) {
    case 0: goto entry0;
    case 3: goto entry3;
    case 5: goto entry5;
    case 6: goto entry6;
   }
entry0:
  while (out_size) {
    if (!in_size) {
      if (final) break;
      vars->entry_point = 0;
      goto exit;
     }
    local.code = *inbuffer++ >> local.bitoffset;
    in_size--;
    local.bitoffset = local.bits - (local.inbits = 8 - local.bitoffset);
    if (local.bitoffset >= 8) {
entry5:
      if (!in_size) {
        if (final) break;
        vars->entry_point = 5;
        goto exit;
       }
      local.code |= (*inbuffer++ << local.inbits);
      in_size--;
      local.inbits += 8;
      local.bitoffset -= 8;
     }
    if (local.bitoffset) {
entry6:
      if (!in_size) {
        if (final) break;
        vars->entry_point = 6;
        goto exit;
       }
      local.code |= ((*inbuffer & bitmask[local.bitoffset]) << local.inbits);
     }
    if (local.control_mode) {
      if (local.code == 1) {
        if (local.bits < 13) {
          local.current_max = 1 << ++local.bits;
         }
       } else {
        /* strip leaves */
        i = local.next_code;
        while (i-- > 257) {
          vars->prefix[i] |= 0x8000;
         }
        i = local.next_code;
        while (i-- > 257) {
          vars->prefix[vars->prefix[i] & 0x7fff] &= 0x7fff;
         }
        i = local.next_code;
        while (i-- > 257) {
          if (vars->prefix[i] & 0x8000) {
            vars->prefix[i] = local.next_code | 0x8000;
            local.next_code = i;
           }
         }
       }
      local.control_mode = 0;
     } else if (local.code == 256) {
      local.control_mode = 1;
     } else {
      local.stackp = 0;
      i = local.code;
      while (i >= 256 && i < 0x8000) {
        if (i == (unsigned __int16)local.next_code) {
          vars->stack[local.stackp++] = (byte)local.firstbyte;
          i = local.lastcode;
         } else {
          vars->stack[local.stackp++] = vars->suffix[i];
          i = vars->prefix[i];
         }
       }
      if (i >= 0x8000) {
        in_size = 0;
        final = 1;
        break;   /* this is an error - compressed data is corrupt */
       }
      vars->stack[local.stackp++] = (byte)local.firstbyte = vars->suffix[i];
entry3:
      do {
        *outbuffer++ = vars->stack[--local.stackp];
         out_size--;
       } while (local.stackp && out_size);
      if (local.stackp) {            /* ask for more output space */
        vars->entry_point = 3;
        goto exit;
       }

      if (local.next_code < MaxCode) {
        i = vars->prefix[local.next_code] & 0x7fff;
        vars->prefix[local.next_code] = local.lastcode;
        vars->suffix[local.next_code] = (byte)local.firstbyte;
        local.next_code = i;
       }
      local.lastcode = local.code;
     }
   }
  if (in_size || !final) {
    vars->entry_point = 0;
exit:
    *input_size -= in_size;
    *output_size -= out_size;
    copy_struct4(&vars->save, &local, sizeof(local));
    return 0;
   }
  *input_size -= in_size;
  *output_size -= out_size;
  return 1;
}
