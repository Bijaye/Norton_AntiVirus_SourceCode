/* ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ» */
/* º Program     :  Z_Shrink.C                     º */
/* º Release     :  2.51                           º */
/* º Description :  Shrinking compression routine  º */
/* º                for PAK                        º */
/* ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶ */
/* º Author      :  Gus Smedstad                   º */
/* º Last Updated:  10/08/90                       º */
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


#define HashMax 4096
#define MaxCode 8192
#define Init_Bits 9
#define Hash(c, n) (((c << 3) + n) & (HashMax - 1))

typedef unsigned char byte;

typedef struct {
  __int16  code;
  __int16  nextbyte;
  __int16  bits;
  __int16  limit;
  __int16  bitoffset;
  __int16  next_code;
 } shrink_common;

typedef struct {
  shrink_common save;
  __int16          entry_point;
  __int16          outbyte;
  __int16          HashTable[HashMax];
  __int16          Hnext[MaxCode];
  __int16          prefix[MaxCode];
  byte         suffix[MaxCode];
 } shrink_vars;


void copy_struct5(void far *dest, void far *src,
                                    unsigned size);
unsigned WINAPI shrinkSize(void);
void WINAPI initShrink(shrink_vars far *vars);
int WINAPI shrink(byte far *inbuffer, unsigned short far *input_size,
                      byte far *outbuffer, unsigned short far *output_size,
                      shrink_vars far *vars, char final);

#ifdef SYM_BUILD_SM
#include "wnfsv.seg"
#else
#include "..\inc\wnfsv.seg"
#endif

void copy_struct5(void far *dest, void far *src,
                                    unsigned size)
{
#ifdef WIN32
  memcpy( dest, src, size );
#else
  if (size & 1)
    *((char *)dest)++ = *((char *)src)++;
  size >>= 1;
  while (size--)
    *((__int16 *)dest)++ = *((__int16 *)src)++;
#endif
}

unsigned WINAPI shrinkSize(void)
 {
  return sizeof(shrink_vars);
 }

void WINAPI initShrink(shrink_vars far *vars)
 {
  __int16 i;

  vars->save.bits = Init_Bits;
  vars->save.limit = 1 << Init_Bits;

  i = HashMax;
  while (i--)
    vars->HashTable[i] = 0;

  vars->save.bitoffset = 0;
  vars->outbyte = 0;
  vars->entry_point = 0;
  i = MaxCode;
  while (i-- > 256) {
    vars->prefix[i] = (i + 1);
   }
  i = 256;
  while (i--) {
    vars->prefix[i] = 0;
    vars->suffix[i] = (byte)i;
   }
  vars->save.next_code = 257;
 }


int WINAPI shrink(byte far *inbuffer, unsigned short far *input_size,
                      byte far *outbuffer, unsigned short far *output_size,
                      shrink_vars far *vars, char final)
 {

  __int16          h;
  __int16          i;

  shrink_common local;
  unsigned     out_size;
  unsigned     in_size;


  in_size = *input_size;
  out_size = *output_size;
  *outbuffer = (byte)vars->outbyte;
  copy_struct5(&local, &vars->save, sizeof(local));

  switch (vars->entry_point) {
    case 0 : goto entry0;
    case 1 : goto entry1;
    case 2 : goto entry2;
    case 3 : goto entry3;
    case 4 : goto entry4;
   }

entry0:
  if (in_size--) {
    local.nextbyte = *inbuffer++;
   } else {
    in_size = 0;
    if (final) {
      local.nextbyte = -1;
     } else {
      vars->entry_point = 0;
      goto exit;
     }
   }
  while (local.nextbyte >= 0) {
    local.code = local.nextbyte;
entry1:
    if (in_size--) {
      local.nextbyte = *inbuffer++;
     } else {
      in_size = 0;
      if (final) {
        local.nextbyte = -1;
       } else {
        vars->entry_point = 1;
        goto exit;
       }
     }
    h = vars->HashTable[Hash(local.code, local.nextbyte)];
    if (h) do {
      if (vars->prefix[h] != local.code ||
          (__int16)vars->suffix[h] != local.nextbyte) {
        h = vars->Hnext[h];
       } else {
   local.code = h;
        if (in_size--) {
          local.nextbyte = *inbuffer++;
         } else {
          in_size = 0;
          if (final) {
            local.nextbyte = -1;
           } else {
            vars->entry_point = 1;
            goto exit;
           }
         }
   h = vars->HashTable[Hash(local.code, local.nextbyte)];
       }
     } while (h);

    while (local.code >= local.limit) {
entry2:
      if (out_size < 3) {
        vars->entry_point = 2;
        goto exit;
       }
      *outbuffer |= 256 << local.bitoffset;
      local.bitoffset += local.bits;
      while (local.bitoffset > 7) {
        out_size--;
        local.bitoffset -= 8;
        *++outbuffer = 256 >> (local.bits - local.bitoffset);
       }
      *outbuffer |= 1 << local.bitoffset;
      local.bitoffset += local.bits;
      while (local.bitoffset > 7) {
        out_size--;
        local.bitoffset -= 8;
        *++outbuffer = 1 >> (local.bits - local.bitoffset);
       }
      local.limit = 1 << ++local.bits;
     }

entry3:
      if (out_size < 3) {
        vars->entry_point = 3;
        goto exit;
       }
    *outbuffer |= local.code << local.bitoffset;
    local.bitoffset += local.bits;
    while (local.bitoffset > 7) {
      out_size--;
      local.bitoffset -= 8;
      *++outbuffer = local.code >> (local.bits - local.bitoffset);
     }

    if (local.next_code == MaxCode) {
      /* strip leaves */
      i = MaxCode;
      while (i-- > 257) {
        vars->prefix[i] |= 0x8000;
       }
      i = MaxCode;
      while (i-- > 257) {
        vars->prefix[vars->prefix[i] & 0x7fff] &= 0x7fff;
       }
      i = HashMax;
      while (i--)
        vars->HashTable[i] = 0;
      i = MaxCode;
      while (i-- > 257) {
        if (vars->prefix[i] & 0x8000) {
          vars->prefix[i] = local.next_code;
          local.next_code = i;
         } else {
          h = Hash(vars->prefix[i], vars->suffix[i]);
          vars->Hnext[i] = vars->HashTable[h];
          vars->HashTable[h] = i;
         }
       }
entry4:
      if (out_size < 3) {
        vars->entry_point = 4;
        goto exit;
       }
      *outbuffer |= 256 << local.bitoffset;
      local.bitoffset += local.bits;
      while (local.bitoffset > 7) {
        out_size--;
        local.bitoffset -= 8;
        *++outbuffer = 256 >> (local.bits - local.bitoffset);
       }
      *outbuffer |= 2 << local.bitoffset;
      local.bitoffset += local.bits;
      while (local.bitoffset > 7) {
        out_size--;
        local.bitoffset -= 8;
        *++outbuffer = 2 >> (local.bits - local.bitoffset);
       }
     }
    i = vars->prefix[local.next_code];
    vars->prefix[local.next_code] = local.code;
    vars->suffix[local.next_code] = (byte)local.nextbyte;
    h = Hash(vars->prefix[local.next_code], vars->suffix[local.next_code]);
    vars->Hnext[local.next_code] = vars->HashTable[h];
    vars->HashTable[h] = local.next_code;
    local.next_code = i;
   }

  out_size -= (local.bitoffset + 7) >> 3;
  *input_size -= in_size;
  *output_size -= out_size;
  return 1;
exit:
  *input_size -= in_size;
  *output_size -= out_size;
  copy_struct5(&vars->save, &local, sizeof(local));
  vars->outbyte = *outbuffer;
  return 0;
 }
